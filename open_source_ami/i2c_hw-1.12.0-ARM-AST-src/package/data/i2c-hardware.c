/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2008 American Megatrends Inc.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.		     */
/* ------------------------------------------------------------------------- */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>

#include "i2c-data.h"
#include "i2c-hardware.h"

//#define  ENABLE_SMBUS_ARP_HOST	1
#define AST_I2C_DEV_1_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_1_BASE)
#define AST_I2C_DEV_2_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_2_BASE)
#define AST_I2C_DEV_3_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_3_BASE)
#define AST_I2C_DEV_4_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_4_BASE)
#define AST_I2C_DEV_5_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_5_BASE)
#define AST_I2C_DEV_6_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_6_BASE)
#define AST_I2C_DEV_7_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_7_BASE)
#if defined(SOC_AST2300)
#define AST_I2C_DEV_8_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_8_BASE)
#define AST_I2C_DEV_9_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_9_BASE)
#endif

extern struct i2c_as_data as_data_ptr[BUS_COUNT];

/** Default values in these arrays are for the as **/
static u32 i2c_as_io_base[ BUS_COUNT ] =
{
#if (BUS_COUNT>=1)
    AST_I2C_DEV_1_VA_BASE, 
#endif
#if (BUS_COUNT>=2)
	AST_I2C_DEV_2_VA_BASE,
#endif
#if (BUS_COUNT>=3)
	AST_I2C_DEV_3_VA_BASE,
#endif
#if (BUS_COUNT>=4)
    AST_I2C_DEV_4_VA_BASE, 
#endif
#if (BUS_COUNT>=5)
	AST_I2C_DEV_5_VA_BASE, 
#endif
#if (BUS_COUNT>=6)
	AST_I2C_DEV_6_VA_BASE,
#endif
#if (BUS_COUNT>=7)
	AST_I2C_DEV_7_VA_BASE,
#endif
#if (BUS_COUNT>=8)
	AST_I2C_DEV_8_VA_BASE,
#endif
#if (BUS_COUNT>=9)
	AST_I2C_DEV_9_VA_BASE,
#endif
};

static int slave_addr[ BUS_COUNT ] =
{
#if (BUS_COUNT>=1)
    DEFAULT_SLAVE_0,
#endif
#if (BUS_COUNT>=2)
	DEFAULT_SLAVE_1, 
#endif
#if (BUS_COUNT>=3)
	DEFAULT_SLAVE_2, 
#endif
#if (BUS_COUNT>=4)
    DEFAULT_SLAVE_3,
#endif
#if (BUS_COUNT>=5)
	DEFAULT_SLAVE_4, 
#endif
#if (BUS_COUNT>=6)
	DEFAULT_SLAVE_5,
#endif
#if (BUS_COUNT>=7)
	DEFAULT_SLAVE_6,
#endif
#if (BUS_COUNT>=8)
	DEFAULT_SLAVE_7,
#endif
#if (BUS_COUNT>=9)
	DEFAULT_SLAVE_8,
#endif
};

static int ast_i2c_clock1[ BUS_COUNT ] =
{
#if (BUS_COUNT>=1)
    AST_I2C_0_CLOCK,
#endif
#if (BUS_COUNT>=2)
	AST_I2C_1_CLOCK, 
#endif
#if (BUS_COUNT>=3)
	AST_I2C_2_CLOCK, 
#endif
#if (BUS_COUNT>=4)
    AST_I2C_3_CLOCK,
#endif
#if (BUS_COUNT>=5)
	AST_I2C_4_CLOCK, 
#endif
#if (BUS_COUNT>=6)
	AST_I2C_5_CLOCK,
#endif
#if (BUS_COUNT>=7)
	AST_I2C_6_CLOCK,
#endif
#if (BUS_COUNT>=8)
	AST_I2C_7_CLOCK,
#endif
#if (BUS_COUNT>=9)
	AST_I2C_8_CLOCK,
#endif
};

u32 i2c_as_read_reg( int bus, u32 offset )
{
    return( ioread32( i2c_as_io_base[ bus ] + offset ) );
}

