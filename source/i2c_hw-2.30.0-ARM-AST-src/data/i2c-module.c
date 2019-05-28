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
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/dma-mapping.h>
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h> /* For Ioctl definitions */

#include "i2c-data.h"
#include "i2c-hardware.h"
#include "i2c-log.h"

#define I2C_ID              ( 1 )

static struct i2c_adapter as_ops[ BUS_COUNT ];
extern struct i2c_as_data as_data_ptr[BUS_COUNT];

static int bus_count = BUS_COUNT;

extern int as_get_recovery_info (int bus, bus_recovery_info_T* info);
extern int as_set_recovery_info (int bus, bus_recovery_info_T* info);
extern int i2c_bus_test (int bus, i2c_bus_test_T* info);
extern unsigned char slave_mode[BUS_COUNT];

static int __init 
i2c_as_init( void )
{
    int i;
    
	/* Initialize driver's itnernal data structures */
    for( i = 0; i < bus_count; i++ )
		i2c_init_internal_data(i);

	for( i = 0; i < bus_count; i++ )
	{
		if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << i)) continue;
 		 i2c_as_disable_all_interrupts(i);
	}

    for( i = 0; i < bus_count; i++ ) slave_mode[i] = 0;
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
    if( request_irq( IRQ_I2C, i2c_handler, IRQF_SHARED, "i2c_as", (void *)AST_I2C_BASE ) < 0 )
#else
    if( request_irq( IRQ_I2C, i2c_handler, SA_SHIRQ, "i2c_as", (void *)AST_I2C_BASE ) < 0 )
#endif
     {
           printk( KERN_ERR "Request for irq %d failed\n", IRQ_I2C );
           //return( -ENODEV );
     }
	else
	{
		IRQ_SET_HIGH_LEVEL (IRQ_I2C);
        IRQ_SET_LEVEL_TRIGGER (IRQ_I2C);
	}

#if defined(GROUP_AST1070_COMPANION)
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
    if( request_irq( IRQ_BMCCC0_I2C, i2c_handler, IRQF_SHARED, "i2c_bmccc0_as", (void *)AST_BMCCC_I2C_BASE(0) ) < 0 )
#else
    if( request_irq( IRQ_BMCCC0_I2C, i2c_handler, SA_SHIRQ, "i2c_bmccc0_as", (void *)AST_BMCCC_I2C_BASE(0) ) < 0 )
#endif
     {
           printk( KERN_ERR "Request for BMCCC0 irq %d failed\n", IRQ_BMCCC0_I2C );
           //return( -ENODEV );
     }
	else
	{
		BMCCC0_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_I2C);
        BMCCC0_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_I2C);
	}

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
    if( request_irq( IRQ_BMCCC1_I2C, i2c_handler, IRQF_SHARED, "i2c_bmccc1_as", (void *)AST_BMCCC_I2C_BASE(1) ) < 0 )
#else
    if( request_irq( IRQ_BMCCC1_I2C, i2c_handler, SA_SHIRQ, "i2c_bmccc1_as", (void *)AST_BMCCC_I2C_BASE(1) ) < 0 )
