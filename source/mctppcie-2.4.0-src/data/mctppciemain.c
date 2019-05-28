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

/*
 * PECI Common Driver
 *
 * Copyright (C) 2009 American Megatrends Inc.
 *
 * This driver provides common layer, independent of the hardware, for the PECI driver.
 */

#include <linux/version.h>
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,28))
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
#include <linux/poll.h>
//#include <linux/ide.h>
#include <asm/delay.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
#include <linux/slab.h>
#endif

#include "mctppcie.h"
#include "mctppcieifc.h"
#include "driver_hal.h"
#include "dbgout.h"	
#include "helper.h"


#define DRIVER_AUTHOR "American Megatrends Inc"
#define DRIVER_DESC "MCTP PCIE Common Driver"
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

struct mctppcie_hal
{
	mctppcie_hal_operations_t *pmctppcie_hal_ops;
};

struct mctppcie_dev
{
	struct mctppcie_hal *pmctppcie_hal;
};

/* Global Variables */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
DEFINE_SEMAPHORE (mutex);
#else
DECLARE_MUTEX (mutex);
#endif
TDBG_DECLARE_DBGVAR(mctppeci);
static int core_hal_module_registered;

static struct cdev *mctppcie_cdev;
static dev_t mctppcie_devno = MKDEV(MCTP_PCIE_MAJOR, MCTP_PCIE_MINOR);
static char banner[] __initdata = KERN_INFO "MCTP PCIE Common Driver, (c) 2014 American Megatrends Inc.\n";

/*-----------------------------------------------*
 **         Prototype Declaration       **
 *-----------------------------------------------*/

static int  __init mctppcie_init(void);
static void __exit mctppcie_exit(void);

static int mctppcie_open(struct inode * inode, struct file * filp);
static int mctppcie_close(struct inode * inode, struct file * filp);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
static long mctppcie_ioctl(struct file *file,unsigned int cmd, unsigned long param);
#else
static int mctppcie_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long param);
#endif

static int register_mctppcie_hal_module(unsigned char num_instances, void *phal_ops, void **phw_data);
static int unregister_mctppcie_hal_module(void *phw_data);

static core_hal_t pcie_core_hal = {
	.owner		       = THIS_MODULE,
	.name		       = "MCTP PCIE CORE",
	.dev_type              = EDEV_TYPE_MCTP_PCIE,
	.register_hal_module   = register_mctppcie_hal_module,
	.unregister_hal_module = unregister_mctppcie_hal_module,
	//.pcore_funcs           = (void *)&pcie_core_funcs
	.pcore_funcs           = NULL
};

static struct file_operations pcie_fops = 
{
	owner:		THIS_MODULE,
	write:		NULL,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
	unlocked_ioctl:		mctppcie_ioctl,
#else
	ioctl:		mctppcie_ioctl,
#endif	
	open:		mctppcie_open,
	release:	mctppcie_close,
};

static int 
mctppcie_open(struct inode * inode, struct file * file)
{
	struct mctppcie_dev	*pdev;
	hw_info_t pcie_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_MCTP_PCIE, iminor(inode), &open_count, &pcie_hw_info);
	if (ret)
		return -ENXIO;
	
	pdev = (struct mctppcie_dev*) kmalloc (sizeof(struct mctppcie_dev), GFP_KERNEL);
	if (!pdev)
	{
		hw_close (EDEV_TYPE_MCTP_PCIE, iminor(inode), &open_count);
		printk (KERN_ERR "%s: failed to allocate pdev structure for mctp peci iminor: %d\n", MCTP_PCIE_DEV_NAME, iminor(inode));
		return -ENOMEM;
	}
	pdev->pmctppcie_hal = pcie_hw_info.pdrv_data;
	
	file->private_data = pdev;		

	dbgprint ("%d, mctp peci_open priv data addr : %p\n", iminor(inode), &file->private_data);	
	return nonseekable_open (inode, file);	
}

static int 
mctppcie_close(struct inode * inode, struct file * file)
{
	int ret;
	unsigned char open_count;
	struct mctppcie_dev *pdev= (struct mctppcie_dev*) file->private_data;
	
	dbgprint("MCTP PCIE CORE: device close \n");
	dbgprint ("%d,pcie_release priv data addr : %p\n", iminor(inode), pdev);

	pdev->pmctppcie_hal = NULL;
	ret = hw_close (EDEV_TYPE_MCTP_PCIE, iminor(inode), &open_count);
	if(ret) { return -1; };
	kfree (pdev);
	return 0;
}

/* --------------- Core Functions of PECI Handling --------------------*/

int register_mctppcie_hal_module(unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct mctppcie_hal *pmctppcie_hal;

	pmctppcie_hal = (struct mctppcie_hal*) kmalloc (sizeof(struct mctppcie_hal), GFP_KERNEL);
	if (!pmctppcie_hal)
	{
		return -ENOMEM;
	}
	
	pmctppcie_hal->pmctppcie_hal_ops = (mctppcie_hal_operations_t *) phal_ops;
	*phw_data = (void *)pmctppcie_hal;
	dbgprint ("private data: %p, %p\n", pmctppcie_hal, *phw_data);

	return 0;
}

