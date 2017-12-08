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

#ifndef _I2C_DATA_H
#define _I2C_DATA_H

#include <linux/i2c.h>

#define TRANSFERSIZE 1024
#define MAX_FIFO_LEN 16

#if defined(SOC_AST2300)
#define BUS_COUNT  9
#else
#define BUS_COUNT  7
#endif

#define DEFAULT_SLAVE_0     ( 0x10 )
#define DEFAULT_SLAVE_1     ( 0x10 )
#define DEFAULT_SLAVE_2     ( 0x10 )
#define DEFAULT_SLAVE_3     ( 0x10 )
#define DEFAULT_SLAVE_4     ( 0x10 )
#define DEFAULT_SLAVE_5     ( 0x10 )
#define DEFAULT_SLAVE_6     ( 0x10 )
#if defined(SOC_AST2300)
#define DEFAULT_SLAVE_7     ( 0x10 )
#define DEFAULT_SLAVE_8     ( 0x10 )
#endif

#define MASTER 0x10
#define SLAVE 0x00
#define MASTER_XMIT 0x11
#define MASTER_RECV 0x10

/* Error codes in compliance with IPMI */
#define ARBLOST		0x80000081
#define BUSERR		0x80000082
#define NACKONWR	0x80000083
#define TRUNKRD		0x80000084

struct i2c_as_data
{
	unsigned char TX_data[TRANSFERSIZE];
	int TX_len;
	int TX_index;

	unsigned char MasterRX_data[TRANSFERSIZE];
	int MasterRX_len;
	int MasterRX_index;

	unsigned char Linear_SlaveRX_data[TRANSFERSIZE];
	int Linear_SlaveRX_len;
	int Linear_SlaveRX_index;

	unsigned char SlaveRX_data[MAX_FIFO_LEN][TRANSFERSIZE];
	int SlaveRX_len[MAX_FIFO_LEN];
	int SlaveRX_index[MAX_FIFO_LEN];
	
	int SlaveRX_Writer;
	int SlaveRX_Reader;
	int SlaveRX_Entries;

	spinlock_t data_lock;

	volatile u32 op_status;
	volatile u32 abort_status;

	int master_xmit_recv_mode_flag;

	wait_queue_head_t as_wait;
    wait_queue_head_t as_slave_data_wait;

	int address_sent;
	int block_read;
	int block_proc_call;
	int host_notify_flag;

	unsigned char SMB_Linear_SlaveRX_data[TRANSFERSIZE];
	int SMB_Linear_SlaveRX_len;
	int SMB_Linear_SlaveRX_index;

	
	unsigned char SMB_SlaveRX_data[MAX_FIFO_LEN][TRANSFERSIZE];
	int SMB_SlaveRX_len[MAX_FIFO_LEN];
	int SMB_SlaveRX_index[MAX_FIFO_LEN];
		
	int SMB_SlaveRX_Writer;
	int SMB_SlaveRX_Reader;
	int SMB_SlaveRX_Entries;
};

int i2c_as_add_bus( struct i2c_adapter * );
int i2c_as_del_bus( struct i2c_adapter * );

void i2c_init_internal_data(int bus);

#endif
