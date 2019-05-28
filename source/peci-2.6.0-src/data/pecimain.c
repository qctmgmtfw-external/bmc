/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */

/*
 * PECI Common Driver
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * This driver provides common layer, independent of the hardware, for the PECI driver.
 */
#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif
#include <linux/kernel.h>	
#include <linux/version.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/sched.h>
//#include <linux/ide.h>
#include <linux/slab.h>
#include <asm/delay.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <asm/uaccess.h>
#include "peci.h"
#include "driver_hal.h"
#include "dbgout.h"	
#include "helper.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
#include "../fs/proc/internal.h"
#endif


#define DRIVER_AUTHOR "American Megatrends Inc"
#define DRIVER_DESC "Pilot-II SOC PECI Common Driver"
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

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

struct peci_hal
{
	peci_hal_operations_t *ppeci_hal_ops;
};

struct peci_dev
{
	struct peci_hal *ppeci_hal;
	unsigned char ch_num;
};

#define PECI_LOG_CHANNELS 1
#ifdef CONFIG_SPX_FEATURE_ENABLE_PECI_LOG
static unsigned char log_enabled[PECI_LOG_CHANNELS];
#endif

/* Global Variables */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
DEFINE_SEMAPHORE (mutex);
#else
DECLARE_MUTEX (mutex);
#endif
TDBG_DECLARE_DBGVAR(peci);
static int core_hal_module_registered;

static struct cdev *peci_cdev;
static dev_t peci_devno = MKDEV(PECI_MAJOR, PECI_MINOR);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static char banner[] __initdata = KERN_INFO "PECI Common Driver, (c) 2009-2015 American Megatrends Inc.\n";
#else
static char banner[] __initdata = KERN_INFO "PECI Common Driver, (c) 2009 American Megatrends Inc.\n";
#endif


/*-----------------------------------------------*
 **         Prototype Declaration       **
 *-----------------------------------------------*/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static int  peci_init(void);
static void peci_exit(void);
#else
static int  __init peci_init(void);
static void __exit peci_exit(void);
#endif

static int peci_open(struct inode * inode, struct file * filp);
static int peci_close(struct inode * inode, struct file * filp);
#ifndef USE_UNLOCKED_IOCTL 
static int peci_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg);
#endif
static long peci_ioctlUnlocked(struct file *file,unsigned int cmd, unsigned long arg);
static int register_peci_hal_module(unsigned char num_instances, void *phal_ops, void **phw_data);
static int unregister_peci_hal_module(void *phw_data);

#ifdef CONFIG_SPX_FEATURE_ENABLE_PECI_LOG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static ssize_t peci_log_proc_read(struct file *file, char *buffer, size_t buffer_length, loff_t *ppos);
static ssize_t peci_log_proc_write(struct file *file, const char __user *buffer, size_t buffer_length, loff_t *ppos);
#endif
static void log_proc_open(unsigned channel);
static void log_proc_close(unsigned channel);
static void log_peci(unsigned channel, peci_cmd_t *pecicmd);
#else
#define log_proc_open(pdev)		do { } while (0)
#define log_proc_close(pdev)		do { } while (0)
#define log_peci(channel, pecicmd)	do { } while (0)
#endif // CONFIG_SPX_FEATURE_ENABLE_PECI_LOG


static core_hal_t peci_core_hal = {
	.owner		       = THIS_MODULE,
	.name		       = "PECI CORE",
	.dev_type              = EDEV_TYPE_PECI,
	.register_hal_module   = register_peci_hal_module,
	.unregister_hal_module = unregister_peci_hal_module,
	.pcore_funcs           = (void *)NULL
};

static struct file_operations peci_fops = 
{
	owner:		THIS_MODULE,
#ifdef CONFIG_SPX_FEATURE_ENABLE_PECI_LOG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	read:		peci_log_proc_read,
	write:		peci_log_proc_write,
#else	
	read:		NULL,
	write:		NULL,
#endif
#else	
	read:		NULL,
	write:		NULL,
#endif	
#ifdef USE_UNLOCKED_IOCTL 
	unlocked_ioctl : peci_ioctlUnlocked,
#else
	ioctl:		peci_ioctl,
#endif
	open:		peci_open,
	release:	peci_close,
};


