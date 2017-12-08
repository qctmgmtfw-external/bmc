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


#ifndef _I2C_HARDWARE_H
#define _I2C_HARDWARE_H
#include <linux/version.h>

/* Sensible Defaults */
#define DEFAULT_TIMEOUT     ( 10 )          /* Timeout/delay for bus activities */
#define DEFAULT_RETRIES     ( 3 )			/* Retries on send/receive */
#define DEFAULT_BB_RETRIES  ( 5 )           /* Retries to get a free bus */

/* These values are as per the data sheet provided for 100 KHz i2c clock */
#if defined(SOC_AST2300)
// CLOCK_AC_1_VAL = 0x77777355 - 100KHz *
// CLOCK_AC_1_VAL = 0x77744333 - 400KHz
#define AST_I2C_0_CLOCK			0x77777355
#define AST_I2C_1_CLOCK			0x77777355
#define AST_I2C_2_CLOCK			0x77777355
#define AST_I2C_3_CLOCK			0x77777355
#define AST_I2C_4_CLOCK			0x77777355
#define AST_I2C_5_CLOCK			0x77777355
#define AST_I2C_6_CLOCK			0x77777355
#define AST_I2C_7_CLOCK			0x77777355
#define AST_I2C_8_CLOCK			0x77777355
#else
// CLOCK_AC_1_VAL = 0x77777344 - 100KHz
#define AST_I2C_0_CLOCK			0x77777344
#define AST_I2C_1_CLOCK			0x77777344
#define AST_I2C_2_CLOCK			0x77777344
#define AST_I2C_3_CLOCK			0x77777344
#define AST_I2C_4_CLOCK			0x77777344
#define AST_I2C_5_CLOCK			0x77777344
#define AST_I2C_6_CLOCK			0x77777344
#endif

#define AST_DEFAULT_NUM_PULSES     ( 10 )          /* i2c recovery pulses number */

#define DEFAULT_CLOCK_AC_2_VAL			0x0
#define DEFAULT_BUF_CONTROL_VAL			0x0000

#define I2C_FUNCTION_CONTROL_REG       (0x00)
#define I2C_CLOCK_AC_1_CONTROL_REG     (0x04)
#define I2C_CLOCK_AC_2_CONTROL_REG     (0x08)
#define I2C_INTR_CONTROL_REG		   (0x0C)	
#define I2C_INTR_STATUS_REG			   (0x10)
#define I2C_CMD_STATUS_REG			   (0x14)	 
#define I2C_SLAVE_ADDR_REG  		   (0x18)
#define I2C_BUF_CONTROL_REG  		   (0x1C)
#define I2C_DATA_REG        		   (0x20)
#define I2C_DMA_MODE_CONTROL_REG	   (0x24)
#define I2C_DMA_MODE_STATUS_REG	   	   (0x28)
#define GENERAL_STATUS_REG			   (0xFF)	 
#define AS_SIGNAL_RECEIVED  		   (0xffffffff)
#define INTR_ENABLE_MASK		   (0xFF)	

/* I2C Device Base Offsets */
#define AST_I2C_GLOBAL_BASE		AST_I2C_BASE
#define AST_I2C_DEV_1_BASE		(AST_I2C_GLOBAL_BASE + 0x40)
#define AST_I2C_DEV_2_BASE		(AST_I2C_DEV_1_BASE  + 0x40)
#define AST_I2C_DEV_3_BASE		(AST_I2C_DEV_2_BASE  + 0x40)
#define AST_I2C_DEV_4_BASE		(AST_I2C_DEV_3_BASE  + 0x40)
#define AST_I2C_DEV_5_BASE		(AST_I2C_DEV_4_BASE  + 0x40)
#define AST_I2C_DEV_6_BASE		(AST_I2C_DEV_5_BASE  + 0x40)
#define AST_I2C_DEV_7_BASE		(AST_I2C_DEV_6_BASE  + 0x40)
#if defined(SOC_AST2300)
#define AST_I2C_DEV_8_BASE		(AST_I2C_DEV_7_BASE  + 0x140)
#define AST_I2C_DEV_9_BASE		(AST_I2C_DEV_8_BASE  + 0x40)
#endif

