/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * adc_mod.c
 * ADC driver Module functions
 *
 *****************************************************************/
#define ADC_DRIVER_MAJOR	1
#define ADC_DRIVER_MINOR    0

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cacheflush.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/version.h>
#include <linux/cdev.h>

#include "helper.h"
#include "driver_hal.h"
#include "adc.h"


/* Local Macros */
#define ADC_MAJOR_NUM		251
#define ADC_MINOR_NUM		0
#define ADC_MAX_DEVICES     1
#define ADC_DEV_NAME        "adc"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

static dev_t adc_devno = MKDEV(ADC_MAJOR_NUM, ADC_MINOR_NUM);
static struct cdev *adc_cdev;
#ifdef USE_UNLOCKED_IOCTL
static long adc_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg);
#else
static int adc_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg);
#endif


static int 	adc_driver_sys_reboot_notify(struct notifier_block *this, unsigned long code, void *unused);
static int adc_open(struct inode *inode, struct file *file);
static int adc_release(struct inode *inode, struct file *file);



/* Module Variables	*/
static struct file_operations adc_ops = {
        owner:      THIS_MODULE,
        read:       NULL,
        write:      NULL,
#ifdef USE_UNLOCKED_IOCTL 
        unlocked_ioctl:	adc_module_ioctlUnlocked, 
#else 
        ioctl:      adc_module_ioctl,
#endif 
        open:       adc_open,
        release:    adc_release,
};


static struct notifier_block adc_driver_notifier =
{
       .notifier_call = adc_driver_sys_reboot_notify,
};

/* Debug Level for the driver - Controlled by sysctl */
static int mdebuglevel = 0;
static struct ctl_table_header *adc_sys 	= NULL;

/* Sysctl related tables */
static struct ctl_table ADCSysCtlTable[] =
{

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	{"dbglevel",&mdebuglevel,sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else
	{CTL_UNNUMBERED,"dbglevel",&mdebuglevel,sizeof(int),0644,NULL,NULL,&proc_dointvec},
#endif	
	{0}
};


int
register_adc_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct adc_hal* phw_hal;

	phw_hal = (struct adc_hal*) kmalloc (sizeof(struct adc_hal), GFP_KERNEL);
	if (!phw_hal)
	{
		return -ENOMEM;
	}

	phw_hal->padc_hal_ops = ( adc_hal_operations_t *) phal_ops;
	*phw_data = (void *) phw_hal;

	return 0;
}

int
unregister_adc_hal_module (void *phw_data)
{
	struct adc_hal *padc_hal = (struct adc_hal*) phw_data;

	kfree (padc_hal);

	return 0;
}

/*
int register_adc_hw_module (ADC_HW* AdcHw)
{
	memcpy (&AdcHwFuncs, AdcHw, sizeof (ADC_HW));
	AdcHwFuncs.adc_init ();
	return 0;
}
*/

/* ----- Driver registration ---------------------------------------------- */

static core_hal_t adc_core_hal = {
	.owner		           = THIS_MODULE,
	.name		           = "ADC CORE",
	.dev_type              = EDEV_TYPE_ADC,
	.register_hal_module   = register_adc_hal_module,
	.unregister_hal_module = unregister_adc_hal_module,
};