static int 
peci_open(struct inode * inode, struct file * file)
{
	struct peci_dev	*pdev;
	hw_info_t peci_hw_info;
	int ret;
	unsigned char open_count;
	
	dbgprint("PECI CORE: device open \n");

	ret = hw_open (EDEV_TYPE_PECI, iminor(inode), &open_count, &peci_hw_info);
	if (ret)
		return -ENXIO;
	
	pdev = (struct peci_dev*) kmalloc (sizeof(struct peci_dev), GFP_KERNEL);
	if (!pdev)
	{
		hw_close (EDEV_TYPE_PECI, iminor(inode), &open_count);
		printk (KERN_ERR "%s: failed to allocate pdev structure for peci iminor: %d\n", PECI_DEV_NAME, iminor(inode));
		return -ENOMEM;
	}
	pdev->ppeci_hal = peci_hw_info.pdrv_data;
	pdev->ch_num = peci_hw_info.inst_num;

	file->private_data = pdev;		

	dbgprint ("%d, peci_open priv data addr : %p\n", iminor(inode), &file->private_data);	
	return nonseekable_open (inode, file);	
}

static int 
peci_close(struct inode * inode, struct file * file)
{
	int ret;
	unsigned char open_count;
	struct peci_dev *pdev= (struct peci_dev*) file->private_data;
	
	dbgprint("PECI CORE: device close \n");
	dbgprint ("%d, ch: %d peci_release priv data addr : %p\n", iminor(inode), pdev->ch_num, pdev);
	pdev->ppeci_hal = NULL;
	ret = hw_close (EDEV_TYPE_PECI, iminor(inode), &open_count);
	if(ret) { return -1; };
	kfree (pdev);
	return 0;
}

/* --------------- Core Functions of PECI Handling --------------------*/

int register_peci_hal_module(unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct peci_hal *ppeci_hal;

	ppeci_hal = (struct peci_hal*) kmalloc (sizeof(struct peci_hal), GFP_KERNEL);
	if (!ppeci_hal)
	{
		return -ENOMEM;
	}

	ppeci_hal->ppeci_hal_ops = (peci_hal_operations_t *) phal_ops;

	*phw_data = (void *)ppeci_hal;
	dbgprint ("private data: %p, %p\n", ppeci_hal, *phw_data);

	return 0;
}

int unregister_peci_hal_module(void *phw_data)
{
	struct peci_hal *ppeci_hal = (struct peci_hal *)phw_data;
	
	TDBG_FLAGGED(peci, 2,"Unregistering PILOT-II PECI Hardware\n");

	if(!ppeci_hal)
		return -ENOMEM;
	
	dbgprint ("unregister hal addr : %p\n", ppeci_hal);

	kfree (ppeci_hal);

	return 0;
}


/**
 * peci_process_cmd - to process the PECI commands
 * @pecicmd:	pointer to PECI command structure
 * @ppeci_hal:	pointer to the PECI hardware hal structure
 * This function is used by the PECI core for processing all peci commands.
 */
static int 
peci_process_cmd(peci_cmd_t *pecicmd, struct peci_dev *pdev)
{
	int retval =0;
	
	struct peci_hal *ppeci_hal=(struct peci_hal*)pdev->ppeci_hal;

	if (!ppeci_hal)
		return -ENXIO;

	if (pecicmd->write_len > MAX_WRITE_LENGTH) 
	{
		dbgprint("ERROR: Invalid PECI Writelen = %d\n",pecicmd->write_len);
		return -EINVAL;
	}

	if (pecicmd->read_len > MAX_READ_LENGTH) 
	{
		dbgprint("ERROR: Invalid PECI Readlen = %d\n",pecicmd->read_len);
		return -EINVAL;
	}
	dbgprint("\nreadlen:%d,writelen:%d\n",pecicmd->read_len,pecicmd->write_len);

	//Initiating the transfer
	if (ppeci_hal->ppeci_hal_ops->send_peci_cmd)
	{
		retval = ppeci_hal->ppeci_hal_ops->send_peci_cmd (pecicmd);
		if (retval >= 0)
			log_peci(pdev->ch_num, pecicmd);
	}   
	
	return retval;
}

