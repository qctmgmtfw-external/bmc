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
#include <mach/platform.h>
#include <asm/io.h>
#include <asm/irq.h>

#include "i2c-data.h"
#include "i2c-hardware.h"

unsigned char slave_mode[BUS_COUNT];
extern struct i2c_as_data as_data_ptr[BUS_COUNT];

#define AST_I2C_GLOBAL_VA_BASE 		IO_ADDRESS(AST_I2C_GLOBAL_BASE)
#if defined(GROUP_AST1070_COMPANION)
#define AST_BMCCC0_I2C_GLOBAL_VA_BASE 		IO_ADDRESS2(AST_BMCCC0_I2C_GLOBAL_BASE)
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#define AST_BMCCC1_I2C_GLOBAL_VA_BASE 		IO_ADDRESS2(AST_BMCCC1_I2C_GLOBAL_BASE)
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

static void i2cSTOPDET_slave_process(int bus); 
 
void
i2cTXFULL_slave_process (int bus)
{
	u8 data = 0x00;

	if (as_data_ptr[bus].i2c_dma_mode == I2C_BYTE_MODE)
	{
		if (as_data_ptr[bus].SlaveTX_index <  as_data_ptr[bus].SlaveTX_RES_len)
		{			   		   
			data = as_data_ptr[bus].SlaveTX_RES_data[as_data_ptr[bus].SlaveTX_index];
			as_data_ptr[bus].SlaveTX_index++;
		}
		else
			data = 0xFF;
				     
		as_data_ptr[bus].SlaveTX_count++;
		i2c_as_write_reg( bus, data,I2C_DATA_REG );
		i2c_as_write_reg(bus, SLAVE_TRANSMIT, I2C_CMD_STATUS_REG);
		if (as_data_ptr[bus].SlaveTX_count == SLAVETX_MAX_RES_SIZE)
		{
			as_data_ptr[bus].SlaveTX_RES_len = 0;
		}
	}
	else
	{
	/*  Default I2C_BYTE_MODE for GROUP_AST2300 and GROUP_AST2300_PLUS.
		Here implement pool buffer transfer mode for testing SSIF.*/
		
#if defined(GROUP_AST1070_COMPANION) || defined(GROUP_AST2500)
		/*SOC_AST1050_1070, SOC_AST2500, SOC_AST2520: Using DMA buffer Mode*/
		for (as_data_ptr[bus].SlaveTX_index=0; as_data_ptr[bus].SlaveTX_index<SLAVETX_MAX_RES_SIZE; as_data_ptr[bus].SlaveTX_index++)
		{
			if (as_data_ptr[bus].SlaveTX_index < as_data_ptr[bus].SlaveTX_RES_len)
				data = as_data_ptr[bus].SlaveTX_RES_data[as_data_ptr[bus].SlaveTX_index];
			else
				data = 0xFF;
			as_data_ptr[bus].dma_buff[as_data_ptr[bus].SlaveTX_index] = data;
		}
		i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG);
#if defined(GROUP_AST2500)
		i2c_as_write_reg(bus, SLAVETX_MAX_RES_SIZE, I2C_DMA_MODE_STATUS_REG);
#elif defined(GROUP_AST1070_COMPANION)
		i2c_as_write_reg(bus, SLAVETX_MAX_RES_SIZE - 1, I2C_DMA_MODE_STATUS_REG);
#endif
		i2c_as_write_reg(bus, SLAVE_TRANSMIT | ENABLE_MASTER_SLAVE_TX_DMA_BUF, I2C_CMD_STATUS_REG);
						   
#elif defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS)
		/*Using Pool buffer Mode*/
		u32 value = 0xFFFFFFFF;
		i2c_as_write_reg(bus, ((SLAVETX_MAX_RES_SIZE - 1) << POOLBUFF_CTRL_TX_END_ADDR_SHIFT), I2C_BUF_CONTROL_REG);
		for (as_data_ptr[bus].SlaveTX_index=0; as_data_ptr[bus].SlaveTX_index<SLAVETX_MAX_RES_SIZE; as_data_ptr[bus].SlaveTX_index++)
		{
			if (as_data_ptr[bus].SlaveTX_index < as_data_ptr[bus].SlaveTX_RES_len)
				data = as_data_ptr[bus].SlaveTX_RES_data[as_data_ptr[bus].SlaveTX_index];
			else
				data = 0xFF;
			switch ((as_data_ptr[bus].SlaveTX_index + 1)%4)
			{
				case 1:
					value = 0xFFFFFF00 | data;
					break;
				case 2:
					value &= 0xFFFF00FF;
					value |= (data << 8);
					break;
				case 3:
					value &= 0xFF00FFFF;
					value |= (data << 16);
					break;
				case 0:
					value &= 0x00FFFFFF;
					value |= (data << 24);
					iowrite32(value, (void * __iomem) as_data_ptr[bus].pool_buff_base + ((as_data_ptr[bus].SlaveTX_index/4) * 4));
			}
			if ((as_data_ptr[bus].SlaveTX_index + 1) == SLAVETX_MAX_RES_SIZE)
			{
				iowrite32(value, (void * __iomem) as_data_ptr[bus].pool_buff_base + ((as_data_ptr[bus].SlaveTX_index/4) * 4));
			}
		}
		i2c_as_write_reg(bus, SLAVE_TRANSMIT | ENABLE_MASTER_SLAVE_TX_DATA_BUF, I2C_CMD_STATUS_REG);
#else
		printk("I2C%d: Slave Tx Error\n", bus);
#endif
		as_data_ptr[bus].SlaveTX_RES_len = 0;
	}
}

