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
#include <linux/dma-mapping.h>
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
    init_waitqueue_head( &(as_data_ptr[bus].as_mctp_data_wait));
	as_data_ptr[bus].op_status = 0;
	as_data_ptr[bus].abort_status = 0;

#if defined(GROUP_AST1070_COMPANION)
	if (bus < AST_BMC_BUS_COUNT)
	{
		as_data_ptr[bus].i2c_dma_mode = I2C_BYTE_MODE;
	}
	else
	{
		#if defined(CONFIG_SPX_FEATURE_AST1070_I2C_DMA_MODE)
		as_data_ptr[bus].i2c_dma_mode = I2C_DMA_MODE;//I2C_DMA_MODE;I2C_BYTE_MODE
		#else
		as_data_ptr[bus].i2c_dma_mode = I2C_BYTE_MODE;
		#endif
	}
#elif defined(CONFIG_SPX_FEATURE_I2C_DMA_MODE)
	as_data_ptr[bus].i2c_dma_mode = I2C_DMA_MODE;
#else
	as_data_ptr[bus].i2c_dma_mode = I2C_BYTE_MODE;
#endif

    if (as_data_ptr[bus].i2c_dma_mode == I2C_DMA_MODE)
    {
        as_data_ptr[bus].dma_buff = dma_alloc_coherent(NULL, AST_I2C_DMA_SIZE, &as_data_ptr[bus].dma_addr, GFP_KERNEL);
        if (!as_data_ptr[bus].dma_buff) {
            printk( KERN_ERR "unable to allocate tx Buffer memory\n");
        }
        if(as_data_ptr[bus].dma_addr%4 !=0) {
            printk( KERN_ERR "not 4 byte boundary \n");
        }
        memset (as_data_ptr[bus].dma_buff, 0, AST_I2C_DMA_SIZE);
    }
    else
    {
        as_data_ptr[bus].dma_buff = NULL;
        as_data_ptr[bus].dma_addr = 0;
    }

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

	as_data_ptr[bus].SlaveTX_Enable = 0;
	as_data_ptr[bus].SlaveTX_READ_DATA = 0;
	as_data_ptr[bus].SlaveTX_REQ_cmd = 0;
	as_data_ptr[bus].SlaveTX_RES_len = 0;
	as_data_ptr[bus].SlaveTX_index = 0;
	as_data_ptr[bus].SlaveTX_count = 0;
	
	for(i=0;i<MAX_FIFO_LEN;i++)
	{
		as_data_ptr[bus].MCTPRX_Len[i] = 0;
	}
	as_data_ptr[bus].MCTPRX_Writer = 0;
	as_data_ptr[bus].MCTPRX_Reader = 0;
	as_data_ptr[bus].MCTPRX_Entries = 0;

	as_data_ptr[bus].master_xmit_recv_mode_flag = SLAVE;

	for(i=0;i<SLAVETX_MAX_RES_SIZE;i++) 
	{ 
		as_data_ptr[bus].SlaveTX_RES_data[i] = 0xFF; 
	}

	as_data_ptr[bus].address_sent= 0;
	as_data_ptr[bus].target_address = 0;
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
	as_data_ptr[bus].i2c_link_state = 0;

#ifdef JM_RECOVERY 
	for (i=0; i<128; i++) {
		as_data_ptr[bus].dev_fail_cnt[i] = 0;
	}
	as_data_ptr[bus].bus_busy_cnt = 0;
#endif
	
}
