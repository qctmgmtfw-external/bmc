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
#ifndef _I2C_HARDWARE_H
#define _I2C_HARDWARE_H
#include <linux/version.h>

/* Sensible Defaults */
#define DEFAULT_TIMEOUT     ( 10 )          /* Timeout/delay for bus activities */
#define DEFAULT_RETRIES     ( 3 )			/* Retries on send/receive */
#define DEFAULT_BB_RETRIES  ( 5 )           /* Retries to get a free bus */
#define MAX_MCTP_MSG_SIZE   164
#define MAX_DMA_TRANSFER_LENGTH_SIZE   4095 /* Define the maximun size of DMA transfer length */

/* protection key register */
#define SCU_PROTECTION_UNLOCK		0x1688A8A8
#define I2C_RESET_CNTLR_BIT		0x04


/* These values are as per the data sheet provided for i2c clock */
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS) || defined(GROUP_AST1070_COMPANION)
#define CLOCK_AC_1_VAL_93K		0x77776305	/* 93.4KHz for AST2300/AST2400/AST3100 */
#define CLOCK_AC_1_VAL_100K		0x77776304 	/* 100KHz for AST2300/AST2400/AST3100 */
//#define CLOCK_AC_1_VAL_400K		0x77776333	/* 400KHz for AST2300/AST2400/AST3100 */
#define CLOCK_AC_1_VAL_400K		0x77766302	/* 400KHz for AST2300/AST2400/AST3100 */
#endif

#if defined(GROUP_AST2500)
#define CLOCK_AC_1_VAL_100K		0x77776344	/* 100KHz for AST2500 */
#define CLOCK_AC_1_VAL_400K		0x77776322	/* 400KHz for AST2500 */
#endif

#ifdef JM_RECOVERY
#define MAX_ALLOWED_DEVICE_FAILURES	( 10 )
#define MAX_ALLOWED_BUS_BUSY_FAILURES	( 10 )
#endif

#if defined(SOC_AST2150)
#define CLOCK_AC_1_VAL_100K		0x77754355	/* 100KHz for AST2150 */
#define CLOCK_AC_1_VAL_400K		0x77754333	/* 400KHz for AST2150 */
#endif
#if defined(SOC_AST2050)
#define CLOCK_AC_1_VAL_100K		0x77777344	/* 100KHz for AST2050 */
#define CLOCK_AC_1_VAL_400K		0x77776322	/* 400KHz for AST2050 */
#endif

#if defined(GROUP_AST2500)
	#define CLOCK_AC_1_VAL_DEFAULT	CLOCK_AC_1_VAL_100K
#else
	#define CLOCK_AC_1_VAL_DEFAULT	CLOCK_AC_1_VAL_93K
#endif

#if defined(CONFIG_SPX_FEATURE_I2C00_HIGH_SPEED_MODE)
	#define AST_I2C_0_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_0_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C01_HIGH_SPEED_MODE)
	#define AST_I2C_1_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_1_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C02_HIGH_SPEED_MODE)
	#define AST_I2C_2_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_2_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C03_HIGH_SPEED_MODE)
	#define AST_I2C_3_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_3_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C04_HIGH_SPEED_MODE)
	#define AST_I2C_4_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_4_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C05_HIGH_SPEED_MODE)
	#define AST_I2C_5_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_5_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C06_HIGH_SPEED_MODE)
	#define AST_I2C_6_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_6_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif

#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#if defined(CONFIG_SPX_FEATURE_I2C07_HIGH_SPEED_MODE)
	#define AST_I2C_7_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_7_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C08_HIGH_SPEED_MODE)
	#define AST_I2C_8_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_8_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#endif

#if defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#if defined(CONFIG_SPX_FEATURE_I2C09_HIGH_SPEED_MODE)
	#define AST_I2C_9_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_9_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C10_HIGH_SPEED_MODE)
	#define AST_I2C_10_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_10_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C11_HIGH_SPEED_MODE)
	#define AST_I2C_11_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_11_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C12_HIGH_SPEED_MODE)
	#define AST_I2C_12_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_12_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C13_HIGH_SPEED_MODE)
	#define AST_I2C_13_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_13_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#endif

