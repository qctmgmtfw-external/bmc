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

extern struct i2c_as_data as_data_ptr[BUS_COUNT];

#define AST_I2C_GLOBAL_VA_BASE 		IO_ADDRESS(AST_I2C_GLOBAL_BASE)

static void 
i2cTXEMPTY_master_process(int bus, u32 status)
{

	if ( as_data_ptr[bus].TX_index == (as_data_ptr[bus].TX_len))
	{
			
			if (as_data_ptr[bus].MasterRX_len == 0) // Single Write command, not repeat start condition */
			{
				/* Generate STOP on bus for write only command */
				i2c_as_write_reg( bus, MASTER_STOP, I2C_CMD_STATUS_REG ); //fire stop command only in single write
			}
		
			/* Disable the TX interrupt */
			i2c_as_disable_interrupt(bus,ENABLE_TX_DONE_WITH_ACK_INTR);
			as_data_ptr[bus].op_status=status;
			as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
			wake_up( &as_data_ptr[bus].as_wait );
				
			return;
	}
	
	else if (as_data_ptr[bus].TX_index < as_data_ptr[bus].TX_len)
	{	
			
			i2c_as_write_reg(bus,as_data_ptr[bus].TX_data[as_data_ptr[bus].TX_index],I2C_DATA_REG);
			i2c_as_write_reg(bus, MASTER_TRANSMIT, I2C_CMD_STATUS_REG );
			
			as_data_ptr[bus].TX_index ++;
	}
	
		return;
}

static int 
i2cRXFULL_master_process(int bus,u32 status)
{
	
		if (as_data_ptr[bus].block_read == 1 ||as_data_ptr[bus].block_proc_call == 1)
		{
			
			if (as_data_ptr[bus].MasterRX_index == 0)
			{
				as_data_ptr[bus].MasterRX_data[as_data_ptr[bus].MasterRX_index]= i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
				as_data_ptr[bus].MasterRX_len = as_data_ptr[bus].MasterRX_data[as_data_ptr[bus].MasterRX_index] + 1;
				
				if (as_data_ptr[bus].MasterRX_index < (as_data_ptr[bus].MasterRX_len -1))
				{
					i2c_as_write_reg( bus, MASTER_RECEIVE, I2C_CMD_STATUS_REG );
				}
				as_data_ptr[bus].MasterRX_index ++;
				return 0;
			}
		}
	if (as_data_ptr[bus].MasterRX_index < as_data_ptr[bus].MasterRX_len)
	{	
		
		as_data_ptr[bus].MasterRX_data[as_data_ptr[bus].MasterRX_index] = i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
		if (as_data_ptr[bus].MasterRX_index < (as_data_ptr[bus].MasterRX_len -1))
		{
			i2c_as_write_reg( bus, MASTER_RECEIVE, I2C_CMD_STATUS_REG );

		}
		
		as_data_ptr[bus].MasterRX_index ++;
		if ( as_data_ptr[bus].MasterRX_index == ((as_data_ptr[bus].MasterRX_len )) )
		{
			as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
			as_data_ptr[bus].op_status=status;
			wake_up( &as_data_ptr[bus].as_wait);
			return 1;
		}
		
	}

	return 0;
}

static int 
i2cRXFULL_slave_process(int bus,u32 status)
{
	if (as_data_ptr[bus].host_notify_flag == 1)
	{
		if (as_data_ptr[bus].SMB_Linear_SlaveRX_index < MAX_IPMB_MSG_SIZE)
		{
			as_data_ptr[bus].SMB_Linear_SlaveRX_data[as_data_ptr[bus].SMB_Linear_SlaveRX_index] =i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
			as_data_ptr[bus].SMB_Linear_SlaveRX_index++;
		}
	}

	else
	{
		if (as_data_ptr[bus].Linear_SlaveRX_index < MAX_IPMB_MSG_SIZE)
		{
			as_data_ptr[bus].Linear_SlaveRX_data[as_data_ptr[bus].Linear_SlaveRX_index] =i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
			as_data_ptr[bus].Linear_SlaveRX_index++;
		}
	}
	return 0;
}

