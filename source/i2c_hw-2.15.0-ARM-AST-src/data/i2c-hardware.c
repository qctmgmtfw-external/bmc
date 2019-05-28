/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2009-2015 American Megatrends Inc.

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
#include <linux/dma-mapping.h>
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>

#include "i2c-data.h"
#include "i2c-hardware.h"

//#define  ENABLE_SMBUS_ARP_HOST	1
#define AST_I2C_DEV_G_VA_BASE 		IO_ADDRESS(AST_I2C_GLOBAL_BASE)
#define AST_I2C_DEV_1_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_1_BASE)
#define AST_I2C_DEV_2_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_2_BASE)
#define AST_I2C_DEV_3_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_3_BASE)
#define AST_I2C_DEV_4_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_4_BASE)
#define AST_I2C_DEV_5_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_5_BASE)
#define AST_I2C_DEV_6_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_6_BASE)
#define AST_I2C_DEV_7_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_7_BASE)
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define AST_I2C_DEV_8_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_8_BASE)
#define AST_I2C_DEV_9_VA_BASE 		IO_ADDRESS(AST_I2C_DEV_9_BASE)
#endif
#if defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define AST_I2C_DEV_10_VA_BASE		IO_ADDRESS(AST_I2C_DEV_10_BASE)
#define AST_I2C_DEV_11_VA_BASE		IO_ADDRESS(AST_I2C_DEV_11_BASE)
#define AST_I2C_DEV_12_VA_BASE		IO_ADDRESS(AST_I2C_DEV_12_BASE)
#define AST_I2C_DEV_13_VA_BASE		IO_ADDRESS(AST_I2C_DEV_13_BASE)
#define AST_I2C_DEV_14_VA_BASE		IO_ADDRESS(AST_I2C_DEV_14_BASE)
#endif
#if defined(GROUP_AST1070_COMPANION)
#define AST_I2C_DEV_15_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_15_BASE)
#define AST_I2C_DEV_16_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_16_BASE)
#define AST_I2C_DEV_17_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_17_BASE)
#define AST_I2C_DEV_18_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_18_BASE)
#define AST_I2C_DEV_19_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_19_BASE)
#define AST_I2C_DEV_20_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_20_BASE)
#define AST_I2C_DEV_21_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_21_BASE)
#define AST_I2C_DEV_22_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_22_BASE)
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#define AST_I2C_DEV_23_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_23_BASE)
#define AST_I2C_DEV_24_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_24_BASE)
#define AST_I2C_DEV_25_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_25_BASE)
#define AST_I2C_DEV_26_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_26_BASE)
#define AST_I2C_DEV_27_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_27_BASE)
#define AST_I2C_DEV_28_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_28_BASE)
#define AST_I2C_DEV_29_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_29_BASE)
#define AST_I2C_DEV_30_VA_BASE		IO_ADDRESS2(AST_I2C_DEV_30_BASE)
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
#define AST_I2C_POLLBUF_VA_BASE		IO_ADDRESS(AST_I2C_POLLBUF_BASE)
#endif
void i2c_as_ssif_enable(int bus);
#endif

extern struct i2c_as_data as_data_ptr[BUS_COUNT];

/** Default values in these arrays are for the as **/
static u32 i2c_as_io_base_global = AST_I2C_DEV_G_VA_BASE;

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
#if (BUS_COUNT>=10)
	AST_I2C_DEV_10_VA_BASE,
#endif
#if (BUS_COUNT>=11)
	AST_I2C_DEV_11_VA_BASE,
#endif
#if (BUS_COUNT>=12)
	AST_I2C_DEV_12_VA_BASE,
#endif
#if (BUS_COUNT>=13)
	AST_I2C_DEV_13_VA_BASE,
#endif
#if (BUS_COUNT>=14)
	AST_I2C_DEV_14_VA_BASE,
#endif
#if (BUS_COUNT>=15)
	AST_I2C_DEV_15_VA_BASE,
#endif
#if (BUS_COUNT>=16)
	AST_I2C_DEV_16_VA_BASE,
#endif
#if (BUS_COUNT>=17)
	AST_I2C_DEV_17_VA_BASE,
#endif
#if (BUS_COUNT>=18)
	AST_I2C_DEV_18_VA_BASE,
#endif
#if (BUS_COUNT>=19)
	AST_I2C_DEV_19_VA_BASE,
#endif
#if (BUS_COUNT>=20)
	AST_I2C_DEV_20_VA_BASE,
#endif
#if (BUS_COUNT>=21)
	AST_I2C_DEV_21_VA_BASE,
