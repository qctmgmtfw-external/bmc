/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

 /*
 * File name: jtagmain.c
 * This driver provides common layer, independent of the hardware, for the JTAG driver.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include "helper.h"
#include "driver_hal.h"
#include "dbgout.h"
#include "jtag.h"
#include "jtag_ioctl.h"

#ifdef HAVE_UNLOCKED_IOCTL
  #if HAVE_UNLOCKED_IOCTL
  #define USE_UNLOCKED_IOCTL
  #endif
#endif

#define JTAG_MAJOR           175
#define JTAG_MINOR	    	   0
#define JTAG_MAX_DEVICES     255
#define JTAG_DEV_NAME        "jtag"

#define AST_JTAG_BUFFER_SIZE 0x40000  // Quanta modify to 0x40000 from 0x10000
#define AST_FW_BUFFER_SIZE  0x200000  // Quanta modify to 2048KB from 0x80000  //512KB

static struct cdev *jtag_cdev;
static dev_t jtag_devno = MKDEV(JTAG_MAJOR, JTAG_MINOR);
static jtag_hw_device_operations_t *pjhwd_ops = NULL;

unsigned int *JTAG_read_buffer = NULL;
unsigned int *JTAG_write_buffer= NULL;
unsigned long *JTAG_other_buffer= NULL;

JTAG_DEVICE_INFO	JTAG_device_information;


int register_jtag_hw_device_ops (jtag_hw_device_operations_t *pjhwd)
{
	pjhwd_ops = pjhwd;

	return 0;
}


int unregister_jtag_hw_device_ops (void)
{
	if(pjhwd_ops != NULL){
			pjhwd_ops = NULL;
	}

	return 0;
}


int register_jtag_hal_module (unsigned char num_instances, void *phal_ops, void **phw_data)
{
	struct jtag_hal *pjtag_hal;

	pjtag_hal = (struct jtag_hal*) kmalloc (sizeof(struct jtag_hal), GFP_KERNEL);
	if (!pjtag_hal)
	{
		return -ENOMEM;
	}

	pjtag_hal->pjtag_hal_ops = ( jtag_hal_operations_t *) phal_ops;
	*phw_data = (void *) pjtag_hal;	

	return 0;	
}


int unregister_jtag_hal_module (void *phw_data)
{
	struct jtag_hal *pjtag_hal = (struct jtag_hal*) phw_data;

	kfree (pjtag_hal);

	return 0;
}


/*
 * get_write_buffer
 */
unsigned int* get_jtag_write_buffer(void)
{
	return JTAG_write_buffer;
}


/*
 * get_read_buffer
 */
unsigned int* get_jtag_read_buffer(void)
{
	return JTAG_read_buffer;
}


static int jtag_open(struct inode *inode, struct file *file)
{
	unsigned int minor = iminor(inode);
	struct jtag_hal *pjtag_hal;
	struct jtag_dev *pdev;
	hw_info_t jtag_hw_info;
	unsigned char open_count;
	int ret;

	ret = hw_open (EDEV_TYPE_JTAG, minor,&open_count, &jtag_hw_info);
	if (ret)
		return -ENXIO;

	pjtag_hal = jtag_hw_info.pdrv_data;

	pdev = (struct jtag_dev*)kmalloc(sizeof(struct jtag_dev), GFP_KERNEL);
	
	if (!pdev)
	{
		hw_close (EDEV_TYPE_JTAG, minor, &open_count);
		printk (KERN_ERR "%s: failed to allocate jtag private dev structure for jtag iminor: %d\n", JTAG_DEV_NAME, minor);
		return -ENOMEM;
	}

	pdev->pjtag_hal = pjtag_hal;
	file->private_data = pdev;
	if ( open_count > 0) // Quanta
		pdev->pjtag_hal->pjtag_hal_ops->enable_jtag(); // Quanta

	return 0;
}


static int jtag_release(struct inode *inode, struct file *file)
{
	int ret;
	unsigned char open_count;
  struct jtag_dev *pdev = (struct jtag_dev*)file->private_data;
  struct jtag_hal *pjtag_hal = pdev->pjtag_hal; // Quanta
	pdev->pjtag_hal = NULL;
  file->private_data = NULL;
  ret = hw_close (EDEV_TYPE_JTAG, iminor(inode), &open_count);
	if ( open_count == 0) // Quanta
		pjtag_hal->pjtag_hal_ops->disable_jtag(); // Quanta
  if(ret) { return -1; }
	kfree (pdev);
	return 0;
}