#endif
     {
           printk( KERN_ERR "Request for BMCCC1 irq %d failed\n", IRQ_BMCCC1_I2C );
           //return( -ENODEV );
     }
	else
	{
		BMCCC1_IRQ_SET_HIGH_LEVEL (IRQ_BMCCC0_I2C);
        BMCCC1_IRQ_SET_LEVEL_TRIGGER (IRQ_BMCCC0_I2C);
	}
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif
	/* Stop SMBus/I2C controllers asyn-chronous reset*/
	i2c_stop_cntlr_reset();

	/* Initialize the hardware */
    for( i = 0; i < bus_count; i++ )
	{
		if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << i)) continue;
		i2c_init_hardware(i);
		i2c_init_slave_address(i);
	}

	/* Register ourselves with the upper layers */
    for( i = 0; i < bus_count; i++ )
	{
        /* Set up i2c_adapter structs */
        as_ops[ i ].owner = THIS_MODULE;
        as_ops[ i ].class = I2C_CLASS_HWMON;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
        as_ops[ i ].id = I2C_ID;
#endif
        as_ops[ i ].algo_data = &as_data_ptr[i];
		as_ops[ i ].nr = i;
#if defined(GROUP_AST1070_COMPANION)
        if (i < AST_BMC_BUS_COUNT)
            sprintf( as_ops[ i ].name, "i2c-as-%d", i );
        else if ((i > AST_BMC_BUS_COUNT) && (i < (AST_BMC_BUS_COUNT+CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES)))
            sprintf( as_ops[ i ].name, "i2c_bmccc0_as-%d", i );
        else
            sprintf( as_ops[ i ].name, "i2c_bmccc1_as-%d", i );
#else
        sprintf( as_ops[ i ].name, "i2c-as-%d", i );
#endif

        /* Add the bus via the algorithm code */
        if( i2c_as_add_bus( &as_ops[ i ] ) != 0 )
        {
            printk( KERN_ERR "Cannot add bus %d to algorithm layer\n", i );
            return( -ENODEV );
        }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,11))
		printk( KERN_INFO "Registered bus id: %s\n", as_ops[ i ].dev.bus_id );
#else
		printk( KERN_INFO "Registered bus id: %s\n", kobject_name(&as_ops[ i ].dev.kobj));
#endif
	}

	/* Enable the bus to start working */
	for( i = 0; i < bus_count; i++ )
	{
		if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << i)) continue;
		i2c_enable_bus(i);
		//printk("INSIDE INIT  I2C_INTR_CONTROL_REG = 0x%08x\n", i2c_as_read_reg (i,I2C_INTR_CONTROL_REG));
	}

	return 0;
}

static void 
i2c_as_exit( void )
{
    int i;

    for( i = 0; i < bus_count; i++ )
    {
        i2c_as_del_bus( &as_ops[ i ] );

	if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << i)) continue;

		i2c_disable_bus(i);

		i2c_as_disable_all_interrupts(i);
    
		if (as_data_ptr[i].i2c_dma_mode == I2C_DMA_MODE)
		{
			dma_free_coherent(NULL, AST_I2C_DMA_SIZE, as_data_ptr[i].dma_buff, as_data_ptr[i].dma_addr);
			as_data_ptr[i].dma_buff = NULL;
			as_data_ptr[i].dma_addr = 0;
		}
    }

        /* Disable the irq */
        disable_irq( IRQ_I2C );
        free_irq( IRQ_I2C, (void *)AST_I2C_BASE );
#if defined(GROUP_AST1070_COMPANION)
        disable_irq( IRQ_BMCCC0_I2C );
        free_irq( IRQ_BMCCC0_I2C, (void *)AST_BMCCC_I2C_BASE(0) );
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        disable_irq( IRQ_BMCCC1_I2C );
        free_irq( IRQ_BMCCC1_I2C, (void *)AST_BMCCC_I2C_BASE(1) );
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif


	return;
}

/* Return bitfield describing our capabilities */
static u32 
as_func( struct i2c_adapter *adap )
{
    return( I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL );
}

