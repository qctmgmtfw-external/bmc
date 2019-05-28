/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
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
 * ssp_mod.c
 * SSP driver Module functions
 *
 *****************************************************************/
#define SSP_DRIVER_MAJOR	1
#define SSP_DRIVER_MINOR    0

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
#include <linux/platform_device.h>
#include <linux/firmware.h>
#include <linux/vmalloc.h>

#include "helper.h"
#include "driver_hal.h"
#include "ssp.h"


/* Local Macros */
#define SSP_MAJOR_NUM		250
#define SSP_MINOR_NUM		0
#define SSP_MAX_DEVICES     1
#define SSP_DEV_NAME        "ssp"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
	#define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

static dev_t ssp_devno = MKDEV(SSP_MAJOR_NUM, SSP_MINOR_NUM);
static struct cdev *ssp_cdev;

#if defined (SOC_PILOT_IV)
static void *ssp_load_addr = NULL;
#endif

#ifdef USE_UNLOCKED_IOCTL
static long ssp_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg);
#else
static int ssp_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg);
#endif
static ssp_metadata_t *ssp_metadata = NULL;

static int 	ssp_driver_sys_reboot_notify(struct notifier_block *this, unsigned long code, void *unused);
static int ssp_open(struct inode *inode, struct file *file);
static int ssp_release(struct inode *inode, struct file *file);

#if defined (SOC_PILOT_IV)
static int ssp_loadfirmware(ssp_fw_t  *ssp_fw);
#endif

/* Module Variables	*/
static struct file_operations ssp_ops = {
        owner:      THIS_MODULE,
        read:       NULL,
        write:      NULL,
#ifdef USE_UNLOCKED_IOCTL 
        unlocked_ioctl:	ssp_module_ioctlUnlocked, 
#else 
        ioctl:      ssp_module_ioctl,
#endif 
        open:       ssp_open,
        release:    ssp_release,
};


static struct notifier_block ssp_driver_notifier =
{
       .notifier_call = ssp_driver_sys_reboot_notify,
};

/* Debug Level for the driver - Controlled by sysctl */
static int mdebuglevel = 0;
static struct ctl_table_header *ssp_sys 	= NULL;

/* Sysctl related tables */
static struct ctl_table SSPSysCtlTable[] =
{

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	{"dbglevel",&mdebuglevel,sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else
	{CTL_UNNUMBERED,"dbglevel",&mdebuglevel,sizeof(int),0644,NULL,NULL,&proc_dointvec},
#endif	
	{0}
};


int
register_ssp_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct ssp_hal* phw_hal;

	phw_hal = (struct ssp_hal*) kmalloc (sizeof(struct ssp_hal), GFP_KERNEL);
	if (!phw_hal)
	{
		return -ENOMEM;
	}

	phw_hal->pssp_hal_ops = ( ssp_hal_operations_t *) phal_ops;
	*phw_data = (void *) phw_hal;

	return 0;
}

int
unregister_ssp_hal_module (void *phw_data)
{
	struct ssp_hal *pssp_hal = (struct ssp_hal*) phw_data;

	kfree (pssp_hal);

	return 0;
}

/* ----- Driver registration ---------------------------------------------- */

static core_hal_t ssp_core_hal = {
	.owner		           = THIS_MODULE,
	.name		           = "SSP CORE",
	.dev_type              = EDEV_TYPE_SSP,
	.register_hal_module   = register_ssp_hal_module,
	.unregister_hal_module = unregister_ssp_hal_module,
};

static int
ssp_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct ssp_hal *pssp_hal;
	struct ssp_dev *pdev;
	hw_info_t ssp_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_SSP, minor, &open_count, &ssp_hw_info);
	if (ret)
		return -ENXIO;

	pssp_hal = ssp_hw_info.pdrv_data;

	pdev = (struct ssp_dev*)kmalloc(sizeof(struct ssp_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_SSP, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate ssp private dev structure for ssp iminor: %d\n", SSP_DEV_NAME, minor);
		return -ENOMEM;
	}

	pdev->pssp_hal = pssp_hal;
	file->private_data = pdev;
	
	ssp_metadata = (ssp_metadata_t *) vmalloc (sizeof(ssp_metadata_t));
	if (!ssp_metadata)
	{
		return -ENOMEM;
	}

	if (pdev->pssp_hal->pssp_hal_ops->ssp_get_metadata)
		pdev->pssp_hal->pssp_hal_ops->ssp_get_metadata(&ssp_metadata);

	return 0;
}