void i2c_as_write_reg( int bus, u32 value, u32 offset )
{
    iowrite32( value, i2c_as_io_base[ bus ] + offset );
}

void i2c_as_disable_all_interrupts(int bus)
{
	i2c_as_write_reg(bus,0,I2C_INTR_CONTROL_REG);
}

void i2c_as_disable_interrupt(int bus, unsigned long mask)
{
	unsigned long current_mask;

	current_mask = i2c_as_read_reg(bus,I2C_INTR_CONTROL_REG);
	i2c_as_write_reg(bus,(current_mask & ~mask),I2C_INTR_CONTROL_REG);

	return;
}

void i2c_as_enable_interrupt(int bus, unsigned long mask)
{
	unsigned long current_mask;

	current_mask = i2c_as_read_reg(bus,I2C_INTR_CONTROL_REG);
	i2c_as_write_reg(bus,current_mask|mask,I2C_INTR_CONTROL_REG);
	return;
}

void i2c_as_reset(int bus)
{
	u32 ctrl_bits;

	/* Disable Both Master and Salve function when resetting device */
	ctrl_bits = i2c_as_read_reg(bus,I2C_FUNCTION_CONTROL_REG);
	ctrl_bits &= ~( ENABLE_MASTER_FUNC |ENABLE_SLAVE_FUNC);
	i2c_as_write_reg( bus,ctrl_bits,I2C_FUNCTION_CONTROL_REG);
	udelay(100);
	i2c_init_hardware(bus);
}

void i2c_init_hardware(int bus)
{
	/* clear interrups */
	i2c_as_disable_all_interrupts(bus);

	/* Set Function Register */
#ifdef ENABLE_SMBUS_ARP_HOST
    i2c_as_write_reg(bus, ENABLE_MASTER_FUNC | ENABLE_SLAVE_FUNC | ENABLE_SMBUS_ARP_HOST_ADDR_RESP, I2C_FUNCTION_CONTROL_REG);
#else
    i2c_as_write_reg(bus, ENABLE_MASTER_FUNC | ENABLE_SLAVE_FUNC , I2C_FUNCTION_CONTROL_REG);
#endif

	/* Set Clock and AC timing Control register 1*/
    i2c_as_write_reg (bus, ast_i2c_clock1[bus], I2C_CLOCK_AC_1_CONTROL_REG);

	/* Set Clock and AC timing Control register 2*/
	i2c_as_write_reg (bus, DEFAULT_CLOCK_AC_2_VAL, I2C_CLOCK_AC_2_CONTROL_REG );
	
    /* Set Function Register */
	i2c_as_write_reg(bus, DEFAULT_BUF_CONTROL_VAL, I2C_BUF_CONTROL_REG);

    /* DMA Mode Control Register */
	i2c_as_write_reg(bus, 0, I2C_DMA_MODE_CONTROL_REG);

	/* clear interrups */
	i2c_as_write_reg (bus, CLR_ALL_INTS_VAL, I2C_INTR_STATUS_REG);

	/* Enable interrupts */
#ifdef ENABLE_SMBUS_ARP_HOST
	i2c_as_write_reg (bus, ENABLE_TX_DONE_WITH_ACK_INTR  			| \
					ENABLE_TX_DONE_WITH_NACK_INTR 			| \
					ENABLE_RX_DONE_INTR		   				| \
					ENABLE_ARBITRATION_LOST_INTR  			| \
					ENABLE_STOP_CONDITION_RECVD_INTR		| \
					ENABLE_ABNORMAL_START_STOP_RECVD_INTR	| \
					ENABLE_SLAVE_ADDR_MATCH_INTR 			| \
					ENABLE_SCL_LOW_TIMEOUT_INTR				| \
					ENABLE_SMBUS_ARP_HOST_ADDR_MATCH_INTR ,
					I2C_INTR_CONTROL_REG);
#else
	i2c_as_write_reg (bus, ENABLE_TX_DONE_WITH_ACK_INTR  			| \
					ENABLE_TX_DONE_WITH_NACK_INTR 			| \
					ENABLE_RX_DONE_INTR		   				| \
					ENABLE_ARBITRATION_LOST_INTR  			| \
					ENABLE_STOP_CONDITION_RECVD_INTR		| \
					ENABLE_ABNORMAL_START_STOP_RECVD_INTR	| \
					ENABLE_SLAVE_ADDR_MATCH_INTR 			| \
					ENABLE_SCL_LOW_TIMEOUT_INTR	,
					I2C_INTR_CONTROL_REG);
#endif

}

