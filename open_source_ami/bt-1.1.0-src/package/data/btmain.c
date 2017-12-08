/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#include <linux/module.h>
#include <linux/kernel.h>
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include <linux/vmalloc.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include "bt.h"
#include "btifc.h"
#include "driver_hal.h"
#include "dbgout.h"
#include "helper.h"
#include "bt_ioctl.h"

#define DRIVER_AUTHOR "Rama Rao Bisa <RamaB@ami.com>"
#define DRIVER_DESC "BT Common Driver"
#define DRIVER_LICENSE "GPL"

/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE ( DRIVER_LICENSE );

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)		 
#endif

#define BT_MAJOR           43
#define BT_MINOR	    	0
#define BT_MAX_CHANNELS    255
#define BT_DEV_NAME        "bt"

struct bt_hal
{
	BTBuf_T *pbt_buf;
	IPMICmdMsg_T* pipmi_cmd_msg;
	bt_hal_operations_t *pbt_hal_ops;
};

struct bt_dev
{
	struct bt_hal *pbt_hal;
	unsigned char ch_num;
};

TDBG_DECLARE_DBGVAR(bt);

/* Sysctl Table */
static struct ctl_table BT_table [] =
{
	{
		.ctl_name = CTL_UNNUMBERED,
		.procname = "DebugLevel",
		.data = &(TDBG_FORM_VAR_NAME(bt)),
		.maxlen = sizeof(int),
		.mode = 0644,
		.child = NULL,
		.parent = NULL,
		.proc_handler = &proc_dointvec
	},
	{
		0
	}
};

/* Proc and Sysctl entries */
static struct proc_dir_entry *moduledir = NULL;
static struct ctl_table_header *my_sys  = NULL;

/*-----------------------------------------------*
 **         Prototype Declaration       **
 *-----------------------------------------------*/
static int bt_open (struct inode *inode, struct file *filp);
static int bt_release (struct inode *inode, struct file *filp);
static int bt_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static ssize_t bt_read (struct file *filp, char __user *buf, size_t count, loff_t *offset);
static ssize_t bt_write (struct file *filp, const char __user *buf, size_t count, loff_t *offset);

static struct file_operations bt_fops = 
{
    .owner 		= THIS_MODULE,
    .read  		= bt_read,
    .write 		= bt_write,
    .ioctl 		= bt_ioctl,
    .open  		= bt_open,
    .release 	= bt_release,
};

static struct cdev *bt_cdev;
static dev_t bt_devno = MKDEV(BT_MAJOR, BT_MINOR);
static char banner[] __initdata = KERN_INFO "BT Common Driver, (c) 2011 American Megatrends Inc...\n";	


static void __exit bt_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_BT);
	unregister_chrdev_region (bt_devno, BT_MAX_CHANNELS);

	if (NULL != bt_cdev)
	{
		dbgprint ("bt char device del\n");
		cdev_del (bt_cdev);
	}
	RemoveSysctlTable(my_sys);
	remove_proc_entry("bt",rootdir);
	printk("BT Common module unloaded successfully\n");
	return;
}

static int
register_bt_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	int i;
	struct bt_hal *pbt_hal;

	pbt_hal = (struct bt_hal*) kmalloc (sizeof(struct bt_hal), GFP_KERNEL);
	if (!pbt_hal)
	{
		TCRIT ("register_bt_hal_module(): kmalloc failed for pbt_hal\n");
		return -ENOMEM;
	}

	pbt_hal->pbt_buf = (BTBuf_T*) kmalloc (num_instances * sizeof(BTBuf_T), GFP_KERNEL);
	if (!pbt_hal->pbt_buf)
	{
		TCRIT ("register_bt_hal_module(): kmalloc failed for pbt_hal->pbt_buf\n");
		kfree (pbt_hal);
		return -ENOMEM;
	}

	pbt_hal->pipmi_cmd_msg = (IPMICmdMsg_T*) vmalloc (num_instances * sizeof(IPMICmdMsg_T));
	if (!pbt_hal->pipmi_cmd_msg)
	{
		TCRIT ("register_bt_hal_module(): kmalloc failed for pbt_hal->pipmi_cmd_msg\n");
		kfree (pbt_hal->pbt_buf);
		kfree (pbt_hal);
		return -ENOMEM;
	}

	for (i = 0; i < num_instances; ++i)
	{
		dbgprint ("%d, bt_buf addr : %p\n", i, &pbt_hal->pbt_buf[i]);						
		pbt_hal->pbt_buf[i].pBTRcvPkt = pbt_hal->pipmi_cmd_msg[i].Request;
		pbt_hal->pbt_buf[i].BtWakeup = 0;
		pbt_hal->pbt_buf[i].FirstTime = 1;
	}

	pbt_hal->pbt_hal_ops = (bt_hal_operations_t *) phal_ops;

	*phw_data = (void *)pbt_hal;
	dbgprint ("private data: %p, %p\n", pbt_hal, *phw_data);

	return 0;
}