static int
ssp_release(struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
    struct ssp_dev *pdev = (struct ssp_dev*)file->private_data;
	pdev->pssp_hal = NULL;
    file->private_data = NULL;
    ret = hw_close (EDEV_TYPE_SSP, iminor(inode), &open_count);
    if(ret) { return -1; }
	kfree (pdev);
	vfree(ssp_metadata);

	return 0;
}

static long
ssp_module_ioctlUnlocked(struct file *file, uint cmd, ulong arg)
{
	ssp_fw_t ssp_fw;
	u32 oem_data_len = 0;
	void *oem_data_ptr = NULL;
	
	struct ssp_dev *pdev = (struct ssp_dev*) file->private_data;
	
    switch (cmd)
	{
		case SSP_IOCTL_START:
			

			ssp_metadata->processor_status = SSP_STATUS_RUNNING;

			if (pdev->pssp_hal->pssp_hal_ops->ssp_start)
				pdev->pssp_hal->pssp_hal_ops->ssp_start();

			break;

		case SSP_IOCTL_STOP:

			if (pdev->pssp_hal->pssp_hal_ops->ssp_stop)
				pdev->pssp_hal->pssp_hal_ops->ssp_stop();				
			ssp_metadata->processor_status = SSP_STATUS_HALT;
			break;

		case SSP_IOCTL_RESET:

			ssp_metadata->processor_status = SSP_STATUS_HALT;
			
			if (pdev->pssp_hal->pssp_hal_ops->ssp_reset)
				pdev->pssp_hal->pssp_hal_ops->ssp_reset();
			
			ssp_metadata->processor_status = SSP_STATUS_RUNNING;
			break;

		case SSP_IOCTL_LOADFIRMWARE:
			
			if (copy_from_user ((void*)&ssp_fw, (void *)arg, sizeof (ssp_fw_t)))
			{
				printk("SSP_IOCTL_LOADFIRMWARE: Error copying data from user \n");
				return -EFAULT;
			}
			
			printk("firmware file name is %s\n", ssp_fw.path);
#if !defined (SOC_PILOT_IV)			
			if (pdev->pssp_hal->pssp_hal_ops->ssp_loadfirmware)
				pdev->pssp_hal->pssp_hal_ops->ssp_loadfirmware(&ssp_fw);
#else
			if ( ssp_loadfirmware(&ssp_fw))
				return -EFAULT;
#endif
			break;
			
		case SSP_IOCTL_METADATA:

			if (copy_to_user ((void*)arg, (void*)ssp_metadata, sizeof(ssp_metadata_t)))
			{
				printk("SSP_RESET: Error copying data to user \n");
				return -EFAULT;
			}
			break;
			
		case SSP_IOCTL_GET_OEMDATA:
			
			if(ssp_metadata->processor_status != SSP_STATUS_RUNNING)
			{
				printk("Please initial SSP!");
				return -EFAULT;
			}
			
			if(ssp_metadata->oem_data_status != SSP_OEM_DATA_READY)
			{
				printk("SSP_IOCTL_GET_OEMDATA: Please do the init oem data, before get!");
				return -EFAULT;
			}

			if (pdev->pssp_hal->pssp_hal_ops->ssp_get_oem_data)
				oem_data_len = pdev->pssp_hal->pssp_hal_ops->ssp_get_oem_data(oem_data_ptr);
			
			if (copy_to_user ((void*)arg, (void*)&oem_data_len, sizeof (oem_data_len)))
			{
				printk("SSP_RESET: Error copying data to user \n");
				return -EFAULT;
			}
			if (copy_to_user ((void*)arg, (void*)oem_data_ptr, oem_data_len))
			{
				printk("SSP_RESET: Error copying data to user \n");
				return -EFAULT;
			}
			break;
		case SSP_IOCTL_SET_OEMDATA:
			if (copy_from_user ((void*)&oem_data_len, (void *)arg, sizeof (oem_data_len)))
			{
				printk("SSP_IOCTL_INIT_OEMDATA_STRUCT: Error copying data len from user \n");
				return -EFAULT;
			}
			
			oem_data_ptr = kmalloc(oem_data_len, GFP_ATOMIC);
			
			if(copy_from_user ((void*)oem_data_ptr, (void *)arg, oem_data_len))
			{
				printk("SSP_IOCTL_INIT_OEMDATA_STRUCT: Error copying data from user \n");
				return -EFAULT;
			}
			
			if (pdev->pssp_hal->pssp_hal_ops->ssp_set_oem_data)
				pdev->pssp_hal->pssp_hal_ops->ssp_set_oem_data(oem_data_len, oem_data_ptr);
			
			kfree(oem_data_ptr);

			ssp_metadata->oem_data_status = SSP_OEM_DATA_READY;
			
			break;
		default:
			printk("ERROR: SSP: Invalid IOCTL \n");
			return -EINVAL;

	}
	return 0;
}