/* Algorithm level handling of ioctls and other control commands */
static int 
as_control( struct i2c_adapter *i2c_adap, unsigned long cmd,
                       unsigned long arg )
{
    int retval = 0;
	bus_recovery_info_T	bus_recovery_info;
	i2c_bus_test_T testData;
	i2c_link_state_T linkData;
    char *tmp;

    switch( cmd )
    {
        case I2C_SET_HOST_ADDR:
            i2c_as_set_slave( i2c_adap->nr, arg );
            break;

        case I2C_GET_HOST_ADDR:
            retval = i2c_as_get_slave( i2c_adap->nr );
            break;

        case I2C_RESET:
			printk("I2C%d: Resetting the controller\n",i2c_adap->nr);
            /* Reset the controller */
           	i2c_as_reset(i2c_adap->nr);
            break;

		case I2C_SET_REC_INFO:
			if (copy_from_user(&bus_recovery_info, (void*)arg, sizeof(bus_recovery_info_T))) 
				return -EFAULT;
			if (0 != as_set_recovery_info (i2c_adap->nr, &bus_recovery_info)) 
			{
				dev_err (&i2c_adap->dev, "as_control: set recovery info asiled\n");
			}
			break;
		case I2C_GET_REC_INFO:
			if (0 != as_get_recovery_info (i2c_adap->nr, &bus_recovery_info)) 
			{
				dev_err (&i2c_adap->dev, "as_control: get recovery info failed\n");
			}
			if (copy_to_user((void*)arg, &bus_recovery_info, sizeof(bus_recovery_info_T))) 
				return -EFAULT;
			break;
		case I2C_SET_SPEED:
			printk("I2C%d. Going to set the speed...\n", i2c_adap->nr);
			retval = i2c_as_set_speed(i2c_adap->nr, arg);
			
			break;
		
		case I2C_GET_LINK_STATE:
			linkData.i2cLinkState = as_data_ptr[i2c_adap->nr].i2c_link_state; 
			if (copy_to_user((void*)arg, &linkData, sizeof(i2c_link_state_T))) 
				retval  =  -EFAULT;

			break;			

		case I2C_BUS_TEST:
			if  (copy_from_user (&testData, (void*) arg, sizeof(i2c_bus_test_T)))
				return -EFAULT;

			if (0 != i2c_bus_test (i2c_adap->nr, &testData)) 
			{
				dev_err (&i2c_adap->dev, "i2c-ioctl: I2C_BUS_TEST failed\n");
			}
			if (copy_to_user((void*)arg, &testData, sizeof(i2c_bus_test_T))) 
				retval =  -EFAULT;
			break;

        case I2C_MCTP_READ:
            tmp = kmalloc(MAX_MCTP_MSG_SIZE,GFP_KERNEL);
            if(!tmp)
                return -ENOMEM;
            retval = as_mctp_recv(i2c_adap,tmp);
            if(retval > 0)
                retval = copy_to_user((char*)arg,tmp,retval)? -EFAULT:retval;
            kfree(tmp);
            break;

        case ENABLE_SSIF:
           	//printk("I2C%d: Enable slave xfer for SSIF\n",i2c_adap->nr);
    	    i2c_as_slave_xfer_enable(i2c_adap->nr); 
		    break; 

        default:
            dev_err( &i2c_adap->dev, "as_control: Unknown ioctl command\n" );
            retval = -ENOTTY;
    }

    return( retval );
}

static struct i2c_algorithm as_algo =
{
    .master_xfer = as_master_xfer,
    .smbus_xfer = as_smb_xfer,
	.smb_slave_recv = as_smb_host_notify_recv,
    .functionality = as_func,
    .i2c_hw_ioctl = as_control,
	.slave_recv = as_slave_recv,
	.slave_send = as_slave_send, 
	.i2c_recovery = as_i2c_bus_recovery,
};

int 
i2c_as_add_bus( struct i2c_adapter *i2c_adap )
{
    /* Register new adapter */
    i2c_adap->algo = &as_algo;

    /* Set some sensible defaults */
    i2c_adap->timeout = DEFAULT_TIMEOUT;
    i2c_adap->retries = DEFAULT_RETRIES;

    i2c_add_adapter( i2c_adap );
    //printk("I2C%d: Hardware routines registered\n",i2c_adap->nr);
    log_i2c_proc_open( i2c_adap );
    
    return(0);
}

int 
i2c_as_del_bus( struct i2c_adapter *i2c_adap )
{	
	log_i2c_proc_release( i2c_adap );
    i2c_del_adapter( i2c_adap );
	return 0;
}

MODULE_AUTHOR( "American Megatrends" );
MODULE_DESCRIPTION( "AST I2C Algorithm" );
MODULE_LICENSE( "GPL" );

module_init( i2c_as_init );
module_exit( i2c_as_exit );
