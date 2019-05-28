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
 * KCS Common Driver
 *
 * Copyright (C) 2009-2015 American Megatrends Inc.
 *
 * Author : Jothiram Selvam <jothirams@ami.com>
 *          Vinay Tandon <vinayt@ami.com>
 * This driver provides common layer, independent of the hardware, for the KCS driver.
 */

#include <linux/version.h>

#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include "kcs.h"
#include "kcsifc.h"
#include "driver_hal.h"
#include "proc.h"
#include "dbgout.h"
#include "helper.h"
#include "kcs_ioctl.h"

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
struct proc_info *kcsproc;
#endif

#define DRIVER_AUTHOR "Jothiram Selvam <jothirams@ami.com>"
#define DRIVER_DESC "KCS Common Driver"
#define DRIVER_LICENSE "GPL"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 
/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE ( DRIVER_LICENSE );

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#define KCS_MAJOR           42
#define KCS_MINOR	    0
#define KCS_MAX_CHANNELS    255
#define KCS_DEV_NAME        "kcs"


#define IS_IBF_SET(STATUS)     (0 != ((STATUS) & 0x02))
#define IS_WRITE_TO_CMD_REG(STATUS)    (0 != ((STATUS) & 0x08))
#define CHK_STATE_IDLE(STATUS)     (0 == ((STATUS) & 0xC0))

#define MAX_SEQ_NO		250		/*< Maximum sequence number and it will repeat again from 0 >*/

unsigned int m_kcs_ch_count = 0;
int m_dev_id = 1;

struct kcs_hal
{
	KCSBuf_T *pkcs_buf;
	IPMICmdMsg_T* pipmi_cmd_msg;
	kcs_hal_operations_t *pkcs_hal_ops;
};

struct kcs_dev
{
	struct kcs_hal *pkcs_hal;
	unsigned char ch_num;
#ifdef CONFIG_SPX_FEATURE_ENABLE_KCS_LOG
	unsigned char log_enabled;
#endif
};

TDBG_DECLARE_DBGVAR(kcs);
/* Sysctl Table */
static struct ctl_table KCS_table [] =
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
        {"DebugLevel",&(TDBG_FORM_VAR_NAME(kcs)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else 
    #if (LINUX_VERSION_CODE >  KERNEL_VERSION(2,6,15)) 
        {CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(kcs)),sizeof(int),0644,NULL,NULL,&proc_dointvec},
    #else
        {1,"DebugLevel",&(TDBG_FORM_VAR_NAME(kcs)),sizeof(int),0644,NULL,&proc_dointvec},
#endif
#endif
        {0}
};

/* Proc and Sysctl entries */
static struct proc_dir_entry *moduledir = NULL;
static struct ctl_table_header *my_sys  = NULL;

#define KCS_LOG_CHANNELS 3
#ifdef CONFIG_SPX_FEATURE_ENABLE_KCS_LOG
static unsigned char log_enabled[KCS_LOG_CHANNELS];
#endif

/*-----------------------------------------------*
 **         Prototype Declaration       **
 *-----------------------------------------------*/

static int kcs_init (void);
static void kcs_exit (void);

