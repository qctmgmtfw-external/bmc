/****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************/

/****************************************************************
 *
 * uartroute_mod.c
 * ASPEED UARTROUTE driver for AST
 *
*****************************************************************/


#include <linux/version.h>
#if (LINUX_VERSION_CODE <  KERNEL_VERSION(3,4,11))
#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#endif

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() 		*/
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/ide.h>
#include <linux/cdev.h>
#include <asm/delay.h>
#include <mach/platform.h>
#include "uartroute.h"

#ifdef HAVE_UNLOCKED_IOCTL  
  #if HAVE_UNLOCKED_IOCTL  
  #define USE_UNLOCKED_IOCTL  
  #endif
#endif

#define UARTROUTE_DRIVER_MAJOR	103
#define UARTROUTE_DRIVER_MINOR	0
#define UARTROUTE_MAX_DEVICES     1
#define UARTROUTE_DEVNAME		"UARTROUTE"

static struct cdev *uartroute_cdev;
static dev_t uartroute_devno = MKDEV(UARTROUTE_DRIVER_MAJOR, UARTROUTE_DRIVER_MINOR);

static int  ast_uartroute_init_module(void);
static void ast_uartroute_exit_module(void);
static void route_io_to_uart(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask);
static void route_uart_to_io(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask);
static void route_uart(int source_port, int destination_port, int source_port_type);

#ifdef USE_UNLOCKED_IOCTL
static long ast_uartroute_ioctl(struct file *file,unsigned int cmd, unsigned long arg);
#else
static int ast_uartroute_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg);
#endif


static struct file_operations uartroute_fops = 
{
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
#ifdef USE_UNLOCKED_IOCTL
	unlocked_ioctl:		ast_uartroute_ioctl,
#else
	ioctl:		ast_uartroute_ioctl,
#endif
};

DECLARE_WAIT_QUEUE_HEAD(rtc_chassis_intrusion_wq);
DECLARE_WAIT_QUEUE_HEAD(swm_powergood_wq);

static void route_uart_to_uart(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask)
{
	switch(destination_port)
	{
		case UART1:
			*shift_pos = SEL_1_DW;
			*bits_mask = 0x00070000;			
			switch(source_port)
			{
				case UART2:
					*shift_val = 0x04;
				break;
				case UART3:
					*shift_val = 0x05;
				break;				
				case UART4:
					*shift_val = 0x06;
				break;
			}
		break;
		case UART2:
			*shift_pos = SEL_2_DW;
			*bits_mask = 0x00380000;					
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x06;
				break;
				case UART3:
					*shift_val = 0x04;
				break;				
				case UART4:
					*shift_val = 0x05;
				break;
			}
		break;
		case UART3:
			*shift_pos = SEL_3_DW;
			*bits_mask = 0x01C00000;				
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x05;
				break;
				case UART2:
					*shift_val = 0x06;
				break;				
				case UART4:
					*shift_val = 0x04;
				break;
			}
		break;
		case UART4:
			*shift_pos = SEL_4_DW;
			*bits_mask = 0x0E000000;							
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x04;
				break;
				case UART2:
					*shift_val = 0x05;
				break;				
				case UART3:
					*shift_val = 0x06;
				break;
			}
		break;
		case UART5:
			*shift_pos = SEL_5_DW;
			*bits_mask = 0xF0000000;							
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x05;
				break;
				case UART2:
					*shift_val = 0x06;
				break;				
				case UART3:
					*shift_val = 0x07;
				break;
				case UART4:
					*shift_val = 0x08;
				break;
			}
		break;
	}

}


static void route_io_to_uart(int source_port, int destination_port, int *shift_pos, int *shift_val , uint32_t *bits_mask)
{
	switch(destination_port)
	{
		case UART1:
			*shift_pos = SEL_1_DW;
			*bits_mask = 0x00070000;
			switch(source_port)
			{
				case IO1:
					*shift_val = 0x00;
				break;
				case IO2:
					*shift_val = 0x01;
				break;
				case IO3:
					*shift_val = 0x02;
				break;
				case IO4:
					*shift_val = 0x03;
				break;
				case IO6:
					*shift_val = 0x07;
				break;
			}
		break;
		case UART2:
			*shift_pos = SEL_2_DW;
			*bits_mask = 0x00380000;			
			switch(source_port)
			{
				case IO1:
					*shift_val = 0x03;
				break;
				case IO2:
					*shift_val = 0x00;
				break;
				case IO3:
					*shift_val = 0x01;
				break;
				case IO4:
					*shift_val = 0x02;
				break;
				case IO6:
					*shift_val = 0x07;
				break;
			}
		break;
		case UART3:
			*shift_pos = SEL_3_DW;
			*bits_mask = 0x01C00000;						
			switch(source_port)
			{
				case IO1:
					*shift_val = 0x02;
				break;
				case IO2:
					*shift_val = 0x03;
				break;
				case IO3:
					*shift_val = 0x00;
				break;
				case IO4:
					*shift_val = 0x01;
				break;
				case IO6:
					*shift_val = 0x07;
				break;
			}
		break;
		case UART4:
			*shift_pos = SEL_4_DW;
			*bits_mask = 0x0E000000;									
			switch(source_port)
			{
				case IO1:
					*shift_val = 0x01;
				break;
				case IO2:
					*shift_val = 0x02;
				break;
				case IO3:
					*shift_val = 0x03;
				break;
				case IO4:
					*shift_val = 0x00;
				break;
				case IO6:
					*shift_val = 0x09;
				break;
			}
		break;
		case UART5:
			*shift_pos = SEL_5_DW;
			*bits_mask = 0xF0000000;				
			switch(source_port)
			{
				case IO1:
					*shift_val = 0x01;
				break;
				case IO2:
					*shift_val = 0x02;
				break;
				case IO3:
					*shift_val = 0x03;
				break;
				case IO4:
					*shift_val = 0x04;
					break;
				case IO5:
					*shift_val = 0x00;
				break;
				case IO6:
					*shift_val = 0x09;
				break;
			}
		break;
	}
}