#if defined(GROUP_AST1070_COMPANION)
#if defined(CONFIG_SPX_FEATURE_I2C14_HIGH_SPEED_MODE)
	#define AST_I2C_14_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_14_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C15_HIGH_SPEED_MODE)
	#define AST_I2C_15_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_15_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C16_HIGH_SPEED_MODE)
	#define AST_I2C_16_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_16_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C17_HIGH_SPEED_MODE)
	#define AST_I2C_17_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_17_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C18_HIGH_SPEED_MODE)
	#define AST_I2C_18_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_18_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C19_HIGH_SPEED_MODE)
	#define AST_I2C_19_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_19_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C20_HIGH_SPEED_MODE)
	#define AST_I2C_20_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_20_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C21_HIGH_SPEED_MODE)
	#define AST_I2C_21_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_21_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#if defined(CONFIG_SPX_FEATURE_I2C22_HIGH_SPEED_MODE)
	#define AST_I2C_22_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_22_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C23_HIGH_SPEED_MODE)
	#define AST_I2C_23_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_23_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C24_HIGH_SPEED_MODE)
	#define AST_I2C_24_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_24_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C25_HIGH_SPEED_MODE)
	#define AST_I2C_25_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_25_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#if defined(CONFIG_SPX_FEATURE_I2C26_HIGH_SPEED_MODE)
	#define AST_I2C_26_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_26_CLOCK			CLOCK_AC_1_VAL_100K
#endif
#if defined(CONFIG_SPX_FEATURE_I2C27_HIGH_SPEED_MODE)
	#define AST_I2C_27_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_27_CLOCK			CLOCK_AC_1_VAL_100K
#endif
#if defined(CONFIG_SPX_FEATURE_I2C28_HIGH_SPEED_MODE)
	#define AST_I2C_28_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_28_CLOCK			CLOCK_AC_1_VAL_100K
#endif
#if defined(CONFIG_SPX_FEATURE_I2C29_HIGH_SPEED_MODE)
	#define AST_I2C_29_CLOCK			CLOCK_AC_1_VAL_400K
#else
	#define AST_I2C_29_CLOCK			CLOCK_AC_1_VAL_DEFAULT
#endif
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

// I2C Bus Speed Modes
#define I2C_BUS_SPEED_MODE_STANDARD     100
#define I2C_BUS_SPEED_MODE_FAST         400

#define AST_DEFAULT_NUM_PULSES     ( 10 )          /* i2c recovery pulses number */

#define DEFAULT_CLOCK_AC_2_VAL			0x0
#define DEFAULT_BUF_CONTROL_VAL			0x0000

#define I2C_GLOBAL_CONTROL_REG		   (0x0C)
#define I2C_FUNCTION_CONTROL_REG       (0x00)
#define I2C_CLOCK_AC_1_CONTROL_REG     (0x04)
#define I2C_CLOCK_AC_2_CONTROL_REG     (0x08)
#define I2C_INTR_CONTROL_REG		   (0x0C)	
#define I2C_INTR_STATUS_REG			   (0x10)
#define I2C_CMD_STATUS_REG			   (0x14)	 
#define I2C_SLAVE_ADDR_REG  		   (0x18)
#define I2C_BUF_CONTROL_REG  		   (0x1C)
#define I2C_DATA_REG        		   (0x20)
#if !defined(SOC_AST3100)
#define I2C_DMA_MODE_CONTROL_REG	   (0x24)
#define I2C_DMA_MODE_STATUS_REG	   	   (0x28)
#endif
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
#if defined(GROUP_AST2300) || defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define AST_I2C_DEV_8_BASE		(AST_I2C_DEV_7_BASE  + 0x140)
#define AST_I2C_DEV_9_BASE		(AST_I2C_DEV_8_BASE  + 0x40)
#endif
#if defined(GROUP_AST2300_PLUS) || defined(GROUP_AST2500) || defined(GROUP_AST1070_COMPANION)
#define AST_I2C_DEV_10_BASE		(AST_I2C_DEV_9_BASE  + 0x40)
#define AST_I2C_DEV_11_BASE		(AST_I2C_DEV_10_BASE + 0x40)
#define AST_I2C_DEV_12_BASE		(AST_I2C_DEV_11_BASE + 0x40)
#define AST_I2C_DEV_13_BASE		(AST_I2C_DEV_12_BASE + 0x40)
#define AST_I2C_DEV_14_BASE		(AST_I2C_DEV_13_BASE + 0x40)
#endif
#if defined(GROUP_AST1070_COMPANION)
#define AST_BMCCC0_I2C_GLOBAL_BASE		AST_BMCCC_I2C_BASE(0)
#define AST_I2C_DEV_15_BASE		(AST_BMCCC0_I2C_GLOBAL_BASE + 0x40)
#define AST_I2C_DEV_16_BASE		(AST_I2C_DEV_15_BASE + 0x40)
#define AST_I2C_DEV_17_BASE		(AST_I2C_DEV_16_BASE + 0x40)
#define AST_I2C_DEV_18_BASE		(AST_I2C_DEV_17_BASE + 0x40)
#define AST_I2C_DEV_19_BASE		(AST_I2C_DEV_18_BASE + 0x40)
#define AST_I2C_DEV_20_BASE		(AST_I2C_DEV_19_BASE + 0x40)
#define AST_I2C_DEV_21_BASE		(AST_I2C_DEV_20_BASE + 0x40)
#define AST_I2C_DEV_22_BASE		(AST_I2C_DEV_21_BASE + 0x40)
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
#define AST_BMCCC1_I2C_GLOBAL_BASE		AST_BMCCC_I2C_BASE(1)
#define AST_I2C_DEV_23_BASE		(AST_BMCCC1_I2C_GLOBAL_BASE + 0x40)
#define AST_I2C_DEV_24_BASE		(AST_I2C_DEV_23_BASE + 0x40)
#define AST_I2C_DEV_25_BASE		(AST_I2C_DEV_24_BASE + 0x40)
#define AST_I2C_DEV_26_BASE		(AST_I2C_DEV_25_BASE + 0x40)
#define AST_I2C_DEV_27_BASE		(AST_I2C_DEV_26_BASE + 0x40)
#define AST_I2C_DEV_28_BASE		(AST_I2C_DEV_27_BASE + 0x40)
#define AST_I2C_DEV_29_BASE		(AST_I2C_DEV_28_BASE + 0x40)
#define AST_I2C_DEV_30_BASE		(AST_I2C_DEV_29_BASE + 0x40)
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
#if defined(GROUP_AST2300)
#define AST_I2C_POLLBUF_BASE		(AST_I2C_GLOBAL_BASE  + 0x200)
#elif defined(GROUP_AST2300_PLUS)
#define AST_I2C_POLLBUF_BASE		(AST_I2C_GLOBAL_BASE  + 0x800)
#endif
#endif

