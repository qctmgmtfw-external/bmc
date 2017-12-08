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

/****************************************************************
 *
 * lpcuart_mod.c
 * ASPEED AST2300 LPCUART driver
 *
*****************************************************************/

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
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
#include "lpcuart.h"


#define LPCUART_DRIVER_MAJOR	103
#define LPCUART_DRIVER_MINOR	0
#define LPCUART_MAX_DEVICES     1
#define LPCUART_DEVNAME		"LPCUART"

static struct cdev *lpcuart_cdev;
static dev_t lpcuart_devno = MKDEV(LPCUART_DRIVER_MAJOR, LPCUART_DRIVER_MINOR);

static int  ast_lpcuart_init_module(void);
static void ast_lpcuart_exit_module(void);
static void route_io_to_uart(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask);
static void route_uart_to_io(int source_port, int destination_port, int *shift_pos, int *shift_val, uint32_t *bits_mask);
static void route_uart(int source_port, int destination_port, int source_port_type);

static int ast_lpcuart_ioctl(struct inode *inode, struct file *file,unsigned int cmd, unsigned long arg);


static struct file_operations lpcuart_fops = 
{
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
	ioctl:		ast_lpcuart_ioctl,
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
		reg_old_val=ioread32(AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_uart_to_io(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;		
		iowrite32(reg_val, AST_LPC_VA_BASE + AST_LPC_HICRA);
	}
	else if(source_port_type == SOURCE_PORT_COM)
	{
		reg_old_val=ioread32(AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_io_to_uart(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;
		iowrite32(reg_val, AST_LPC_VA_BASE + AST_LPC_HICRA);
	}
	else if(source_port_type == SOURCE_PORT_BOTH)
	{
		reg_old_val=ioread32(AST_LPC_VA_BASE + AST_LPC_HICRA);	
		route_uart_to_uart(source_port, destination_port, &shift_pos, &shift_val, &bits_mask);
		reg_old_val&=(~bits_mask);
		reg_tmp_val = shift_val << shift_pos;
		reg_val = reg_old_val|reg_tmp_val;
		iowrite32(reg_val, AST_LPC_VA_BASE + AST_LPC_HICRA);
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

	iowrite32(AST_SCU_UNLOCK_MAGIC, SCU_KEY_CONTROL_REG); /* unlock SCU */

	reg = ioread32(AST_SCU_VA_BASE + reg_offset);	
	reg &= ~reg_val;
	iowrite32(reg, AST_SCU_VA_BASE + reg_offset);

	iowrite32(0, SCU_KEY_CONTROL_REG);  /* lock SCU */

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

	iowrite32(AST_SCU_UNLOCK_MAGIC, SCU_KEY_CONTROL_REG); /* unlock SCU */

	reg = ioread32(AST_SCU_VA_BASE + reg_offset);
	reg |= reg_val;
	iowrite32(reg, AST_SCU_VA_BASE + reg_offset);
	
	iowrite32(0, SCU_KEY_CONTROL_REG);  /* lock SCU */

}

static int
ast_lpcuart_init_module(void)
{
	int ret = 0;

	printk("AST2300 LPCUART Driver Version %d.%d\n",LPCUART_DRIVER_MAJOR,LPCUART_DRIVER_MINOR);
	printk("Copyright (c) 2010 American Megatrends Inc.\n");

    /* ----  Register the character device ------------------- */
	
	if ((ret = register_chrdev_region (lpcuart_devno, LPCUART_MAX_DEVICES, LPCUART_DEVNAME)) < 0)
	{
		printk (KERN_ERR "failed to register lpcuart device <%s> (err: %d)\n", LPCUART_DEVNAME, ret);
		return ret;
	}
	
	lpcuart_cdev = cdev_alloc ();
	if (!lpcuart_cdev)
	{
		printk (KERN_ERR "%s: failed to allocate lpcuart cdev structure\n", LPCUART_DEVNAME);
		unregister_chrdev_region (lpcuart_devno, LPCUART_MAX_DEVICES);
		return -1;
	}
	
	cdev_init (lpcuart_cdev, &lpcuart_fops);
	lpcuart_cdev->owner = THIS_MODULE;
	
	if ((ret = cdev_add (lpcuart_cdev, lpcuart_devno, LPCUART_MAX_DEVICES)) < 0)
	{
		printk	(KERN_ERR "failed to add <%s> char device\n", LPCUART_DEVNAME);
		cdev_del (lpcuart_cdev);
		unregister_chrdev_region (lpcuart_devno, LPCUART_MAX_DEVICES);
		ret = -ENODEV;
		return ret; 
	}

	return ret;	

}

static int 
ast_lpcuart_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	get_lpcuart_value_t lpcuart_field;

	//printk("LPCUART: device ioctl \n");

	if (__copy_from_user((void *)&lpcuart_field ,(void *)arg,sizeof(get_lpcuart_value_t)))
 	{ 
        printk("ast_lpcuart_ioctl: Error copying data from user \n"); 
        return -EFAULT; 
	}

	switch (cmd)
	{
		case ENABLE_SCU_UART :
			enable_scu_uart(lpcuart_field.uart_port);
		break;
		case DISABLE_SCU_UART :
			disable_scu_uart(lpcuart_field.uart_port);
		break;
		case ROUTE_UART :
			route_uart(lpcuart_field.source_port, lpcuart_field.destination_port, lpcuart_field.source_port_type);
		break;

	default :
		printk ( "Invalid LPCUART IOCTL Command\n");
		return -EINVAL;
	}
	return 0;
}



static void
ast_lpcuart_exit_module(void)
{
	printk("Unloading AST2300 LPCUART Module ..............\n");

	
	unregister_chrdev_region (lpcuart_devno, LPCUART_MAX_DEVICES);
	
	if (NULL != lpcuart_cdev)
	{
		printk ("lpcuart char device del\n");
		cdev_del (lpcuart_cdev);
	}

	printk("AST2300 LPCUART module unloaded sucessfully.\n");

	return;
}

module_init (ast_lpcuart_init_module);
module_exit (ast_lpcuart_exit_module);

MODULE_AUTHOR("American Megatrends Inc");
MODULE_DESCRIPTION("AST2300 SOC LPCUART Driver.");
MODULE_LICENSE ("GPL");

