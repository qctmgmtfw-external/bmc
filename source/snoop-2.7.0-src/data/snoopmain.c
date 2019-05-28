/*****************************************************************
 **                                                             **
 **     (C) Copyright 2009-2015, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

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
#include <linux/kfifo.h>

#include "snoop.h"
#include "snoop_ioctl.h"
#include "reset.h"
#include "driver_hal.h"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
        #define USE_UNLOCKED_IOCTL  
  #endif  
#endif 

#define DRIVER_AUTHOR 	"American Megatrends Inc"
#define DRIVER_DESC 	"Snoop Common Driver"
#define DRIVER_LICENSE	"GPL"

/* Module information */
MODULE_AUTHOR( DRIVER_AUTHOR );
MODULE_DESCRIPTION( DRIVER_DESC );
MODULE_LICENSE ( DRIVER_LICENSE );

#ifdef DEBUG
#define dbgprint(fmt, args...)       printk (KERN_INFO fmt, ##args) 
#else
#define dbgprint(fmt, args...)       
#endif

#define SNOOP_MAJOR		44
#define SNOOP_MINOR		0
#define SNOOP_MAX_DEVICES	255
#define SNOOP_DEV_NAME		"snoop"
#define SNOOP_BUFSIZE		512

struct snoop_hal
{
    unsigned int num_instances;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
    struct kfifo current_codes[SNOOP_MAX_DEVICES];
    struct kfifo previous_codes[SNOOP_MAX_DEVICES];
#else
    struct kfifo* current_codes[SNOOP_MAX_DEVICES];
    struct kfifo* previous_codes[SNOOP_MAX_DEVICES];
#endif
    snoop_hal_operations_t* psnoop_hal_ops;
};

struct snoop_dev
{
	struct snoop_hal* psnoop_hal;
	unsigned char instance_num;
};

static int __init snoop_init(void);
static void __exit snoop_exit(void);

static int snoop_open (struct inode * inode, struct file *file);
#ifdef USE_UNLOCKED_IOCTL
static long snoop_ioctlUnlocked(struct file *file, unsigned int cmd, unsigned long arg);
#else
static int snoop_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
#endif
static int snoop_release (struct inode *inode, struct file *file);

static int handle_snoop_reset (void);
static inline void snoop_kfifo_get (struct kfifo *fifo, unsigned char *buffer, unsigned int len);

static struct file_operations snoop_fops = 
{
	owner:		THIS_MODULE,
	open:		snoop_open,
	release:	snoop_release,
#ifdef USE_UNLOCKED_IOCTL
	unlocked_ioctl:    snoop_ioctlUnlocked,
#else 
	ioctl:             snoop_ioctl
#endif
};


static struct cdev *snoop_cdev;
static dev_t snoop_devno = MKDEV(SNOOP_MAJOR, SNOOP_MINOR);
static char banner[] __initdata = KERN_INFO "Snoop Common Driver, (c) 2009-2015 American Megatrends Inc.\n";


/*********************************************************************************************/

static void __exit
snoop_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_SNOOP);
	unregister_chrdev_region (snoop_devno, SNOOP_MAX_DEVICES);

	if (NULL != snoop_cdev)
	{
		dbgprint ("snoop char device del\n");	
		cdev_del (snoop_cdev);
	}
	printk("Snoop common module unloaded successfully\n");
	return;
}

int
register_snoop_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct snoop_hal *psnoop_hal = NULL;
    int i=0;

	psnoop_hal = (struct snoop_hal*) kmalloc (sizeof(struct snoop_hal), GFP_KERNEL);
	if (!psnoop_hal)
	{
		return -ENOMEM;
	}

    for(i=0;i<num_instances;i++) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
		if ( kfifo_alloc (&psnoop_hal->current_codes[i], SNOOP_BUFSIZE, GFP_KERNEL) )
#else
        psnoop_hal->current_codes[i] = kfifo_alloc (SNOOP_BUFSIZE, GFP_KERNEL, NULL);
        if (!psnoop_hal->current_codes[i])