/* Bits */
/* I2CG0C: Global Control Register : Init = 0 */
#define ENABLE_SRAM_BUFFER							(1 << 0)

/* I2CD00: Function Control Register : Init = 0x00200000 */
#define ENABLE_MASTER_FUNC							(1 << 0)
#define ENABLE_SLAVE_FUNC							(1 << 1)
#define ENABLE_SLAVE_GENERAL_CALL_ADDR				(1 << 2)
#define ENABLE_SMBUS_ARP_HOST_ADDR_RESP				(1 << 3)
#define ENABLE_SMBUS_ALERT_RESP_ADDR				(1 << 4)
#define ENABLE_SMBUS_DEFAULT_ADDR					(1 << 5)
#if defined(SOC_AST3100)	
//AST3100 supports high speed (3.4Mbps). Can only act as Master in this mode. Should use buffer. 
#define ENABLE_HIGH_SPEED_MODE						(1 << 6)
#else
#define ENABLE_P2P_MODE								(1 << 6)
#endif
#define ENABLE_SCL_ACTIVE_PULL_HIGH					(1 << 7)
#define ENABLE_SDA_ACTIVE_PULL_HIGH					(1 << 8)
#if !defined(SOC_AST3100)	//These bits are reserved in AST3100
#define ENABLE_LSB_FIRST							(1 << 9)
#define ENABLE_RECV_FAST_MODE						(1 << 10)
#else
#define ENABLE_SCL_DIRECT_DRIVE_MODE				(1 << 14)
#define DISABLE_MULTI_MASTER_CAPABILITY				(1 << 15)
#define ENABLE_MASTER_AUTO_RECOVERY					(1 << 16)
#endif
#define BUFFER_SELECTION_PAGE_MASK                  0x00700000
#define BUFFER_SELECTION_PAGE_0						(0 << 20)
#define BUFFER_SELECTION_PAGE_1						(1 << 20)
#define BUFFER_SELECTION_PAGE_2						(2 << 20)
#define BUFFER_SELECTION_PAGE_3						(3 << 20)
#define BUFFER_SELECTION_PAGE_4						(4 << 20)
#define BUFFER_SELECTION_PAGE_5						(5 << 20)
#define BUFFER_SELECTION_PAGE_6						(6 << 20)
#define BUFFER_SELECTION_PAGE_7						(7 << 20)

/* I2CD0C: Interrupt Control Register : Init = 0 */
#define ENABLE_TX_DONE_WITH_ACK_INTR				(1 << 0)
#define ENABLE_TX_DONE_WITH_NACK_INTR				(1 << 1)
#define ENABLE_RX_DONE_INTR							(1 << 2)
#define ENABLE_ARBITRATION_LOST_INTR				(1 << 3)
#define ENABLE_STOP_CONDITION_RECVD_INTR			(1 << 4)
#define ENABLE_ABNORMAL_START_STOP_RECVD_INTR		(1 << 5)
#define ENABLE_SCL_LOW_TIMEOUT_INTR					(1 << 6)
#define ENABLE_SLAVE_ADDR_MATCH_INTR				(1 << 7)
#if !defined(SOC_AST3100)	//These bits are reserved in AST3100
#define ENABLE_GENERAL_CALL_ADDR_MATCH_INTR			(1 << 8)
#define ENABLE_SMBUS_DEFAULT_ADDR_MATCH_INTR		(1 << 9)
#define ENABLE_SMBUS_ALERT_RESP_ADDR_MATCH_INTR		(1 << 10)
#define ENABLE_SMBUS_ARP_HOST_ADDR_MATCH_INTR		(1 << 11)
#endif
#define ENABLE_SMBUS_DEVICE_ALERT_INTR				(1 << 12)
#define ENABLE_BUS_RECOVER_DONE_INTR				(1 << 13)
#if defined(SOC_AST3100)
#define ENABLE_SDA_LOW_TIMEOUT_INTR					(1 << 14)
#endif