static long 
peci_ioctlUnlocked(struct file *file,
		     unsigned int cmd, unsigned long arg)
{
	peci_cmd_t pecicmd;
	struct peci_dev *pdev= (struct peci_dev*) file->private_data;
	int retval = 0;
	
	dbgprint("PECI: device ioctl \n");
	TDBG_FLAGGED(peci, 3,"PILOT-II PECI Device IOCTL\n");

	if (arg == 0) 
	{
		dbgprint("ERROR: PECI Argument is NULL \n");
		return -EINVAL;
	}
  	down (&mutex);

	if (__copy_from_user((void *)&pecicmd ,(void *)arg,sizeof(peci_cmd_t)))
 	{ 
                dbgprint(" peci_ioctl:Error copying data from user \n"); 
    		retval = -EFAULT; 
		goto ret_ioctl;
        } 

	if (cmd != PECI_ISSUE_CMD)
	{
		dbgprint("ERROR: Invalid PECI IOCTL Cmd = %d\n",cmd);
 		retval = -EINVAL; 
		goto ret_ioctl;		
	}

	if((retval = peci_process_cmd(&pecicmd,pdev)) < 0)
	{
		dbgprint("\npeci process cmd failed (err %d)\n",retval);
		goto ret_ioctl;
	}
	
	if (__copy_to_user( (void *)arg, (void *)&pecicmd,sizeof(peci_cmd_t)))
 	{ 
                dbgprint(" peci_ioctl:Error copying data to user \n"); 
                retval = -EFAULT; 
		goto ret_ioctl;
        } 

ret_ioctl:
  up (&mutex);
	return retval;
}
#ifndef USE_UNLOCKED_IOCTL 
static int
peci_ioctl(struct inode *inode, struct file *file,
             unsigned int cmd, unsigned long arg)
{
	return(peci_ioctlUnlocked(file, cmd, arg));
}
#endif
#ifdef CONFIG_SPX_FEATURE_ENABLE_PECI_LOG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static ssize_t peci_log_proc_read(struct file *file, char *buffer, size_t buffer_length, loff_t *ppos)
{
	void *data = PDE_DATA(file_inode(file));
#else
static int peci_log_proc_read(char *buffer, char **buffer_location,
	off_t offset, int buffer_length, int *eof, void *data)
{
	if (offset > 0)
		return 0;
#endif

	if (buffer_length < 2)
		return -1;

	if (!data)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -1;
	}

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
	*eof = 1;
	#endif
	buffer[0] = *(unsigned char*)data ? '1' : '0';
	buffer[1] = '\n';
	return 2;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static ssize_t peci_log_proc_write(struct file *file, const char __user *buffer, size_t buffer_length, loff_t *ppos)
{
	void *data = PDE_DATA(file_inode(file));
#else
static int peci_log_proc_write(struct file *file, const char __user *buffer,
	unsigned long buffer_length, void *data)
{
#endif
	if (!data)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -1;
	}

	if (buffer_length > 0)
	{
		if ('1' == buffer[0])
			*(unsigned char*)data = 1;
		else if ('0' == buffer[0])
			*(unsigned char*)data = 0;
	}
	return buffer_length;
}

#define PECI_LOG_PROCFS_NAME "log-peci"

static void log_proc_open(unsigned channel)
{
	struct proc_dir_entry *pde;
	char procfs_name[32];

	snprintf(procfs_name, sizeof(procfs_name), PECI_LOG_PROCFS_NAME"%d",
		channel);

	// create entry in /proc root
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	pde = proc_create(procfs_name, 0644, NULL, &peci_fops);
	#else
	pde = create_proc_entry(procfs_name, 0644, NULL);
	#endif
	if (pde == NULL)
	{
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",
			procfs_name);
		return;
	}

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
	// specify read/write callbacks
	pde->read_proc	= peci_log_proc_read;
	pde->write_proc	= peci_log_proc_write;

	// specify file attributes for file
	pde->owner	= THIS_MODULE;
	pde->uid 	= 0;
	pde->gid 	= 0;
	#endif
	pde->mode 	= S_IFREG | S_IRUGO;	

	pde->data = &log_enabled[channel];
}