static void 
i2cTXEMPTY_master_process(int bus, u32 status)
{
	//int i;//Quanta--
	
	if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
	{
	
		if ( as_data_ptr[bus].TX_index == (as_data_ptr[bus].TX_len))
		{
			if (as_data_ptr[bus].MasterRX_len == 0) // Single Write command, not repeat start condition */
			{
				/* Generate STOP on bus for write only command */
				i2c_as_write_reg( bus, MASTER_STOP, I2C_CMD_STATUS_REG ); 
			}
	
			/* Disable the TX interrupt */
			i2c_as_disable_interrupt(bus,ENABLE_TX_DONE_WITH_ACK_INTR);
			as_data_ptr[bus].op_status=status;
			as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
			wake_up( &as_data_ptr[bus].as_wait );
	
			return;
		}
	
		//for (i=0;i<as_data_ptr[bus].TX_len;i++)//Quanta--
		//{
		//	as_data_ptr[bus].dma_buff[i] = as_data_ptr[bus].TX_data[as_data_ptr[bus].TX_index];			
		//	as_data_ptr[bus].TX_index ++;
		//}
		memcpy(as_data_ptr[bus].dma_buff, &as_data_ptr[bus].TX_data[as_data_ptr[bus].TX_index], as_data_ptr[bus].TX_len);//Quanta
		as_data_ptr[bus].TX_index+=as_data_ptr[bus].TX_len;//Quanta

		i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
		i2c_as_write_reg(bus, as_data_ptr[bus].TX_len, I2C_DMA_MODE_STATUS_REG );
#else
		i2c_as_write_reg(bus, as_data_ptr[bus].TX_len-1, I2C_DMA_MODE_STATUS_REG );
#endif
		i2c_as_write_reg(bus, ENABLE_MASTER_SLAVE_TX_DMA_BUF|MASTER_TRANSMIT, I2C_CMD_STATUS_REG );
	}
	else
	{ 
		/* Default BYTE MODE */
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
			i2c_as_enable_interrupt(bus,STOP_DETECTED);

			wake_up( &as_data_ptr[bus].as_wait );
				
			return;
		}
		else if (as_data_ptr[bus].TX_index < as_data_ptr[bus].TX_len)
		{	
			
			i2c_as_write_reg(bus,as_data_ptr[bus].TX_data[as_data_ptr[bus].TX_index],I2C_DATA_REG);
			i2c_as_write_reg(bus, MASTER_TRANSMIT, I2C_CMD_STATUS_REG );
			
			as_data_ptr[bus].TX_index ++;
		}
	}
		return;
}