static int kcs_open (struct inode *inode, struct file *filp);
static int kcs_release (struct inode *inode, struct file *filp);
#ifdef USE_UNLOCKED_IOCTL
static long kcs_ioctlUnlocked (struct file *file, unsigned int cmd, unsigned long arg);
#else
static int kcs_ioctl (struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif

static ssize_t kcs_read (struct file *filp, char __user *buf, size_t count, loff_t *offset);
static ssize_t kcs_write (struct file *filp, const char __user *buf, size_t count, loff_t *offset);

static int kcs_interrupt_enable(struct kcs_dev *pdev,unsigned long arg);
static int kcs_interrupt_disable(struct kcs_dev *pdev,unsigned long arg);
static int kcs_set_user_interrupt_enable(struct kcs_dev *pdev,unsigned long arg);
static int kcs_set_user_interrupt_disable(struct kcs_dev *pdev,unsigned long arg);


static int kcs_clear_sms_bit (struct kcs_dev *pdev, unsigned long arg);
static int kcs_set_sms_bit (struct kcs_dev *pdev, unsigned long arg);
static int kcs_set_obf_bit (struct kcs_dev *pdev, unsigned long arg);
static int kcs_hw_unit_test (struct kcs_dev *pdev, unsigned long arg);
static int read_kcs_data (struct kcs_dev *pdev, unsigned long arg);
static int write_kcs_data (struct kcs_dev *pdev, unsigned long arg);
static int Write_kcs_status_data (struct kcs_dev *pdev, unsigned long arg);

static int kcs_request (struct kcs_dev *pdev, char *pbuf);
static size_t kcs_write_request (struct kcs_dev *pdev, const char* buf, size_t count);
static void send_kcs_response (struct kcs_dev *pdev);
static void kcs_recv_byte (struct kcs_hal *pkcs_hal, unsigned char ch_num);

static void set_kcs_state (struct kcs_hal *pkcs_hal, unsigned char ch_num, u8 state_value);


extern unsigned int m_kcs_hw_test_enable;

extern void Kcs_OsInitSleepStruct(Kcs_OsSleepStruct *Sleep);
extern void Kcs_OsWakeupOnTimeout(Kcs_OsSleepStruct *Sleep);
extern long Kcs_OsSleepOnTimeout(Kcs_OsSleepStruct *Sleep,u8 *Var,long msecs);
extern void SleepTimeOut(unsigned long SleepPtr);
void read_kcs_status(u8 ch_num, u8* status_value);
void write_kcs_status(u8 ch_num, u8 status_value);

static struct file_operations kcs_fops = {
        owner:      THIS_MODULE,
        read:       kcs_read,
        write:      kcs_write,
#ifdef USE_UNLOCKED_IOCTL 
        unlocked_ioctl:	kcs_ioctlUnlocked, 
#else 
        ioctl:     	kcs_ioctl,
#endif
        open:       kcs_open,
        release:    kcs_release,
};

struct kcs_hal kcs_hal_list;

static struct cdev *kcs_cdev;
static dev_t kcs_devno = MKDEV(KCS_MAJOR, KCS_MINOR);
static char banner[] __initdata = KERN_INFO "KCS Common Driver, (c) 2009-2015 American Megatrends Inc.\n";	

#ifdef CONFIG_SPX_FEATURE_ENABLE_KCS_LOG
static void log_proc_open(unsigned channel);
static void log_proc_close(unsigned channel);
static void log_kcs(unsigned channel, unsigned size, u8 * data);
#else
#define log_proc_open(ch)	do { } while (0)
#define log_proc_close(ch)	do { } while (0)
#define log_kcs(ch, size, data)	do { } while (0)
#endif // CONFIG_SPX_FEATURE_ENABLE_KCS_LOG

/***********************************************************************************************/

static void __exit kcs_exit(void)
{
	int i;
	
	for (i = 0; i < KCS_LOG_CHANNELS; ++i)
		log_proc_close(i);

	unregister_core_hal_module (EDEV_TYPE_KCS);
	unregister_chrdev_region (kcs_devno, KCS_MAX_CHANNELS);

	if (NULL != kcs_cdev)
	{
		dbgprint ("kcs char device del\n");
		cdev_del (kcs_cdev);
	}
	RemoveSysctlTable(my_sys);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
        RemoveProcEntry(kcsproc);
#else
        RemoveProcEntry(moduledir,"KCS");
#endif
        remove_proc_entry("kcs",rootdir);

	printk("KCS Common module unloaded successfully\n");
	return;
}

int
register_kcs_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	int i;
	struct kcs_hal *pkcs_hal;

	pkcs_hal = (struct kcs_hal*) kmalloc (sizeof(struct kcs_hal), GFP_KERNEL);
	if (!pkcs_hal)
	{
		return -ENOMEM;
	}

	pkcs_hal->pkcs_buf = (KCSBuf_T*) kmalloc (num_instances * sizeof(KCSBuf_T), GFP_KERNEL);
	if (!pkcs_hal->pkcs_buf)
	{
		kfree (pkcs_hal);
		return -ENOMEM;
	}

	pkcs_hal->pipmi_cmd_msg = (IPMICmdMsg_T*) vmalloc (num_instances * sizeof(IPMICmdMsg_T));
	if (!pkcs_hal->pipmi_cmd_msg)
	{
		kfree (pkcs_hal->pkcs_buf);
		kfree (pkcs_hal);
		return -ENOMEM;
	}

	for (i = 0; i < num_instances; ++i)
	{
		dbgprint ("%d, kcs_buf addr : %p\n", i, &pkcs_hal->pkcs_buf[i]);						
		pkcs_hal->pkcs_buf[i].pKCSRcvPkt = pkcs_hal->pipmi_cmd_msg[i].Request;
		pkcs_hal->pkcs_buf[i].KcsWakeup = 0;
		pkcs_hal->pkcs_buf[i].TxReady = 0;
		pkcs_hal->pkcs_buf[i].KcsIFActive = 0;
		pkcs_hal->pkcs_buf[i].KcsWtIFActive = 0;
		pkcs_hal->pkcs_buf[i].FirstTime = 1;
		pkcs_hal->pkcs_buf[i].KcsResFirstTime = 1;
		pkcs_hal->pkcs_buf[i].KcsINuse = 0;
		pkcs_hal->pkcs_buf[i].PrevCmdAborted = 0;
		pkcs_hal->pkcs_buf[i].KCSSeqNo = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
		pkcs_hal->pkcs_buf[i].kcs_lock = __SPIN_LOCK_UNLOCKED(pkcs_hal->pkcs_buf[i].kcs_lock);
#else
		pkcs_hal->pkcs_buf[i].kcs_lock = SPIN_LOCK_UNLOCKED;
#endif
	}

	pkcs_hal->pkcs_hal_ops = (kcs_hal_operations_t *) phal_ops;

	*phw_data = (void *)pkcs_hal;
	dbgprint ("private data: %p, %p\n", pkcs_hal, *phw_data);

	return 0;
}

int
unregister_kcs_hal_module (void *phw_data)
{
	struct kcs_hal *pkcs_hal = (struct kcs_hal *)phw_data;

	dbgprint ("unregister hal addr : %p\n", pkcs_hal);

	vfree (pkcs_hal->pipmi_cmd_msg);
	kfree (pkcs_hal->pkcs_buf);
	kfree (pkcs_hal);

    return 0;
}

static int
kcs_open(struct inode *inode, struct file *file)
{
	struct kcs_hal *pkcs_hal;
	unsigned int minor = iminor(inode);
	struct kcs_dev* pdev;
	KCSBuf_T* pKCSBuffer;
	hw_info_t kcs_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_KCS, minor, &open_count, &kcs_hw_info);
	if (ret)
		return -ENXIO;

	pkcs_hal = kcs_hw_info.pdrv_data;

	pdev = (struct kcs_dev*) kmalloc (sizeof(struct kcs_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_KCS, minor, &open_count);	
		printk (KERN_ERR "%s: failed to allocate kcs private dev structure for kcs iminor: %d\n", KCS_DEV_NAME, minor);
		return -ENOMEM;
	}

	pKCSBuffer = &pkcs_hal->pkcs_buf [kcs_hw_info.inst_num];
	dbgprint ("%d, kcs_open hal addr : %p\n", minor, pkcs_hal);
	dbgprint ("%d, kcs_open bufffer addr : %p\n", minor, pKCSBuffer);	
	
	pdev->pkcs_hal = pkcs_hal;
	pdev->ch_num = kcs_hw_info.inst_num;
	dbgprint ("%d, kcs_open ch num : %d\n", minor, pdev->ch_num);
	file->private_data = pdev;		
	dbgprint ("%d, kcs_open priv data addr : %p\n", minor, &file->private_data);	
	
	if (open_count == 1) // On open descriptors, the initialization would have been done in the first open. Hence skip initialization
	{
		pKCSBuffer->KCSRcvPktIx = 0;
		pKCSBuffer->KCSSendPktIx = 0;
		pKCSBuffer->KCSSendPktLen = 0;
		pKCSBuffer->KCSPhase = KCS_PHASE_IDLE;
		pKCSBuffer->KcsINuse = 1;
	}

	return nonseekable_open (inode, file);	
}

