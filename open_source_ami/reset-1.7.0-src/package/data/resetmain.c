/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include <linux/cdev.h>
#include "helper.h"
#include "reset.h"
#include "proc.h"
#include "dbgout.h"
#include "proc.h"
#include "driver_hal.h"
#include "reset_ioctl.h"

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
	#define USE_UNLOCKED_IOCTL
  #endif
#endif

MODULE_AUTHOR("American Megatrends Inc");
MODULE_DESCRIPTION("This is Host Reset Distributer module.");
MODULE_LICENSE("GPL");


#define SAMPLE_MAJOR	1
#define SAMPLE_MINOR	0


#define RESET_MAJOR 		110
#define RESET_MINOR 		0
#define RESET_MAX_DEVICES	255
#define RESET_MAX_CALLBACKS	255
#define RESET_DEV_NAME		"reset"


/********************************************************
 * 		Global Data Initializations		*
 ********************************************************/

unsigned int m_dev_id = 0;
static unsigned int m_reset_count = 0;
extern unsigned int m_reset_test_enable;
static DECLARE_WAIT_QUEUE_HEAD (reset_wait_queue);
static unsigned int reset_event = 0;
static unsigned int reset_wait_count = 0;

int  (*ResetFunction)(void) = NULL;


typedef struct reset_callback_t {
	int  (*CallBackHandler)(void);
} reset_callback;


struct reset_hal
{
        reset_hal_operations_t *preset_hal_ops;
};

struct reset_dev
{
        struct reset_hal  *preset_hal;
};


struct reset_hal   m_reset_device_data[RESET_MAX_DEVICES]; 
reset_callback     m_reset_callback[RESET_MAX_CALLBACKS];


TDBG_DECLARE_DBGVAR(reset);
/* Sysctl Table */
static struct ctl_table LPCRESET_table [] =
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0))
        {"DebugLevel",&(TDBG_FORM_VAR_NAME(reset)),sizeof(int),0644,NULL,NULL,&proc_dointvec},
#else
        {CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(reset)),sizeof(int),0644,NULL,NULL,&proc_dointvec},
#endif
        {0}
};


/* Proc and Sysctl entries */
static struct proc_dir_entry *moduledir = NULL;
static struct ctl_table_header *my_sys  = NULL;

/***********************************************************************************************/

static int reset_init(void);
static void reset_exit(void);

#ifdef USE_UNLOCKED_IOCTL 
static long reset_ioctl_unlocked(struct file *file, unsigned int cmd, unsigned long arg);
#else
static int reset_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
#endif

static int get_lpcrst_count(unsigned long arg);
static int set_lpcrst_count(unsigned long arg);

static struct file_operations reset_fops =
{
        owner:          THIS_MODULE,
        read:           NULL,
        open:           NULL,
#ifdef USE_UNLOCKED_IOCTL 
       	.unlocked_ioctl =     reset_ioctl_unlocked,
#else
       	.ioctl  	=      reset_ioctl,
#endif
};

static struct cdev *reset_cdev;
static dev_t reset_devno = MKDEV(RESET_MAJOR, RESET_MINOR);
static char banner[] __initdata = KERN_INFO "Reset Common Driver, (c) 2009 American Megatrends Inc.\n";


static int ResetCallBack(void);
static void UnregisterforHostReset(void);
static int RegisterforHostReset(int (*pt2Func)(void));


/***********************************************************************************************/


static void __exit 
reset_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_RESET);	
	unregister_chrdev_region (reset_devno, RESET_MAX_DEVICES);

	if (NULL != reset_cdev)
	{
		printk (KERN_INFO "reset char device del\n");
		cdev_del (reset_cdev);
	}

	RemoveSysctlTable(my_sys);
        remove_proc_entry("reset",rootdir);

	UnregisterforHostReset();
	printk("Reset Common module unloaded successfully\n");
	return;
}

int 
register_reset_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct reset_hal* phw_hal;

	phw_hal = (struct reset_hal*) kmalloc (sizeof(struct reset_hal), GFP_KERNEL);
	if (!phw_hal)
	{
		return -ENOMEM;
	}

	phw_hal->preset_hal_ops = ( reset_hal_operations_t *) phal_ops;
	*phw_data = (void *) phw_hal;	

	AddProcEntry (moduledir, "RESET", reset_read_test_setting, reset_write_test_setting, NULL);
	return 0;	
}


int 
unregister_reset_hal_module (void *phw_data)
{
	struct reset_hal *preset_hal = (struct reset_hal*) phw_data;
	
	printk (KERN_INFO "unregister hal is : %p\n",  preset_hal);
	RemoveProcEntry(moduledir, "RESET");

	kfree (preset_hal);

	return 0;
}


static long
reset_ioctl_unlocked(struct file *file, unsigned int cmd, unsigned long arg)
{
        int ret = 0;

        switch(cmd)
        {
                case GET_LPCRST_COUNT:
                        if (m_reset_test_enable == ENABLE_RESET_DRIVER_TESTING)
                        {
                                ret = get_lpcrst_count(arg);
                        }
                        else
                        {
                                printk("Invalid IOCTL command\n");
                                ret = -1;
                        }
                        break;
		case SET_LPCRST_COUNT:
			if (m_reset_test_enable == ENABLE_RESET_DRIVER_TESTING)
                        {
                                ret = set_lpcrst_count(arg);
                        }
                        else
                        {
                                printk("Invalid IOCTL command\n");
                                ret = -1;
                        }
                        break;
		case WAIT_ON_RESET:
			reset_wait_count ++;
			ret = wait_event_interruptible( reset_wait_queue, reset_event != 0 );
			reset_wait_count --;
			if (reset_wait_count == 0)
				reset_event = 0;
			break;
                default:
                        printk("Invalid IOCTL command\n");
                        break;
        }
        return ret;
}

