/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h> /* For Ioctl definitions */

#include "i2c-data.h"
#include "i2c-hardware.h"

#define I2C_ID              ( 1 )

#define I2C_IRQ_NUM		    IRQ_I2C

static struct i2c_adapter as_ops[ BUS_COUNT ];
extern struct i2c_as_data as_data_ptr[BUS_COUNT];

static int bus_count = BUS_COUNT;

extern int as_get_recovery_info (int bus, bus_recovery_info_T* info);
extern int as_set_recovery_info (int bus, bus_recovery_info_T* info);

static int __init 
i2c_as_init( void )
{
    int i;
    
	/* Initialize driver's itnernal data structures */
    for( i = 0; i < bus_count; i++ )
		i2c_init_internal_data(i);

	for( i = 0; i < bus_count; i++ )
	{
 		 i2c_as_disable_all_interrupts(i);
	
	}
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
    if( request_irq( I2C_IRQ_NUM, i2c_handler, IRQF_DISABLED, "i2c_as", NULL ) < 0 )
#else
    if( request_irq( I2C_IRQ_NUM, i2c_handler, SA_INTERRUPT, "i2c_as", NULL ) < 0 )
#endif
     {
           printk( KERN_ERR "Request for irq %d failed\n", I2C_IRQ_NUM );
           //return( -ENODEV );
     }
	else
	{
		IRQ_SET_HIGH_LEVEL (I2C_IRQ_NUM);
        IRQ_SET_LEVEL_TRIGGER (I2C_IRQ_NUM);
	}

	/* Initialize the hardware */
    for( i = 0; i < bus_count; i++ )
	{
		i2c_init_hardware(i);
		i2c_init_slave_address(i);
	}

	/* Register ourselves with the upper layers */
    for( i = 0; i < bus_count; i++ )
	{
        /* Set up i2c_adapter structs */
        as_ops[ i ].owner = THIS_MODULE;
        as_ops[ i ].class = I2C_CLASS_HWMON;
        as_ops[ i ].id = I2C_ID;
        as_ops[ i ].algo_data = &as_data_ptr[i];
		as_ops[ i ].nr = i;
        sprintf( as_ops[ i ].name, "i2c-as-%d", i );

        /* Add the bus via the algorithm code */
        if( i2c_as_add_bus( &as_ops[ i ] ) != 0 )
        {
            printk( KERN_ERR "Cannot add bus %d to algorithm layer\n", i );
            return( -ENODEV );
        }
		printk( KERN_INFO "Registered bus id: %s\n", as_ops[ i ].dev.bus_id );
	}

	/* Enable the bus to start working */
	for( i = 0; i < bus_count; i++ )
	{
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
		i2c_disable_bus(i);

        i2c_as_del_bus( &as_ops[ i ] );

		i2c_as_disable_all_interrupts(i);
    }

        /* Disable the irq */
        disable_irq( I2C_IRQ_NUM );
        free_irq( I2C_IRQ_NUM, NULL );
    
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
	printk("I2C%d: Hardware routines registered\n",i2c_adap->nr);
    
    return(0);
}

int 
i2c_as_del_bus( struct i2c_adapter *i2c_adap )
{
    return( i2c_del_adapter( i2c_adap ) );
}

MODULE_AUTHOR( "American Megatrends" );
MODULE_DESCRIPTION( "AST I2C Algorithm" );
MODULE_LICENSE( "GPL" );

module_init( i2c_as_init );
module_exit( i2c_as_exit );