#endif
        {
            kfree (psnoop_hal);
            return -ENOMEM;
        }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
		if ( kfifo_alloc (&psnoop_hal->previous_codes[i], SNOOP_BUFSIZE, GFP_KERNEL) )
		{
			kfifo_free (&psnoop_hal->current_codes[i]);
#else
        psnoop_hal->previous_codes[i] = kfifo_alloc (SNOOP_BUFSIZE, GFP_KERNEL, NULL);
        if (!psnoop_hal->previous_codes[i])
        {
			kfifo_free (psnoop_hal->current_codes[i]);
#endif
			kfree (psnoop_hal);
            return -ENOMEM;
        }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
		psnoop_hal->current_codes[i].kfifo.mask = 0;
		psnoop_hal->previous_codes[i].kfifo.mask = 0;
#else
    	psnoop_hal->current_codes[i]->size = 0;
	    psnoop_hal->previous_codes[i]->size = 0;
        dbgprint ("adddr %p %p %p\n", psnoop_hal, psnoop_hal->current_codes[i], psnoop_hal->previous_codes[i]);
#endif
    }

    psnoop_hal->num_instances=num_instances;

	psnoop_hal->psnoop_hal_ops = (snoop_hal_operations_t *) phal_ops;

	*phw_data = (void *)psnoop_hal;

	install_reset_handler (handle_snoop_reset);
	return 0;
}


int
unregister_snoop_hal_module (void *phw_data)
{
	struct snoop_hal *psnoop_hal = (struct snoop_hal *)phw_data;
    int i=0;

	dbgprint ("unregister hal addr : %p\n", psnoop_hal);

	uninstall_reset_handler (handle_snoop_reset);
    for(i=0;i<psnoop_hal->num_instances;i++) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
		kfifo_free (&psnoop_hal->previous_codes[i]);
		kfifo_free (&psnoop_hal->current_codes[i]);
#else
		kfifo_free (psnoop_hal->previous_codes[i]);
		kfifo_free (psnoop_hal->current_codes[i]);
#endif
	}
	kfree (psnoop_hal);
	return 0;
}