#ifndef USE_UNLOCKED_IOCTL 
static int     
reset_ioctl(struct inode * inode, struct file * file, unsigned int cmd,unsigned long arg)
{
	return reset_ioctl_unlockednlocked(file,cmd,arg);
}
#endif




/* Function for  Register the Resetfunction in KCS function pointer  */
int
RegisterforHostReset(int (*pt2Func)(void))
{
        if (ResetFunction != NULL)
                return 1;
        ResetFunction=pt2Func;
        return 0;
}

/*  Unregister the Reset function in KCS function  pointer */
void
UnregisterforHostReset(void)
{
        ResetFunction=NULL;
        return;
}

int   
ResetCallBack(void)
{
	int err,i;

	for(i=0;i<RESET_MAX_CALLBACKS;i++)
	{
		if (NULL != m_reset_callback[i].CallBackHandler)
			err = (*(m_reset_callback[i].CallBackHandler)) ();
	}
	return 0;
}



int
install_reset_handler(int (*pt2Func)(void))
{
	int i ;
	for (i=0; i<RESET_MAX_CALLBACKS; i++)
	{
		if (m_reset_callback[i].CallBackHandler == NULL)
		{
			m_reset_callback[i].CallBackHandler = pt2Func;
			return 0;
		}
	}

	printk("ERROR: No Available Slot to Install Reset Handler\n");
	return 1;
}


int
uninstall_reset_handler(int (*pt2Func)(void))
{
	int i;
	for (i=0; i<RESET_MAX_CALLBACKS; i++)
	{
		if(pt2Func == m_reset_callback[i].CallBackHandler)
		{
			m_reset_callback[i].CallBackHandler = NULL;
			return 0;
		}
	}
	printk("ERROR: Trying to uninstall a invalid Reset Handler\n");
	return 1;
}


int
process_reset_intr (int dev_id)
{
        int err = 0;

	//wake up all processes waiting on reset event
	reset_event = 1;
	wake_up_interruptible_all( &reset_wait_queue );

        if(NULL != ResetFunction)
                err = (*ResetFunction)();

        return err;
}


static int
get_lpcrst_count(unsigned long arg)
{
        if (copy_to_user ((void*)arg, (void*)&m_reset_count, sizeof(m_reset_count)))
        {
                printk("GET_LPCRST_COUNT: Error copying data to user \n");
                return -EFAULT;
        }
        return 0;
}

static int
set_lpcrst_count(unsigned long arg)
{
	if (copy_from_user ((void*)&m_reset_count, (void*)arg, sizeof(m_reset_count)))
	{
		printk("GET_LPCRST_COUNT: Error copying data to user \n");
                return -EFAULT;
	}
	return 0;
}


int TestResetCount (void)
{
        m_reset_count ++;
        return 0;
}


/***********************************************************************************************/


static reset_core_funcs_t reset_core_funcs = {
        .process_reset_intr = process_reset_intr,
};

static core_hal_t reset_core_hal = {
        .owner                  =  THIS_MODULE,
        .name                   =  "RESET CORE",
        .dev_type               =  EDEV_TYPE_RESET,
        .register_hal_module    =  register_reset_hal_module,
        .unregister_hal_module  =  unregister_reset_hal_module,
        .pcore_funcs            =  (void *)&reset_core_funcs
};


static int __init 
reset_init(void)
{
	int ret=0;
	int i;
	printk (banner);
	
    	if ((ret = register_chrdev_region (reset_devno, RESET_MAX_DEVICES, RESET_DEV_NAME)) < 0)
    	{
		printk (KERN_ERR "failed to register reset device <%s> (err %d)\n", RESET_DEV_NAME, ret);
		return ret;
   	}

	reset_cdev = cdev_alloc ();
	if (!reset_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate reset cdev structure\n", RESET_DEV_NAME);
		unregister_chrdev_region (reset_devno, RESET_MAX_DEVICES);
		return -1;
	}

	cdev_init (reset_cdev, &reset_fops);

	if ((ret = cdev_add (reset_cdev, reset_devno, RESET_MAX_DEVICES)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", RESET_DEV_NAME);
		cdev_del (reset_cdev);
		unregister_chrdev_region (reset_devno, RESET_MAX_DEVICES);
		ret=-ENODEV;
                return ret;
	}

	for (i=0; i<RESET_MAX_DEVICES; i++) 
		m_reset_device_data[i].preset_hal_ops = NULL;
	
	for (i=0; i<RESET_MAX_CALLBACKS; i++) 
		m_reset_callback[i].CallBackHandler = NULL;

	if ((ret = register_core_hal_module (&reset_core_hal)) < 0)
	{
		printk  (KERN_ERR "failed to register the core reset module\n");
		cdev_del (reset_cdev);
		unregister_chrdev_region (reset_devno, RESET_MAX_DEVICES);
		ret=-EINVAL;
                return ret;
	}

	moduledir = proc_mkdir("reset", rootdir);
	my_sys  = AddSysctlTable("reset", &LPCRESET_table[0]);

	RegisterforHostReset(&ResetCallBack);
	return ret;
}

module_init (reset_init);
module_exit (reset_exit);

EXPORT_SYMBOL(install_reset_handler);
EXPORT_SYMBOL(uninstall_reset_handler);