static int 
i2cRXFULL_master_process(int bus,u32 status)
{
    //int i;//Quanta--
	int datacount = 0;

	if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
    {
        if (as_data_ptr[bus].block_read == 1 || as_data_ptr[bus].block_proc_call == 1)   
        {
            // Extract the Data Length and Data of Block data
            if (as_data_ptr[bus].MasterRX_index == 0)
            {
				as_data_ptr[bus].MasterRX_data[0] = as_data_ptr[bus].dma_buff[0]; 
				as_data_ptr[bus].MasterRX_index ++;;
				as_data_ptr[bus].MasterRX_len = as_data_ptr[bus].MasterRX_data[0];
                if (as_data_ptr[bus].master_read_with_PEC)
                    as_data_ptr[bus].MasterRX_len += 1;

                i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );
                i2c_as_write_reg(bus, as_data_ptr[bus].MasterRX_len, I2C_DMA_MODE_STATUS_REG );
				i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
				
                return 0;
            }
			else
			{
				
				datacount = (as_data_ptr[bus].MasterRX_len-(i2c_as_read_reg(bus,I2C_DMA_MODE_STATUS_REG)));
				
				//for (i=0; i < datacount ;i++)//Quanta--
				//{
				//	as_data_ptr[bus].MasterRX_data[i+1] = as_data_ptr[bus].dma_buff[i]; 
				//}			
				memcpy(&as_data_ptr[bus].MasterRX_data[1], as_data_ptr[bus].dma_buff, datacount);//Quanta

				as_data_ptr[bus].MasterRX_index += datacount;
				as_data_ptr[bus].MasterRX_len++;

				if ( as_data_ptr[bus].MasterRX_index == ((as_data_ptr[bus].MasterRX_len)) )
				{
					as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
					i2c_as_enable_interrupt(bus,STOP_DETECTED); 	
					as_data_ptr[bus].op_status=status;
					wake_up( &as_data_ptr[bus].as_wait);
					return 1;
				}
								
				return 0;
			}
        }
        // Extract the data
        if (as_data_ptr[bus].MasterRX_index < as_data_ptr[bus].MasterRX_len)
        {
			datacount = (as_data_ptr[bus].MasterRX_len-(i2c_as_read_reg(bus,I2C_DMA_MODE_STATUS_REG)));
            //for (i=0; i < datacount ;i++)//Quanta--
            //{
            //    as_data_ptr[bus].MasterRX_data[i] = as_data_ptr[bus].dma_buff[i]; 
            //}
			memcpy(as_data_ptr[bus].MasterRX_data, as_data_ptr[bus].dma_buff, datacount);//Quanta

       		as_data_ptr[bus].MasterRX_index += datacount;

            if (as_data_ptr[bus].MasterRX_index < (as_data_ptr[bus].MasterRX_len -1))
            {
                i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );
                i2c_as_write_reg(bus, (as_data_ptr[bus].MasterRX_len) - (as_data_ptr[bus].MasterRX_index), I2C_DMA_MODE_STATUS_REG );
                i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF | MASTER_RECEIVE, I2C_CMD_STATUS_REG );
				return 1;
            }

            if ( as_data_ptr[bus].MasterRX_index == ((as_data_ptr[bus].MasterRX_len )) )
            {
            	as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
				i2c_as_enable_interrupt(bus,STOP_DETECTED);	
                as_data_ptr[bus].op_status=status;
                wake_up( &as_data_ptr[bus].as_wait);
                return 1;
            }
			
			return 0;
			
        }
    }
    else
    { 
    	/* Default BYTE MODE */
		if (as_data_ptr[bus].block_read == 1 ||as_data_ptr[bus].block_proc_call == 1)
		{
			
			if (as_data_ptr[bus].MasterRX_index == 0)
			{
				as_data_ptr[bus].MasterRX_data[as_data_ptr[bus].MasterRX_index]= i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
				as_data_ptr[bus].MasterRX_len = as_data_ptr[bus].MasterRX_data[as_data_ptr[bus].MasterRX_index] + 1;
                if (as_data_ptr[bus].master_read_with_PEC)
                    as_data_ptr[bus].MasterRX_len += 1;
                
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
				i2c_as_enable_interrupt(bus,STOP_DETECTED);			
				as_data_ptr[bus].op_status=status;
				wake_up( &as_data_ptr[bus].as_wait);
				return 1;
			}
		
		}
    }
	return 0;
}

