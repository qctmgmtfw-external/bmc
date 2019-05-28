/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

/****************************************************************
 *
 * watchdog_core.c
 * common module for watchdog timer driver
 *
 ****************************************************************/

#include <linux/version.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "watchdog_core.h"
#include "helper.h"
#include "driver_hal.h"

#define WATCHDOG_DRIVER_NAME "watchdog"
#define WATCHDOG_DEFAULT_TIMEOUT 900 /* in seconds */

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

static int watchdog_timeout = WATCHDOG_DEFAULT_TIMEOUT;

#ifdef CONFIG_WATCHDOG_NOWAYOUT
static int nowayout = 1;
#else
static int nowayout = 0;
#endif

static unsigned long watchdog_is_open;
static struct watchdog_info watchdog_info;
static char watchdog_expect_close;

static struct watchdog_core_funcs_t watchdog_core_funcs;
static struct watchdog_hal_t *watchdog_hal;
static struct proc_dir_entry *wd_dir = NULL;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
static struct proc_info  *wdt_proc = NULL;
#endif

static char banner[] __initdata = KERN_INFO "Watchdog Common Driver, (c) 2010 American Megatrends Inc.\n";	

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
int watchdog_timer_read(struct file *file,  char __user *buf, size_t count, loff_t *offset)
#else
int watchdog_timer_read( char *buf, char **start, off_t offset, int count, int *eof, void *data )
#endif
{
	
	 int len=0;

/* We don't support seeked reads. read should start from 0 */	
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	if (*offset != 0)
		return 0;
#else
	*eof=1;
	if (offset != 0)
		return 0;
#endif
	
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
	/* This is a requirement. So don't change it */
	*start = buf;
#endif

	 len = sprintf(buf, "%d\n", watchdog_timeout);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	 *offset +=len;
#endif
	 
	 return len;
}

int register_watchdog_hal_module(unsigned char num_instances, void *hal_ops, void **hw_data)
{
	if (!hal_ops)
		return -ENOSYS;

	watchdog_hal = (struct watchdog_hal_t *) kmalloc(sizeof(struct watchdog_hal_t), GFP_KERNEL);
	if (!watchdog_hal) {
		return -ENOMEM;
	}

	watchdog_hal->hal_ops = (struct watchdog_hal_ops_t *) hal_ops;
	*hw_data = (void *) watchdog_hal;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	wdt_proc = 
#endif
	AddProcEntry (wd_dir, "TimerValue", watchdog_timer_read, NULL, NULL);

	return 0;
}

int unregister_watchdog_hal_module(void *hw_data)
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
	RemoveProcEntry(wdt_proc);
#else
	RemoveProcEntry(wd_dir, "TimerValue");
#endif

	if (watchdog_hal != NULL)
		kfree(watchdog_hal);
    return 0;
}

static core_hal_t watchdog_core_hal = {
	.owner = THIS_MODULE,
	.name = "WATCHDOG CORE",
	.dev_type = EDEV_TYPE_WATCHDOG,
	.register_hal_module = register_watchdog_hal_module,
	.unregister_hal_module = unregister_watchdog_hal_module,
	.pcore_funcs = (void *) (&watchdog_core_funcs)
};

static struct watchdog_info watchdog_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_MAGICCLOSE,
	.firmware_version = 0,
	.identity = WATCHDOG_DRIVER_NAME,
};

static ssize_t watchdog_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	size_t i;
	char c;

	if (!watchdog_hal)
		return -ENOSYS;

	if (count) {
		watchdog_expect_close = 0;
		for (i = 0; i < count; i ++) {
			if (get_user(c, buf + i))
				return -EFAULT;
			if (c == 'V') /* special character, userspace daemon would like to disable watchdog */
				watchdog_expect_close = 1;
		}
		watchdog_hal->hal_ops->count(); /* pat the watchdog */
	}
	return count;
}

static int watchdog_open(struct inode *index_node, struct file *file)
{
	if (!watchdog_hal)
		return -ENOSYS;

	if (test_and_set_bit(0, &watchdog_is_open)) /* we have been opened */
		return -EBUSY;

	watchdog_hal->hal_ops->enable();
	watchdog_hal->hal_ops->set_value(watchdog_timeout);
	watchdog_hal->hal_ops->count();
	return 0;
}