static int 
kcs_release (struct inode *inode, struct file *file)
{
	int ret;
	struct kcs_dev *pdev = (struct kcs_dev*) file->private_data;
	KCSBuf_T *pKCSBuffer = &pdev->pkcs_hal->pkcs_buf [pdev->ch_num];
	unsigned char open_count;

	ret = hw_close (EDEV_TYPE_KCS, iminor(inode), &open_count);
	if (ret) { return -1; }

	if (open_count == 0) // Cleanup should be done only in the last close
		pKCSBuffer->KcsINuse = 0;

	dbgprint ("%d, ch: %d kcs_release priv data addr : %p\n", iminor(inode), pdev->ch_num, pdev);
	dbgprint ("%d, kcs_release kcs_buf addr : %p\n", iminor(inode), pKCSBuffer);	
	pdev->pkcs_hal = NULL;
	kfree (pdev);
	return 0;
}

static long 
kcs_ioctlUnlocked (struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct kcs_dev *pdev = (struct kcs_dev*) file->private_data;

	switch(cmd)
	{
		case SET_SMS_BIT:
			ret = kcs_set_sms_bit (pdev, arg);
			break;
		case CLEAR_SMS_BIT:
			ret = kcs_clear_sms_bit (pdev, arg);
			break;
		case ENABLE_KCS_INTERRUPT:
			if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
			{
                ret = kcs_interrupt_enable(pdev,arg);
			}
			else
				printk("Invalid IOCTL command\n");
			break;
		case DISABLE_KCS_INTERRUPT:
			if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
			{
                ret = kcs_interrupt_disable(pdev,arg);
			}
			else
				printk("Invalid IOCTL command\n");
			break;
		case START_HW_UNIT_TEST:
			if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
			{
				ret = kcs_hw_unit_test ( pdev, arg );
			}
			else 
				printk("Invalid IOCTL command\n");
			break;
		case READ_KCS_DATA:
			if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
			{		
				ret = read_kcs_data ( pdev, arg );
			}
			break;
		case WRITE_KCS_DATA:
			if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
			{		
				ret = write_kcs_data ( pdev, arg );
			}
			break;		
		case SET_OBF_BIT:
			ret = kcs_set_obf_bit (pdev, arg);	
			break;
		case KCS_ENABLE:
            ret = kcs_set_user_interrupt_enable(pdev,arg);
			break;
		case KCS_DISABLE:
            ret = kcs_set_user_interrupt_disable(pdev,arg);
			break;
		case SET_STATUS_DATA:	
			ret=Write_kcs_status_data(pdev, arg);
			break;			
		default:
			dbgprint ("invalid ioctl command in <%s> char device\n", KCS_DEV_NAME);
			break;
	}
	return ret;
}

#ifndef USE_UNLOCKED_IOCTL  
static int
kcs_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	return(kcs_ioctlUnlocked (file, cmd, arg));
}
#endif

int
kcs_interrupt_enable(struct kcs_dev *pdev,unsigned long arg)
{
	  kcs_data_t kcs_data;

    if(copy_from_user(&kcs_data,(void*) arg,sizeof(kcs_data_t)))
        return -EFAULT;

     pdev->pkcs_hal->pkcs_hal_ops->enable_kcs_interrupt(kcs_data.kcsifcnum);

     return 0;
}

int
kcs_interrupt_disable(struct kcs_dev *pdev,unsigned long arg)
{
	  kcs_data_t kcs_data;

    if(copy_from_user(&kcs_data,(void*) arg,sizeof(kcs_data_t)))
        return -EFAULT;

     pdev->pkcs_hal->pkcs_hal_ops->disable_kcs_interrupt(kcs_data.kcsifcnum);

     return 0;
}


int 
kcs_set_user_interrupt_enable(struct kcs_dev *pdev,unsigned long arg)
{
	  kcs_data_t kcs_data;

    if(copy_from_user (&kcs_data, (void*) arg, sizeof(kcs_data_t)))
         return -EFAULT;

    pdev->pkcs_hal->pkcs_hal_ops->kcs_interrupt_enable_user(kcs_data.kcsifcnum);

    return 0;
}

int 
kcs_set_user_interrupt_disable(struct kcs_dev *pdev,unsigned long arg)
{
	 kcs_data_t kcs_data;

    if(copy_from_user (&kcs_data, (void*) arg, sizeof(kcs_data_t)))
         return -EFAULT;

    pdev->pkcs_hal->pkcs_hal_ops->kcs_interrupt_disable_user(kcs_data.kcsifcnum);

    return 0;
}


int 
kcs_set_sms_bit (struct kcs_dev *pdev, unsigned long arg)
{
	u8 status = 0;
	kcs_data_t kcs_data;

	if (copy_from_user (&kcs_data, (void*)arg, sizeof(kcs_data_t)))
		return -EFAULT;

	pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kcs_data.kcsifcnum, &status);

	if (IS_IBF_SET(status))
		kcs_recv_byte (pdev->pkcs_hal, 1);
	status = status | 0x04;
	pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status (kcs_data.kcsifcnum, status);

	return 0;
}