#ifndef USE_UNLOCKED_IOCTL  
static int
ssp_module_ioctl(struct inode *inode, struct file *file, uint cmd, ulong arg)
{
	return (ssp_module_ioctlUnlocked(file, cmd, arg));
}
#endif
/*
 * ssp_driver_sys_reboot_notify
 */
static int
ssp_driver_sys_reboot_notify(struct notifier_block *this, unsigned long code, void *unused)
{
	struct ssp_hal *pssp_hal;
	struct ssp_dev *pdev;
	hw_info_t ssp_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_SSP, 0, &open_count, &ssp_hw_info);
	if (ret)
		return NOTIFY_DONE;

	pssp_hal = ssp_hw_info.pdrv_data;

	pdev = (struct ssp_dev*)kmalloc(sizeof(struct ssp_dev), GFP_KERNEL);

	if (!pdev)
	{
		hw_close (EDEV_TYPE_SSP, 0 , &open_count);
		printk (KERN_ERR "%s: failed to allocate ssp private dev structure for ssp iminor: %d\n", SSP_DEV_NAME, 0);
		return NOTIFY_DONE;
	}

	pdev->pssp_hal = pssp_hal;

	if (code == SYS_DOWN || code == SYS_HALT)
	{
		if (pdev->pssp_hal->pssp_hal_ops->ssp_reboot_notifier)
			pdev->pssp_hal->pssp_hal_ops->ssp_reboot_notifier();
	}
	return NOTIFY_DONE;
}