static int
unregister_bt_hal_module (void *phw_data)
{
	struct bt_hal *pbt_hal = (struct bt_hal *)phw_data;

	dbgprint ("unregister hal addr : %p\n", pbt_hal);
	RemoveProcEntry(moduledir,"BT");
	vfree (pbt_hal->pipmi_cmd_msg);
	kfree (pbt_hal->pbt_buf);
	kfree (pbt_hal);

    return 0;
}

static int
bt_open(struct inode *inode, struct file *file)
{
	struct bt_hal *pbt_hal;
	unsigned int minor = iminor(inode);
	struct bt_dev* pdev;
	BTBuf_T* pBTBuffer;
	hw_info_t bt_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_BT, minor, &open_count, &bt_hw_info);
	if (ret)
	{
		TCRIT ("bt_open(): hw_open call failed\n");
		return -ENXIO;
	}
	pbt_hal = bt_hw_info.pdrv_data;

	pdev = (struct bt_dev*) kmalloc (sizeof(struct bt_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_BT, minor, &open_count);	
		TCRIT ("bt_open() %s: failed to allocate bt private dev structure for bt iminor: %d\n", BT_DEV_NAME, minor);
		return -ENOMEM;
	}

	pBTBuffer = &pbt_hal->pbt_buf [bt_hw_info.inst_num];
	dbgprint ("%d, bt_open hal addr : %p\n", minor, pbt_hal);
	dbgprint ("%d, bt_open bufffer addr : %p\n", minor, pBTBuffer);				
	if (open_count == 1) // On open descriptors, the initialization would have been done in the first open. Hence skip initialization
	{
		pBTBuffer->BTRcvPktLen = 0;
		pBTBuffer->BTSendPktLen = 0;
	}

	pdev->pbt_hal = pbt_hal;
	pdev->ch_num = bt_hw_info.inst_num;
	dbgprint ("%d, bt_open ch num : %d\n", minor, pdev->ch_num);
	file->private_data = pdev;		

	dbgprint ("%d, bt_open priv data addr : %p\n", minor, &file->private_data);	
	return nonseekable_open (inode, file);	
}

static int 
bt_release (struct inode *inode, struct file *file)
{
	int ret;
	struct bt_dev *pdev = (struct bt_dev*) file->private_data;
	unsigned char open_count;

	ret = hw_close (EDEV_TYPE_BT, iminor(inode), &open_count);
	if (ret) 
	{
		TCRIT ("bt_release(): hw_close call failed\n");
		return -1;
	}

	dbgprint ("%d, ch: %d bt_release priv data addr : %p\n", iminor(inode), pdev->ch_num, pdev);

	pdev->pbt_hal = NULL;
	kfree (pdev);

	return 0;
}

static int 
bt_set_sms_bit (struct bt_dev *pdev, unsigned long arg)
{
	bt_data_t bt_data;

	if (copy_from_user (&bt_data, (void*)arg, sizeof(bt_data_t)))
	{
		TCRIT ("bt_set_sms_bit(): copy from user failed\n");
		return -EFAULT;
	}
	pdev->pbt_hal->pbt_hal_ops->set_sms_bit (bt_data.btifcnum);

	return 0;
}

