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



extern struct i2c_as_data as_data_ptr[BUS_COUNT];


static bus_recovery_info_T m_bus_recovery_info [BUS_COUNT] =
{
#if (BUS_COUNT>=1)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=2)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=3)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=4)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=5)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=6)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=7)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=8)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=9)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=10)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=11)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=12)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=13)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=14)
	{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=15)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=16)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=17)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=18)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=19)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=20)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=21)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=22)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=23)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=24)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=25)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=26)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=27)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=28)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=29)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif
#if (BUS_COUNT>=30)
{ DISABLE_RECOVERY, DISABLE_SMBUS_RESET, ENABLE_CLOCK_PULSE, DISABLE_FORCE_STOP, AST_DEFAULT_NUM_PULSES, DEFAULT_FREQ },
#endif

};

static void
drive_data_low (int bus)
{
	i2c_as_write_reg( bus, SDA_OE | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
	i2c_as_write_reg( bus, SDA_LOW & i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
}

static void
drive_data_high (int bus)
{
	i2c_as_write_reg( bus, SDA_OE | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
	i2c_as_write_reg( bus, SDA_HIGH | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
}

static void
drive_clock_low (int bus)
{
	i2c_as_write_reg( bus, SCL_OE | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
	i2c_as_write_reg( bus, SCL_LOW & i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
}

static void
drive_clock_high (int bus)
{
	i2c_as_write_reg( bus, SCL_OE | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
	i2c_as_write_reg( bus, SCL_HIGH | i2c_as_read_reg (bus,I2C_CMD_STATUS_REG), I2C_CMD_STATUS_REG );
}

static void
drive_clock_data_disable (int bus)
{
	i2c_as_write_reg( bus, (~(SDA_OE|SCL_OE) & i2c_as_read_reg (bus,I2C_CMD_STATUS_REG)), I2C_CMD_STATUS_REG );
}

static unsigned long
get_clk_status(int bus)
{
	return (i2c_as_read_reg (bus,I2C_CMD_STATUS_REG)& SAMPLE_SCL_LINE);
}

static unsigned long
get_sda_status(int bus)
{
	return (i2c_as_read_reg( bus,I2C_CMD_STATUS_REG) & SAMPLE_SDA_LINE);
}

static int perform_slave_recovery(int bus)
{
	int pulse_period = 5;
	int pulse_num = 0; 
	
	/* Disable Both Master and Salve function when using GPIO recovery method */
	i2c_disable_bus (bus);
	pulse_period = (int) (500000/(m_bus_recovery_info[bus].frequency));

	/*see if Generate Clock pulses option is enabled */		
	if (m_bus_recovery_info[bus].is_generate_clock_pulses_enabled)
	{
		//printk("I2C%d: Slave recovery-Generate Clock Pulses...\n",bus);
		pulse_num = m_bus_recovery_info[bus].num_clock_pulses;
		while(pulse_num)
		{
			pulse_num -= 1; 

			/* make SCL low */ 
			drive_clock_low (bus);

			udelay (pulse_period); 

			/* make SCL high */
			drive_clock_high (bus);

			udelay (pulse_period);
			if (0 != (get_sda_status(bus))) 
			{
				//printk("I2C%d: Slave recovery-Bus Recovered\n",bus);
				break;
			}
		}
	}

	if ((0 == get_sda_status(bus))||(0 == get_clk_status(bus)))
	{
		if (m_bus_recovery_info[bus].is_force_stop_enabled)
		{
			//printk("I2C%d: Slave recovery-Creating Forced STOP Condition\n",bus);

			/* make Data Low */
			drive_data_low(bus);
			
			/* make SCL high */
			drive_clock_high (bus);

			/* make Data High */
			drive_data_high (bus);

			udelay (5);
		}
	}

	drive_clock_data_disable (bus);
	i2c_enable_bus (bus);
			
	/* If any of the line is still low, it's a failure */ 
	if ((0 == get_sda_status(bus))||(0 == get_clk_status(bus)))
	{
		if (0 == get_sda_status(bus))
			as_data_ptr[bus].i2c_link_state = I2C_UNABLE_DRIVE_DATA_HIGH;
		if (0 == get_clk_status(bus))
			as_data_ptr[bus].i2c_link_state = I2C_UNABLE_DRIVE_CLK_HIGH;
		return 1;
	}
	return 0;
}

static int perform_as_recovery(int bus)
{
	i2c_as_reset(bus);

	/* If any of the line is still low, it's a failure */ 
	if ((0 == get_sda_status(bus))||(0 == get_clk_status(bus)))
		return 1;

	return 0;
}

int as_i2c_bus_recovery(struct i2c_adapter *i2c_adap)
{
	if (m_bus_recovery_info[i2c_adap->nr].enable_recovery == DISABLE_RECOVERY)
		return -ENOSYS;

	return i2c_bus_recovery(i2c_adap->nr);
}

int i2c_bus_recovery(int bus)
{
	int SDAStatusCheckTimes;
	int CLKStatusCheckTimes;
	unsigned long SDAStatus;
	unsigned long CLKStatus;
	int bus_status = 0;

	as_data_ptr[bus].i2c_link_state = I2C_UNDER_TEST;

	SDAStatusCheckTimes = 100;
	while (SDAStatusCheckTimes)
	{
		touch_softlockup_watchdog();
		SDAStatus = get_sda_status(bus);
		if (0 != SDAStatus) { break; }
		else SDAStatusCheckTimes--;
		udelay (25);
	}

	CLKStatusCheckTimes = 100;
	while (CLKStatusCheckTimes)
	{
		touch_softlockup_watchdog();
		CLKStatus = get_clk_status(bus);
		if (0 != CLKStatus) { break; }
		else CLKStatusCheckTimes--;
		udelay (25);
	}

	if ((SDAStatusCheckTimes == 0) || (CLKStatusCheckTimes == 0))
	{
		//printk("I2C%d: data or clock low by another device\n",bus);
		/* data is pulled low by a slave on the bus */
		bus_status = perform_slave_recovery(bus);
		if(bus_status == 0)
		{
			//printk("I2C%d: Slave recovery succeed\n",bus);
			return 0;
		}
		
		//printk("I2C%d: Recover AST: Data or Clock is stuck\n",bus);
		bus_status = perform_as_recovery(bus);
		if(bus_status == 0)
		{
			//printk("I2C%d: AST recovery succeed\n",bus);
			return 0;
		}
		
		printk("ERROR: I2C%d: BUS recovery did not succeed\n",bus);
		return -EIO;
	}
	else
	{
		as_data_ptr[bus].i2c_link_state = I2C_LINK_NO_FAILURE;
	}


	return 0;
}

int as_set_recovery_info (int bus, bus_recovery_info_T* info)
{
	if (bus >= BUS_COUNT) 
		return -1;

	memcpy (&m_bus_recovery_info [bus], info, sizeof (bus_recovery_info_T));
	return 0;	
}


int as_get_recovery_info (int bus, bus_recovery_info_T* info)
{
	if (bus >= BUS_COUNT) 
		return -1;

	memcpy (info, &m_bus_recovery_info [bus], sizeof (bus_recovery_info_T));
	return 0;	
}

int i2c_is_bus_hung(int bus)			//JM_RECOVERY
{
	int StatusCheckTimes;
	unsigned long Status;

	//printk("I2C%d: checking for bus hang...",bus);

	/* Check if data line stuck low for 2.5ms */
	StatusCheckTimes = 100;
	while (StatusCheckTimes)
	{
		touch_softlockup_watchdog();
		//Status = (i2c_as_read_reg( bus,I2C_CMD_STATUS_REG) & SAMPLE_SDA_LINE) ;
		Status = get_sda_status(bus);
		if (0 != Status) 
		{
			as_data_ptr[bus].i2c_link_state =  I2C_LINK_NO_FAILURE;
			break; 
		}
		else 
		{
			
			StatusCheckTimes--;
		}
		udelay (25);
	}
	if (StatusCheckTimes == 0)
	{
		//printk("data stuck low\n");
		
		as_data_ptr[bus].i2c_link_state = I2C_UNABLE_DRIVE_DATA_HIGH;
		return 1; /* data stuck low */
	}
	
	/* Check if clock line stuck low for 2.5ms */
	StatusCheckTimes = 100;
	while (StatusCheckTimes)
	{
		touch_softlockup_watchdog();
		//Status = (i2c_as_read_reg( bus,I2C_CMD_STATUS_REG) & SAMPLE_SCL_LINE);
		Status = get_clk_status(bus);
		if (0 != Status) 
		{
			as_data_ptr[bus].i2c_link_state =  I2C_LINK_NO_FAILURE;
			break;
		}
		else 
		{
			StatusCheckTimes--;
		}
		udelay (25);
	}
	if (StatusCheckTimes == 0)
	{
		//printk("clock stuck low");
		as_data_ptr[bus].i2c_link_state = I2C_UNABLE_DRIVE_CLK_HIGH;
		return 1; /* clock stuck low */
	}
	return 0;
}

int
i2c_bus_test (int bus, i2c_bus_test_T* info)
{
	
	i2c_bus_test_T *i2cTestData = info;

	/* Initilaize the result to zero. (Bus free)*/
	i2cTestData->result = I2C_LINK_NO_FAILURE;
	
	if ((i2cTestData->operation  & I2C_TEST_BUS_BUSY) == I2C_TEST_BUS_BUSY )
	{
		if ( i2c_is_bus_hung( bus) == 0 )
		{
			/*Bus is free */
			i2cTestData->result =( i2cTestData->result  &  (~I2C_TEST_BUS_BUSY));
		}
		else 
		{
			/*Bus hung*/
			i2cTestData->result = i2cTestData->result  |I2C_TEST_BUS_BUSY;
		}
	}

	if ( ( i2cTestData->operation  & I2C_TEST_BUS_RECOVERABLE )  ==  I2C_TEST_BUS_RECOVERABLE )
	{
		if ( i2c_bus_recovery( bus) == 0 )
		{
			i2cTestData->result =( i2cTestData->result  &  (~I2C_TEST_BUS_RECOVERABLE));
		}
		else 
		{
			i2cTestData->result = i2cTestData->result  |I2C_TEST_BUS_RECOVERABLE;
		}
	}
 
	return 0;	
}