int __init
ssp_module_init(void)
{
	int	ret;

	printk("SSP Driver Version %d.%d\n", SSP_DRIVER_MAJOR, SSP_DRIVER_MINOR);
	printk("Copyright (c) 2014 American Megatrends Inc.\n");

 	/* SSP device initialization */
	if ((ret = register_chrdev_region (ssp_devno, SSP_MAX_DEVICES, SSP_DEV_NAME)) < 0)
	{
	   printk (KERN_ERR "failed to register ssp device <%s> (err: %d)\n", SSP_DEV_NAME, ret);
	   return -1;
	}

	ssp_cdev = cdev_alloc ();
	if (!ssp_cdev)
	{
	   unregister_chrdev_region (ssp_devno, SSP_MAX_DEVICES);
	   printk (KERN_ERR "%s: failed to allocate ssp cdev structure\n", SSP_DEV_NAME);
	   return -1;
	}

	cdev_init (ssp_cdev, &ssp_ops);
	ssp_cdev->owner = THIS_MODULE;

	if ((ret = cdev_add (ssp_cdev, ssp_devno, SSP_MAX_DEVICES)) < 0)
	{
		cdev_del (ssp_cdev);
		unregister_chrdev_region (ssp_devno, SSP_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", SSP_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}

	if ((ret = register_core_hal_module (&ssp_core_hal)) < 0)
	{
		printk(KERN_ERR "failed to register the Core SSP module\n");
		cdev_del (ssp_cdev);
		unregister_chrdev_region (ssp_devno, SSP_MAX_DEVICES);
		ret = -EINVAL;
		return ret;
	}


   	register_reboot_notifier(&ssp_driver_notifier);

	/* Add sysctl to access the DebugLevel */
	ssp_sys  = AddSysctlTable("ssp",&SSPSysCtlTable[0]);
	if (!ssp_sys)
	{
		printk (KERN_CRIT "ssp_mod.c: failed to register ssp sysctl table hierarchy\n");
		unregister_core_hal_module (EDEV_TYPE_SSP);
		unregister_chrdev_region (ssp_devno, SSP_MAX_DEVICES);

		if (NULL != ssp_cdev)
			cdev_del (ssp_cdev);

		unregister_reboot_notifier(&ssp_driver_notifier);
		return -EBUSY;

	}

#if defined (SOC_PILOT_IV)
	if((ssp_load_addr = ioremap_nocache(SSP_LOAD_ADDR, CONFIG_SPX_FEATURE_GLOBAL_SSP_MEMORY_SIZE)) == NULL)
	{
		printk("failed to map ssp memory space\n");
		ret = -EIO;
	}
#endif
	
	return 0;
}

static void
ssp_module_exit(void)
{
	printk ("Unloading SSP driver ..\n");

	unregister_core_hal_module (EDEV_TYPE_SSP);

	unregister_chrdev_region (ssp_devno, SSP_MAX_DEVICES);

	if (NULL != ssp_cdev)
	{
		cdev_del (ssp_cdev);
	}

	/* Remove driver related sysctl entries */
	RemoveSysctlTable(ssp_sys);

	unregister_reboot_notifier(&ssp_driver_notifier);
	
#if defined (SOC_PILOT_IV)	
	if (ssp_load_addr)
	{
		iounmap(ssp_load_addr);
		ssp_load_addr = NULL;
	}
#endif

}

module_init(ssp_module_init);
module_exit(ssp_module_exit);

MODULE_AUTHOR("Adam Lin");
MODULE_DESCRIPTION("SSP driver module");
MODULE_LICENSE("GPL");


int ssp_core_loaded =1;
EXPORT_SYMBOL(ssp_core_loaded);

#if defined (SOC_PILOT_IV)
static int ssp_loadfirmware(ssp_fw_t  *ssp_fw)
{
	int ret=0; 
	const struct firmware *fw;
	struct platform_device *pdev;
	
	if (!ssp_fw)
		return -EINVAL;

	pdev = platform_device_register_simple("sspFirmware", 0, NULL, 0);
	
	if (IS_ERR(pdev)) {
		 printk(KERN_ERR "Failed to register device for \"%s\"\n",
				 ssp_fw->path);
		 return -EINVAL;
	}
	
	printk("%s firmware file name is %s\n", __FUNCTION__, ssp_fw->path);
	
	ret = request_firmware(&fw, ssp_fw->path, &pdev->dev);
	
	if (ret) 
	{
        printk(KERN_ERR "Failed to load image \"%s\" err %d\n", ssp_fw->path, ret);
        goto out;
	}
	
	if ( fw->size > CONFIG_SPX_FEATURE_GLOBAL_SSP_MEMORY_SIZE )
	{
		uint32_t reserveSize = CONFIG_SPX_FEATURE_GLOBAL_SSP_MEMORY_SIZE;
	    printk(KERN_ERR "firmware size 0x%x is bigger than 0X%x\n", fw->size, reserveSize);
		goto out;		
	}
	
	memcpy_toio(ssp_load_addr, fw->data, fw->size);
	
	printk("%s is loaded at physical addr0x%x, IO addr 0x%x\n", ssp_fw->path, (unsigned int)SSP_LOAD_ADDR,(unsigned int)ssp_load_addr);
	
	
out:
	platform_device_unregister(pdev);
	release_firmware(fw);
	
	return ret;
	
}
#endif