void i2c_init_slave_address(int bus)
{
	i2c_as_write_reg( bus, slave_addr[bus], I2C_SLAVE_ADDR_REG );
}

void i2c_enable_bus(int bus)
{
	/* Enable I2C interface */
#ifdef ENABLE_SMBUS_ARP_HOST
    i2c_as_write_reg(bus, ENABLE_MASTER_FUNC | ENABLE_SLAVE_FUNC | ENABLE_SMBUS_ARP_HOST_ADDR_RESP, I2C_FUNCTION_CONTROL_REG);
#else
	i2c_as_write_reg(bus, ENABLE_MASTER_FUNC | ENABLE_SLAVE_FUNC , I2C_FUNCTION_CONTROL_REG);
#endif

}

void i2c_disable_bus(int bus)
{
	/* Disable I2C interface */
  	i2c_as_write_reg(bus, 0, I2C_FUNCTION_CONTROL_REG);
}

int i2c_wait_for_bus_free(int bus)
{
	int	retries = DEFAULT_BB_RETRIES;

	while ( (i2c_as_read_reg(bus, I2C_CMD_STATUS_REG ) & BUS_BUSY_STATUS ) && (retries > 0))
	{
		mdelay (DEFAULT_TIMEOUT);
		retries--;
	}
	
	if(retries == 0)
	{
		//printk("I2C%d:I2C bus busy\n",bus);
		return 1;
	}

	return 0;
}

int i2c_as_disable_slave(int bus)
{
	u32 ctrl_bits;
	unsigned long flags;

	if(i2c_wait_for_bus_free(bus) != 0)
	{
		printk("ERROR: I2C%d: Disable slave failed due to bus-busy\n",bus);
		return 1;
	}

	local_save_flags(flags);
	local_irq_disable();

	ctrl_bits = i2c_as_read_reg(bus,I2C_FUNCTION_CONTROL_REG);
	if ((ctrl_bits & ENABLE_SLAVE_FUNC) !=0) /*if slave interface is on*/
	{
		
		i2c_as_write_reg( bus,ctrl_bits & ~ENABLE_SLAVE_FUNC,I2C_FUNCTION_CONTROL_REG);
		
	}

	local_irq_restore(flags);

	return 0;
}

int i2c_as_enable_slave(int bus)
{
	u32 ctrl_bits;
	unsigned long flags;

	if(i2c_wait_for_bus_free(bus) != 0)
	{
		printk("ERROR: I2C%d: Enable slave failed due to bus-busy\n",bus);
		return 1;
	}
	
	local_save_flags(flags);
	local_irq_disable();

	if(as_data_ptr[bus].SlaveRX_Entries != MAX_FIFO_LEN)
	{
		as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;

		ctrl_bits = i2c_as_read_reg(bus,I2C_FUNCTION_CONTROL_REG);
		if ((ctrl_bits & ENABLE_SLAVE_FUNC) ==0) /* if slave interface is off */
		{
			
			i2c_as_write_reg( bus,ctrl_bits |ENABLE_SLAVE_FUNC,I2C_FUNCTION_CONTROL_REG );
			
		}
	}
	local_irq_restore(flags);

	return 0;
}

void i2c_as_set_slave( int bus, int slave )
{
    /* Save the save address for future uses */
    slave_addr[ bus ] = slave >> 1;

	/* set slave mode */
    i2c_as_write_reg( bus, slave >> 1 , I2C_SLAVE_ADDR_REG );
}

int i2c_as_get_slave( int bus )
{
	return i2c_as_read_reg( bus, I2C_SLAVE_ADDR_REG );
}
