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
#ifndef _I2C_DATA_H
#define _I2C_DATA_H

#include <linux/i2c.h>

#define TRANSFERSIZE 1024
#define MAX_FIFO_LEN 16

#define AST_I2C_DMA_SIZE 4096

#if defined(SOC_AST2300)
#define GROUP_AST2300 1
#endif
#if defined(SOC_AST2400) || defined(SOC_AST3100) || defined(SOC_AST3200)
#define GROUP_AST2300_PLUS 1
#endif
#if defined(SOC_AST2500) || defined(SOC_AST2530)
#define GROUP_AST2500 1
#endif
#if defined(CONFIG_SPX_FEATURE_BMCCOMPANIONDEVICE_AST1070)
#define GROUP_AST1070_COMPANION 1
#elif defined(SOC_AST1250)
#define GROUP_AST2300_PLUS 1
#endif
#if defined(SOC_AST1050_1070)
#define GROUP_AST1070_COMPANION 1
#endif

#if defined(GROUP_AST1070_COMPANION)
#define AST_BMCCC_BUS_COUNT (CONFIG_SPX_FEATURE_NUM_SINGLE_BMC_COMPANION_I2C_BUSES * CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES)
#define BUS_COUNT  (CONFIG_SPX_FEATURE_NUM_BMC_I2C_BUSES + AST_BMCCC_BUS_COUNT)
#else
#define BUS_COUNT  CONFIG_SPX_FEATURE_NUM_BMC_I2C_BUSES
#endif

#define DEFAULT_SLAVE_0     ( 0x10 )
#define DEFAULT_SLAVE_1     ( 0x10 )
#define DEFAULT_SLAVE_2     ( 0x10 )
#define DEFAULT_SLAVE_3     ( 0x10 )
#define DEFAULT_SLAVE_4     ( 0x10 )
#define DEFAULT_SLAVE_5     ( 0x10 )
#define DEFAULT_SLAVE_6     ( 0x10 )
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define DEFAULT_SLAVE_7     ( 0x10 )
#define DEFAULT_SLAVE_8     ( 0x10 )
#endif
#if defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define DEFAULT_SLAVE_9     ( 0x10 )
#define DEFAULT_SLAVE_10    ( 0x10 )
#define DEFAULT_SLAVE_11    ( 0x10 )
#define DEFAULT_SLAVE_12    ( 0x10 )
#define DEFAULT_SLAVE_13    ( 0x10 )
#endif
#if defined(GROUP_AST1070_COMPANION)
#define DEFAULT_SLAVE_14   ( 0x10 )
#define DEFAULT_SLAVE_15   ( 0x10 )
#define DEFAULT_SLAVE_16   ( 0x10 )
#define DEFAULT_SLAVE_17   ( 0x10 )
#define DEFAULT_SLAVE_18   ( 0x10 )
#define DEFAULT_SLAVE_19   ( 0x10 )
#define DEFAULT_SLAVE_20   ( 0x10 )
#define DEFAULT_SLAVE_21   ( 0x10 )
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#define DEFAULT_SLAVE_22   ( 0x10 )
#define DEFAULT_SLAVE_23   ( 0x10 )
#define DEFAULT_SLAVE_24   ( 0x10 )
#define DEFAULT_SLAVE_25   ( 0x10 )
#define DEFAULT_SLAVE_26   ( 0x10 )
#define DEFAULT_SLAVE_27   ( 0x10 )
#define DEFAULT_SLAVE_28   ( 0x10 )
#define DEFAULT_SLAVE_29   ( 0x10 )
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

#define MASTER 0x10
#define SLAVE 0x00
#define MASTER_XMIT 0x11
#define MASTER_RECV 0x10

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
#define SSIF_MAX_RES_SIZE   34   //Length, Data[1..32], PEC 
#define SSIF_RES_BUFFER_SIZE  ((SSIF_MAX_RES_SIZE/4) + (SSIF_MAX_RES_SIZE%4 == 0?0:1)) * 4 
#define SSIF_TX_DMA_BUFFER_SIZE  SSIF_RES_BUFFER_SIZE + 8
#define SSIF_RES_BYTE_MODE    0

/* Extended IOCTLS - Implemented in i2c-core and hardware for SSIF */ 
#define ENABLE_SSIF							0x0852 
#define SET_SSIF_RES_BLOCK                  0x0853 