static void route_uart_to_io(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask)
{
	switch(destination_port)
	{
		case IO1:
			*shift_pos = SEL_1_IO;
			*bits_mask = 0x00000007;
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x00;
				break;
				case UART2:
					*shift_val = 0x01;
				break;
				case UART3:
					*shift_val = 0x02;
				break;
				case UART4:
					*shift_val = 0x03;
				break;
				case UART5:
					*shift_val = 0x04;
				break;
			}
		break;
		case IO2:
			*shift_pos = SEL_2_IO;
			*bits_mask = 0x00000038;			
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x04;
				break;
				case UART2:
					*shift_val = 0x00;
				break;
				case UART3:
					*shift_val = 0x01;
				break;
				case UART4:
					*shift_val = 0x02;
				break;
				case UART5:
					*shift_val = 0x03;
				break;
			}
		break;
		case IO3:
			*shift_pos = SEL_3_IO;
			*bits_mask = 0x000001C0;				
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x03;
				break;
				case UART2:
					*shift_val = 0x04;
				break;
				case UART3:
					*shift_val = 0x00;
				break;
				case UART4:
					*shift_val = 0x01;
				break;
				case UART5:
					*shift_val = 0x02;
				break;
			}
		break;
		case IO4:
			*shift_pos = SEL_4_IO;
			*bits_mask = 0x00000E00;					
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x02;
				break;
				case UART2:
					*shift_val = 0x03;
				break;
				case UART3:
					*shift_val = 0x04;
				break;
				case UART4:
					*shift_val = 0x00;
				break;
				case UART5:
					*shift_val = 0x01;
				break;
			}
		break;
		case IO5:
			*shift_pos = SEL_5_IO;
			*bits_mask = 0x00007000;								
			switch(source_port)
			{
				case UART1:
					*shift_val = 0x01;
				break;
				case UART2:
					*shift_val = 0x02;
				break;
				case UART3:
					*shift_val = 0x03;
				break;
				case UART4:
					*shift_val = 0x04;
				break;
				case UART5:
					*shift_val = 0x00;
				break;
			}
		break;
	}

}