#ifdef USE_UNLOCKED_IOCTL
static long jtag_ioctl(struct file *file,unsigned int cmd, unsigned long arg)
#else
static int jtag_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg)
#endif
{
	struct jtag_dev *pdev = (struct jtag_dev*) file->private_data;
	unsigned long	idcode;
	IO_ACCESS_DATA Kernal_IO_Data;
	int ret = 0;

	memset (&Kernal_IO_Data, 0x00, sizeof(IO_ACCESS_DATA));
	Kernal_IO_Data = *(IO_ACCESS_DATA *)arg;	


	switch (cmd)
	{
		case IOCTL_IO_READ:
			Kernal_IO_Data.Data  = *(u32 *)(IO_ADDRESS(Kernal_IO_Data.Address));
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_IO_WRITE:
			*(u32 *)(IO_ADDRESS(Kernal_IO_Data.Address)) = Kernal_IO_Data.Data;
			break;
			
		case IOCTL_JTAG_RESET:
			pdev->pjtag_hal->pjtag_hal_ops->reset_jtag();
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_JTAG_IDCODE_READ:
			//pjhwd_ops->get_hw_device_idcode(&idcode);
			ret=pjhwd_ops->get_hw_device_idcode(&idcode);
			if(ret!=0) ret=-1;

			Kernal_IO_Data.Data = idcode;
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_JTAG_ERASE_DEVICE:
			Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_ERASE_DEVICE, 0, 0);
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_JTAG_PROGRAM_DEVICE:
			if(Kernal_IO_Data.Data * 8 != JTAG_device_information.Device_All_Bits_Length){
				Kernal_IO_Data.Data= 1;
				printk("%s: Oops~ program bits should be %d!\n", JTAG_DEV_NAME, (int)JTAG_device_information.Device_All_Bits_Length);
			}
			else{
    		ret = copy_from_user ((u32 *)JTAG_write_buffer, (u8 *)Kernal_IO_Data.Input_Buffer_Base, Kernal_IO_Data.Data);
    		Kernal_IO_Data.Data  = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_PROGRAM_DEVICE,(void*)JTAG_write_buffer, Kernal_IO_Data.Data * 8);
      }
      *(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_JTAG_VERIFY_DEVICE:
    	if(Kernal_IO_Data.Data * 8 != JTAG_device_information.Device_All_Bits_Length){
    		printk("%s: Oops~ verify bits should be %d!,\nYou send %d bits.\n", JTAG_DEV_NAME, (int)JTAG_device_information.Device_All_Bits_Length,(int)Kernal_IO_Data.Data * 8);
    		Kernal_IO_Data.Data= 1;
			}
			else{
				ret = copy_from_user ((u32 *)JTAG_write_buffer, (u8 *)Kernal_IO_Data.Input_Buffer_Base, Kernal_IO_Data.Data);
        Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_VERIFY_DEVICE,(void*)JTAG_write_buffer, Kernal_IO_Data.Data * 8);
			}
      *(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
				
		case IOCTL_JTAG_DEVICE_TFR:
      Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_DEVICE_TFR, 0, 0);
      *(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
		case IOCTL_JTAG_DEVICE_CHECKSUM:
			pjhwd_ops->get_hw_device_idcode(&idcode);
			ret = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_DEVICE_CHECKSUM,(void*)&Kernal_IO_Data.Data, 0); 
      *(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
			
    /*case IOCTL_JTAG_UPDATE_DEVICE:
    	pjhwd_ops->get_hw_device_idcode(&idcode);
      if(Kernal_IO_Data.Data < AST_FW_BUFFER_SIZE){
        ret = copy_from_user ((u32 *)JTAG_other_buffer, (u8 *)Kernal_IO_Data.Input_Buffer_Base, Kernal_IO_Data.Data);
        Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_UPDATE_DEVICE,(void*)JTAG_other_buffer,Kernal_IO_Data.Data);
        *(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
      }
      else {
        ret = -1;
      }
      break;*/

		case IOCTL_JTAG_UPDATE_DEVICE:
			ret=pjhwd_ops->get_hw_device_idcode(&idcode);
			
			if(ret != 0){
				ret=-1;
			}
			else if(Kernal_IO_Data.Data < AST_FW_BUFFER_SIZE){
				ret = copy_from_user ((u32 *)JTAG_other_buffer, (u8 *)Kernal_IO_Data.Input_Buffer_Base, Kernal_IO_Data.Data);
				Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_UPDATE_DEVICE,(void*)JTAG_other_buffer,Kernal_IO_Data.Data);
				*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			}
			else {
				ret = -1;
			}
			break;
     		
		case IOCTL_JTAG_DEVICE_INDEX:
			Kernal_IO_Data.Data = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_DEVICE_INDEX,(void*)0,Kernal_IO_Data.Data);
			if(Kernal_IO_Data.Data == 0 ){
				Kernal_IO_Data.Data  = pjhwd_ops->get_hw_device_idcode(&idcode);
				*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			}
			else{
				ret = -1;
			}
			
			break;
		case IOCTL_JTAG_READ_USERCODE:
			pjhwd_ops->get_hw_device_idcode(&idcode);
			ret = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_READ_USERCODE,(void*)&Kernal_IO_Data.Value, 0);
			Kernal_IO_Data.Data = ret;
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
		case IOCTL_JTAG_SET_IO:
			ret = pjhwd_ops->jtag_io_ctl(Kernal_IO_Data.Data, Kernal_IO_Data.Value);
			//--- return reading val::
			Kernal_IO_Data.Value = ret;
			*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			break;
		case IOCTL_JTAG_JBC:
			{	
				char array_buf[256]={0,};
			    char*argv[8]={ &array_buf[0],&array_buf[32], &array_buf[64], &array_buf[96], &array_buf[128], &array_buf[160], &array_buf[192], &array_buf[224]};
			    if(Kernal_IO_Data.Value > 6){
			    	printk("%s: Oops~ too many argument strings.\n", JTAG_DEV_NAME);
			    	Kernal_IO_Data.Data= 1;
					*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			    	break;
			    }
				memset(array_buf, 0, sizeof(array_buf));
				//Get argv strings form user space.
				ret = copy_from_user ((u32 *)array_buf, (u8 *)Kernal_IO_Data.Address, Kernal_IO_Data.Value * 32);
				snprintf(argv[Kernal_IO_Data.Value ++], 32, "-@%08x ",(unsigned int)JTAG_write_buffer);
				snprintf(argv[Kernal_IO_Data.Value ++], 32, "-L%ld ",(long int) Kernal_IO_Data.Data);
				
				//DEBUG
				//for(i = 0 ; i <  Kernal_IO_Data.Value ; i++) printk("@%d@ %s. \n", i, argv[i]);
				//Copy code from user space.
				if(Kernal_IO_Data.Data < AST_FW_BUFFER_SIZE){
					ret = copy_from_user ((u32 *)JTAG_write_buffer, (u8 *)Kernal_IO_Data.Input_Buffer_Base, Kernal_IO_Data.Data);
					if(pjhwd_ops->jbcmain_proc != NULL){
						ret = pjhwd_ops->jbcmain_proc(Kernal_IO_Data.Value,argv);
					}
					else{
						ret = 1;
					}
					Kernal_IO_Data.Data =ret ;
				}
				else{
					printk("%s: Oops~ size of jbc file is too big(%d).\n", JTAG_DEV_NAME, (int)Kernal_IO_Data.Data);
					Kernal_IO_Data.Data= 1;
				}
				 
				*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
			}
			break;
    		