/* I2CD10: Interrupt Status Register : Init = 0 */
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
#if defined(SOC_AST3100)
#define SDA_LOW_TIMEOUT								(1 << 14)
#endif

/* I2CD14: Command-Status Register : Init = 0 */
#define MASTER_START								(1 << 0)
#define MASTER_TRANSMIT								(1 << 1)
#define SLAVE_TRANSMIT								(1 << 2)
#define MASTER_RECEIVE								(1 << 3)
#define MASTER_SLAVE_RECEIVE_COMMAND_LAST			(1 << 4)
#define MASTER_STOP									(1 << 5)
#define ENABLE_MASTER_SLAVE_TX_DATA_BUF				(1 << 6)
#define ENABLE_MASTER_SLAVE_RX_DATA_BUF				(1 << 7)
#if !defined(SOC_AST3100)	//These bits are reserved in AST3100
#define ENABLE_MASTER_SLAVE_TX_DMA_BUF				(1 << 8)
#define ENABLE_MASTER_SLAVE_RX_DMA_BUF				(1 << 9)
#endif
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

/* I2CD1C: Pool Buffer Ctrl Register : Init = X */
#define POOLBUFF_CTRL_TX_END_ADDR_SHIFT      8

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


//#define JM_RECOVERY 1


u32 i2c_as_read_reg( int bus, u32 offset );
void i2c_as_write_reg( int bus, u32 value, u32 offset );
u32 i2c_as_read_global_reg( u32 offset );
void i2c_as_write_global_reg( u32 value, u32 offset );

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
#if defined(GROUP_AST1070_COMPANION)
irqreturn_t bmccc0_i2c_handler( int this_irq, void *dev_id);
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
irqreturn_t bmccc1_i2c_handler( int this_irq, void *dev_id);
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

#else

irqreturn_t i2c_handler( int this_irq, void *dev_id,struct pt_regs *regs );
#if defined(GROUP_AST1070_COMPANION)
irqreturn_t bmccc0_i2c_handler( int this_irq, void *dev_id,struct pt_regs *regs );
#if (CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2)
irqreturn_t bmccc1_i2c_handler( int this_irq, void *dev_id,struct pt_regs *regs );
#endif /* CONFIG_SPX_FEATURE_NUM_BMC_COMPANION_DEVICES >= 2 */
#endif

#endif



void i2c_enable_bus(int bus);
void i2c_disable_bus(int bus);
void i2c_as_disable_interrupt(int bus, unsigned long mask);
void i2c_as_enable_interrupt(int bus, unsigned long mask);

int as_master_xfer(struct i2c_adapter *i2c_adap,
                    struct i2c_msg *msgs, int num );
int as_smb_xfer( struct i2c_adapter *i2c_adap, u16 addr, unsigned short flags,
							char read_write, u8 command, int size, union i2c_smbus_data * data);

int as_mctp_recv(struct i2c_adapter *i2c_adap,char *buf);
int as_slave_recv( struct i2c_adapter *i2c_adap,
                    char *buf, int num );

int as_smb_host_notify_recv( struct i2c_adapter *i2c_adap,
                    char *buf, int num );

int i2c_bus_recovery(int bus);
int as_i2c_bus_recovery(struct i2c_adapter *i2c_adap);

void i2c_as_set_slave( int bus, int slave );
int i2c_as_get_slave( int bus );
void i2c_as_reset(int bus);
int i2c_as_set_speed(int bus, unsigned short speed);
void i2c_stop_cntlr_reset(void);
#ifdef JM_RECOVERY
	int i2c_is_bus_hung(int bus);
#endif

#ifdef CONFIG_SPX_FEATURE_SSIF_SUPPORT
int as_slave_send( struct i2c_adapter *i2c_adap, char *buf, int num ); 
void i2c_as_ssif_enable(int bus);
void i2c_as_set_ssif_block(int bus, u8 block_num);
void i2c_as_ssif_alloc_tx_dma_buf(int bus);
void i2c_as_ssif_free_tx_dma_buf(int bus);
#endif
	
#endif

