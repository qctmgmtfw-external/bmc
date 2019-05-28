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
#include "i2c-log.h"

#include <linux/i2c.h>
extern struct i2c_as_data as_data_ptr[BUS_COUNT];


static int 
i2c_as_wait_for_int( int bus , int ms_timeout)
{
	int status;
	if (wait_event_timeout(as_data_ptr[bus].as_wait,as_data_ptr[bus].op_status,(ms_timeout*HZ/1000)) == 0)
	{
   		as_data_ptr[bus].op_status = i2c_as_read_reg( bus, I2C_INTR_STATUS_REG );
		//printk("!!! time out !!!!\n");
	}
	status = as_data_ptr[bus].op_status;
	as_data_ptr[bus].op_status = 0;

	return status;

}

static int 
as_prepare_transmit_data( struct i2c_adapter *i2c_adap,
                          struct i2c_msg msg )
{
	int i;
	
	for (i=0; i<msg.len; i++)
			as_data_ptr[i2c_adap->nr].TX_data[i] = msg.buf[i];
	
		as_data_ptr[i2c_adap->nr].TX_len = msg.len;
		as_data_ptr[i2c_adap->nr].TX_index = 0;
		as_data_ptr[i2c_adap->nr].MasterRX_len = 0;
		as_data_ptr[i2c_adap->nr].op_status = 0;
	return 0;
}