static void 
i2cSTOPDET_slave_process(int bus)
{
	int FifoPtr;
	unsigned char *DataBuffer;
	unsigned long length = 0;
   	spin_lock( &as_data_ptr[bus].data_lock );		
if (as_data_ptr[bus].host_notify_flag == 0)
{

	if (as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN)
	{
		/* Return back */
		spin_unlock( &as_data_ptr[bus].data_lock );		
		return;
	}

	FifoPtr = as_data_ptr[bus].SlaveRX_Writer;

	as_data_ptr[bus].SlaveRX_index[FifoPtr] = 0;

	DataBuffer = as_data_ptr[bus].SlaveRX_data[FifoPtr];
	length = as_data_ptr[bus].Linear_SlaveRX_index;
	if (length==0)
	{
		/* Dummy STOP or Master STOP ?*/
		spin_unlock( &as_data_ptr[bus].data_lock );		
		return;
	}

	/* First byte of buffer should be filled with slave address */
	//DataBuffer[as_data_ptr[bus].SlaveRX_index[FifoPtr]++] =i2c_as_read_reg(bus,I2C_SLAVE_ADDR_REG) << 1;
	
	/* Read the Length and oopy to buffer */
	if(length)
		//memcpy(&DataBuffer[1],as_data_ptr[bus].Linear_SlaveRX_data,length);
		memcpy(&DataBuffer[0],as_data_ptr[bus].Linear_SlaveRX_data,length);
		
	//as_data_ptr[bus].SlaveRX_index[FifoPtr] = as_data_ptr[bus].SlaveRX_len[FifoPtr]= length+1;
	as_data_ptr[bus].SlaveRX_index[FifoPtr] = as_data_ptr[bus].SlaveRX_len[FifoPtr]= length;
	as_data_ptr[bus].Linear_SlaveRX_index = 0;

	/* The following happens when emtpy packet (START, followed by a STOP) is detected*/
	if (as_data_ptr[bus].SlaveRX_len[FifoPtr]  == 1)
	{
		as_data_ptr[bus].SlaveRX_len[FifoPtr] = 0;
		as_data_ptr[bus].SlaveRX_index[FifoPtr] = 0;
	}
	else
	{
		if ((++as_data_ptr[bus].SlaveRX_Writer) == MAX_FIFO_LEN)
			as_data_ptr[bus].SlaveRX_Writer = 0;
		as_data_ptr[bus].SlaveRX_Entries++;

		/* Check if buffer full. If so disable slave */
		if (as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN)
		{
			i2c_as_disable_slave(bus);
		//	printk("I2C%d: Disabling Slave as the Fifo is Full\n",bus);
		}
	}
}
else
{
	if (as_data_ptr[bus].SMB_SlaveRX_Entries == MAX_FIFO_LEN)
	{
		/* Return back */
		spin_unlock( &as_data_ptr[bus].data_lock );		
		return;
	}
	FifoPtr = as_data_ptr[bus].SMB_SlaveRX_Writer;

	as_data_ptr[bus].SMB_SlaveRX_index[FifoPtr] = 0;

	DataBuffer = as_data_ptr[bus].SMB_SlaveRX_data[FifoPtr];
	//length = as_data_ptr[bus].SMB_Linear_SlaveRX_index;
	length = as_data_ptr[bus].SMB_Linear_SlaveRX_index-2; // When host-notify protocol, host slave receive first and second data same value of 0x10. 
	if (length==0)
	{
		/* Dummy STOP or Master STOP ?*/
		spin_unlock( &as_data_ptr[bus].data_lock );		
		return;
	}
	/* First byte of buffer should be filled with slave address */
	//DataBuffer[as_data_ptr[bus].SlaveRX_index[FifoPtr]++] =i2c_as_read_reg(bus,I2C_SLAVE_ADDR_REG) << 1;
	
	/* Read the Length and copy to buffer */
	if(length)
		//memcpy(&DataBuffer[1],as_data_ptr[bus].Linear_SlaveRX_data,length);
		memcpy(&DataBuffer[0],as_data_ptr[bus].SMB_Linear_SlaveRX_data + 2,length); //only copy real 3 byte data from master to host slave buffer
		
	//as_data_ptr[bus].SlaveRX_index[FifoPtr] = as_data_ptr[bus].SlaveRX_len[FifoPtr]= length+1;
	as_data_ptr[bus].SMB_SlaveRX_index[FifoPtr] = as_data_ptr[bus].SMB_SlaveRX_len[FifoPtr]= length;
	as_data_ptr[bus].SMB_Linear_SlaveRX_index = 0;

	/* The following happens when emtpy packet (START, followed by a STOP) is detected*/
	if (as_data_ptr[bus].SMB_SlaveRX_len[FifoPtr]  == 1)
	{
		as_data_ptr[bus].SMB_SlaveRX_len[FifoPtr] = 0;
		as_data_ptr[bus].SMB_SlaveRX_index[FifoPtr] = 0;
	}
	else
	{
		if ((++as_data_ptr[bus].SMB_SlaveRX_Writer) == MAX_FIFO_LEN)
			as_data_ptr[bus].SMB_SlaveRX_Writer = 0;
		as_data_ptr[bus].SMB_SlaveRX_Entries++;

		/* Check if buffer full. If so disable slave */
		if (as_data_ptr[bus].SMB_SlaveRX_Entries == MAX_FIFO_LEN)
		{
			i2c_as_disable_slave(bus);
		//	printk("I2C%d: Disabling Slave as the Fifo is Full\n",bus);
		}
	}
}

	spin_unlock( &as_data_ptr[bus].data_lock );		
	wake_up_interruptible(&as_data_ptr[bus].as_slave_data_wait);

	return;
}