static void log_proc_close(unsigned channel)
{
	char procfs_name[32];
	snprintf(procfs_name, sizeof(procfs_name), PECI_LOG_PROCFS_NAME"%d",
		channel);
	remove_proc_entry(procfs_name, NULL);
}

static void log_peci(unsigned channel, peci_cmd_t *cmd)
{
	if (log_enabled[channel])
	{
		unsigned i;
		
		printk(KERN_DEBUG "PECI%d: %02X %02X %02X",
			channel, cmd->target, cmd->write_len, cmd->read_len);

		if (cmd->write_len > 0)
		{
			for (i = 0; i < cmd->write_len; ++i)
				printk(" %02X", cmd->write_buffer[i]);
			
			if((cmd->write_buffer[0] == 0xb5) || 
			   (cmd->write_buffer[0] == 0x65) ||  
			   (cmd->write_buffer[0] == 0xe5))  
				printk(" %02X", cmd->AWFCS);
				
		}
		printk(" FCS");
		if (cmd->read_len > 0)
		{
			for (i = 0; i < cmd->read_len; ++i)
				printk(" %02X", cmd->read_buffer[i]);
		}

		printk(" FCS \n");
	}
}
#endif // CONFIG_SPX_FEATURE_ENABLE_PECI_LOG
/* ----- Driver registration ---------------------------------------------- */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static void 
peci_exit(void)
#else
static void __exit
peci_exit(void)
#endif
{
	int i;
	TDBG_FLAGGED(peci, 1,"Unloading PILOT-II PECI CORE Layer\n");
	
	for (i = 0; i < PECI_LOG_CHANNELS; ++i)
			log_proc_close(i);

	unregister_chrdev_region (peci_devno, PECI_MAX_DEVICES);

	if (NULL != peci_cdev)
	{
		dbgprint ("peci char device del\n");
		cdev_del (peci_cdev);
	}
	
	if(core_hal_module_registered)
	{
		unregister_core_hal_module (EDEV_TYPE_PECI);
		core_hal_module_registered = 0;
	}

	return;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static int 
peci_init(void)
#else
static int __init
peci_init(void)
#endif
{
	int ret=0,err,i;
	core_hal_module_registered = 0;
	printk (banner);

		TDBG_FLAGGED(peci, 1,"loading PILOT-II PECI CORE Layer\n");

 	/* ----  Register the character device ------------------- */
	if ((ret = register_chrdev_region (peci_devno, PECI_MAX_DEVICES, PECI_DEV_NAME)) < 0)
	{
		printk ("failed to register device peci device <%s> (err: %d)\n", PECI_DEV_NAME, ret);
		return ret;
	}
	
	peci_cdev = cdev_alloc();
	if (!peci_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate peci cdev structure\n", PECI_DEV_NAME);
		ret = -1;
		goto clean;
	}

	cdev_init (peci_cdev, &peci_fops);

	if ((ret = cdev_add (peci_cdev, peci_devno, PECI_MAX_DEVICES)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", PECI_DEV_NAME);
		ret=-ENODEV;
		goto clean;
	}
	
	if ((err = register_core_hal_module (&peci_core_hal)) < 0)
	{
		printk (KERN_ERR "ERROR: Failed to register core hal module (err %d)\n", err);
		ret=-EINVAL;
		goto clean;
	}
	core_hal_module_registered = 1;
	
	for (i = 0; i < PECI_LOG_CHANNELS; ++i)
		log_proc_open(i);
	
	return ret;

clean:

	peci_exit();
	
	return ret;
}


module_init (peci_init);
module_exit (peci_exit);

int peci_core_loaded =1;
EXPORT_SYMBOL(peci_core_loaded);