int 
kcs_clear_sms_bit (struct kcs_dev *pdev, unsigned long arg)
{
	u8 status = 0;
	kcs_data_t kcs_data;

	if ( copy_from_user(&kcs_data, (void*)arg, sizeof(kcs_data_t)) )
		return -EFAULT;

	pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kcs_data.kcsifcnum, &status);

	if (IS_IBF_SET(status))
		kcs_recv_byte (pdev->pkcs_hal, 1);
	status = status & ~0x04;
	pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status (kcs_data.kcsifcnum, status);
	
	return 0;
}
int 
kcs_set_obf_bit (struct kcs_dev *pdev, unsigned long arg)
{
	u8 status = 0;
	kcs_data_t kcs_data;

	if ( copy_from_user(&kcs_data, (void*)arg, sizeof(kcs_data_t)) )
		return -EFAULT;
	
	pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kcs_data.kcsifcnum, &status);
	
	if (CHK_STATE_IDLE(status))
	{
		pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out(kcs_data.kcsifcnum, status);
	}
	
	return 0;
}

int
kcs_hw_unit_test (struct kcs_dev *pdev, unsigned long arg)
{
	u8 data_packet;
	u8 status_reg;
	int i = 0;
	unsigned int channel_num;

	channel_num = (unsigned int) arg; 
    
	pdev->pkcs_hal->pkcs_hal_ops->disable_kcs_interrupt(channel_num);
	pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);

	for (i=0; i<10; i++)
	{
		printk("\tTest Run %d\n", i);
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);
		while (0 == (status_reg & 0x02))
		{
			schedule_timeout_interruptible ( 100 );
			pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);
		}
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_data_in (channel_num, &data_packet);
		printk("\t\tHost Data = %d\n", data_packet);
		pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (channel_num, data_packet);

		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);
		if ( 0 != (status_reg & 0x02))
			pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status(channel_num, (status_reg | (0x02)));
		schedule_timeout_interruptible ( 100 );
	}
    
    pdev->pkcs_hal->pkcs_hal_ops->enable_kcs_interrupt(channel_num);

	return 0;
}


int
read_kcs_data (struct kcs_dev *pdev, unsigned long arg)
{
	unsigned char status_reg;
	struct kcs_data_t kdata;
	int i = 0;

	if  (copy_from_user (&kdata, (void*) arg, sizeof(struct kcs_data_t)))
		return -EFAULT;
	for (i=0; i < kdata.num_bytes; i++)
	{
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kdata.channel_num, &status_reg);
		while ( 0 == (status_reg & 0x02) )
		{
			schedule_timeout_interruptible ( 100 );
			pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kdata.channel_num, &status_reg);
		}
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_data_in (kdata.channel_num, &(kdata.buffer[i]));
		
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kdata.channel_num, &status_reg);
		if (status_reg & 0x02)
			pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status (kdata.channel_num, status_reg | 0x02 );
			
	}
	if  (copy_to_user ((void*) arg, &kdata, sizeof(struct kcs_data_t)))
		return -EFAULT;
	return 0;
}

int
write_kcs_data (struct kcs_dev *pdev, unsigned long arg)
{
	unsigned char status_reg;
	struct kcs_data_t kdata;
	int i = 0;

	if  (copy_from_user (&kdata, (void*) arg, sizeof(struct kcs_data_t)))
		return -EFAULT;
	for (i=0; i < kdata.num_bytes; i++)
	{
		pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (kdata.channel_num, kdata.buffer[i]);
		pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kdata.channel_num, &status_reg);
		while ( 0 == (status_reg & 0x01) )
		{
			schedule_timeout_interruptible ( 100 );
			pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kdata.channel_num, &status_reg);
		}
	}
	return 0;
}

int
Write_kcs_status_data (struct kcs_dev *pdev, unsigned long arg)
{	
	u8  status_reg = 0;
	struct kcs_status_data_t kstatusdata;

	if  (copy_from_user (&kstatusdata, (void*) arg, sizeof(struct kcs_status_data_t)))
		return -EFAULT;
	pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status (kstatusdata.channel_num, &status_reg);
	status_reg = status_reg & 0x3F;
	kstatusdata.status = kstatusdata.status << 6;
	status_reg = status_reg | kstatusdata.status;
	pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status (kstatusdata.channel_num, status_reg);
	
	return 0;
}
/*int
snoop_unit_test (struct kcs_dev *pdev, unsigned long arg)
{
        u8 data_packet;
        u8 status_reg;
        unsigned int snoop_data_count = 0;
        unsigned char snoop_host_data[512];
        unsigned char snoop_bmc_data[512];
        u8 channel_num = 1;
        int i = 0;
        int firstval = 0;
        int t = 0;
        unsigned int eq = 1;

        while ( (firstval == 0) || (i < snoop_data_count) )
        {
		t = 0;
                pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);
                while ( 0 == (status_reg & 0x02) )
                {
                        pdev->pkcs_hal->pkcs_hal_ops->read_kcs_status(channel_num, &status_reg);
                }

                pdev->pkcs_hal->pkcs_hal_ops->read_kcs_data_in (channel_num, &data_packet);
		if ( 0 != (status_reg & 0x02))
	                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status(channel_num, status_reg | 0x02);
                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (channel_num, data_packet);//dummy byte
		if ( 0 == (status_reg & 0x01))
	                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status(channel_num, status_reg | 0x01);
                if (firstval == 0)
                {
                        snoop_data_count = data_packet;
                        firstval = 1;
                }
                else
                {
                        snoop_host_data[i++] = data_packet;
                }
        }

        if ( copy_from_user(snoop_bmc_data, (void*)arg, 512) )
                return -EFAULT;

        for (i=0; i<snoop_data_count; i++)
        {
                if (snoop_bmc_data[i] != snoop_host_data[i])
                {
                        //printk("snoop_bmc_data[%d] = %d, snoop_host_data[%d] = %d\n", i, snoop_bmc_data[i], i, snoop_host_data[i]);
                        eq = 0;
                        break;
                }
        }

        if (eq)
        {
                status_reg &= 0x3f;
                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status(channel_num, status_reg);
                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (channel_num, data_packet);//dummy byte
        }
        else
        {
                status_reg |= 0xc0;
                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_status(channel_num, status_reg);
                pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (channel_num, data_packet);//dummy byte
        }
        return snoop_data_count;
}*/