static int 
snoop_open (struct inode * inode, struct file * file)
{
	struct snoop_hal *psnoop_hal;
	unsigned int minor = iminor(inode);
	struct snoop_dev* pdev;
	hw_info_t snoop_hw_info;
	int ret;
	unsigned char open_count;

	ret = hw_open (EDEV_TYPE_SNOOP, minor, &open_count, &snoop_hw_info);
	if (ret)
		return -ENXIO;

	psnoop_hal = snoop_hw_info.pdrv_data;

	pdev = (struct snoop_dev*) kmalloc (sizeof(struct snoop_dev), GFP_KERNEL);
	if (!pdev)
	{
		hw_close (EDEV_TYPE_SNOOP, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate snoop private dev structure for snoop iminor: %d\n", SNOOP_DEV_NAME, minor);
		return -ENOMEM;
	}

	pdev->psnoop_hal = psnoop_hal;
	pdev->instance_num = snoop_hw_info.inst_num;
	file->private_data = pdev;
	dbgprint ("%d, snoop_open priv data addr : %p\n", minor, &file->private_data);
	return nonseekable_open (inode, file);
}

static int snoop_release (struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
	
	struct snoop_dev *pdev = (struct snoop_dev*) file->private_data;
	pdev->psnoop_hal = NULL;
	ret = hw_close (EDEV_TYPE_SNOOP, iminor(inode), &open_count);
	if(ret) { return -1; }
	kfree (pdev);
	return 0;
}


static long
snoop_ioctlUnlocked (struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	struct snoop_dev* pdev = (struct snoop_dev*) file->private_data;
	unsigned char* buf = (unsigned char*) arg;
	unsigned char databuf[SNOOP_BUFSIZE];
	unsigned int size = 0;
	unsigned int minor = iminor(file->f_dentry->d_inode);

	switch (cmd)
	{
		case READ_PREVIOUS_CODES:
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
			size = pdev->psnoop_hal->previous_codes[minor].kfifo.mask;
			snoop_kfifo_get ( &pdev->psnoop_hal->previous_codes[minor], databuf, size );
#else
			size = pdev->psnoop_hal->previous_codes[minor]->size;
			snoop_kfifo_get ( pdev->psnoop_hal->previous_codes[minor], databuf, size );
#endif
			if ((ret = __copy_to_user( (void*) (buf), (void*) databuf, size )) != 0)
			{
				printk("READ_PREVIOUS_CODES: Error copying data to user \n");
				return ret;
			}
			break;

		case READ_CURRENT_CODES:
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
			dbgprint ("BR kfifo %d: %d, %d, %d\n", minor, pdev->psnoop_hal->current_codes[minor].kfifo.in , 
				pdev->psnoop_hal->current_codes[minor].kfifo.out , pdev->psnoop_hal->current_codes[minor].kfifo.mask );
			size = pdev->psnoop_hal->current_codes[minor].kfifo.mask;
			snoop_kfifo_get ( &pdev->psnoop_hal->current_codes[minor], databuf, size );
			dbgprint ("AR kfifo %d: %d, %d, %d\n", minor, pdev->psnoop_hal->current_codes[minor].kfifo.in , 
				pdev->psnoop_hal->current_codes[minor].kfifo.out , pdev->psnoop_hal->current_codes[minor].kfifo.mask );
#else
			dbgprint ("BR kfifo %d: %d, %d, %d\n", minor, pdev->psnoop_hal->current_codes[minor]->in , 
				pdev->psnoop_hal->current_codes[minor]->out , pdev->psnoop_hal->current_codes[minor]->size );
			size = pdev->psnoop_hal->current_codes[minor]->size;
			snoop_kfifo_get ( pdev->psnoop_hal->current_codes[minor], databuf, size );
			dbgprint ("AR kfifo %d: %d, %d, %d\n", minor, pdev->psnoop_hal->current_codes[minor]->in , 
				pdev->psnoop_hal->current_codes[minor]->out , pdev->psnoop_hal->current_codes[minor]->size );
#endif

#ifdef CONFIG_SPX_FEATURE_SNOOP_CLEAR_DATA_SUPPORT
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
			pdev->psnoop_hal->current_codes[minor].kfifo.mask = 0;
			pdev->psnoop_hal->current_codes[minor].kfifo.out = pdev->psnoop_hal->current_codes[minor].kfifo.in;
#else
				pdev->psnoop_hal->current_codes[minor]->size = 0;
				pdev->psnoop_hal->current_codes[minor]->out = pdev->psnoop_hal->current_codes[minor]->in;
#endif
#endif
			if ((ret = __copy_to_user( (void*) (buf), databuf, size )) != 0)
			{
				printk ("READ_CURRENT_CODES: Error copying data to user \n");
				return ret;
			}
			break;
			
		//Allow user space programs to Enable/Disable the snoop interrupts
		case ENABLE_SNOOP_IRQ:
			pdev->psnoop_hal->psnoop_hal_ops->enable_snoop_interrupt((int)arg);
			return ret;
				
		case DISABLE_SNOOP_IRQ:
			pdev->psnoop_hal->psnoop_hal_ops->disable_snoop_interrupt((int)arg);
			return ret;
				
		default:
			printk("unrecognized IOCTL call\n");
			return -1;
			break;
	}

	return size;
}

#ifndef USE_UNLOCKED_IOCTL  
static int
snoop_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
    return ( snoop_ioctlUnlocked (file, cmd, arg) );
}
#endif

static int
handle_snoop_reset (void)
{
	int i = 0, j=0;
	struct snoop_hal *psnoop_hal;
	unsigned int size = 0;

	for (i=0; i< SNOOP_MAX_DEVICES; i++)
	{
		psnoop_hal = hw_intr (EDEV_TYPE_SNOOP, i);
		if (!psnoop_hal)
			continue;

        for (j=0; j<psnoop_hal->num_instances; j++) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
			size = psnoop_hal->current_codes[j].kfifo.mask;
			snoop_kfifo_get ( &psnoop_hal->current_codes[j], (unsigned char *)psnoop_hal->previous_codes[j].kfifo.data, size);
			psnoop_hal->previous_codes[j].kfifo.mask = size;
			psnoop_hal->previous_codes[j].kfifo.in = size;
			psnoop_hal->previous_codes[j].kfifo.out = 0;
			psnoop_hal->current_codes[j].kfifo.mask = 0;
			psnoop_hal->current_codes[j].kfifo.in = 0;
			psnoop_hal->current_codes[j].kfifo.out = 0;
#else
            size = psnoop_hal->current_codes[j]->size;
            snoop_kfifo_get ( psnoop_hal->current_codes[j], psnoop_hal->previous_codes[j]->buffer, size);
            psnoop_hal->previous_codes[j]->size = size;
            psnoop_hal->previous_codes[j]->in = size;
            psnoop_hal->previous_codes[j]->out = 0;
            psnoop_hal->current_codes[j]->size = 0;
            psnoop_hal->current_codes[j]->in = 0;
            psnoop_hal->current_codes[j]->out = 0;
#endif
            psnoop_hal->psnoop_hal_ops->reset_snoop(j);
        }
	}
	return 0;
}