static void route_uart(int source_port, int destination_port, int source_port_type)
{
	int shift_pos = 0;
	int shift_val = 0;
	uint32_t bits_mask=0;
	
	uint32_t reg_old_val = 0;
	uint32_t reg_val = 0;	
	uint32_t reg_tmp_val = 0;

	if(source_port_type == SOURCE_PORT_UART)
	{
		reg_old_val=ioread32((void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_uart_to_io(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;		
		iowrite32(reg_val, (void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);
	}
	else if(source_port_type == SOURCE_PORT_COM)
	{
		reg_old_val=ioread32((void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_io_to_uart(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;
		iowrite32(reg_val, (void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);
	}
	else if(source_port_type == SOURCE_PORT_BOTH)
	{
		reg_old_val=ioread32((void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_uart_to_uart(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;
		iowrite32(reg_val, (void * __iomem)AST_LPC_VA_BASE + AST_LPC_HICRA);
	}
	
}


static void disable_scu_uart(int uart_port)
{
	uint32_t reg=0;
	uint32_t reg_offset=0;
	uint32_t reg_val=0;

	switch(uart_port)
	{
		case UART1:
			reg_offset = AST_SCU_MULTI_FUNC_2;
			reg_val = AST_SCU_MULTI_PIN_UART1;
		break;
		case UART2:
			reg_offset = AST_SCU_MULTI_FUNC_2;
			reg_val = AST_SCU_MULTI_PIN_UART2;
		break;
		case UART3:
			reg_offset = AST_SCU_MULTI_FUNC_1;
			reg_val = AST_SCU_MULTI_PIN_UART3;
		break;
		case UART4:
			reg_offset = AST_SCU_MULTI_FUNC_1;
			reg_val = AST_SCU_MULTI_PIN_UART4;
		break;

	}	

	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */

	reg = ioread32((void * __iomem)AST_SCU_VA_BASE + reg_offset);	
	reg &= ~reg_val;
	iowrite32(reg, (void * __iomem)AST_SCU_VA_BASE + reg_offset);

	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG);  /* lock SCU */

}


static void enable_scu_uart(int uart_port)
{
	uint32_t reg=0;
	uint32_t reg_offset=0;
	uint32_t reg_val=0;

	switch(uart_port)
	{
		case UART1:
			reg_offset = AST_SCU_MULTI_FUNC_2;
			reg_val = AST_SCU_MULTI_PIN_UART1;
		break;
		case UART2:
			reg_offset = AST_SCU_MULTI_FUNC_2;
			reg_val = AST_SCU_MULTI_PIN_UART2;
		break;
		case UART3:
			reg_offset = AST_SCU_MULTI_FUNC_1;
			reg_val = AST_SCU_MULTI_PIN_UART3;
		break;
		case UART4:
			reg_offset = AST_SCU_MULTI_FUNC_1;
			reg_val = AST_SCU_MULTI_PIN_UART4;
		break;
	}	

	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */

	reg = ioread32((void * __iomem)AST_SCU_VA_BASE + reg_offset);
	reg |= reg_val;
	iowrite32(reg, (void * __iomem)AST_SCU_VA_BASE + reg_offset);
	
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG);  /* lock SCU */

}

static int
ast_uartroute_init_module(void)
{
	int ret = 0;

	printk("AST UARTROUTE Driver Version %d.%d\n",UARTROUTE_DRIVER_MAJOR,UARTROUTE_DRIVER_MINOR);
	printk("Copyright (c) 2010 American Megatrends Inc.\n");

    /* ----  Register the character device ------------------- */
	
	if ((ret = register_chrdev_region (uartroute_devno, UARTROUTE_MAX_DEVICES, UARTROUTE_DEVNAME)) < 0)
	{
		printk (KERN_ERR "failed to register uartroute device <%s> (err: %d)\n", UARTROUTE_DEVNAME, ret);
		return ret;
	}
	
	uartroute_cdev = cdev_alloc ();
	if (!uartroute_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate uartroute cdev structure\n", UARTROUTE_DEVNAME);
		unregister_chrdev_region (uartroute_devno, UARTROUTE_MAX_DEVICES);
		return -1;
	}
	
	cdev_init (uartroute_cdev, &uartroute_fops);
	uartroute_cdev->owner = THIS_MODULE;
	
	if ((ret = cdev_add (uartroute_cdev, uartroute_devno, UARTROUTE_MAX_DEVICES)) < 0)
	{
		printk	(KERN_ERR "failed to add <%s> char device\n", UARTROUTE_DEVNAME);
		cdev_del (uartroute_cdev);
		unregister_chrdev_region (uartroute_devno, UARTROUTE_MAX_DEVICES);
		ret = -ENODEV;
		return ret; 
	}

	return ret;	

}
#ifdef USE_UNLOCKED_IOCTL
static long 
ast_uartroute_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int 
ast_uartroute_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	get_uartroute_value_t uartroute_field;

	//printk("UARTROUTE: device ioctl \n");

	if (__copy_from_user((void *)&uartroute_field ,(void *)arg,sizeof(get_uartroute_value_t)))
 	{ 
        printk("ast_uartroute_ioctl: Error copying data from user \n"); 
        return -EFAULT; 
	}

	switch (cmd)
	{
		case ENABLE_SCU_UART :
			enable_scu_uart(uartroute_field.uart_port);
		break;
		case DISABLE_SCU_UART :
			disable_scu_uart(uartroute_field.uart_port);
		break;
		case ROUTE_UART :
			route_uart(uartroute_field.source_port, uartroute_field.destination_port, uartroute_field.source_port_type);
		break;

	default :
		printk ( "Invalid UARTROUTE IOCTL Command\n");
		return -EINVAL;
	}
	return 0;
}



static void
ast_uartroute_exit_module(void)
{
	printk("Unloading AST UARTROUTE Module ..............\n");

	
	unregister_chrdev_region (uartroute_devno, UARTROUTE_MAX_DEVICES);
	
	if (NULL != uartroute_cdev)
	{
		printk ("uartroute char device del\n");
		cdev_del (uartroute_cdev);
	}

	printk("AST2400 UARTROUTE module unloaded sucessfully.\n");

	return;
}

module_init (ast_uartroute_init_module);
module_exit (ast_uartroute_exit_module);

MODULE_AUTHOR("American Megatrends Inc");
MODULE_DESCRIPTION("AST UARTROUTE Driver.");
MODULE_LICENSE ("GPL");