static ssize_t
kcs_read (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct kcs_dev *pdev = (struct kcs_dev*) file->private_data;
	dbgprint ("kcs_read: ch num: %d, priv data addr : %p\n", pdev->ch_num, file->private_data);		
	return kcs_request (pdev, buf);
}

int 
kcs_request (struct kcs_dev *pdev, char *pbuf)
{
	KCSBuf_T *pKCSBuffer = &pdev->pkcs_hal->pkcs_buf [pdev->ch_num];
	unsigned long flags;

	dbgprint ("kcs_request ch num: %d, kcs_buf addr : %p\n", pdev->ch_num, pKCSBuffer);

	/* Initialize Sleep Structure for the First Time */
	if (pKCSBuffer->FirstTime)
	{
		Kcs_OsInitSleepStruct(&(pKCSBuffer->KcsReqWait));
		pKCSBuffer->FirstTime = 0;
	}

	dbgprint ("sleeping in kcs_request for ch num: %d\n", pdev->ch_num);
	spin_lock_irqsave(&pKCSBuffer->kcs_lock, flags);
	pKCSBuffer->KcsIFActive     = 1;
	spin_unlock_irqrestore(&pKCSBuffer->kcs_lock, flags);
	
	Kcs_OsSleepOnTimeout(&(pKCSBuffer->KcsReqWait),&(pKCSBuffer->KcsWakeup),0);
	
	spin_lock_irqsave(&pKCSBuffer->kcs_lock, flags);
	pKCSBuffer->KcsWakeup       = 0;
	pKCSBuffer->KcsIFActive     = 0;
	spin_unlock_irqrestore(&pKCSBuffer->kcs_lock, flags);

	dbgprint ("out of sleep in kcs_request for ch num: %d\n", pdev->ch_num);

	if (__copy_to_user( (void *)pbuf, (void *)pKCSBuffer->pKCSRcvPkt,  pKCSBuffer->KCSRcvPktIx ))
		return -EFAULT;
	log_kcs(pdev->ch_num, pKCSBuffer->KCSRcvPktIx, pKCSBuffer->pKCSRcvPkt);
	return pKCSBuffer->KCSRcvPktIx;
}

static ssize_t
kcs_write (struct file *file, const char *buf, size_t count, loff_t *offset)
{
	struct kcs_dev *pdev = (struct kcs_dev*) file->private_data;	
	return kcs_write_request (pdev, buf, count);
}

size_t 
kcs_write_request (struct kcs_dev *pdev, const char* buf, size_t count)
{
	IPMICmdMsg_T *pKCSCmdMsg = &pdev->pkcs_hal->pipmi_cmd_msg [pdev->ch_num];	
	KCSBuf_T *pKCSBuffer = &pdev->pkcs_hal->pkcs_buf [pdev->ch_num];

	/* Copy from user data area to kernel data area*/
	if (__copy_from_user((void *)(pKCSCmdMsg->Response),(void *)buf,count))
		return -EFAULT;

	pKCSCmdMsg->SessionID = pKCSCmdMsg->Response[0]; 							/*! First Byte is SeqNo */
	memmove (&pKCSCmdMsg->Response[0], &pKCSCmdMsg->Response[1], count - 1);	/*! Replace the correct bytes to send */
	pKCSCmdMsg->ResponseSize = count - 1;

	/* Send the response to KCS Channel */
	send_kcs_response (pdev);

	/* Initialize Sleep Structure for the First Time */
	if (pKCSBuffer->KcsResFirstTime)
	{
		Kcs_OsInitSleepStruct(&(pKCSBuffer->KcsResWait));
		pKCSBuffer->KcsResFirstTime = 0;
	}

	pKCSBuffer->KcsWtIFActive = 1;
	pKCSBuffer->TxReady       = 0;
	pKCSBuffer->KcsWtIFActive = 0;
	return count;
}

/**
 * @brief Prepare KCS buffer and send it on given channel.
 * @param ChannelNum - KCS channel number.
 * @param MsgPkt     - Response message packet.
 **/
void 
send_kcs_response (struct kcs_dev *pdev)
{
	IPMICmdMsg_T *pKCSCmdMsg = &pdev->pkcs_hal->pipmi_cmd_msg [pdev->ch_num];	
	KCSBuf_T *pKCSBuffer = &pdev->pkcs_hal->pkcs_buf [pdev->ch_num];
	
    /* If we are responding to an aborted request skip sending data */ 
	if (pKCSBuffer->PrevCmdAborted)
	{ 
		pKCSBuffer->PrevCmdAborted = 0; 
		return; 
	}
	
	dbgprint ("Sequence Number : %d %d, NetFn : %x, Cmd : %x\n", 
			pKCSBuffer->KCSSeqNo, pKCSCmdMsg->SessionID, 
			pKCSCmdMsg->Response[0], pKCSCmdMsg->Response[1]);
	
	if (pKCSBuffer->KCSSeqNo != pKCSCmdMsg->SessionID)
	{
		//! The response is not belong to the current request, so ignore the response.
		dbgprint ("Session ID mismatch, so ignoring the response Request : %d, Response : %d\n", 
				pKCSBuffer->KCSSeqNo, pKCSCmdMsg->SessionID);
		return;
	}
	
	log_kcs(pdev->ch_num, 	pKCSCmdMsg->ResponseSize, pKCSCmdMsg->Response);
	
	/* Update the send buffer and associated indexes */
	pKCSBuffer->pKCSSendPkt = pKCSCmdMsg->Response;
	pKCSBuffer->KCSSendPktLen   = pKCSCmdMsg->ResponseSize;
	pKCSBuffer->KCSSendPktIx   = 0;

	/* Send the first byte */
	pKCSBuffer->KCSSendPktIx++;
	pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (pdev->ch_num, pKCSBuffer->pKCSSendPkt[0]);
	//SA Set OBF Byte
	pdev->pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (pdev->ch_num);

	/* From now onwords the data is sent from the IBF Interrupt handler */
	return;
}