#define IPMI_SSIF_SINGLE_PART_WRITE_SSIF_CMD           0x02
#define IPMI_SSIF_MULTI_PART_WRITE_START_SSIF_CMD      0x06 
#define IPMI_SSIF_READ_START_SMBUS_CMD                 0x03 
#define IPMI_SSIF_READ_MIDDLE_SMBUS_CMD                0x09 
#endif

/* Error codes in compliance with IPMI */
#define ARBLOST		0x80000081
#define BUSERR		0x80000082
#define NACKONWR	0x80000083
#define TRUNKRD		0x80000084

#define I2C_BYTE_MODE   0 
#define I2C_DMA_MODE    1 

//#define JM_RECOVERY 1

#ifdef CONFIG_SPX_FEATURE_ENABLE_I2C_LOG
#define I2C_LOG_FILTER_COUNT_MAX 6

#define I2C_LOG_NONE    0
#define I2C_LOG_ALL     1
#define I2C_LOG_FILTER  2

struct log_i2c_filter
{
        unsigned addr;
        unsigned mode;
        unsigned offset;
        unsigned val;
};

struct log_i2c_conf
{
        u8 log_enabled;
        struct log_i2c_filter filters[I2C_LOG_FILTER_COUNT_MAX];
};
#endif // CONFIG_SPX_FEATURE_ENABLE_I2C_LOG

struct i2c_as_data
{
    int i2c_dma_mode;
    unsigned char *dma_buff;
    dma_addr_t dma_addr;

	unsigned char TX_data[TRANSFERSIZE];
	int TX_len;
	int TX_index;

	unsigned char MCTPRX_data[MAX_FIFO_LEN][TRANSFERSIZE];
	int MCTPRX_Writer;
	int MCTPRX_Reader;
	int MCTPRX_Len[MAX_FIFO_LEN];
	int MCTPRX_Entries;
 
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
	int expected_mode_flag; // Quanta

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
	int SSIF_enable; 
	int SSIF_REQ_cmd; 
	int SSIF_RES_blocknum; 
	int SSIF_RES_len; 
	int SSIF_TX_index;
	int SSIF_TX_count; //For test usage, show how many data including padding byte are sent

	unsigned char SSIF_RES_data[SSIF_RES_BUFFER_SIZE];
	u32 SSIF_tx_buff_base;
	dma_addr_t ast_i2c_ssif_tx_dma_addr;
	unsigned char *ast_i2c_ssif_tx_dma_buff;
#endif

	wait_queue_head_t as_wait;
    wait_queue_head_t as_slave_data_wait;
	wait_queue_head_t as_mctp_data_wait;

	int address_sent;
	int target_address;
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
	unsigned short i2c_link_state;
	
#ifdef JM_RECOVERY 
	unsigned char dev_fail_cnt[128];
	unsigned char bus_busy_cnt;
	int i2c_message_slave_address;
#endif
#ifdef CONFIG_SPX_FEATURE_ENABLE_I2C_LOG
        struct log_i2c_conf log_conf;
#endif

};

int i2c_as_add_bus( struct i2c_adapter * );
int i2c_as_del_bus( struct i2c_adapter * );

void i2c_init_internal_data(int bus);





/*
* I2C BUS states 
*/

#define I2C_LINK_NO_FAILURE						0x00  //
#define I2C_UNABLE_DRIVE_CLK_HIGH				0x01  //
#define I2C_UNABLE_DRIVE_DATA_HIGH				0x02  //
#define I2C_UNABLE_DRIVE_CLK_LOW				0x03
#define I2C_UNABLE_DRIVE_DATA_LOW				0x04
#define I2C_CLOCK_LOW_TIMEOUT_CLK_HIGH			0x05   
#define I2C_UNDER_TEST							0x06  //  
#define I2C_UNDIAGNOSED_COMM_FAILURE			0x07  // 

/*
* I2c Test Operations
*/
#define I2C_TEST_BUS_BUSY					0x01
#define I2C_TEST_BUS_RECOVERABLE				0x02

typedef struct
{
       int i2cLinkState;
 } i2c_link_state_T;

typedef struct
{
 	int operation;
	int result;
 } i2c_bus_test_T;


/*
 * Flag for as_check_SignalPending func
 */
#define I2C_RECEIVE 		(1 << 0)
#define I2c_TRANSMIT 		(1 << 1)
#endif