static int
as_check_SignalPending( struct i2c_adapter *i2c_adap, struct i2c_msg msg, 
		                int flags, int status)
{
	const char *status_str[] = {"unknown","receive_bytes","send_bytes","send_receive"};
	char *i2c_status_str = (char*)status_str[0];
	int i;

	if(flags > 0 && flags <= 3){
		i2c_status_str = (char*)status_str[flags];
	}

	/* Check for signal pending */
	if( status == AS_SIGNAL_RECEIVED )
	{
		dev_info( &i2c_adap->dev, "%s: Signal pending\n", i2c_status_str);
		return( -ERESTARTSYS );
	}

	if ((status & TX_DONE_WITH_NACK) != 0)
	{
		//dev_info( &i2c_adap->dev, "send_bytes: Nack on data transmission\n" );
		return( NACKONWR );
	}

	if( (status & MASTER_ARBITRATION_LOST) != 0 )
	{
		//dev_info( &i2c_adap->dev, "%s: Master Arbitration Lost\n", i2c_status_str );
		return( ARBLOST );
	}
	
	if( (status & SCL_LOW_TIMEOUT) != 0 )
	{
        dev_info( &i2c_adap->dev, "%s: SCL_LOW_TIMEOUT\n", i2c_status_str );
		i2c_as_reset(i2c_adap->nr);
		return( BUSERR );
	}
	/* Check for timeout */
	if( status == 0 )
	{
		dev_err( &i2c_adap->dev, "%s: Timed out on i2c data Operating\n", i2c_status_str);
		return( BUSERR );
	}

	if((flags & I2C_RECEIVE) != 0)
	{
		if ((as_data_ptr[i2c_adap->nr].block_read == 1) && (as_data_ptr[i2c_adap->nr].MasterRX_len > I2C_SMBUS_BLOCK_MAX + 2)) 
		{
			dev_err(&i2c_adap->dev, "smbus_access called with "
			"invalid block read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len - 1);
			return -1;
		}

		if ((as_data_ptr[i2c_adap->nr].block_proc_call == 1) && as_data_ptr[i2c_adap->nr].MasterRX_len < 1)
		{
			dev_err(&i2c_adap->dev, "smbus_access called with "
			"invalid block proc call read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len );
			return -1;
		}

		if ((as_data_ptr[i2c_adap->nr].block_proc_call == 1) && 
			(as_data_ptr[i2c_adap->nr].MasterRX_len + as_data_ptr[i2c_adap->nr].TX_len > I2C_SMBUS_BLOCK_MAX))
		{
			dev_err(&i2c_adap->dev, "smbus_access called with "
			"invalid block proc call write plus read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len + as_data_ptr[i2c_adap->nr].TX_len);
			
			return -1;
		}
		
		for ( i = 0; i<as_data_ptr[i2c_adap->nr].MasterRX_len; i++)
		{
			
			msg.buf[i] = as_data_ptr[i2c_adap->nr].MasterRX_data[i];
			//printk("rev = %x\n",msg.buf[i]);
		}
	}

   return( 0 );
}
static int 
as_send_bytes( struct i2c_adapter *i2c_adap,
                          struct i2c_msg msg, int messages_left )
{
   	int status;
   
   	i2c_as_enable_interrupt(i2c_adap->nr,ENABLE_TX_DONE_WITH_ACK_INTR);
#if !defined(GROUP_AST2500)
	/* disable slave stop detect interrupt */
 	i2c_as_disable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif
	
     /* Wait for interrupt */
 	status = i2c_as_wait_for_int( i2c_adap->nr,1000);

	as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = SLAVE;
	as_data_ptr[i2c_adap->nr].expected_mode_flag = SLAVE; // Quanta

  	i2c_as_disable_interrupt(i2c_adap->nr,ENABLE_TX_DONE_WITH_ACK_INTR);
#if !defined(GROUP_AST2500)	
 	i2c_as_enable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif	

	/* Check for signal pending */
  	if( status == AS_SIGNAL_RECEIVED )
   	{
     	//dev_info( &i2c_adap->dev, "send_bytes: Signal pending\n" );
       	return( -ERESTARTSYS );
   	}

	if ((status & TX_DONE_WITH_NACK) != 0)
	{
		//dev_info( &i2c_adap->dev, "send_bytes: Nack on data transmission\n" );
       	return( NACKONWR );
	}

	if( (status & MASTER_ARBITRATION_LOST) != 0 )
   	{
     	//dev_info( &i2c_adap->dev, "send_bytes: Master Arbitration Lost on sending data\n" );
       	return( ARBLOST );
   	}
	
   	/* Check for timeout */
   	if( status == 0 )
   	{
        dev_err( &i2c_adap->dev, "send_bytes: Timed out sending data\n" );
	  //dev_err( &i2c_adap->dev, "send_bytes: Got status: 0x%08x\n", status );
       	return( BUSERR );
   	}

   	return( 0 );
}

static int 
as_prepare_receive_data( struct i2c_adapter *i2c_adap,
                             struct i2c_msg msg )
{
	as_data_ptr[i2c_adap->nr].MasterRX_index = 0;
	as_data_ptr[i2c_adap->nr].MasterRX_len = msg.len;

	as_data_ptr[i2c_adap->nr].TX_len = 0;

	as_data_ptr[i2c_adap->nr].op_status = 0;

	return 0;
}

static int 
as_receive_bytes( struct i2c_adapter *i2c_adap,
                             struct i2c_msg msg, int messages_left )
{
   	int status;
   	int i;
	
	i2c_as_enable_interrupt(i2c_adap->nr,ENABLE_RX_DONE_INTR);
#if !defined(GROUP_AST2500)	
	/* disable slave stop detect interrupt */
	i2c_as_disable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif

    /* Wait for interrupt */
	
	status = i2c_as_wait_for_int( i2c_adap->nr,1000);

	as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = SLAVE;
	as_data_ptr[i2c_adap->nr].expected_mode_flag = SLAVE; // Quanta
#if !defined(GROUP_AST2500)	
 	i2c_as_enable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif

    /* Check for signal pending */
    if( status == AS_SIGNAL_RECEIVED )
    {
      	//dev_info( &i2c_adap->dev, "as_receive_bytes: Signal pending\n" );
       	return( -ERESTARTSYS );
    }

	if ((status & TX_DONE_WITH_NACK) != 0)
	{
		//dev_info( &i2c_adap->dev, "send_bytes: Nack on data transmission\n" );
       	return( NACKONWR );
	}

	if( (status & MASTER_ARBITRATION_LOST)!= 0 )
   	{
     	//dev_info( &i2c_adap->dev, "Master Arbitration Lost on receiving data\n" );
       	return( ARBLOST );
   	}
    if( status == 0 )
   	{
       	dev_err( &i2c_adap->dev, "as_receive_bytes: Timed out receiving data\n" );
       	dev_err( &i2c_adap->dev, "as_receive_bytes: Got status: 0x%08x\n", status );
       	return( BUSERR );
   	}
	
	if ((as_data_ptr[i2c_adap->nr].block_read == 1) && (as_data_ptr[i2c_adap->nr].MasterRX_len > I2C_SMBUS_BLOCK_MAX + 2)) 
	{
		dev_err(&i2c_adap->dev, "smbus_access called with "
		"invalid block read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len - 1);
		return -1;
	}

	if ((as_data_ptr[i2c_adap->nr].block_proc_call == 1) && as_data_ptr[i2c_adap->nr].MasterRX_len < 1)
	{
		dev_err(&i2c_adap->dev, "smbus_access called with "
		"invalid block proc call read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len );
		return -1;
	}

	if ((as_data_ptr[i2c_adap->nr].block_proc_call == 1) && 
		(as_data_ptr[i2c_adap->nr].MasterRX_len + as_data_ptr[i2c_adap->nr].TX_len > I2C_SMBUS_BLOCK_MAX))
	{
		dev_err(&i2c_adap->dev, "smbus_access called with "
		"invalid block proc call write plus read size (%d)\n",as_data_ptr[i2c_adap->nr].MasterRX_len + as_data_ptr[i2c_adap->nr].TX_len);
		
		return -1;
	}
	
	for ( i = 0; i<as_data_ptr[i2c_adap->nr].MasterRX_len; i++)
	{
		
		msg.buf[i] = as_data_ptr[i2c_adap->nr].MasterRX_data[i];
		//printk("rev = %x\n",msg.buf[i]);
	}
	
    return( 0 );
}


static int 
send_slave_addr( struct i2c_adapter *i2c_adap, int addr, int flags )
{

	/* Check for 10-bit slave addresses (unsupported) */
	if( flags & I2C_M_TEN )
		return( -ENOTTY );

	i2c_as_enable_interrupt(i2c_adap->nr,ENABLE_TX_DONE_WITH_ACK_INTR);
	/* Set target slave address and direction bit */
	if( flags & I2C_M_RD )
	{
		//as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = MASTER_RECV; // Quanta
		as_data_ptr[i2c_adap->nr].expected_mode_flag = MASTER_RECV; // Quanta change mode after first TX_DONE_WITH_ACK
		as_data_ptr[i2c_adap->nr].address_sent = 1;
		i2c_as_write_reg( i2c_adap->nr, ( addr << 1 ) | 0x01, I2C_DATA_REG );
	}
	else
	{
		//as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = MASTER_XMIT; // Quanta
		as_data_ptr[i2c_adap->nr].expected_mode_flag = MASTER_XMIT; // Quanta change mode after first TX_DONE_WITH_ACK
		as_data_ptr[i2c_adap->nr].address_sent = 1;
		i2c_as_write_reg( i2c_adap->nr, ( addr << 1 ), I2C_DATA_REG );
	}
	/* Create Start and transmit target address */
    i2c_as_write_reg( i2c_adap->nr, MASTER_START | MASTER_TRANSMIT, I2C_CMD_STATUS_REG );
	
	return 0;
}


int as_master_xfer(struct i2c_adapter *i2c_adap,
                    struct i2c_msg *msgs, int num )
{
  	int i = 0;
  	int retval = 0;
   	int status;

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
	if (as_data_ptr[i2c_adap->nr].SSIF_enable)
	{
		printk("ERROR: I2C%d: bus for SSIF, disallow master transfer\n", i2c_adap->nr); 
		return( BUSERR ); 
	}
#endif

/* Initialize Index and length */
	as_data_ptr[i2c_adap->nr].TX_len = 0;
	as_data_ptr[i2c_adap->nr].TX_index = 0;
	
	as_data_ptr[i2c_adap->nr].MasterRX_len = 0;
	as_data_ptr[i2c_adap->nr].MasterRX_index = 0;

	/* Check if bus busy */
	if(i2c_wait_for_bus_free(i2c_adap->nr) == 1)
	{
		
#ifdef JM_RECOVERY
//		printk("I2C%d: Bus Busy! bus_busy_cnt=%d MX_ALLOWED=%d\n", i2c_adap->nr,  
//			as_data_ptr[i2c_adap->nr].bus_busy_cnt, MAX_ALLOWED_BUS_BUSY_FAILURES);

		/* track number of times bus is consecutively busy */
		as_data_ptr[i2c_adap->nr].bus_busy_cnt++;

		/* if access to this bus has failed multiple times due to bus busy */
		if (as_data_ptr[i2c_adap->nr].bus_busy_cnt > MAX_ALLOWED_BUS_BUSY_FAILURES)
		{
			/* reset failure tracking var */
			as_data_ptr[i2c_adap->nr].bus_busy_cnt = 0;

			/* is bus hung ? */
			if (i2c_is_bus_hung(i2c_adap->nr))
			{
				/* if so, perform bus recovery */
				status = i2c_bus_recovery(i2c_adap->nr);
				if (0 != status)
					return status;
			}
		}

			return -EIO;
#else
		/* Bus busy. Perform a recovery */
		if(i2c_bus_recovery(i2c_adap->nr) != 0)
		{
			printk("I2C%d: Master-Xfer failed. Bus busy\n",i2c_adap->nr);
			return( BUSERR );
		}
#endif
	}

	/* Loop across all the messages */
	/* Send the destination slave address onto the bus */
		
	if(num == 1)
	{
		
		for (i=0;i<num;i++)
		{
			if (msgs[i].flags & I2C_M_RD)
				as_prepare_receive_data( i2c_adap, msgs[i]);
			else	
				as_prepare_transmit_data( i2c_adap, msgs[i]);
			
			retval = send_slave_addr(i2c_adap, msgs[i].addr, msgs[i].flags);
#ifdef JM_RECOVERY
			as_data_ptr[i2c_adap->nr].i2c_message_slave_address = msgs[i].addr;
#endif
			
			if(retval != 0)
			{
				log_i2c_plain(i2c_adap, (char *)&msgs[i].addr, 1);
				return -EIO;
			}
			if (msgs[i].flags & I2C_M_RD)
				retval = as_receive_bytes(i2c_adap, msgs[i], num-(i+1));
			else
				retval = as_send_bytes(i2c_adap, msgs[i],num-(i+1));
				
			if(retval != 0)
			{
				if(msgs[i].flags & I2C_M_RD)
				{
					log_i2c_err(i2c_adap,"Master-Read failed %x\n",retval);
				}
				else
				{
					log_i2c_err(i2c_adap,"Master-Write failed %x\n",retval);
				}
			}
			if (msgs[i].flags & I2C_M_RD)
				log_i2c(i2c_adap, &msgs[i], LOG_I2C_MR);
			else
				log_i2c(i2c_adap, &msgs[i], LOG_I2C_MW);

#ifdef JM_RECOVERY
			if (0 != retval)
			{
				/* track number of failures for this slave address */
				as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address]++;
//				printk("ERROR: I2C%d: dev_fail_cnt[0x%02x]=%d\n", i2c_adap->nr, msgs[i].addr, 
//						as_data_ptr[i2c_adap->nr].dev_fail_cnt[msgs[i].addr]);
				/* if access to this device has failed multiple times */
				if (as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] > MAX_ALLOWED_DEVICE_FAILURES)
				{
					/* reset failure tracking var */
					as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] = 0;

					/* is bus hung ? */
					if (i2c_is_bus_hung(i2c_adap->nr))
					{
						/* if so, perform bus recovery */
						status = i2c_bus_recovery(i2c_adap->nr);
						if (0 != status)
							return status;
					}
				}
				return retval; /* return error code */
			}
#endif
		}
#ifdef JM_RECOVERY
		/* successfully accessed device so reset failure tracking var */
		as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] = 0;
		return i; /* return number of messages sent */
#else
		return(retval == 0?i:retval);
#endif
	}


	/* ????????????????? */
	if (num != 2)
	{
		as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = SLAVE;
		return (-EINVAL);
	}

	/* ---------------Num == 2  Repeated Start Condition --------------------------*/
	
	/* Check for 10-bit slave addresses (unsupported) */
	if( msgs[0].flags & I2C_M_TEN )
	{
		printk("I2C%d: Rep-Master-Xfer failed. Unable to Send Slave Addr\n",i2c_adap->nr);
		return -EIO;
	}

	as_data_ptr[i2c_adap->nr].address_sent = 2;    
	as_data_ptr[i2c_adap->nr].target_address = msgs[1].addr;
   
	for (i=0; i<msgs[0].len; i++)
	{
		as_data_ptr[i2c_adap->nr].TX_data[i] = msgs[0].buf[i];
	}

	/* set TX buffer len and index to 0 when as restart condition */
	as_data_ptr[i2c_adap->nr].TX_len = msgs[0].len;
	as_data_ptr[i2c_adap->nr].TX_index = 0;

	/* set RX buffer len as requested bytes lengh */
#if defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
	as_data_ptr[i2c_adap->nr].MasterRX_len = (msgs[1].len + 1);
#else
	as_data_ptr[i2c_adap->nr].MasterRX_len = msgs[1].len;
#endif
	as_data_ptr[i2c_adap->nr].MasterRX_index = 0;

	as_data_ptr[i2c_adap->nr].op_status = 0;
	//as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = MASTER_XMIT; // Quanta
	as_data_ptr[i2c_adap->nr].expected_mode_flag = MASTER_XMIT; // Quanta change mode after first TX_DONE_WITH_ACK
	
	i2c_as_enable_interrupt(i2c_adap->nr,ENABLE_TX_DONE_WITH_ACK_INTR);
#if !defined(GROUP_AST2500)		
	/* disable slave stop detect interrupt */
	i2c_as_disable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif
	
	i2c_as_write_reg( i2c_adap->nr, ( msgs[1].addr << 1 ), I2C_DATA_REG );
	/* Create Start and transmit target address */
	i2c_as_write_reg( i2c_adap->nr, MASTER_START | MASTER_TRANSMIT, I2C_CMD_STATUS_REG );

	status = i2c_as_wait_for_int( i2c_adap->nr,1000);

	as_data_ptr[i2c_adap->nr].master_xmit_recv_mode_flag = SLAVE;
	as_data_ptr[i2c_adap->nr].expected_mode_flag = SLAVE; // Quanta
#if !defined(GROUP_AST2500)	
	i2c_as_enable_interrupt(i2c_adap->nr,STOP_DETECTED);
#endif	

	retval =  as_check_SignalPending(i2c_adap, msgs[1],I2c_TRANSMIT | I2C_RECEIVE, status);

#ifdef JM_RECOVERY
	as_data_ptr[i2c_adap->nr].i2c_message_slave_address = msgs[1].addr;
#endif
#ifdef JM_RECOVERY
	if (0 != retval)
	{
		/* track number of failures for this slave address */
		as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address]++;
//		printk("ERROR: I2C%d: dev_fail_cnt[0x%02x]=%d\n", i2c_adap->nr, msgs[i].addr, 
//			as_data_ptr[i2c_adap->nr].dev_fail_cnt[msgs[i].addr]);
		/* if access to this device has failed multiple times */
		if (as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] > MAX_ALLOWED_DEVICE_FAILURES)
		{
			/* reset failure tracking var */
			as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] = 0;

//			/* is bus hung ? */
//			if (i2c_is_bus_hung(i2c_adap->nr))
//			{
				/* if so, perform bus recovery */
				status = i2c_bus_recovery(i2c_adap->nr);
				if (0 != status)
					return retval;
//			}
		}
				return retval; /* return error code */
	}
