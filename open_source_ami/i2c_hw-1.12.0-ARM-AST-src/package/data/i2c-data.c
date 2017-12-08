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

struct i2c_as_data as_data_ptr[BUS_COUNT];

void i2c_init_internal_data(int bus)
{
    int i;
	
    /* Initialize locks, queues and variables */
    spin_lock_init( &as_data_ptr[bus].data_lock );
    init_waitqueue_head( &(as_data_ptr[bus].as_wait)); 
    init_waitqueue_head( &(as_data_ptr[bus].as_slave_data_wait)); 
	as_data_ptr[bus].op_status = 0;
	as_data_ptr[bus].abort_status = 0;

	as_data_ptr[bus].TX_len = 0;
	as_data_ptr[bus].TX_index = 0;

	as_data_ptr[bus].MasterRX_len = 0;
	as_data_ptr[bus].MasterRX_index = 0;

	as_data_ptr[bus].Linear_SlaveRX_len = 0;
	as_data_ptr[bus].Linear_SlaveRX_index = 0;

	for(i=0;i<MAX_FIFO_LEN;i++)
	{
		as_data_ptr[bus].SlaveRX_len[i] = 0;
		as_data_ptr[bus].SlaveRX_index[i] = 0;
	}
	as_data_ptr[bus].SlaveRX_Writer = 0;
	as_data_ptr[bus].SlaveRX_Reader = 0;
	as_data_ptr[bus].SlaveRX_Entries = 0;

	as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;

	as_data_ptr[bus].address_sent= 0;
	as_data_ptr[bus].block_read = 0;
	as_data_ptr[bus].block_proc_call= 0;
	as_data_ptr[bus].host_notify_flag = 0;


	as_data_ptr[bus].SMB_Linear_SlaveRX_len = 0;
	as_data_ptr[bus].SMB_Linear_SlaveRX_index = 0;

	for(i=0;i<MAX_FIFO_LEN;i++)
	{
		as_data_ptr[bus].SMB_SlaveRX_len[i] = 0;
		as_data_ptr[bus].SMB_SlaveRX_index[i] = 0;
	}
	as_data_ptr[bus].SMB_SlaveRX_Writer = 0;
	as_data_ptr[bus].SMB_SlaveRX_Reader = 0;
	as_data_ptr[bus].SMB_SlaveRX_Entries = 0;
	
	
}