int unregister_mctppcie_hal_module(void *phw_data)
{
	struct mctppcie_hal *pmctppcie_hal = (struct mctppcie_hal *)phw_data;
	
	if(!pmctppcie_hal)
		return -ENOMEM;
	
	dbgprint ("unregister hal addr : %p\n", pmctppcie_hal);

	kfree (pmctppcie_hal);

	return 0;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,28))
static long 
mctppcie_ioctl(struct file *file, unsigned int cmd, unsigned long param)
#else
static int 
mctppcie_ioctl(struct inode *inode, struct file *file,
		     unsigned int cmd, unsigned long param)
#endif
{

	struct mctppcie_dev *pdev= (struct mctppcie_dev*) file->private_data;
	struct mctppcie_hal *pmctppcie_hal=(struct mctppcie_hal*)pdev->pmctppcie_hal;
	mctp_iodata iodata;
	int ret = 0;

	//printk ( "ioctl [%u] called\n", cmd );
	switch (cmd)
	{

		case MCTP_PCIE_IOCTL_READ:

			if(copy_from_user(&iodata,(void *)param, sizeof(mctp_iodata)))
				return -EFAULT;
			
			ret = pmctppcie_hal->pmctppcie_hal_ops->downstream_mctp_pkt(&iodata);

			// Update the iodata structure and copy_to_user
			if(copy_to_user((void *)param, &iodata, sizeof(mctp_iodata)))
				return -EFAULT;
		break;
		
		case MCTP_PCIE_IOCTL_WRITE:

			if(copy_from_user(&iodata,(void *)param, sizeof(mctp_iodata)))
				return -EFAULT;

			ret = pmctppcie_hal->pmctppcie_hal_ops->upstream_mctp_pkt(&iodata);

			if(copy_to_user((void *)param, &iodata, sizeof(mctp_iodata)))
				return -EFAULT;
		break;

		case MCTP_PCIE_IOCTL_CAPABILITIES:

			ret = pmctppcie_hal->pmctppcie_hal_ops->enable_mctp_pkt( );

			printk("Enable MCTP over PCIE Downstream function\n");

		break;
		
		default:
			printk("ERROR: MCTP over PCIE: Invalid IOCTL \n");
	}
	return ret;
}

/* ----- Driver registration ---------------------------------------------- */

static void __exit
mctppcie_exit(void)
{

	unregister_chrdev_region (mctppcie_devno, MCTP_PCIE_MAX_DEVICES);

	if (NULL != mctppcie_cdev)
	{
		dbgprint ("pcie char device del\n");
		cdev_del (mctppcie_cdev);
	}
	
	if(core_hal_module_registered)
	{
		unregister_core_hal_module (EDEV_TYPE_MCTP_PCIE);
		core_hal_module_registered = 0;
	}

	return;
}

static int __init
mctppcie_init(void)
{
	int ret=0,err;

	printk (banner);

	core_hal_module_registered = 0;

 	/* ----  Register the character device ------------------- */
	if ((ret = register_chrdev_region (mctppcie_devno, MCTP_PCIE_MAX_DEVICES, MCTP_PCIE_DEV_NAME)) < 0)
	{
		printk ("failed to register device mctp peci device <%s> (err: %d)\n", MCTP_PCIE_DEV_NAME, ret);
		return ret;
	}
	
	mctppcie_cdev = cdev_alloc();
	if (!mctppcie_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate mctp peci cdev structure\n", MCTP_PCIE_DEV_NAME);
		ret = -1;
		goto clean;
	}

	cdev_init (mctppcie_cdev, &pcie_fops);

	if ((ret = cdev_add (mctppcie_cdev, mctppcie_devno, MCTP_PCIE_MAX_DEVICES)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", MCTP_PCIE_DEV_NAME);
		ret=-ENODEV;
		goto clean;
	}
	
	if ((err = register_core_hal_module (&pcie_core_hal)) < 0)
	{
		printk (KERN_ERR "ERROR: Failed to register core hal module (err %d)\n", err);
		ret=-EINVAL;
		goto clean;
	}
	
	core_hal_module_registered = 1;
	
	return ret;

clean:
	unregister_chrdev_region (mctppcie_devno, MCTP_PCIE_MAX_DEVICES);

	if (NULL != mctppcie_cdev)
	{
		dbgprint ("pcie char device del\n");
		cdev_del (mctppcie_cdev);
	}
	
	return ret;
}


module_init (mctppcie_init);
module_exit (mctppcie_exit);

int mctppcie_core_loaded =1;
EXPORT_SYMBOL(mctppcie_core_loaded);