#endif

	for (i = 0; i < num; ++i) {
		if (msgs[i].flags & I2C_M_RD)
		{
			log_i2c(i2c_adap, &msgs[i], LOG_I2C_MR);
		}	
		else
		{
			log_i2c(i2c_adap, &msgs[i], LOG_I2C_MW);
		}
	}
#ifdef JM_RECOVERY
		/* successfully accessed device so reset failure tracking var */
		as_data_ptr[i2c_adap->nr].dev_fail_cnt[as_data_ptr[i2c_adap->nr].i2c_message_slave_address] = 0;
		return num; /* return number of messages sent */
#else
	return(retval == 0? num:retval);
#endif
}	

int as_smb_xfer( struct i2c_adapter *i2c_adap, u16 addr, unsigned short flags,
							char read_write, u8 command, int size, union i2c_smbus_data * data)
                    
{
	


	unsigned char msgbuf0[I2C_SMBUS_BLOCK_MAX+3]; //msg0
	unsigned char msgbuf1[I2C_SMBUS_BLOCK_MAX+2]; //msg1
	int num = read_write == I2C_SMBUS_READ?2:1;
	struct i2c_msg msg[2] = { { addr, flags, 1, msgbuf0 }, 
	                          { addr, flags | I2C_M_RD, 0, msgbuf1 }
	                        };
	int i;

	int retval = 0;
	int ret = 0;
	
	u8 partial_pec = 0;

	
	msgbuf0[0] = command;
	switch(size) {
	case I2C_SMBUS_QUICK:
		msg[0].len = 0;
		/* Special case: The read/write field is used as data */
		msg[0].flags = flags | (read_write==I2C_SMBUS_READ)?I2C_M_RD:0;
		msg[0].addr = addr;
		num = 1;
		as_data_ptr[i2c_adap->nr].block_read = 0;
		as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		break;
	case I2C_SMBUS_BYTE:
		if (read_write == I2C_SMBUS_READ) {
			/* Special case: only a read! */
			msg[0].flags = I2C_M_RD | flags;
			msg[0].addr = addr;
			msg[0].len = 1;
			num = 1;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		else
		{
			msg[0].flags = 0 | flags;
			msg[0].addr = addr;
			num = 1;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		break;
	case I2C_SMBUS_BYTE_DATA:
		if (read_write == I2C_SMBUS_READ)
		{
			msg[0].len = 1;
			msg[0].addr = addr;
			msg[0].flags = 0 | flags;
			
			msg[1].len = 1;
			msg[1].addr = addr;
			msg[1].flags = I2C_M_RD | flags;

			num = 2;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
			
		else {
			msg[0].addr = addr;
			msg[0].flags = 0 | flags;
			msg[0].len = 2;
			msgbuf0[1] = data->byte;
			num = 1;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		break;
	case I2C_SMBUS_WORD_DATA:
		if (read_write == I2C_SMBUS_READ)
		{
			msg[0].len=1;
			msg[0].addr = addr;
			msg[0].flags = 0 | flags;
			
			msg[1].len = 2;
			msg[1].addr = addr;
			msg[1].flags = I2C_M_RD | flags;
			num = 2;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		else {
			msg[0].len=3;
			msg[0].addr = addr;
			msg[0].flags = 0 | flags;
			msgbuf0[1] = data->word & 0xff;
			msgbuf0[2] = (data->word >> 8) & 0xff;
			num = 1;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		break;
	case I2C_SMBUS_PROC_CALL:
		num = 2; /* Special case */
		read_write = I2C_SMBUS_READ;
		msg[0].len = 3;
		msg[0].addr = addr;
		msg[0].flags = 0 | flags;
		msgbuf0[1] = data->word & 0xff;
		msgbuf0[2] = (data->word >> 8) & 0xff;
		msg[1].len = 2;
		as_data_ptr[i2c_adap->nr].block_read = 0;
		as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		break;
	case I2C_SMBUS_BLOCK_DATA:
		if (read_write == I2C_SMBUS_READ) {

			
			msg[0].len = 1;
			msg[0].addr = addr;
			msg[0].flags = 0 | flags;
			
			msg[1].len = 32; // real len should be send by device in the first received byte, set to default value 32.
			msg[1].addr = addr;
			msg[1].flags = I2C_M_RD | flags;
			num = 2;
			as_data_ptr[i2c_adap->nr].block_read = 1;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
			
		}
		else {
			msg[0].len = data->block[0] + 2;
			if (msg[0].len > I2C_SMBUS_BLOCK_MAX + 2) {
				dev_err(&i2c_adap->dev, "smbus_access called with "
				       "invalid block write size (%d)\n",
				       data->block[0]);
				return -1;
			}
			for (i = 1; i < msg[0].len; i++)
				msgbuf0[i] = data->block[i-1];
			num = 1;
			as_data_ptr[i2c_adap->nr].block_read = 0;
			as_data_ptr[i2c_adap->nr].block_proc_call= 0;
		}
		break;
	case I2C_SMBUS_BLOCK_PROC_CALL:
		num = 2; /* Special case */
		read_write = I2C_SMBUS_READ;
		
		msg[0].len = data->block[0] + 2;
		msg[0].addr = addr;
		msg[0].flags = 0 | flags;
		
		

		if (msg[0].len < 1) {
			dev_err(&i2c_adap->dev, "smbus_access called with "
				       "invalid block write size (%d)\n",
				       data->block[0]);
			return -1;
		}
		for (i = 1; i < msg[0].len; i++)
			msgbuf0[i] = data->block[i-1];

		msg[1].len = 32; // real len should be send by device in the first received byte, set to default value 32
		msg[1].addr = addr;
		msg[1].flags = I2C_M_RD | flags;
		as_data_ptr[i2c_adap->nr].block_read = 0;
		as_data_ptr[i2c_adap->nr].block_proc_call= 1;
		break;
	case I2C_SMBUS_I2C_BLOCK_DATA:
		#if 0
		if (read_write == I2C_SMBUS_READ) {
			msg[1].len = I2C_SMBUS_BLOCK_MAX;
			msg[1].addr = addr;
			msg[1].flags = I2C_M_RD | flags;
			
		} else {
			msg[0].flags = 0 | flags;
			msg[0].addr = addr;
			msg[0].len = data->block[0] + 1;
			if (msg[0].len > I2C_SMBUS_BLOCK_MAX + 1) {
				dev_err(&i2c_adap->dev, "i2c_smbus_xfer_emulated called with "
				       "invalid block write size (%d)\n",
				       data->block[0]);
				return -1;
			}
			for (i = 1; i <= data->block[0]; i++)
				msgbuf0[i] = data->block[i];
		}
		#endif
		break;
	default:
		dev_err(&i2c_adap->dev, "smbus_access called with invalid size (%d)\n",
		       size);
		return -1;
	}

	
	
	i = ((flags & I2C_CLIENT_PEC) && size != I2C_SMBUS_QUICK
						  && size != I2C_SMBUS_I2C_BLOCK_DATA);	

	
	if (i) {
		/* Compute PEC if first message is a write */
		if (!(msg[0].flags & I2C_M_RD)) {
			if (num == 1) /* Write only */
				i2c_smbus_add_pec(&msg[0]);
			else /* Write followed by read */
					partial_pec = i2c_smbus_msg_pec(0, &msg[0]);
		}
			/* Ask for PEC if last message is a read */
		if (msg[num-1].flags & I2C_M_RD)
			msg[num-1].len++;
	}

	
	
	if (size == 0 )
	{
		retval = send_slave_addr(i2c_adap, msg[0].addr, msg[0].flags);
		if(retval != 0)
			return -EIO;
				
	}

	else
	{
		if ((retval = as_master_xfer(i2c_adap, msg, num)) < 0)
		{
			/* these are flags used by smbus, clear them after Smbus transfer finish*/
			as_data_ptr[i2c_adap->nr].block_read = 0; 
			as_data_ptr[i2c_adap->nr].block_proc_call = 0;
			return -1;
		}

		/* these are flags used by smbus, clear them after Smbus transfer finish */
		as_data_ptr[i2c_adap->nr].block_read = 0; 
		as_data_ptr[i2c_adap->nr].block_proc_call = 0;
		
	}

	
	
	
	/* Check PEC if last message is a read */
	if (i && (msg[num-1].flags & I2C_M_RD)) {
		//printk(" msg[num-1] data1 = %x data2 = %x\n",msgbuf0[0], msgbuf0[1]);
		if (i2c_smbus_check_pec(partial_pec, &msg[num-1]) < 0)
			return -1;
	}

	
	

	if (read_write == I2C_SMBUS_READ)
		switch(size) {
			case I2C_SMBUS_BYTE:
				data->byte = msgbuf0[0];
				ret = 1;
				break;
			case I2C_SMBUS_BYTE_DATA:
				data->byte = msgbuf1[0];
				ret = 1;
				break;
			case I2C_SMBUS_WORD_DATA: 
				data->word = msgbuf1[1]<<8 | msgbuf1[0];
				ret = 2;
				break;
			case I2C_SMBUS_PROC_CALL:
				data->word = msgbuf1[1]<< 8 | msgbuf1[0] ;
				ret = 2;
				break;
			case I2C_SMBUS_BLOCK_DATA:

				
				for (i = 0; i < as_data_ptr[i2c_adap->nr].MasterRX_len; i++)
				{
					data->block[i] = msgbuf1[i];
					
				}
				ret = (data->block[0]==(I2C_SMBUS_BLOCK_MAX+2))?0:as_data_ptr[i2c_adap->nr].MasterRX_len;
				break;
			case I2C_SMBUS_BLOCK_PROC_CALL:
				for (i = 0; i < as_data_ptr[i2c_adap->nr].MasterRX_len; i++)
				{
					data->block[i] = msgbuf1[i];
					
				}
				ret = (data->block[0]==(I2C_SMBUS_BLOCK_MAX+2))?0:as_data_ptr[i2c_adap->nr].MasterRX_len;
				break;
			case I2C_SMBUS_I2C_BLOCK_DATA:
				#if 0
				/* fixed at 32 for now */
				data->block[0] = I2C_SMBUS_BLOCK_MAX;
				for (i = 0; i < I2C_SMBUS_BLOCK_MAX; i++)
					data->block[i+1] = msgbuf1[i];
				#endif
				ret =I2C_SMBUS_BLOCK_MAX;
				break;
		}

	
	return ret;
}
int as_slave_recv( struct i2c_adapter *i2c_adap,
                    char *buf, int num )
{
	int len;
	unsigned long flags;
	int FifoPtr;
	//int i;
	
#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
 u8 retry = 20;
 u32 ctrl_bits;
 int ret;
retry:
 if (as_data_ptr[i2c_adap->nr].SSIF_enable)
	{
					if (i2c_wait_for_bus_free(i2c_adap->nr) != 0)
					{
				    //Recover the bus for SSIF
				    retry--;
				    if (retry) goto retry;
				    else
				    {
				    	   //Chipset error, Can not recover bus. Disable bus function then enable it
				    	   ctrl_bits = i2c_as_read_reg(i2c_adap->nr,I2C_FUNCTION_CONTROL_REG);
				    	   i2c_as_write_reg( i2c_adap->nr,ctrl_bits & (~ENABLE_SLAVE_FUNC) & (~ENABLE_MASTER_FUNC),I2C_FUNCTION_CONTROL_REG);
				    	   i2c_as_write_reg( i2c_adap->nr,ctrl_bits | ENABLE_SLAVE_FUNC,I2C_FUNCTION_CONTROL_REG );
				    	   if (i2c_wait_for_bus_free(i2c_adap->nr) != 0)
				    	   {
				    	   	   //printk("as_slave_recv Recover bus fail\n");
				    	   }
				    }
					}
	}
#else
retry:
#endif
	len = -1;
	/* Copy to user space buffer */
   	spin_lock_irqsave( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
	if (as_data_ptr[i2c_adap->nr].SlaveRX_Entries)
	{
		FifoPtr = as_data_ptr[i2c_adap->nr].SlaveRX_Reader;
		len = as_data_ptr[i2c_adap->nr].SlaveRX_len[FifoPtr];
		if (len > 0)
		{	
			log_i2c_plain(i2c_adap, buf, len);
			memcpy(buf,as_data_ptr[i2c_adap->nr].SlaveRX_data[FifoPtr],len);
#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
			if (as_data_ptr[i2c_adap->nr].SSIF_enable && len > 1)
			{
			    if (buf[1] == IPMI_SSIF_SINGLE_PART_WRITE_SSIF_CMD || buf[1] == IPMI_SSIF_MULTI_PART_WRITE_START_SSIF_CMD)
			    {
				    //got new SSIF req, clear last response buffer
				    as_data_ptr[i2c_adap->nr].TX_len = 0;
				    as_data_ptr[i2c_adap->nr].TX_index = 0;
			    }
			}
#endif
			#if 0
			
			for ( i= 0; i < len; i++)
			{
				printk("d[%x]  %x\n",i,(unsigned)buf[i]);
				//printk("as_slave_xfer.............. \n");
			}
			#endif
			as_data_ptr[i2c_adap->nr].SlaveRX_len[FifoPtr] = 0;
		}
		else
		{
			printk("WARNING: I2C%d: Empty Slave Data !!!\n",i2c_adap->nr);
			len  = -1;
		}
		if ((++as_data_ptr[i2c_adap->nr].SlaveRX_Reader) == MAX_FIFO_LEN)
			as_data_ptr[i2c_adap->nr].SlaveRX_Reader = 0;
		as_data_ptr[i2c_adap->nr].SlaveRX_Entries--;

	   	spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
		/* Enable slave as we read data out of buffer */
		i2c_as_enable_slave(i2c_adap->nr);
	}
	else
	{
  	 	spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
	if (as_data_ptr[i2c_adap->nr].SSIF_enable)
	{
  	 	ret = wait_event_interruptible_timeout(as_data_ptr[i2c_adap->nr].as_slave_data_wait,
     	                                   as_data_ptr[i2c_adap->nr].SlaveRX_Entries,
     	                                   HZ);
			if ( ret < 0)
			    return -1;	/* Signal received */	
			else if (ret == 0)
			    return -2; /*timeout */
	} else
#endif
		if (wait_event_interruptible(as_data_ptr[i2c_adap->nr].as_slave_data_wait,
					as_data_ptr[i2c_adap->nr].SlaveRX_Entries) < 0)
					
			return -1;	/* Signal received */		
		
		goto retry;
	}

	if (len <=0)
		goto retry;

	return (len==MAX_IPMB_MSG_SIZE)?0:len;
}

int as_mctp_recv (struct i2c_adapter *i2c_adap,
                    char *buf )
{
	int len=-1;
	unsigned long flags;
	int FifoPtr;
	//int i;
	int ret;


retry:
	len = -1;	
	/* Copy to user space buffer */
   	spin_lock_irqsave( &as_data_ptr[i2c_adap->nr ].data_lock , flags);

	if (as_data_ptr[i2c_adap->nr].MCTPRX_Entries)
	{
		FifoPtr = as_data_ptr[i2c_adap->nr].MCTPRX_Reader;
		len = as_data_ptr[i2c_adap->nr].MCTPRX_Len[FifoPtr];
		if (len > 0)
		{
			log_i2c_plain(i2c_adap, buf, len);
			memcpy(buf,as_data_ptr[i2c_adap->nr].MCTPRX_data[FifoPtr],len);
			#if 0
			for ( i= 0; i < len; i++)
			{
				printk("d[%x]  %x\n",i,(unsigned)buf[i]);
			}
			#endif
			as_data_ptr[i2c_adap->nr].MCTPRX_Len[FifoPtr] = 0;
		}
		else
		{
			printk("WARNING: I2C%d: Empty Slave Data !!!\n",i2c_adap->nr);
			len  = -1;
		}
		if ((++as_data_ptr[i2c_adap->nr].MCTPRX_Reader) == MAX_FIFO_LEN)
			as_data_ptr[i2c_adap->nr].MCTPRX_Reader = 0;
		as_data_ptr[i2c_adap->nr].MCTPRX_Entries--;

		spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
		/* Enable slave as we read data out of buffer */
		i2c_as_enable_slave(i2c_adap->nr);
	}

   else
	{
  	 	spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
		
		ret = wait_event_interruptible_timeout(as_data_ptr[i2c_adap->nr].as_mctp_data_wait,as_data_ptr[i2c_adap->nr].MCTPRX_Entries, HZ);
		if ( ret < 0)
			return -1;	/* Signal received */	
		else if (ret == 0)
			return -2; /*timeout */
		
		goto retry;
	}

	if (len <=0)
		goto retry;

	return (len==MAX_IPMB_MSG_SIZE)?0:len;
}

int as_smb_host_notify_recv (struct i2c_adapter *i2c_adap,
                    char *buf, int num )
{
	int len=-1;
	unsigned long flags;
	int FifoPtr;
	int i;


retry:
	len = -1;	
	/* Copy to user space buffer */
   	spin_lock_irqsave( &as_data_ptr[i2c_adap->nr ].data_lock , flags);

	if (as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Entries)
	{
		FifoPtr = as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Reader;
		len = as_data_ptr[i2c_adap->nr].SMB_SlaveRX_len[FifoPtr];
		if (len > 0)
		{
			memcpy(buf,as_data_ptr[i2c_adap->nr].SMB_SlaveRX_data[FifoPtr],len);
			//#if 0
			for ( i= 0; i < len; i++)
			{
				printk("d[%x]  %x\n",i,(unsigned)buf[i]);
			}
			//#endif
			
			as_data_ptr[i2c_adap->nr].SMB_SlaveRX_len[FifoPtr] = 0;
		}
		else
		{
			printk("WARNING: I2C%d: Empty Slave Data !!!\n",i2c_adap->nr);
			len  = -1;
		}
		if ((++as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Reader) == MAX_FIFO_LEN)
			as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Reader = 0;
		as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Entries--;

		spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
		/* Enable slave as we read data out of buffer */
		i2c_as_enable_slave(i2c_adap->nr);
	}

   else
	{
  	 	spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags);
		if (wait_event_interruptible(as_data_ptr[i2c_adap->nr].as_slave_data_wait,
					as_data_ptr[i2c_adap->nr].SMB_SlaveRX_Entries) < 0)
					
			return -1;	/* Signal received */		
		
		goto retry;
	}

	if (len <=0)
		goto retry;

	return (len==MAX_IPMB_MSG_SIZE)?0:len;
}

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
int as_slave_send( struct i2c_adapter *i2c_adap, 
                    char *buf, int num ) 
{ 
        unsigned long flags; 
        int  size = 0; 
    int i; 
     
       
    if (as_data_ptr[i2c_adap->nr].SSIF_enable) 
    { 
        /* Copy to user space buffer */ 
        spin_lock_irqsave( &as_data_ptr[i2c_adap->nr ].data_lock , flags); 
     
        size = buf[0]; 
        memcpy(&buf[0],&buf[1],size); 
        
        as_data_ptr[i2c_adap->nr].TX_len = 0;
        as_data_ptr[i2c_adap->nr].TX_index = 0;
        for (i=0; i<size; i++) 
        { 
            as_data_ptr[i2c_adap->nr].TX_data[i] = buf[i]; 
        }
        as_data_ptr[i2c_adap->nr].TX_len = size; 
        
        spin_unlock_irqrestore( &as_data_ptr[i2c_adap->nr ].data_lock , flags); 
    } 
        kfree(buf); 
        
        return size + 1; 
} 
#endif


