//Quanta---
	case IOCTL_JTAG_UES_READ:
		pjhwd_ops->get_hw_device_idcode(&idcode);
		ret = pjhwd_ops->set_hw_device_ctl(IOCTL_JTAG_UES_READ,(void*)&Kernal_IO_Data.Data, 0); 
		*(IO_ACCESS_DATA *)arg = Kernal_IO_Data;
		break;
//---Quanta	    		
		default:
			printk ( "Invalid JTAG Function\n");
			return -EINVAL;
	}
  return ret;
}


/* ----- Driver registration ---------------------------------------------- */
static struct file_operations jtag_ops = {
  owner:      THIS_MODULE,
  read:       NULL,
  write:      NULL,
#ifdef USE_UNLOCKED_IOCTL
  unlocked_ioctl: jtag_ioctl,
#else
	ioctl:      jtag_ioctl,
#endif
  open:       jtag_open,
  release:    jtag_release,
};


static jtag_core_funcs_t jtag_core_funcs = {
	.get_jtag_core_data = NULL,
};

static core_hal_t jtag_core_hal = {
	.owner		             = THIS_MODULE,
	.name		               = "JTAG CORE",
	.dev_type              = EDEV_TYPE_JTAG,
	.register_hal_module   = register_jtag_hal_module,
	.unregister_hal_module = unregister_jtag_hal_module,
	.pcore_funcs           = (void *)&jtag_core_funcs
};


/*
 * JTGA driver init function
 */