/* Bits */
#define ENABLE_MASTER_FUNC							(1 << 0)
#define ENABLE_SLAVE_FUNC							(1 << 1)
#define ENABLE_SLAVE_GENERAL_CALL_ADDR				(1 << 2)
#define ENABLE_SMBUS_ARP_HOST_ADDR_RESP				(1 << 3)
#define ENABLE_SMBUS_ALERT_RESP_ADDR				(1 << 4)
#define ENABLE_SMBUS_DEFAULT_ADDR					(1 << 5)
#define ENABLE_P2P_MODE								(1 << 6)
#define ENABLE_SCL_ACTIVE_PULL_HIGH					(1 << 7)
#define ENABLE_SDA_ACTIVE_PULL_HIGH					(1 << 8)
#define ENABLE_LSB_FIRST							(1 << 9)
#define ENABLE_RECV_FAST_MODE						(1 << 10)

#define ENABLE_TX_DONE_WITH_ACK_INTR				(1 << 0)
#define ENABLE_TX_DONE_WITH_NACK_INTR				(1 << 1)
#define ENABLE_RX_DONE_INTR							(1 << 2)
#define ENABLE_ARBITRATION_LOST_INTR				(1 << 3)
#define ENABLE_STOP_CONDITION_RECVD_INTR			(1 << 4)
#define ENABLE_ABNORMAL_START_STOP_RECVD_INTR		(1 << 5)
#define ENABLE_SCL_LOW_TIMEOUT_INTR					(1 << 6)
#define ENABLE_SLAVE_ADDR_MATCH_INTR				(1 << 7)
#define ENABLE_GENERAL_CALL_ADDR_MATCH_INTR			(1 << 8)
#define ENABLE_SMBUS_DEFAULT_ADDR_MATCH_INTR		(1 << 9)
#define ENABLE_SMBUS_ALERT_RESP_ADDR_MATCH_INTR		(1 << 10)
#define ENABLE_SMBUS_ARP_HOST_ADDR_MATCH_INTR		(1 << 11)
#define ENABLE_SMBUS_DEVICE_ALERT_INTR				(1 << 12)
#define ENABLE_BUS_RECOVER_DONE_INTR				(1 << 13)

#define TX_DONE_WITH_ACK							(1 << 0)
#define TX_DONE_WITH_NACK							(1 << 1)
#define RX_DONE										(1 << 2)
#define MASTER_ARBITRATION_LOST						(1 << 3)
#define STOP_DETECTED								(1 << 4)
#define ABNORMAL_START_STOP_DETECTED				(1 << 5)
#define SCL_LOW_TIMEOUT								(1 << 6)
#define SLAVE_ADDR_MATCH							(1 << 7)
#define GENERAL_CALL_ADDR_MATCH						(1 << 8)
#define SMBUS_DEFAULT_ADDR_MATCH					(1 << 9)
#define SMBUS_ALERT_RESP_ADDR_MATCH					(1 << 10)
#define SMBUS_ARP_HOST_ADDR_MATCH					(1 << 11)
#define SMBUS_DEVICE_ALERT							(1 << 12)
#define BUS_RECOVER_DONE							(1 << 13)

#define MASTER_START								(1 << 0)
#define MASTER_TRANSMIT								(1 << 1)
#define SLAVE_TRANSMIT								(1 << 2)
#define MASTER_RECEIVE								(1 << 3)
#define MASTER_SLAVE_RECEIVE_COMMAND_LAST			(1 << 4)
#define MASTER_STOP									(1 << 5)
#define ENABLE_MASTER_SLAVE_TX_DATA_BUF				(1 << 6)
#define ENABLE_MASTER_SLAVE_RX_DATA_BUF				(1 << 7)
#define ENABLE_MASTER_SLAVE_TX_DMA_BUF				(1 << 8)
#define ENABLE_MASTER_SLAVE_RX_DMA_BUF				(1 << 9)
#define ENABLE_SMBUS_SLAVE_ALERT					(1 << 10)
#define ENABLE_BUS_RECOVER_COMMAND					(1 << 11)
#define BUS_BUSY_STATUS								(1 << 16)
#define SAMPLE_SDA_LINE								(1 << 17)
#define SAMPLE_SCL_LINE								(1 << 18)
#define TRANSFER_MODE_STATE							(0x0F << 19)
#define TRANSFER_MODE_TIMING_STATE					(0x03 << 23)
#define SCL_O_STATE									(1 << 25)
#define SCL_OE_STATE								(1 << 26)
#define SDA_O_STATE									(1 << 27)
#define SDA_OE_STATE								(1 << 28)
//#define SCL_O										(1 << 12)
#define SCL_OE										(1 << 13)
//#define SDA_O										(1 << 14)
#define SDA_OE										(1 << 15)

#define SDA_HIGH	(1 << 14)
#define SDA_LOW		~(1 << 14)

#define SCL_HIGH	(1 << 12)
#define SCL_LOW		~(1 << 12)