int __init
adc_module_init(void)
{
	int	ret;

	printk("ADC Driver Version %d.%d\n",ADC_DRIVER_MAJOR,ADC_DRIVER_MINOR);
	printk("Copyright (c) 2009-2015 American Megatrends Inc.\n");

 	/* gpio device initialization */
	if ((ret = register_chrdev_region (adc_devno, ADC_MAX_DEVICES, ADC_DEV_NAME)) < 0)
	{
	   printk (KERN_ERR "failed to register adc device <%s> (err: %d)\n", ADC_DEV_NAME, ret);
	   return -1;
	}

	adc_cdev = cdev_alloc ();
	if (!adc_cdev)
	{
	   unregister_chrdev_region (adc_devno, ADC_MAX_DEVICES);
	   printk (KERN_ERR "%s: failed to allocate adc cdev structure\n", ADC_DEV_NAME);
	   return -1;
	}

	cdev_init (adc_cdev, &adc_ops);
	adc_cdev->owner = THIS_MODULE;

	if ((ret = cdev_add (adc_cdev, adc_devno, ADC_MAX_DEVICES)) < 0)
	{
		cdev_del (adc_cdev);
		unregister_chrdev_region (adc_devno, ADC_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", ADC_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}

	if ((ret = register_core_hal_module (&adc_core_hal)) < 0)
	{
		printk(KERN_ERR "failed to register the Core ADC module\n");
		cdev_del (adc_cdev);
		unregister_chrdev_region (adc_devno, ADC_MAX_DEVICES);
		ret = -EINVAL;
		return ret;
	}


   	register_reboot_notifier(&adc_driver_notifier);

	/* Add sysctl to access the DebugLevel */
	adc_sys  = AddSysctlTable("adc",&ADCSysCtlTable[0]);
	if (!adc_sys)
	{
		printk (KERN_CRIT "adc_mod.c: failed to register adc sysctl table hierarchy\n");
		unregister_core_hal_module (EDEV_TYPE_ADC);
		unregister_chrdev_region (adc_devno, ADC_MAX_DEVICES);

		if (NULL != adc_cdev)
			cdev_del (adc_cdev);

		unregister_reboot_notifier(&adc_driver_notifier);
		return -EBUSY;

	}

	return 0;
}

static void
adc_module_exit(void)
{
	printk ("Unloading ADC driver ..\n");

	unregister_core_hal_module (EDEV_TYPE_ADC);

	unregister_chrdev_region (adc_devno, ADC_MAX_DEVICES);

	if (NULL != adc_cdev)
	{
		cdev_del (adc_cdev);
	}

	/* Remove driver related sysctl entries */
	RemoveSysctlTable(adc_sys);

	unregister_reboot_notifier(&adc_driver_notifier);
}

static int
adc_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct adc_hal *padc_hal;
	struct adc_dev *pdev;
	hw_info_t adc_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_ADC, minor, &open_count, &adc_hw_info);
	if (ret)
		return -ENXIO;

	padc_hal = adc_hw_info.pdrv_data;

	pdev = (struct adc_dev*)kmalloc(sizeof(struct adc_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_ADC, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate adc private dev structure for adc iminor: %d\n", ADC_DEV_NAME, minor);
		return -ENOMEM;
	}

	pdev->padc_hal = padc_hal;
	file->private_data = pdev;

	return 0;
}


static int
adc_release(struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
    struct adc_dev *pdev = (struct adc_dev*)file->private_data;
	pdev->padc_hal = NULL;
    file->private_data = NULL;
    ret = hw_close (EDEV_TYPE_ADC, iminor(inode), &open_count);
    if(ret) { return -1; }
	kfree (pdev);
	return 0;
}


static long
adc_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg)
{
	get_adc_value_t	get_adc_value;
	uint16_t		ref_voltage, resolution;

	struct adc_dev *pdev = (struct adc_dev*) file->private_data;

    switch (cmd)
	{
		case READ_ADC_CHANNEL:
			
			if (copy_from_user ((void*)&get_adc_value, (void *)arg, sizeof (get_adc_value_t)))
			{
				printk("READ_ADC_CHANNEL: Error copying data from user \n");
				return -EFAULT;
			}

			if (pdev->padc_hal->padc_hal_ops->adc_read_channel)
				pdev->padc_hal->padc_hal_ops->adc_read_channel(&get_adc_value.channel_value, get_adc_value.channel_num);

			if (copy_to_user ((void*)arg, (void*)&get_adc_value, sizeof (get_adc_value_t)))
 			{
              		  	printk("READ_ADC_CHANNEL: Error copying data to user \n");
        		        return -EFAULT;
			}

			break;

		case READ_ADC_REF_VOLATGE:

			if (pdev->padc_hal->padc_hal_ops->adc_get_reference_voltage)
				pdev->padc_hal->padc_hal_ops->adc_get_reference_voltage(&ref_voltage);

			if (copy_to_user ((void*)arg, (void*)&ref_voltage, sizeof (uint16_t)))
 			{
              		  	printk("READ_ADC_REF_VOLTAGE: Error copying data to user \n");
        		        return -EFAULT;
			}
			break;

		case READ_ADC_RESOLUTION:

			if (pdev->padc_hal->padc_hal_ops->adc_get_resolution)
				pdev->padc_hal->padc_hal_ops->adc_get_resolution(&resolution);

			if (copy_to_user ((void*)arg, (void*)&resolution, sizeof (uint16_t)))
 			{
              		  	printk("READ_ADC_RESOLUTION: Error copying data to user \n");
        		        return -EFAULT;
			}
			break;
			
		default:
			printk("ERROR: ADC: Invalid IOCTL \n");
			return -EINVAL;

	}
	return 0;
}

#ifndef USE_UNLOCKED_IOCTL  
static int
adc_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg)
{
	return (adc_module_ioctlUnlocked(file, cmd, arg));
}
#endif
/*
 * adc_driver_sys_reboot_notify
 */
static int
adc_driver_sys_reboot_notify(struct notifier_block *this, unsigned long code, void *unused)
{
	struct adc_hal *padc_hal;
	struct adc_dev *pdev;
	hw_info_t adc_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_ADC, 0, &open_count, &adc_hw_info);
	if (ret)
		return NOTIFY_DONE;

	padc_hal = adc_hw_info.pdrv_data;

	pdev = (struct adc_dev*)kmalloc(sizeof(struct adc_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_ADC, 0 , &open_count);
		printk (KERN_ERR "%s: failed to allocate adc private dev structure for adc iminor: %d\n", ADC_DEV_NAME, 0);
		return NOTIFY_DONE;
	}

	pdev->padc_hal = padc_hal;

	if (code == SYS_DOWN || code == SYS_HALT)
	{
		if (pdev->padc_hal->padc_hal_ops->adc_reboot_notifier)
			pdev->padc_hal->padc_hal_ops->adc_reboot_notifier();
	}
	return NOTIFY_DONE;
}


module_init(adc_module_init);
module_exit(adc_module_exit);

MODULE_AUTHOR("Samvinesh Christopher");
MODULE_DESCRIPTION("ADC driver module");
MODULE_LICENSE("GPL");


int adc_core_loaded =1;
EXPORT_SYMBOL(adc_core_loaded);