int __init jtag_init(void)
{
	int ret =0 ;
  
  /* jtag device initialization */ 
	if ((ret = register_chrdev_region (jtag_devno, JTAG_MAX_DEVICES, JTAG_DEV_NAME)) < 0)
	{
	   printk (KERN_ERR "failed to register jtag device <%s> (err: %d)\n", JTAG_DEV_NAME, ret);
	   return ret;
	}
   
	jtag_cdev = cdev_alloc ();
	if (!jtag_cdev)
	{
	   unregister_chrdev_region (jtag_devno, JTAG_MAX_DEVICES);
	   printk (KERN_ERR "%s: failed to allocate jtag cdev structure\n", JTAG_DEV_NAME);
	   return -1;
	}
   
	cdev_init (jtag_cdev, &jtag_ops);
	
	jtag_cdev->owner = THIS_MODULE;
	
	if ((ret = cdev_add (jtag_cdev, jtag_devno, JTAG_MAX_DEVICES)) < 0)
	{
		cdev_del (jtag_cdev);
		unregister_chrdev_region (jtag_devno, JTAG_MAX_DEVICES);
		printk	(KERN_ERR "failed to add <%s> char device\n", JTAG_DEV_NAME);
		ret = -ENODEV;
		return ret;
	}

	if ((ret = register_core_hal_module (&jtag_core_hal)) < 0)
	{
		printk(KERN_ERR "failed to register the Core JTAG module\n");
		cdev_del (jtag_cdev);
		unregister_chrdev_region (jtag_devno, JTAG_MAX_DEVICES);
		ret = -EINVAL;
		return ret;
	}

  // alloc write/read/other buffer
  memset (&JTAG_device_information, 0, sizeof(JTAG_DEVICE_INFO));
  
  JTAG_read_buffer = kmalloc (AST_JTAG_BUFFER_SIZE, GFP_DMA|GFP_KERNEL);
  if (JTAG_read_buffer == NULL) {
  	printk (KERN_WARNING "%s: Can't allocate read_buffer\n", JTAG_DEV_NAME);
    ret = -ENOMEM;
    goto out_no_mem;
  }

  JTAG_write_buffer = kmalloc (AST_JTAG_BUFFER_SIZE, GFP_DMA|GFP_KERNEL);
  if (JTAG_write_buffer == NULL) {
    printk (KERN_WARNING "%s: Can't allocate write_buffer\n", JTAG_DEV_NAME);
    ret = -ENOMEM;
    goto out_no_mem;
  }

  JTAG_other_buffer = kmalloc (AST_FW_BUFFER_SIZE, GFP_DMA|GFP_KERNEL);
  if (JTAG_other_buffer == NULL) {
    printk (KERN_WARNING "%s: Can't allocate other_buffer\n", JTAG_DEV_NAME);
    ret = -ENOMEM;
    goto out_no_mem;
  }
  memset(JTAG_other_buffer, 0xff, AST_FW_BUFFER_SIZE);

	printk("The JTAG Driver is loaded successfully.\n" );
  return 0;
  
out_no_mem:
	cdev_del (jtag_cdev);
	unregister_chrdev_region (jtag_devno, JTAG_MAX_DEVICES);	
	
  if (JTAG_read_buffer != NULL)
  	kfree(JTAG_read_buffer);
  if (JTAG_write_buffer != NULL)
  	kfree(JTAG_write_buffer);
	if (JTAG_other_buffer != NULL)
		kfree(JTAG_other_buffer);  
  return ret;
}

/*!
 * JTGA driver exit function
 */
void __exit jtag_exit(void)
{
	unregister_core_hal_module (EDEV_TYPE_JTAG);
	unregister_chrdev_region (jtag_devno, JTAG_MAX_DEVICES);

	if (NULL != jtag_cdev)
	{
		cdev_del (jtag_cdev);
	}
	
	kfree(JTAG_read_buffer);
	kfree(JTAG_write_buffer);
	kfree(JTAG_other_buffer);

  printk ( "Unregistered the JTAG Driver Sucessfully\n");

  return;	
}

EXPORT_SYMBOL(JTAG_device_information);
EXPORT_SYMBOL(register_jtag_hw_device_ops);
EXPORT_SYMBOL(unregister_jtag_hw_device_ops);
EXPORT_SYMBOL(get_jtag_write_buffer);
EXPORT_SYMBOL(get_jtag_read_buffer);

module_init(jtag_init);
module_exit(jtag_exit);

MODULE_AUTHOR("American Megatrends Inc.");
MODULE_DESCRIPTION("JTAG Common Driver");
MODULE_LICENSE ("GPL");

int jtag_core_loaded =1;
EXPORT_SYMBOL(jtag_core_loaded);