#endif
#if (BUS_COUNT>=22)
	AST_I2C_DEV_22_VA_BASE,
#endif
#if (BUS_COUNT>=23)
	AST_I2C_DEV_23_VA_BASE,
#endif
#if (BUS_COUNT>=24)
	AST_I2C_DEV_24_VA_BASE,
#endif
#if (BUS_COUNT>=25)
	AST_I2C_DEV_25_VA_BASE,
#endif
#if (BUS_COUNT>=26)
	AST_I2C_DEV_26_VA_BASE,
#endif
#if (BUS_COUNT>=27)
	AST_I2C_DEV_27_VA_BASE,
#endif
#if (BUS_COUNT>=28)
	AST_I2C_DEV_28_VA_BASE,
#endif
#if (BUS_COUNT>=29)
	AST_I2C_DEV_29_VA_BASE,
#endif
#if (BUS_COUNT>=30)
	AST_I2C_DEV_30_VA_BASE,
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
#if (BUS_COUNT>=10)
	DEFAULT_SLAVE_9,
#endif
#if (BUS_COUNT>=11)
	DEFAULT_SLAVE_10,
#endif
#if (BUS_COUNT>=12)
	DEFAULT_SLAVE_11,
#endif
#if (BUS_COUNT>=13)
	DEFAULT_SLAVE_12,
#endif
#if (BUS_COUNT>=14)
	DEFAULT_SLAVE_13,
#endif
#if (BUS_COUNT>=15)
	DEFAULT_SLAVE_14,
#endif
#if (BUS_COUNT>=16)
	DEFAULT_SLAVE_15,
#endif
#if (BUS_COUNT>=17)
	DEFAULT_SLAVE_16,
#endif
#if (BUS_COUNT>=18)
	DEFAULT_SLAVE_17,
#endif
#if (BUS_COUNT>=19)
	DEFAULT_SLAVE_18,
#endif
#if (BUS_COUNT>=20)
	DEFAULT_SLAVE_19,
#endif
#if (BUS_COUNT>=21)
	DEFAULT_SLAVE_20,
#endif
#if (BUS_COUNT>=22)
	DEFAULT_SLAVE_21,
#endif
#if (BUS_COUNT>=23)
	DEFAULT_SLAVE_22,
#endif
#if (BUS_COUNT>=24)
	DEFAULT_SLAVE_23,
#endif
#if (BUS_COUNT>=25)
	DEFAULT_SLAVE_24,
#endif
#if (BUS_COUNT>=26)
	DEFAULT_SLAVE_25,
#endif
#if (BUS_COUNT>=27)
	DEFAULT_SLAVE_26,
#endif
#if (BUS_COUNT>=28)
	DEFAULT_SLAVE_27,
#endif
#if (BUS_COUNT>=29)
	DEFAULT_SLAVE_28,
#endif
#if (BUS_COUNT>=30)
	DEFAULT_SLAVE_29,
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
#if (BUS_COUNT>=10)
	AST_I2C_9_CLOCK,
#endif
#if (BUS_COUNT>=11)
	AST_I2C_10_CLOCK,
#endif
#if (BUS_COUNT>=12)
	AST_I2C_11_CLOCK,
#endif
#if (BUS_COUNT>=13)
	AST_I2C_12_CLOCK,
#endif
#if (BUS_COUNT>=14)
	AST_I2C_13_CLOCK,
#endif
#if (BUS_COUNT>=15)
	AST_I2C_14_CLOCK,
#endif
#if (BUS_COUNT>=16)
	AST_I2C_15_CLOCK,
#endif
#if (BUS_COUNT>=17)
	AST_I2C_16_CLOCK,
#endif
#if (BUS_COUNT>=18)
	AST_I2C_17_CLOCK,
#endif
#if (BUS_COUNT>=19)
	AST_I2C_18_CLOCK,
#endif
#if (BUS_COUNT>=20)
	AST_I2C_19_CLOCK,
#endif
#if (BUS_COUNT>=21)
	AST_I2C_20_CLOCK,
#endif
#if (BUS_COUNT>=22)
	AST_I2C_21_CLOCK,
#endif
#if (BUS_COUNT>=23)
	AST_I2C_22_CLOCK,
#endif
#if (BUS_COUNT>=24)
	AST_I2C_23_CLOCK,
#endif
#if (BUS_COUNT>=25)
	AST_I2C_24_CLOCK,
#endif
#if (BUS_COUNT>=26)
	AST_I2C_25_CLOCK,
#endif
#if (BUS_COUNT>=27)
	AST_I2C_26_CLOCK,