static int 
bt_clear_sms_bit (struct bt_dev *pdev, unsigned long arg)
{
	bt_data_t bt_data;

	if ( copy_from_user(&bt_data, (void*)arg, sizeof(bt_data_t)) )
	{
		TCRIT ("bt_clear_sms_bit(): copy from user failed\n");
		return -EFAULT;
	}
	pdev->pbt_hal->pbt_hal_ops->clr_sms_bit (bt_data.btifcnum);
	return 0;
}

static int 
bt_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct bt_dev *pdev = (struct bt_dev*) file->private_data;

	switch(cmd)
	{
		case SET_SMS_BIT:
			ret = bt_set_sms_bit (pdev, arg);
			break;
		case CLEAR_SMS_BIT:
			ret = bt_clear_sms_bit (pdev, arg);
			break;
		case BT_ENABLE:
			pdev->pbt_hal->pbt_hal_ops->bt_interrupt_enable_user();
			break;
		case BT_DISABLE:
			pdev->pbt_hal->pbt_hal_ops->bt_interrupt_disable_user();
			break;
		default:
			dbgprint ("invalid ioctl command in <%s> char device\n", BT_DEV_NAME);
			break;
	}
	return ret;
}

static int 
bt_request (struct bt_dev *pdev, char *pbuf)
{
	BTBuf_T *pBTBuffer = &pdev->pbt_hal->pbt_buf [pdev->ch_num];

	dbgprint ("bt_request ch num: %d, bt_buf addr : %p\n", pdev->ch_num, pBTBuffer);

	/* Initialize Sleep Structure for the First Time */
	if (pBTBuffer->FirstTime)
	{
		Bt_OsInitSleepStruct(&(pBTBuffer->BtReqWait));
		pBTBuffer->FirstTime = 0;
	}
	pBTBuffer->BtReqWaiting = 1;
	dbgprint ("sleeping in bt_request for ch num: %d\n", pdev->ch_num);
	Bt_OsSleepOnTimeout(&(pBTBuffer->BtReqWait),&(pBTBuffer->BtWakeup),0);
	pBTBuffer->BtWakeup       = 0;
	pBTBuffer->BtReqWaiting = 0;
	dbgprint ("out of sleep in bt_request for ch num: %d\n", pdev->ch_num);

	if (__copy_to_user( (void *)pbuf, (void *)pBTBuffer->pBTRcvPkt,  pBTBuffer->BTRcvPktLen ))
	{
		TCRIT ("bt_request(): copy to user failed\n");
		return -EFAULT;
	}
	return pBTBuffer->BTRcvPktLen;
}

static ssize_t
bt_read (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct bt_dev *pdev = (struct bt_dev*) file->private_data;
	dbgprint ("bt_read: ch num: %d, priv data addr : %p\n", pdev->ch_num, file->private_data);		
	return bt_request (pdev, buf);
}

static void 
send_bt_response (struct bt_dev *pdev)
{
	IPMICmdMsg_T *pBTCmdMsg = &pdev->pbt_hal->pipmi_cmd_msg [pdev->ch_num];	
	BTBuf_T *pBTBuffer = &pdev->pbt_hal->pbt_buf [pdev->ch_num];

	/* Update the send buffer and associated indexes */
	pBTBuffer->pBTSendPkt = pBTCmdMsg->Response;
	pBTBuffer->BTSendPktLen   = pBTCmdMsg->ResponseSize;

	/* Send the first byte */
	pdev->pbt_hal->pbt_hal_ops->write_bt_data_buf (pdev->ch_num, pBTBuffer->pBTSendPkt, pBTBuffer->BTSendPktLen);

}