#define BUF_BASE_ADDR_PTR							(0x3F << 0)
#define TX_DATA_BUF_END_ADDR						(0xFF << 8)
#define RX_DATA_BUF_END_ADDR						(0xFF << 16)
#define REAL_RX_DATA_BUF_ADDR_PTR					(0xFF << 24)

#define READ_RX_DATA								(0xFF << 8)
#define READ_TX_DATA								(0xFF << 0)

#define DMA_BUF_SIZE								(0xFFF << 0)
#define DMA_BUF_BASE_ADDR							(0xFFFF << 12)

#define DMA_LAST_ACCESSED_ADDR						(0xFFF << 12)

/* Init Values */
#define	DEFAULT_FUNCTION_VAL						(ENABLE_MASTER_FUNC | ENABLE_SLAVE_FUNC)

#define ENABLE_INTS_VAL								(ENABLE_TX_DONE_WITH_ACK_INTR  			| \
													 ENABLE_TX_DONE_WITH_NACK_INTR 			| \
													 ENABLE_RX_DONE_INTR		   			| \
													 ENABLE_ARBITRATION_LOST_INTR  			| \
													 ENABLE_STOP_CONDITION_RECVD_INTR		| \
													 ENABLE_ABNORMAL_START_STOP_RECVD_INTR	| \
													 ENABLE_SCL_LOW_TIMEOUT_INTR			| \
													 ENABLE_SLAVE_ADDR_MATCH_INTR)

#define CLR_ALL_INTS_VAL							 0xFFFFFFFF

#define AS_INTR_MASK								 0x1FFF	
#define AS_JUST_STOP								(MASTER_STOP)
#define AS_STOP_XMIT								(MASTER_STOP | MASTER_TRANSMIT)
#define AS_XMIT_TX									(MASTER_TRANSMIT)
#define AS_STOP_RCV									(MASTER_STOP | MASTER_SLAVE_RECEIVE_COMMAND_LAST | MASTER_RECEIVE)
#define AS_XMIT_RCV									(MASTER_RECEIVE)
#define AS_START					 				(MASTER_START | MASTER_TRANSMIT)

#define I2C_BB										(BUS_BUSY_STATUS)
#define I2C_CLRSAM									(SLAVE_ADDR_MATCH)
#define I2C_SDA										(SAMPLE_SDA_LINE)
#define I2C_SCL										(SAMPLE_SCL_LINE)
#define I2C_ACK										(TX_DONE_WITH_ACK)
#define I2C_NACK									(TX_DONE_WITH_NACK)
#define I2C_CLRAL									(MASTER_ARBITRATION_LOST)
#define I2C_DT										(TX_DONE_WITH_ACK)
#define I2C_DR										(RX_DONE)
#define	I2C_I2CRST									(~ENABLE_MASTER_FUNC | ~ENABLE_SLAVE_FUNC)
#define I2C_CLRSTOP									(STOP_DETECTED)



u32 i2c_as_read_reg( int bus, u32 offset );
void i2c_as_write_reg( int bus, u32 value, u32 offset );

void i2c_as_reset(int bus);
void i2c_init_hardware(int bus);
void i2c_init_slave_address(int bus);
int i2c_wait_for_bus_free(int bus);
int i2c_bus_recovery(int bus);
int i2c_as_disable_slave(int bus);
int i2c_as_enable_slave(int bus);
void i2c_as_disable_all_interrupts(int bus);



#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(2,6,24)) 
irqreturn_t i2c_handler( int this_irq, void *dev_id);
#else
irqreturn_t i2c_handler( int this_irq, void *dev_id,struct pt_regs *regs );
#endif



void i2c_enable_bus(int bus);
void i2c_disable_bus(int bus);
void i2c_as_disable_interrupt(int bus, unsigned long mask);
void i2c_as_enable_interrupt(int bus, unsigned long mask);

int as_master_xfer(struct i2c_adapter *i2c_adap,
                    struct i2c_msg *msgs, int num );
int as_smb_xfer( struct i2c_adapter *i2c_adap, u16 addr, unsigned short flags,
							char read_write, u8 command, int size, union i2c_smbus_data * data);

int as_slave_recv( struct i2c_adapter *i2c_adap,
                    char *buf, int num );

int as_smb_host_notify_recv( struct i2c_adapter *i2c_adap,
                    char *buf, int num );

int i2c_bus_recovery(int bus);
int as_i2c_bus_recovery(struct i2c_adapter *i2c_adap);

void i2c_as_set_slave( int bus, int slave );
int i2c_as_get_slave( int bus );
void i2c_as_reset(int bus);

#endif