static void 
set_kcs_state (struct kcs_hal *pkcs_hal, unsigned char ch_num, u8 state_value)
{
        u8 status_val = 0;
        pkcs_hal->pkcs_hal_ops->read_kcs_status (ch_num, &status_val);
        status_val = ((status_val & (~0xC0)) | (state_value));
        pkcs_hal->pkcs_hal_ops->write_kcs_status (ch_num, status_val);
}

int 
process_kcs_intr (int dev_id, unsigned char ch_num)
{
	struct kcs_hal *pkcs_hal;	

	dbgprint ("dev_id in process_kcs_intr = %d\n", dev_id);
	pkcs_hal = hw_intr (EDEV_TYPE_KCS, dev_id);
	if (!pkcs_hal)
		return -ENXIO;

	kcs_recv_byte (pkcs_hal, ch_num);
	return 0;
}

int
is_hw_test_mode ( void )
{
	if (m_kcs_hw_test_enable == ENABLE_DRIVER_TESTING)
                return 1;
	else
		return 0;
}

void 
kcs_recv_byte (struct kcs_hal *pkcs_hal, unsigned char ch_num)
{
	u8      Status;
	u8          DummyByte=0;
	int         i;
	u8      Cmd;
	unsigned long flags;
	u8     b;
	KCSBuf_T *pKCSBuffer = &pkcs_hal->pkcs_buf [ch_num];
	dbgprint ("kcs_recv_byte: ch_num:  %d, kcs_hal addr : %p\n", ch_num, pkcs_hal);
	dbgprint ("kcs_recv_byte: ch_num:  %d, kcs_buf is : %p\n", ch_num, pKCSBuffer);
	Status = 0; 

	/* Read the Present Status of KCS Port */
	pkcs_hal->pkcs_hal_ops->read_kcs_status (ch_num, &Status);
	
	dbgprint ("KCS status reg = %x\n", Status);
	/* If write to command register */
	if (IS_WRITE_TO_CMD_REG(Status))
	{
		dbgprint ("WRITE_TO_CMD reg\n");
		Cmd = 0;

		/* Set the status to WRITE_STATE */
		set_kcs_state (pkcs_hal, ch_num, KCS_WRITE_STATE);
		/* Read the command */
		pkcs_hal->pkcs_hal_ops->read_kcs_command (ch_num, &Cmd);

		switch (Cmd)
		{
			case KCS_WRITE_START :
				dbgprint ("KCS_WRITE_START\n");
				/* Set the Index to 0 */
				pKCSBuffer->KCSRcvPktIx = 0;
				/* Clearing Abort flag at the start of transaction */
				pKCSBuffer->PrevCmdAborted = 0;
				/*! Set the sequence number for validating the response */
				pKCSBuffer->KCSSeqNo = (pKCSBuffer->KCSSeqNo + 1) % MAX_SEQ_NO;
				pKCSBuffer->pKCSRcvPkt[pKCSBuffer->KCSRcvPktIx] = pKCSBuffer->KCSSeqNo;
				pKCSBuffer->KCSRcvPktIx++;
				/* Set the phase to WRITE */
				pKCSBuffer->KCSPhase = KCS_PHASE_WRITE;

				break;

			case KCS_WRITE_END :
				dbgprint ("KCS_WRITE_END\n");
				/* Set the phase to write end */
				pKCSBuffer->KCSPhase = KCS_PHASE_WRITE_END;
				break;

			case KCS_ABORT : 
				dbgprint ("KCS_ABORT\n");
				spin_lock_irqsave(&pKCSBuffer->kcs_lock, flags);
				if (!pKCSBuffer->KcsIFActive && !pKCSBuffer->FirstTime)	/* Don't set if driver has not yet processed a request */
				{ 
					/* Set flag to avoid sending response*/ 
					pKCSBuffer->PrevCmdAborted = 1; 
				}
				spin_unlock_irqrestore(&pKCSBuffer->kcs_lock, flags); 
				/* Set the error code */
				if (KCS_NO_ERROR == pKCSBuffer->KCSError)
				{
					pKCSBuffer->KCSError = KCS_ABORTED_BY_COMMAND;
				}
				/* Set the phase to write end */
				pKCSBuffer->KCSPhase = KCS_PHASE_ABORT;
				/* Set the abort phase to be error1 */
				pKCSBuffer->AbortPhase = ABORT_PHASE_ERROR1;

				/* Send the dummy byte  */
				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
				//SA Set OBF Byte
				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);

				break;

			default :
				dbgprint ("wrong value in CMD reg\n");
				/* Set the error code */
				pKCSBuffer->KCSError = KCS_ILLEGAL_CONTROL_CODE;
				/* Invalid command code - Set an error state */
				set_kcs_state (pkcs_hal, ch_num, KCS_ERROR_STATE);
				/* Set the phase to error phase */
				pKCSBuffer->KCSPhase = KCS_PHASE_ERROR;
				break;
		}
	}
	else
	{
		switch (pKCSBuffer->KCSPhase)
		{
			case KCS_PHASE_WRITE :
				dbgprint ("KCS_PHASE_WRITE\n");
				/* Set the state to write state */
				set_kcs_state (pkcs_hal, ch_num, KCS_WRITE_STATE);
				/* Read the BYTE from the data register */
				pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &(pKCSBuffer->pKCSRcvPkt [pKCSBuffer->KCSRcvPktIx]));

				if (pKCSBuffer->KCSRcvPktIx < MAX_KCS_PKT_LEN)
				{
					pKCSBuffer->KCSRcvPktIx++;
				}
				break;

			case KCS_PHASE_WRITE_END :
				dbgprint ("KCS_PHASE_WRITE_END\n");
				/* Set the state to READ_STATE */
				set_kcs_state (pkcs_hal, ch_num, KCS_READ_STATE);

				/* Read the BYTE from the data register */
				pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &(pKCSBuffer->pKCSRcvPkt [pKCSBuffer->KCSRcvPktIx]));

				pKCSBuffer->KCSRcvPktIx++;

				//SA lets print all data received from SMS

				dbgprint ("Total bytes received 0x%x\n", pKCSBuffer->KCSRcvPktIx );
				for(i=0;i < pKCSBuffer->KCSRcvPktIx ; i++)
				{
					dbgprint("0x%x ", pKCSBuffer->pKCSRcvPkt [i]);
				}
				dbgprint ("\npKCSBuffer->KcsIFActive:%x\n",pKCSBuffer->KcsIFActive);


				/* Signal receive data ready */
				/* Move to READ Phase */
				pKCSBuffer->KCSPhase = KCS_PHASE_READ;


				/* SA Wakeup KcsRequest to notify Request Packet is ready*/
				/* Wakeup only if KCS device is opened and is being used */
				spin_lock_irqsave(&pKCSBuffer->kcs_lock, flags); 
				if ( pKCSBuffer->KcsINuse)
					pKCSBuffer->KcsWakeup = 1;
				if (pKCSBuffer->KcsIFActive)
				{
					spin_unlock_irqrestore(&pKCSBuffer->kcs_lock, flags); 
					dbgprint ("Request received successfully\n");
					Kcs_OsWakeupOnTimeout(&(pKCSBuffer->KcsReqWait));
				}
				else {
					spin_unlock_irqrestore(&pKCSBuffer->kcs_lock, flags); 
				}
				//else some error...
				break;

			case KCS_PHASE_READ :
				dbgprint ("KCS_PHASE_READ\n");
				/* If we have reached the end of the packet move to idle state */
				if (pKCSBuffer->KCSSendPktIx == pKCSBuffer->KCSSendPktLen)
				{
					set_kcs_state (pkcs_hal, ch_num, KCS_IDLE_STATE);
				}
				/* Read the byte returned by the SMS */
				{ 
					b = 0;
					pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &b); 
					//SA Need to clear IBF
					//sa_0111 clear_IBF_status(ch_num);

					if (b != KCS_READ)
					{
						set_kcs_state (pkcs_hal, ch_num, KCS_ERROR_STATE);
						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
						//SA Set OBF Byte
						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
						break;
					}
				}
				/* If we are finished transmitting, send the dummy byte */
				if (pKCSBuffer->KCSSendPktIx == pKCSBuffer->KCSSendPktLen) 
				{
					pKCSBuffer->KCSPhase = KCS_PHASE_IDLE;
					pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
					//SA Set OBF Byte
					pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);

					/* Set Transmission Complete */
					pKCSBuffer->TxReady     = 1;
					/* Wakeup Sleeping Process */
					//if(pKCSBuffer->KcsWtIFActive)
					//  Kcs_OsWakeupOnTimeout(&(pKCSBuffer->KcsResWait));
					break;
				}
				/* Transmit the next byte from the send buffer */
				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, pKCSBuffer->pKCSSendPkt [pKCSBuffer->KCSSendPktIx]);
				//SA Set OBF Byte
				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
				pKCSBuffer->KCSSendPktIx++;
				break;

			case KCS_PHASE_ABORT :
				dbgprint ("KCS_PHASE_ABORT\n");
				switch (pKCSBuffer->AbortPhase) 
				{
					case ABORT_PHASE_ERROR1 :
						/* Set the KCS State to READ_STATE */
						set_kcs_state (pkcs_hal, ch_num, KCS_READ_STATE);
						/* Read the Dummy byte  */
						pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &DummyByte); 
						//SA Need to clear IBF
						//sa_0111 clear_IBF_status(ch_num);
						/* Write the error code to Data out register */
						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, pKCSBuffer->KCSError);
						//SA Set OBF Byte
						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
						/* Set the abort phase to be error2 */
						pKCSBuffer->AbortPhase = ABORT_PHASE_ERROR2;

						break;

					case ABORT_PHASE_ERROR2 :

						/**
						 * The system software has read the error code. Go to idle
						 * state.
						 **/
						set_kcs_state (pkcs_hal, ch_num, KCS_IDLE_STATE);

						/* Read the Dummy byte  */
						pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &DummyByte); 

						pKCSBuffer->KCSPhase = KCS_PHASE_IDLE;
						pKCSBuffer->AbortPhase = 0;

						/* Send the dummy byte  */
						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
						//SA Set OBF Byte
						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);

				}
				break;

			default:
				dbgprint ("incorrect Phase value\n");
				/* Read the Dummy byte  */
				pkcs_hal->pkcs_hal_ops->read_kcs_data_in (ch_num, &DummyByte); 
				set_kcs_state (pkcs_hal, ch_num, KCS_ERROR_STATE);
				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
				//SA Set OBF Byte
				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
        }
    }
}
#ifdef CONFIG_SPX_FEATURE_ENABLE_KCS_LOG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	static int kcs_log_proc_read(struct file *file,char __user *buffer, size_t buffer_length,loff_t *offset)
#else
	static int kcs_log_proc_read(char *buffer, char **buffer_location,off_t offset, int buffer_length, int *eof, void *data)