#endif
#if (BUS_COUNT>=28)
	AST_I2C_27_CLOCK,
#endif
#if (BUS_COUNT>=29)
	AST_I2C_28_CLOCK,
#endif
#if (BUS_COUNT>=30)
	AST_I2C_29_CLOCK,
#endif
};

u32 i2c_as_read_reg( int bus, u32 offset )
{
  if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << bus))
    return( 0 );
  else
    return( ioread32( (void * __iomem)i2c_as_io_base[ bus ] + offset ) );
}
EXPORT_SYMBOL(i2c_as_read_reg);

void i2c_as_write_reg( int bus, u32 value, u32 offset )
{
  if (!(CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << bus)))
    iowrite32( value, (void * __iomem)i2c_as_io_base[ bus ] + offset );
}
EXPORT_SYMBOL(i2c_as_write_reg);

u32 i2c_as_read_global_reg( u32 offset )
{
  return( ioread32( (void * __iomem)i2c_as_io_base_global + offset ) );
}
EXPORT_SYMBOL(i2c_as_read_global_reg);

void i2c_as_write_global_reg( u32 value, u32 offset )
{
  iowrite32( value, (void * __iomem)i2c_as_io_base_global + offset );
}
EXPORT_SYMBOL(i2c_as_write_global_reg);

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

	/* Disable Both Master and Slave function when resetting device */
	ctrl_bits = i2c_as_read_reg(bus,I2C_FUNCTION_CONTROL_REG);
	ctrl_bits &= ~( ENABLE_MASTER_FUNC |ENABLE_SLAVE_FUNC);
	i2c_as_write_reg( bus,ctrl_bits,I2C_FUNCTION_CONTROL_REG);
	udelay(100);
	i2c_init_hardware(bus);
}