static int 
i2cRXFULL_slave_process(int bus,u32 status)
{
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
    int i;
	int datacount = 0;

	if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
	{
		if (as_data_ptr[bus].SlaveTX_Enable)
		{
			if	(as_data_ptr[bus].SlaveTX_REQ_cmd != 0)
			{			
				u8 slave_addr_w;
			
				slave_addr_w = (i2c_as_read_reg( bus, I2C_SLAVE_ADDR_REG ) << 1);

				as_data_ptr[bus].Linear_SlaveRX_index = 2;
				as_data_ptr[bus].Linear_SlaveRX_data[0] = slave_addr_w;
				as_data_ptr[bus].Linear_SlaveRX_data[1] = as_data_ptr[bus].SlaveTX_REQ_cmd;
				//Start to prepare response data and clear RX_DONE interrupt status till data is ready afterwards
				as_data_ptr[bus].SlaveTX_READ_DATA = 1;
				i2c_as_disable_interrupt(bus, RX_DONE);
				i2c_as_disable_interrupt(bus, SLAVE_ADDR_MATCH);
				i2cSTOPDET_slave_process(bus);				
			}
			else
			{
				/* Calculate the real number of byte master transmit */
				datacount = (MAX_DMA_TRANSFER_LENGTH_SIZE-(i2c_as_read_reg(bus,I2C_DMA_MODE_STATUS_REG)) + 1);

				as_data_ptr[bus].Linear_SlaveRX_data[0] = (i2c_as_read_reg( bus, I2C_SLAVE_ADDR_REG ) << 1);
				for (i=1; i < datacount ;i++)
				{
					as_data_ptr[bus].Linear_SlaveRX_data[i] = as_data_ptr[bus].dma_buff[i-1];
				}
				as_data_ptr[bus].Linear_SlaveRX_index += datacount;
			}
			return 0;
		}
		/* Calculate the real number of byte master transmit */
		datacount = (MAX_DMA_TRANSFER_LENGTH_SIZE-(i2c_as_read_reg(bus,I2C_DMA_MODE_STATUS_REG)) + 1);

		as_data_ptr[bus].Linear_SlaveRX_data[0] = (i2c_as_read_reg( bus, I2C_SLAVE_ADDR_REG ) << 1);
		for (i=1; i < datacount ;i++)
		{
			as_data_ptr[bus].Linear_SlaveRX_data[i] = as_data_ptr[bus].dma_buff[i-1];
		}
		as_data_ptr[bus].Linear_SlaveRX_index += datacount;
	}
	else
#endif
	{
		/* Byte Mode */
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
				if (as_data_ptr[bus].SlaveTX_Enable)
				{
					u8 slave_addr_w;
					as_data_ptr[bus].Linear_SlaveRX_data[as_data_ptr[bus].Linear_SlaveRX_index] =i2c_as_read_reg(bus,I2C_DATA_REG) >> 8;
		
					if	(as_data_ptr[bus].Linear_SlaveRX_index == 0 &&
						(as_data_ptr[bus].Linear_SlaveRX_data[0] >> 1 == i2c_as_read_reg(bus, I2C_SLAVE_ADDR_REG))  &&
						(as_data_ptr[bus].Linear_SlaveRX_data[0] & 0x01))
					{
						if (as_data_ptr[bus].SlaveTX_REQ_cmd != 0)
						{
							slave_addr_w = as_data_ptr[bus].Linear_SlaveRX_data[0] & 0xFE;

							as_data_ptr[bus].Linear_SlaveRX_index = 2;
							as_data_ptr[bus].Linear_SlaveRX_data[0] = slave_addr_w;
							as_data_ptr[bus].Linear_SlaveRX_data[1] = as_data_ptr[bus].SlaveTX_REQ_cmd;
							//Start to prepare response data and clear RX_DONE interrupt status till data is ready afterwards
							as_data_ptr[bus].SlaveTX_READ_DATA = 1;
							i2c_as_disable_interrupt(bus, RX_DONE);
							i2c_as_disable_interrupt(bus, SLAVE_ADDR_MATCH);
							i2cSTOPDET_slave_process(bus);						
						}
						else
						{
							//got I2C probe request
							/*start to slave-send out I2C probe response*/
							as_data_ptr[bus].SlaveTX_RES_data[0]=0x80;
							as_data_ptr[bus].SlaveTX_RES_len = 1;
							i2cTXFULL_slave_process(bus);
						}
					}
					else if (as_data_ptr[bus].Linear_SlaveRX_index == 0 &&
							(as_data_ptr[bus].Linear_SlaveRX_data[0] >> 1 != i2c_as_read_reg(bus, I2C_SLAVE_ADDR_REG)) )
					{
						//In case of controller does not raise RX_DONE to receive slave address in register, but just raise SLAVE_ADDR_MATCH interrupt status.
						as_data_ptr[bus].Linear_SlaveRX_data[1] = as_data_ptr[bus].Linear_SlaveRX_data[0];
						as_data_ptr[bus].Linear_SlaveRX_data[0] = (i2c_as_read_reg( bus, I2C_SLAVE_ADDR_REG ) << 1);
						as_data_ptr[bus].Linear_SlaveRX_index = as_data_ptr[bus].Linear_SlaveRX_index + 2;
					}
					else
					{
						as_data_ptr[bus].Linear_SlaveRX_index++;
					}
					return 0;
				}
				as_data_ptr[bus].Linear_SlaveRX_data[as_data_ptr[bus].Linear_SlaveRX_index] =i2c_as_read_reg(bus,I2C_DATA_REG)>>8;
				as_data_ptr[bus].Linear_SlaveRX_index++;
			}
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

	if ((as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN) && (as_data_ptr[bus].MCTPRX_Entries == MAX_FIFO_LEN))
	{
		/* Return back */
		spin_unlock( &as_data_ptr[bus].data_lock );		
		return;
	}

	length = as_data_ptr[bus].Linear_SlaveRX_index;
	if (length==0)
	{
		/* Dummy STOP or Master STOP ?*/
		spin_unlock( &as_data_ptr[bus].data_lock );
		return;
	}

	if((length >= 8) && ((as_data_ptr[bus].Linear_SlaveRX_data[3] & 0x01) || (as_data_ptr[bus].Linear_SlaveRX_data[1] == 0x0F)) && (as_data_ptr[bus].SlaveTX_Enable == 0)) 
	{
        if(as_data_ptr[bus].MCTPRX_Entries == MAX_FIFO_LEN)
        {
            spin_unlock( &as_data_ptr[bus].data_lock );
            return;
        }

		FifoPtr = as_data_ptr[bus].MCTPRX_Writer;
//		as_data_ptr[bus].SlaveRX_index[FifoPtr] = 0;
		DataBuffer = as_data_ptr[bus].MCTPRX_data[FifoPtr];

		if(length)
			 memcpy(&DataBuffer[0],as_data_ptr[bus].Linear_SlaveRX_data,length);
		as_data_ptr[bus].MCTPRX_Len[FifoPtr]= length;
		as_data_ptr[bus].Linear_SlaveRX_index = 0;

		/* The following happens when emtpy packet (START, followed by a STOP) is detected*/
		if (as_data_ptr[bus].MCTPRX_Len[FifoPtr]  == 1)
		{
			as_data_ptr[bus].MCTPRX_Len[FifoPtr] = 0;
		}
		else
		{
			if ((++as_data_ptr[bus].MCTPRX_Writer) == MAX_FIFO_LEN)
				as_data_ptr[bus].MCTPRX_Writer = 0;
			as_data_ptr[bus].MCTPRX_Entries++;
			/* Check if buffer full. If so disable slave */
			if ((as_data_ptr[bus].MCTPRX_Entries == MAX_FIFO_LEN) && (as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN))
			{
				i2c_as_disable_slave(bus);
			}
		}
		
		spin_unlock( &as_data_ptr[bus].data_lock );
		wake_up_interruptible(&as_data_ptr[bus].as_mctp_data_wait);
		return;
	}
	else
	{
        if(as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN)
        {
            spin_unlock( &as_data_ptr[bus].data_lock );
            return;
        }

		FifoPtr = as_data_ptr[bus].SlaveRX_Writer;

		as_data_ptr[bus].SlaveRX_index[FifoPtr] = 0;

		DataBuffer = as_data_ptr[bus].SlaveRX_data[FifoPtr];
	
		/* Read the Length and oopy to buffer */
		if(length)
			memcpy(&DataBuffer[0],as_data_ptr[bus].Linear_SlaveRX_data,length);
		
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
			if ((as_data_ptr[bus].SlaveRX_Entries == MAX_FIFO_LEN) && (as_data_ptr[bus].MCTPRX_Entries == MAX_FIFO_LEN))
			{
				i2c_as_disable_slave(bus);
			//	printk("I2C%d: Disabling Slave as the Fifo is Full\n",bus);
			}
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
	unsigned long unhandled_status;	//Quanta
	unsigned long intEnb;
	unsigned long i2c_int;

	if (CONFIG_SPX_FEATURE_I2C_BUS_DISABLE_MASK & (1 << bus)) return;

	status2 = i2c_as_read_reg( bus, I2C_INTR_STATUS_REG );
	unhandled_status = status2;	//Quanta
	intEnb = i2c_as_read_reg( bus, I2C_INTR_CONTROL_REG );
	
	i2c_int = status2 & intEnb;

	/* NACK handling */
	if(i2c_int & TX_DONE_WITH_NACK)
	{
		//printk("\n !!!!! NACK on data Transmission!!!!!\n");
		if (as_data_ptr[bus].SlaveTX_Enable)
		{
			/* clear status bit */
			i2c_as_write_reg( bus, (/*status2 |*/TX_DONE_WITH_NACK),I2C_INTR_STATUS_REG );//Quanta--
			unhandled_status &= ~TX_DONE_WITH_NACK;//Quanta
		}
		else
		{
			/* Stop bus when NACK happened */
			i2c_as_write_reg( bus, MASTER_STOP, I2C_CMD_STATUS_REG );
			as_data_ptr[bus].op_status = i2c_int;
		
			/* clear status bit */
			i2c_as_write_reg( bus, (/*status2 |*/TX_DONE_WITH_NACK),I2C_INTR_STATUS_REG );//Quanta--
			unhandled_status &= ~TX_DONE_WITH_NACK;//Quanta
			as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;  // Quanta
			wake_up( &as_data_ptr[bus].as_wait );
		}
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
		i2c_as_write_reg( bus, (/*status2 |*/SMBUS_ARP_HOST_ADDR_MATCH),I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~SMBUS_ARP_HOST_ADDR_MATCH;//Quanta
	}

	if (i2c_int & SCL_LOW_TIMEOUT)
	{
		printk(" SMB TIMEOUT \n");
		/* clear status bit */
		i2c_as_write_reg( bus, (/*status2 |*/SCL_LOW_TIMEOUT),I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~SCL_LOW_TIMEOUT;//Quanta

		if(i2c_bus_recovery(bus) != 0)
		{
			printk("SMBus timeout. cannot recover %x bus \n",bus);
		}
		i2c_init_hardware(bus);
	}

	if (status2 & SLAVE_ADDR_MATCH)
	{
		if (as_data_ptr[bus].SlaveTX_Enable)
		{	
			slave_mode[bus] = 1;

			if (as_data_ptr[bus].Linear_SlaveRX_index == 2 &&	 
				(as_data_ptr[bus].Linear_SlaveRX_data[0] >> 1 == i2c_as_read_reg(bus, I2C_SLAVE_ADDR_REG)) &&
				((as_data_ptr[bus].Linear_SlaveRX_data[0] & 0x01) == 0x00) )
				as_data_ptr[bus].SlaveTX_REQ_cmd = as_data_ptr[bus].Linear_SlaveRX_data[1];
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
			else if (as_data_ptr[bus].SlaveTX_READ_DATA != 1)
#else
			else
#endif
			{
				as_data_ptr[bus].SlaveTX_REQ_cmd = 0;
			}
			  
			as_data_ptr[bus].SlaveTX_index = 0;
			as_data_ptr[bus].SlaveTX_count = 0;
			as_data_ptr[bus].Linear_SlaveRX_index = 0;

			if((i2c_int & RX_DONE) != RX_DONE)
			{
				i2c_as_write_reg( bus, (/*status2 |*/SLAVE_ADDR_MATCH),I2C_INTR_STATUS_REG );//Quanta--
				unhandled_status &= ~SLAVE_ADDR_MATCH;//Quanta
			}
		}
	}
	
	/* Slave_Master_Receive */
	if(i2c_int & RX_DONE)
	{
		//printk("\n mr\n");
		//printk("m %x r %x\n",as_data_ptr[bus].master_xmit_recv_mode_flag,as_data_ptr[bus].restart);

		if (as_data_ptr[bus].master_xmit_recv_mode_flag == SLAVE) 
		{
				if (status2 & SLAVE_ADDR_MATCH)
				{
					slave_mode[bus] = 1;
					as_data_ptr[bus].Linear_SlaveRX_index = 0;
					
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
					/* Set 1 byte length of DMA buffer for indicating receiving slave address byte */
					if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
						i2c_as_write_reg(bus, 1, I2C_DMA_MODE_STATUS_REG );
#endif
				}

#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
				if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
				{
					if (1 == i2c_as_read_reg(bus,I2C_DMA_MODE_STATUS_REG))
					{
						i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );
						/* Set MAX_DMA_TRANSFER_LENGTH_SIZE as default receiving length */
						i2c_as_write_reg(bus, MAX_DMA_TRANSFER_LENGTH_SIZE, I2C_DMA_MODE_STATUS_REG );
						i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF | MASTER_RECEIVE, I2C_CMD_STATUS_REG );

						if (as_data_ptr[bus].SlaveTX_Enable)
						{
							if (as_data_ptr[bus].SlaveTX_READ_DATA == 1)
							{
								i2cRXFULL_slave_process(bus,status2);
							}
							else if (as_data_ptr[bus].SlaveTX_REQ_cmd != 0)
							{
								as_data_ptr[bus].SlaveTX_READ_DATA = 1;
							}
						}
					}
					else
					{
						i2cRXFULL_slave_process(bus,status2);
						i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF | MASTER_STOP | MASTER_RECEIVE | MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
					}
				}
				else
#endif
				{ /* Default BYTE MODE */
					i2cRXFULL_slave_process(bus,status2);
				}
		}
		else if ((as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_RECV))
		{
			i2cRXFULL_master_process(bus,status2);
            /* if this is the last byte */
            if (as_data_ptr[bus].MasterRX_index == (as_data_ptr[bus].MasterRX_len)-1)
            {
                if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
                {
                    i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
                }
                else
                { /* Default BYTE MODE */
					if (as_data_ptr[bus].MasterRX_index == (as_data_ptr[bus].MasterRX_len)-1)
					{
						i2c_as_write_reg( bus, MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
					}
                }
			}
		}
		if ((status2 & TX_DONE_WITH_ACK) != TX_DONE_WITH_ACK)
		{
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)		
			if (( as_data_ptr[bus].SlaveTX_READ_DATA == 0 ) && (as_data_ptr[bus].SlaveTX_REQ_cmd == 0))
#else
			if ( as_data_ptr[bus].SlaveTX_READ_DATA == 0 )
#endif			
			{
				/* clear status bit */
				i2c_as_write_reg( bus, (/*status2 |*/RX_DONE),I2C_INTR_STATUS_REG );//Quanta--
				unhandled_status &= ~RX_DONE;//Quanta
			}
		}
	}

	
	/* Slave_Stop Received */ 
 	if (i2c_int & STOP_DETECTED)	
	{
		if(as_data_ptr[bus].master_xmit_recv_mode_flag == SLAVE && (slave_mode[bus] == 1 || as_data_ptr[bus].host_notify_flag ==1 )) {
				i2cSTOPDET_slave_process(bus);
		} else {
				//printk("ERROR: Spurious SLAVE STOP_RECEVIED interrupt\n");
		}

		slave_mode[bus] = 0;
		as_data_ptr[bus].host_notify_flag = 0;

		/* clear status bit */
		i2c_as_write_reg( bus, (/*status2 |*/STOP_DETECTED),I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~STOP_DETECTED;//Quanta
 	}

	/* Master_Transmit */
	if(i2c_int & TX_DONE_WITH_ACK)
	{
		if (as_data_ptr[bus].SlaveTX_Enable)
		{
			i2cTXFULL_slave_process(bus);
		}
		
		/* clear status bit */
		i2c_as_write_reg( bus, (/*status2 |*/TX_DONE_WITH_ACK), I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~TX_DONE_WITH_ACK;//Quanta

		if (as_data_ptr[bus].address_sent == 1) 
		{
			as_data_ptr[bus].address_sent = 0;
			if (as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_XMIT)
				i2cTXEMPTY_master_process(bus,status2);
			
			else if ((as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_RECV))
			{
			    /* SMBus Quick Command Read, Not reading data */
			    if ((as_data_ptr[bus].MasterRX_index == 0 &&  as_data_ptr[bus].MasterRX_len == 0)) {
			        /* Generate STOP on bus for write only command */
			        i2c_as_write_reg( bus, MASTER_STOP, I2C_CMD_STATUS_REG );  
			    }
				/* for receive only one byte */
			
				else if ((as_data_ptr[bus].MasterRX_index == (as_data_ptr[bus].MasterRX_len -1)))
				{
					if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
                    {
						i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );
						i2c_as_write_reg(bus, as_data_ptr[bus].MasterRX_len, I2C_DMA_MODE_STATUS_REG );
                        i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
                    }
                    else
                    { /* Default BYTE MODE */
						i2c_as_write_reg( bus, MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
                    }
				}
				/* for receive multiple bytes */
				else 
				{
					if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
					{

						i2c_as_write_reg(bus, as_data_ptr[bus].dma_addr, I2C_DMA_MODE_CONTROL_REG );

						if(as_data_ptr[bus].block_read == 1 || as_data_ptr[bus].block_proc_call == 1)
						{
							//SMB First byte receive for data length
							i2c_as_write_reg(bus, 0, I2C_DMA_MODE_STATUS_REG );
							i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_RECEIVE,I2C_CMD_STATUS_REG);
						}
						else
						{
							if(as_data_ptr[bus].MasterRX_len <= MAX_DMA_TRANSFER_LENGTH_SIZE){
							    i2c_as_write_reg(bus, as_data_ptr[bus].MasterRX_len, I2C_DMA_MODE_STATUS_REG );
							    i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_STOP|MASTER_RECEIVE| MASTER_SLAVE_RECEIVE_COMMAND_LAST, I2C_CMD_STATUS_REG );
							} else {
							    i2c_as_write_reg(bus, MAX_DMA_TRANSFER_LENGTH_SIZE, I2C_DMA_MODE_STATUS_REG );
							    i2c_as_write_reg( bus, ENABLE_MASTER_SLAVE_RX_DMA_BUF|MASTER_RECEIVE, I2C_CMD_STATUS_REG );
							}
						}
					}
					else
					{ /* Default BYTE MODE */
						i2c_as_write_reg( bus, MASTER_RECEIVE,I2C_CMD_STATUS_REG);
					}
				}
			}
		}
		else if(as_data_ptr[bus].address_sent == 2) 
		{
			if(as_data_ptr[bus].TX_index < as_data_ptr[bus].TX_len)
		   	{
				if (as_data_ptr[bus].master_xmit_recv_mode_flag == MASTER_XMIT)
					i2cTXEMPTY_master_process(bus,status2);
			}
			else
			{
				/* for MASTER_RECV, send start signal and target addr */
				as_data_ptr[bus].address_sent = 1;
				as_data_ptr[bus].master_xmit_recv_mode_flag = MASTER_RECV;
				i2c_as_enable_interrupt(bus,ENABLE_RX_DONE_INTR);
				i2c_as_write_reg(bus,((as_data_ptr[bus].target_address & 0x7f) << 1) | 0x01, I2C_DATA_REG);
				
				i2c_as_write_reg(bus, MASTER_START |MASTER_TRANSMIT, I2C_CMD_STATUS_REG );
			
				as_data_ptr[bus].TX_index ++;
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
		i2c_as_write_reg( bus, (/*status2 |*/ MASTER_ARBITRATION_LOST),I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~MASTER_ARBITRATION_LOST;//Quanta

		as_data_ptr[bus].op_status = status2;
		as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
		wake_up(&as_data_ptr[bus].as_wait);  //wake_up_interruptible( &as_data_ptr[bus].as_wait );  // Quanta
	}

	if(i2c_int & ABNORMAL_START_STOP_DETECTED)
	{
		printk("\n !!!!!! master-abnormal stop-start !!!!!!!!!\n");

		/* clear status bit */
		i2c_as_write_reg( bus, (/*status2 |*/ABNORMAL_START_STOP_DETECTED),I2C_INTR_STATUS_REG );//Quanta--
		unhandled_status &= ~ABNORMAL_START_STOP_DETECTED;//Quanta
		// Quanta +++
		as_data_ptr[bus].op_status = status2;
		as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;
		wake_up(&as_data_ptr[bus].as_wait);
		// +++ Quanta
	}
	i2c_as_write_reg( bus, (unhandled_status), I2C_INTR_STATUS_REG);//Quanta
}

#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
irqreturn_t 
i2c_handler( int this_irq, void *dev_id)
{
    int 		bus=0, bus_base_no=0, bus_amount=0;
	
	volatile unsigned long dev_intr_status_reg;

    switch (this_irq)
    {
        case IRQ_I2C:
            bus_amount = AST_BMC_BUS_COUNT;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_I2C_GLOBAL_VA_BASE);
            break;

#if defined(GROUP_AST1070_COMPANION)
        case IRQ_BMCCC0_I2C:
            bus_amount = CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            bus_base_no = AST_BMC_BUS_COUNT;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_BMCCC0_I2C_GLOBAL_VA_BASE);
            break;

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        case IRQ_BMCCC1_I2C:
            bus_amount = CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            bus_base_no = AST_BMC_BUS_COUNT+CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_BMCCC1_I2C_GLOBAL_VA_BASE);
            break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

        default:
    	    return IRQ_NONE;
    }

	/* Execute the interrupt for all i2c devices */
    for (bus = 0; bus < bus_amount; bus++) {
		if (dev_intr_status_reg & (1 << bus)) {
			as_handler(bus+bus_base_no);
		}
    }
	return IRQ_HANDLED;
}

#else
irqreturn_t 
i2c_handler( int this_irq, void *dev_id,
                                 struct pt_regs *regs )
{
    int 		bus=0, bus_base_no=0, bus_amount=0;
	
	volatile unsigned long dev_intr_status_reg;

    switch (this_irq)
    {
        case IRQ_I2C:
            bus_amount = AST_BMC_BUS_COUNT;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_I2C_GLOBAL_VA_BASE);
            break;

#if defined(GROUP_AST1070_COMPANION)
        case IRQ_BMCCC0_I2C:
            bus_amount = CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            bus_base_no = AST_BMC_BUS_COUNT;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_BMCCC0_I2C_GLOBAL_VA_BASE);
            break;

#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
        case IRQ_BMCCC1_I2C:
            bus_amount = CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            bus_base_no = AST_BMC_BUS_COUNT+CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES;
            /* Determine the bus for this interrupt */
            dev_intr_status_reg = *(unsigned long *) (AST_BMCCC1_I2C_GLOBAL_VA_BASE);
            break;
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif
        default:
    	    return IRQ_NONE;
    }

	/* Execute the interrupt for all i2c devices */
    for (bus = 0; bus < bus_amount; bus++) {
		if (dev_intr_status_reg & (1 << bus)) {
			as_handler(bus+bus_base_no);
		}
    }
	return IRQ_HANDLED;
}

#endif