#endif
{
	unsigned char *prvData=NULL;
	
	if (buffer_length < 2)
		return -1;
		
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	if (*offset != 0)
		return 0;
	prvData=PDE_DATA(file_inode(file));
	if(!(prvData)){
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -1;
	}
#else
	*eof=1;
	if (offset != 0)
		return 0;
	
	if (!data)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -1;
	}
	prvData = data;
	*eof = 1;
#endif
		
	buffer[0] = *(unsigned char*)prvData ? '1' : '0';
	buffer[1] = '\n';
	
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	*offset+=2;
#endif
	return 2;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	static int kcs_log_proc_write(struct file *file,const char *buffer,size_t buffer_length,loff_t *offp)
#else 
	static int kcs_log_proc_write(struct file *file,const char __user *buffer,unsigned long buffer_length, void *data)
#endif
{
		unsigned char *prvData=NULL;
			
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
		prvData=PDE_DATA(file_inode(file));
		if(!(prvData)){
			printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
			return -1;
		}
#else
	if (!data)
	{
		printk(KERN_ERR "%s() empty data\n", __FUNCTION__);
		return -1;
	}
	prvData = data;
#endif

	if(buffer_length > 0)
	{
		if ('1' == buffer[0])
			*(unsigned char*)prvData = 1;
		else if ('0' == buffer[0])
			*(unsigned char*)prvData = 0;
	}
	return buffer_length;
}