void i2c_stop_cntlr_reset()
{
	uint32_t reg;
    
	iowrite32(SCU_PROTECTION_UNLOCK, (void * __iomem)SCU_KEY_CONTROL_REG); /* unlock SCU */
	reg = ioread32((void * __iomem)SCU_SYS_RESET_REG);  // stop I2C controller reset

	reg &= ~I2C_RESET_CNTLR_BIT;                       
	iowrite32(reg, (void * __iomem)SCU_SYS_RESET_REG);	 
	iowrite32(0, (void * __iomem)SCU_KEY_CONTROL_REG); /* lock SCU */
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

#if !defined(SOC_AST3100)
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
	/* Enable SRAM Buffer */
	i2c_as_write_global_reg( ENABLE_SRAM_BUFFER, I2C_GLOBAL_CONTROL_REG);
#endif
    /* DMA Mode Control Register */
	i2c_as_write_reg(bus, 0, I2C_DMA_MODE_CONTROL_REG);
#endif
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

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
	if (as_data_ptr[bus].SSIF_enable == 1)
		i2c_as_ssif_enable(bus); 
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

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
	if (as_data_ptr[bus].SSIF_enable == 1) 
		i2c_as_ssif_enable(bus); 
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

int i2c_as_set_speed(int bus, unsigned short speed)
{
	/* Need to disable the I2C interface before writing to the I2C Control Register */
	i2c_disable_bus(bus);

	switch(speed)
	{
		case I2C_BUS_SPEED_MODE_STANDARD: // 100 Kbit/s
		    i2c_as_write_reg (bus, CLOCK_AC_1_VAL_100K, I2C_CLOCK_AC_1_CONTROL_REG);
			break;
			
		case I2C_BUS_SPEED_MODE_FAST:	// 400 Kbit/s
		    i2c_as_write_reg (bus, CLOCK_AC_1_VAL_400K, I2C_CLOCK_AC_1_CONTROL_REG);
			break;
			
		default:	
			printk("ERROR: I2C%d: Invalid Speed Mode requested to set.\n", bus);
			return -1;
	}	

	/* Control Register is set. Now enable back the i2c interface */
	i2c_enable_bus(bus);
	return 0;
}

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
void i2c_as_ssif_enable(int bus) 
{ 
		u32 ctrl_bits; 
 
		/* Disable Master function  , Set buffer page selection = 0*/
		ctrl_bits = i2c_as_read_reg(bus,I2C_FUNCTION_CONTROL_REG); 
		ctrl_bits &= ~( ENABLE_MASTER_FUNC); 
		ctrl_bits |= ENABLE_SLAVE_FUNC;
		#if defined(GROUP_AST2300_PLUS)
		ctrl_bits &= ~( BUFFER_SELECTION_PAGE_MASK);
		ctrl_bits |= BUFFER_SELECTION_PAGE_0;
		#endif
		i2c_as_write_reg( bus,ctrl_bits,I2C_FUNCTION_CONTROL_REG); 
		udelay(100); 
		 
	 /* clear interrups */ 
		i2c_as_disable_all_interrupts(bus); 
		i2c_as_write_reg (bus, CLR_ALL_INTS_VAL, I2C_INTR_STATUS_REG); 
		 
		as_data_ptr[bus].SSIF_enable = 1; 
		as_data_ptr[bus].TX_len = 0; 
		as_data_ptr[bus].TX_index = 0; 
		 
		as_data_ptr[bus].MasterRX_len = 0; 
		as_data_ptr[bus].MasterRX_index = 0; 

		as_data_ptr[bus].i2c_dma_mode = I2C_BYTE_MODE;

		/*enable SRAM buffer for pool buffer or DMA mode*/
		#if defined(GROUP_AST2500)
		i2c_as_write_global_reg( ENABLE_SRAM_BUFFER, I2C_GLOBAL_CONTROL_REG);
		#endif
	
		/*init ssif tx pool/DMA buffer */
		#if defined(GROUP_AST1070_COMPANION) || defined(GROUP_AST2500)
		i2c_as_ssif_alloc_tx_dma_buf(bus);
		#else
		as_data_ptr[bus].SSIF_tx_buff_base = AST_I2C_POLLBUF_VA_BASE;
		#endif
	
		/* Enable interrupts */ 
		i2c_as_write_reg (bus,	ENABLE_TX_DONE_WITH_ACK_INTR  			| \
					   ENABLE_TX_DONE_WITH_NACK_INTR 			| \
					   ENABLE_RX_DONE_INTR|
								ENABLE_STOP_CONDITION_RECVD_INTR|
								ENABLE_ABNORMAL_START_STOP_RECVD_INTR|
								ENABLE_SLAVE_ADDR_MATCH_INTR,
										I2C_INTR_CONTROL_REG); 
}

void i2c_as_ssif_alloc_tx_dma_buf(int bus)
{
		 if (as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff == NULL)
		 {
		     as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff = dma_alloc_coherent(NULL, SSIF_TX_DMA_BUFFER_SIZE, &as_data_ptr[bus].ast_i2c_ssif_tx_dma_addr, GFP_KERNEL);
			    if (as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff == NULL)
			    {
			        printk("ERROR: allocate SSIF Tx DMA buffer failed.\n");
			        i2c_disable_bus(bus);
			    }
       if(as_data_ptr[bus].ast_i2c_ssif_tx_dma_addr%4 != 0)
			    {
           printk("ERROR: allocate SSIF Tx DMA buffer failed: not 4 byte boundary.\n");
           i2c_as_ssif_free_tx_dma_buf(bus);
			        i2c_disable_bus(bus);
			    }
       memset(as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff, 0xFF, SSIF_TX_DMA_BUFFER_SIZE);
		 }
}

void i2c_as_ssif_free_tx_dma_buf(int bus)
{
    if(as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff != NULL)
    {
        dma_free_coherent(NULL, SSIF_TX_DMA_BUFFER_SIZE, as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff, as_data_ptr[bus].ast_i2c_ssif_tx_dma_addr);
        as_data_ptr[bus].ast_i2c_ssif_tx_dma_buff = NULL;
        as_data_ptr[bus].ast_i2c_ssif_tx_dma_addr = 0;
    }
}

void i2c_as_set_ssif_block(int bus, u8 block_num) 
{ 
		 if (as_data_ptr[bus].TX_len < (SSIF_MAX_RES_SIZE-2)) 
		printk("ERROR: I2C%d: can not set SSIF retry block number, the response data length %d < %d with single-part format\n", bus, as_data_ptr[bus].TX_len, SSIF_MAX_RES_SIZE-2); 
	else 
	{ 
		   if (30 + (block_num * 31) <= as_data_ptr[bus].TX_len) 
			as_data_ptr[bus].SSIF_RES_blocknum = block_num; 
		else 
		{ 
				  as_data_ptr[bus].SSIF_RES_blocknum =((as_data_ptr[bus].TX_len - 30) / 31) + (((as_data_ptr[bus].TX_len - 30)%31 == 0)?0:1) - 1; 
			printk("ERROR: I2C%d: Invalid SSIF retry block number %d\n", bus, block_num); 
		} 
		//printk("I2C%d: set SSIF retry block number %d\n", bus, as_data_ptr[bus].SSIF_RES_blocknum); 
	} 
}
#endif