static size_t 
bt_write_request (struct bt_dev *pdev, const char* buf, size_t count)
{
	IPMICmdMsg_T *pBTCmdMsg = &pdev->pbt_hal->pipmi_cmd_msg [pdev->ch_num];	

	/* Copy from user data area to kernel data area*/
	if (__copy_from_user((void *)(pBTCmdMsg->Response),(void *)buf,count))
	{
		TCRIT ("bt_write_request(): copy from user failed\n");
		return -EFAULT;
	}
	pBTCmdMsg->ResponseSize = count;

	/* Send the response to BT Channel */
	send_bt_response (pdev);

	return count;
}

static ssize_t
bt_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
	struct bt_dev *pdev = (struct bt_dev*) file->private_data;	
	return bt_write_request (pdev, buf, count);
}

static int 
process_bt_intr (int dev_id, unsigned char ch_num, int Error)
{
	struct bt_hal *pbt_hal;	
	BTBuf_T *pBTBuffer;

	dbgprint ("dev_id in process_bt_intr = %d\n", dev_id);
	pbt_hal = hw_intr (EDEV_TYPE_BT, dev_id);
	if (!pbt_hal)
	{
		TCRIT ("pbt_hal is NULL\n");
		return -ENXIO;
	}
	pBTBuffer = &pbt_hal->pbt_buf [ch_num];


	if (Error == 0)
	{
		pbt_hal->pbt_hal_ops->read_bt_data_buf(ch_num,pBTBuffer->pBTRcvPkt);
		if (pBTBuffer->pBTRcvPkt[0] == 0)
		{
			pBTBuffer->BTRcvPktLen = 0;	
		}
		else
		{
			pBTBuffer->BTRcvPktLen = pBTBuffer->pBTRcvPkt[0] + 1;
		}
	}
	else
	{
		pBTBuffer->BTRcvPktLen = 1;
		pBTBuffer->pBTRcvPkt[0] = 0xFF;
	}
	if (pBTBuffer->BtReqWaiting == 1)
	{
		/* Process BT Intr */
		pBTBuffer->BtWakeup = 1;
		Bt_OsWakeupOnTimeout(&(pBTBuffer->BtReqWait));
	}
	return 0;
}


static bt_core_funcs_t bt_core_funcs =
{
	.process_bt_intr = process_bt_intr,
};

static core_hal_t bt_core_hal = 
{
	.owner		           = THIS_MODULE,
	.name		           = "BT CORE",
	.dev_type              = EDEV_TYPE_BT,
	.register_hal_module   = register_bt_hal_module,
	.unregister_hal_module = unregister_bt_hal_module,
	.pcore_funcs           = (void *)&bt_core_funcs
};
	
static int __init bt_init (void)
{
	int ret = 0;
	printk (banner);

	if ((ret = register_chrdev_region (bt_devno, BT_MAX_CHANNELS, BT_DEV_NAME)) < 0)
	{
		TCRIT ("failed to register bt device <%s> (err: %d)\n", BT_DEV_NAME, ret);
		return ret;
	}

	bt_cdev = cdev_alloc ();
	if (!bt_cdev)
	{
		TCRIT ("%s: failed to allocate bt cdev structure\n", BT_DEV_NAME);
		unregister_chrdev_region (bt_devno, BT_MAX_CHANNELS);
		return -1;
	}

	cdev_init (bt_cdev, &bt_fops);
	bt_cdev->owner = THIS_MODULE;

	if ((ret = cdev_add (bt_cdev, bt_devno, BT_MAX_CHANNELS)) < 0)
	{
		TCRIT ("failed to add <%s> char device\n", BT_DEV_NAME);
		cdev_del (bt_cdev);
		unregister_chrdev_region (bt_devno, BT_MAX_CHANNELS);
		ret = -ENODEV;
		return ret;	
	}

	if ((ret = register_core_hal_module (&bt_core_hal)) < 0)
	{
		TCRIT ("failed to register the Core BT module\n");
		cdev_del (bt_cdev);
		unregister_chrdev_region (bt_devno, BT_MAX_CHANNELS);
		ret = -EINVAL;
		return ret;	
	}

	moduledir = proc_mkdir("bt",rootdir);
	my_sys  = AddSysctlTable("bt", &BT_table[0]);
	return 0;
}

module_init (bt_init);
module_exit (bt_exit);