static void
as_handler(int bus)
{
	unsigned long status2;
	
	unsigned long intEnb;
	unsigned long i2c_int;
	static unsigned slave_mode[9] = {0,0,0,0,0,0,0,0,0};
	
	status2 = i2c_as_read_reg( bus, I2C_INTR_STATUS_REG );
	
	intEnb = i2c_as_read_reg( bus, I2C_INTR_CONTROL_REG );
	
	i2c_int = status2 & intEnb;

	/* NACK handling */
	if(i2c_int & TX_DONE_WITH_NACK)
	{
		//printk("\n !!!!! NACK on data Transmission!!!!!\n");

		/* Stop bus when NACK happened */
		i2c_as_write_reg( bus, MASTER_STOP, I2C_CMD_STATUS_REG );
		as_data_ptr[bus].op_status = i2c_int;
		
		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |TX_DONE_WITH_NACK),I2C_INTR_STATUS_REG );	
		wake_up( &as_data_ptr[bus].as_wait );
	}

	/* the following 2 interrupt are for smbus specs */
	if (i2c_int & SMBUS_ARP_HOST_ADDR_MATCH)
	{
		//printk("\n SMBUS_ARP_HOST_ADDR_MATCH\n");
		as_data_ptr[bus].host_notify_flag = 1;
		
		if (as_data_ptr[bus].master_xmit_recv_mode_flag == SLAVE)
		{
			i2cRXFULL_slave_process(bus,status2);
		}

		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |SMBUS_ARP_HOST_ADDR_MATCH),I2C_INTR_STATUS_REG );
	}

	if (i2c_int & SCL_LOW_TIMEOUT)
	{
		printk(" SMB TIMEOUT \n");
		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |SCL_LOW_TIMEOUT),I2C_INTR_STATUS_REG );
		
		if(i2c_bus_recovery(bus) != 0)
		{
			printk("SMBus timeout. cannot recover %x bus \n",bus);
		}
		i2c_init_hardware(bus);
	}

	if (i2c_int & SLAVE_ADDR_MATCH)
	{
		slave_mode[bus] = 1;

		if((i2c_int & RX_DONE) != RX_DONE)
		{
			i2c_as_write_reg( bus, (status2 |SLAVE_ADDR_MATCH),I2C_INTR_STATUS_REG );
		}
	}
	
	/* Slave_Master_Receive */
	if(i2c_int & RX_DONE)
	{
		//printk("\n mr\n");
		//printk("m %x r %x\n",as_data_ptr[bus].master_xmit_recv_mode_flag,as_data_ptr[bus].restart);

		if (as_data_ptr[bus].master_xmit_recv_mode_flag == SLAVE) 
		{
				i2cRXFULL_slave_process(bus,status2);
		}
		else if ((as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_RECV))
		{
			i2cRXFULL_master_process(bus,status2);
			/* if this is the last byte */
			if (as_data_ptr[bus].MasterRX_index == (as_data_ptr[bus].MasterRX_len)-1)
			{
				i2c_as_write_reg( bus, MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
			}
		}
		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |RX_DONE),I2C_INTR_STATUS_REG );
	}

	
	/* Slave_Stop Received */ 
 	if (i2c_int & STOP_DETECTED)	
	{
		if(as_data_ptr[bus].master_xmit_recv_mode_flag == SLAVE && (slave_mode[bus] == 1 || as_data_ptr[bus].host_notify_flag ==1 ))
			{
				i2cSTOPDET_slave_process(bus);
			}
			else
			{
				//printk("ERROR: Spurious SLAVE STOP_RECEVIED interrupt\n");
			}

		slave_mode[bus] = 0;
		as_data_ptr[bus].host_notify_flag = 0;

		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |STOP_DETECTED),I2C_INTR_STATUS_REG );
 	}

	/* Master_Transmit */
	if(i2c_int & TX_DONE_WITH_ACK)
	{
		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |TX_DONE_WITH_ACK), I2C_INTR_STATUS_REG );

		if (as_data_ptr[bus].address_sent == 1) 
		{
			as_data_ptr[bus].address_sent = 0;
			if (as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_XMIT)
				i2cTXEMPTY_master_process(bus,status2);
			
			else if ((as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_RECV))
			{
				/* for receive only one byte */
			
				if ((as_data_ptr[bus].MasterRX_index == (as_data_ptr[bus].MasterRX_len -1)))
				{
					i2c_as_write_reg( bus, MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
				}
				/* for receive multiple bytes */
				else 
				{
					i2c_as_write_reg( bus, MASTER_RECEIVE,I2C_CMD_STATUS_REG);
				}
			}
		}
		else
		{
			if (as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_XMIT)
				i2cTXEMPTY_master_process(bus,status2);
		}
	}

	/* TX_ABRT geenrated as master */
	if(i2c_int & MASTER_ARBITRATION_LOST)
	{
		printk("\n !!!!!master-abort!!!!!!!!!\n");
		
		/* clear status bit */
		i2c_as_write_reg( bus, (status2 | MASTER_ARBITRATION_LOST),I2C_INTR_STATUS_REG );
		
		as_data_ptr[bus].op_status = status2;
		as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
		wake_up_interruptible( &as_data_ptr[bus].as_wait );
	}

	if(i2c_int & ABNORMAL_START_STOP_DETECTED)
	{
		printk("\n !!!!!! master-abnormal stop-start !!!!!!!!!\n");

		/* clear status bit */
		i2c_as_write_reg( bus, (status2 |ABNORMAL_START_STOP_DETECTED),I2C_INTR_STATUS_REG );
	}
}

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
irqreturn_t 
i2c_handler( int this_irq, void *dev_id)
{
	   int 		bus;
	
	volatile unsigned long dev_intr_status_reg;

	/* Determine the bus for this interrupt */
	dev_intr_status_reg = *(unsigned long *) (AST_I2C_GLOBAL_VA_BASE);

	/* Execute the interrupt for all i2c devices */
    for (bus = 0; bus < BUS_COUNT; bus++) {
		if (dev_intr_status_reg & (1 << bus)) {
			as_handler(bus);
		}
    }
	return IRQ_HANDLED;
}

#else
irqreturn_t 
i2c_handler( int this_irq, void *dev_id,
                                 struct pt_regs *regs )
{
	   int 		bus;
	
	volatile unsigned long dev_intr_status_reg;

	/* Determine the bus for this interrupt */
	dev_intr_status_reg = *(unsigned long *) (AST_I2C_GLOBAL_VA_BASE);	

	/* Execute the interrupt for all i2c devices */
    for (bus = 0; bus < BUS_COUNT; bus++) {
		if (dev_intr_status_reg & (1 << bus)) {
			as_handler(bus);
		}
    }
	return IRQ_HANDLED;
}

#endif