#define KCS_LOG_PROCFS_NAME "log-kcs"

static void log_proc_open(unsigned channel)
{
	struct proc_dir_entry *pde = NULL;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	struct file_operations *proc_fops = NULL;
#endif
	char procfs_name[32]={0};
	mode_t mode = 0644;

	snprintf(procfs_name, sizeof(procfs_name), KCS_LOG_PROCFS_NAME"%d",channel);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	proc_fops = kmalloc(sizeof (struct file_operations),GFP_KERNEL);
	if (proc_fops == NULL)
	{
		return;
	}
	
	memset(proc_fops,0,sizeof(struct file_operations));
	proc_fops->owner = THIS_MODULE;
	mode = 0644;
	// specify read/write callbacks
	proc_fops->read = kcs_log_proc_read;
	proc_fops->write = kcs_log_proc_write;

	pde = proc_create_data(procfs_name, mode, NULL,proc_fops,&log_enabled[channel]);
	if (pde == NULL)
	{
		kfree(proc_fops);
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",procfs_name);
		return;
	}
#else
	// create entry in /proc root
	pde = create_proc_entry(procfs_name, mode, NULL);
	if (pde == NULL)
	{
		remove_proc_entry(procfs_name, NULL);
		printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",procfs_name);
		return;
	}

	// specify read/write callbacks
	pde->read_proc	= kcs_log_proc_read;
	pde->write_proc	= kcs_log_proc_write;

	// specify file attributes for file
	pde->owner	= THIS_MODULE;
	pde->mode 	= S_IFREG | S_IRUGO;
	pde->uid 	= 0;
	pde->gid 	= 0;
	pde->data = &log_enabled[channel]; //private data for different identical files
#endif

}

static void log_proc_close(unsigned channel)
{
	char procfs_name[32];
	snprintf(procfs_name, sizeof(procfs_name), KCS_LOG_PROCFS_NAME"%d",
		channel);
	remove_proc_entry(procfs_name, NULL);
}

static void log_kcs(unsigned channel, unsigned size, u8 * data)
{
	if ((channel < KCS_LOG_CHANNELS) && log_enabled[channel])
	{
		unsigned i;

		printk(KERN_DEBUG "KCS%d:", channel);

		for (i = 0; i < size; ++i)
			printk(" %02X", data[i]);

		printk("\n");
	}
}
#endif // CONFIG_SPX_FEATURE_ENABLE_KCS_LOG


/* ----- Driver registration ---------------------------------------------- */

static kcs_core_funcs_t kcs_core_funcs = {
	.process_kcs_intr = process_kcs_intr,
	.is_hw_test_mode  = is_hw_test_mode
};

static core_hal_t kcs_core_hal = {
	.owner		           = THIS_MODULE,
	.name		           = "KCS CORE",
	.dev_type              = EDEV_TYPE_KCS,
	.register_hal_module   = register_kcs_hal_module,
	.unregister_hal_module = unregister_kcs_hal_module,
	.pcore_funcs           = (void *)&kcs_core_funcs
};
	
static int __init kcs_init (void)
{
	int ret = 0,i;
	printk (banner);

	if ((ret = register_chrdev_region (kcs_devno, KCS_MAX_CHANNELS, KCS_DEV_NAME)) < 0)
	{
		printk (KERN_ERR "failed to register kcs device <%s> (err: %d)\n", KCS_DEV_NAME, ret);
		return ret;
	}

	kcs_cdev = cdev_alloc ();
	if (!kcs_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate kcs cdev structure\n", KCS_DEV_NAME);
		unregister_chrdev_region (kcs_devno, KCS_MAX_CHANNELS);
		return -1;
	}

	cdev_init (kcs_cdev, &kcs_fops);
	kcs_cdev->owner = THIS_MODULE;

	if ((ret = cdev_add (kcs_cdev, kcs_devno, KCS_MAX_CHANNELS)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", KCS_DEV_NAME);
		cdev_del (kcs_cdev);
		unregister_chrdev_region (kcs_devno, KCS_MAX_CHANNELS);
		ret = -ENODEV;
		return ret;	
	}

	if ((ret = register_core_hal_module (&kcs_core_hal)) < 0)
	{
		printk  (KERN_ERR "failed to register the Core KCS module\n");
		cdev_del (kcs_cdev);
		unregister_chrdev_region (kcs_devno, KCS_MAX_CHANNELS);
		ret = -EINVAL;
		return ret;	
	}

	moduledir = proc_mkdir("kcs",rootdir);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
        kcsproc =
#endif
        AddProcEntry (moduledir, "KCS", kcs_hw_read_test_setting, kcs_hw_write_test_setting, NULL);

	my_sys  = AddSysctlTable("kcs", &KCS_table[0]);
	
	for (i = 0; i < KCS_LOG_CHANNELS; ++i)
		log_proc_open(i);

	return 0;
}

module_init (kcs_init);
module_exit (kcs_exit);

int kcs_core_loaded =1;
EXPORT_SYMBOL(kcs_core_loaded);