static int watchdog_close(struct inode *inode, struct file *file)
{
	if (!watchdog_hal)
		return -ENOSYS;

	if (!nowayout && (watchdog_expect_close == 1)) {
		watchdog_hal->hal_ops->disable();
	} else {
		printk(KERN_CRIT WATCHDOG_DRIVER_NAME ": Unexpected close, not stopping watchdog!\n");
	}

	clear_bit(0, &watchdog_is_open);
	watchdog_expect_close = 0;
	return 0;
}

static long watchdog_ioctlUnlocked(struct file *file, unsigned int cmd, unsigned long arg)
{
	int new_timeout;
	int options;
	int retval = -EINVAL;

	if (!watchdog_hal)
		return -ENOSYS;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		if (copy_to_user((struct watchdog_info *)arg, &watchdog_info, sizeof(watchdog_info)))
			return -EFAULT;
		return 0;

	case WDIOC_GETSTATUS:
	case WDIOC_GETBOOTSTATUS:
		return put_user(0, (int *) arg);

	case WDIOC_KEEPALIVE:
		watchdog_hal->hal_ops->count(); /* pat the watchdog */
		return 0;

	case WDIOC_SETTIMEOUT:
		if (get_user(new_timeout, (int *) arg))
			return -EFAULT;
		if (new_timeout < 3 || new_timeout > 3600) /* arbitrary upper limit */
			return -EINVAL;

		watchdog_timeout = new_timeout;
		watchdog_hal->hal_ops->set_value(watchdog_timeout);
		watchdog_hal->hal_ops->count();

		return put_user(watchdog_timeout, (int *) arg);

	case WDIOC_GETTIMEOUT:
		return put_user(watchdog_timeout, (int *) arg);

	case WDIOC_SETOPTIONS:
		if (get_user(options, (int *)arg))
			return -EFAULT;

		if (options & WDIOS_DISABLECARD) {
			watchdog_hal->hal_ops->disable();
			retval = 0;
		}

		if (options & WDIOS_ENABLECARD) {
			watchdog_hal->hal_ops->enable();
			retval = 0;
		}
		break;

	default:
		return -ENOIOCTLCMD;
	}

	return retval;
}
#ifndef USE_UNLOCKED_IOCTL   
static int       
watchdog_ioctl(struct inode * inode, struct file * file, unsigned int cmd,unsigned long arg)  
        return watchdog_ioctlUnlocked(file,cmd,arg);  
}  
#endif  

static struct file_operations watchdog_fops = {
	.owner = THIS_MODULE,
	.write = watchdog_write,
#ifdef USE_UNLOCKED_IOCTL 
	.unlocked_ioctl = watchdog_ioctlUnlocked, 
#else 
	.ioctl = watchdog_ioctl,
#endif
	.open = watchdog_open,
	.release = watchdog_close,
};

static struct miscdevice watchdog_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &watchdog_fops,
};

static int __init watchdog_init(void)
{
	int ret;

	printk(banner);

	ret = register_core_hal_module (&watchdog_core_hal);
	if (ret < 0) {
		printk(KERN_ERR "failed to register the common watchdog module\n");
		return -EINVAL;
	}

	ret = misc_register(&watchdog_miscdev);
	if (ret) {
		printk(KERN_ERR WATCHDOG_DRIVER_NAME ": Can't register misc device\n");
		return ret;
	}
	wd_dir = proc_mkdir("watchdog", rootdir);
	return ret;
}

static void __exit watchdog_exit(void)
{
	unregister_core_hal_module(EDEV_TYPE_WATCHDOG);
	remove_proc_entry("watchdog", rootdir);
	misc_deregister(&watchdog_miscdev);
}

module_init(watchdog_init);
module_exit(watchdog_exit);

module_param(watchdog_timeout, int, 0);
MODULE_PARM_DESC(watchdog_timeout, "Watchdog timeout in seconds, default=" __MODULE_STRING(WATCHDOG_DEFAULT_TIMEOUT) ")");

module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started (default=CONFIG_WATCHDOG_NOWAYOUT)");

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("common module for watchdog timer driver");
MODULE_LICENSE("GPL");

int watchdog_core_loaded =1;
EXPORT_SYMBOL(watchdog_core_loaded);