void
get_snoop_core_data ( struct kfifo ** pcurrent_codes, struct kfifo ** pprevious_codes, int dev_id, int ch_num )
{
	struct snoop_hal *psnoop_hal;

	psnoop_hal = hw_intr (EDEV_TYPE_SNOOP, dev_id);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	*pcurrent_codes = &psnoop_hal->current_codes[ch_num];
	*pprevious_codes = &psnoop_hal->previous_codes[ch_num];
#else
	*pcurrent_codes = psnoop_hal->current_codes[ch_num];
	*pprevious_codes = psnoop_hal->previous_codes[ch_num];
#endif
}


static void
snoop_kfifo_get (struct kfifo *fifo, unsigned char *buffer, unsigned int length)
{
	unsigned int l;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	if ( fifo->kfifo.in < fifo->kfifo.out )
	{
		l = fifo->kfifo.mask - fifo->kfifo.out;
		memcpy ( buffer, fifo->kfifo.data + (fifo->kfifo.out - 1)%SNOOP_BUFSIZE , l+1 );
		memcpy ( &buffer[l+1], fifo->kfifo.data, fifo->kfifo.out-1 );
	}
	else
		memcpy ( buffer, fifo->kfifo.data + fifo->kfifo.out, fifo->kfifo.in - fifo->kfifo.out );
#else
	if ( fifo->in < fifo->out )
	{
		l = fifo->size - fifo->out;
		memcpy ( buffer, &fifo->buffer[ (fifo->out - 1)%SNOOP_BUFSIZE ], l+1 );
		memcpy ( &buffer[l+1], &fifo->buffer[0], fifo->out-1 );
	}
	else
		memcpy ( buffer, &fifo->buffer[ fifo->out ], fifo->in - fifo->out );
#endif
}


/* ------------------- Driver registration ------------------------------- */

static snoop_core_funcs_t snoop_core_funcs = {
	.get_snoop_core_data = get_snoop_core_data,
};

static core_hal_t snoop_core_hal = {
        .owner                 = THIS_MODULE,
        .name                  = "SNOOP CORE",
        .dev_type              = EDEV_TYPE_SNOOP,
        .register_hal_module   = register_snoop_hal_module,
        .unregister_hal_module = unregister_snoop_hal_module,
        .pcore_funcs           = (void *)&snoop_core_funcs
};


static int __init
snoop_init(void)
{
	int ret = 0;
	printk (banner);

	if ((ret = register_chrdev_region (snoop_devno, SNOOP_MAX_DEVICES, SNOOP_DEV_NAME)) < 0)
	{
		printk (KERN_ERR "failed to register snoop device <%s> (err: %d)\n", SNOOP_DEV_NAME, ret);
		return ret;
	}

	snoop_cdev = cdev_alloc();
	if (!snoop_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate snoop cdev structure\n", SNOOP_DEV_NAME);
		unregister_chrdev_region (snoop_devno, SNOOP_MAX_DEVICES);
		return -1;
	}

	cdev_init (snoop_cdev, &snoop_fops);

	if ((ret = cdev_add (snoop_cdev, snoop_devno, SNOOP_MAX_DEVICES)) < 0)
	{
		printk  (KERN_ERR "failed to add <%s> char device\n", SNOOP_DEV_NAME);
		cdev_del (snoop_cdev);
		unregister_chrdev_region (snoop_devno, SNOOP_MAX_DEVICES);
		ret=-ENODEV;	
		return ret;
	}

	ret = register_core_hal_module (&snoop_core_hal);
	if (ret < 0)
	{
		cdev_del (snoop_cdev);
		unregister_chrdev_region (snoop_devno, SNOOP_MAX_DEVICES);
		printk("error in registering the core snoop module\n");
		ret=-EINVAL;
		return ret;
	}
	return ret;
}


module_init (snoop_init);
module_exit (snoop_exit);

int snoop_core_loaded =1;
EXPORT_SYMBOL(snoop_core_loaded);
