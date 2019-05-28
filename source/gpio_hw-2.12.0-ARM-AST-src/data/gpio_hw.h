/*********************************************************
 **                                                     **
 **    (C)Copyright 2009-2015, American Megatrends Inc. **
 **                                                     **
 **            All Rights Reserved.                     **
 **                                                     **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,      **
 **                                                     **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.  **
 **                                                     **
 ********************************************************/
#ifndef __GPIO_MOD__
#define __GPIO_MOD__


/* Basic Definition */
#if defined(SOC_AST2300) || defined(SOC_AST1050_1070)
#define GROUP_AST2300 1
#endif
#if defined(SOC_AST2400) || defined(SOC_AST1250) || defined(SOC_AST3100)
#define GROUP_AST2400 1
#endif
#if defined(SOC_AST2500) || defined(SOC_AST2530)
#define GROUP_AST2500 1
#endif

#define BANK_GPIO_DATA_OFFSET		0	// A,B,C,D   bank(00~03)
#define BANK_EXTGPIO_DATA_OFFSET	3	// E,F,G,H   bank(04~07)
#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
#define BANK_SIMGPIO_0_DATA_OFFSET	7	// I,J,K,L   bank(08~11)
#define BANK_SIMGPIO_1_DATA_OFFSET	11	// M,N,O,P   bank(12~15)
#define BANK_SIMGPIO_2_DATA_OFFSET	15	// Q,R,S bank(16~18) for AST2300, Q,R,S,T bank(16~19) for AST2400
 #if defined(GROUP_AST2300)
 #define BANK_SGPIO_DATA_OFFSET		19	// A,B,C,D   bank(19~22)
 #define BANK_EXTSGPIO_DATA_OFFSET	23	// E,F,G,H   bank(23~26)
 #endif /* GROUP_AST2300 */
 #if defined(GROUP_AST2400)
 #define BANK_SIMGPIO_3_DATA_OFFSET	19	// U,V,W,X   bank(20~23)
 #define BANK_SIMGPIO_4_DATA_OFFSET	23	// Y,Z,AA,AB bank(24~27)
 #define BANK_SGPIO_DATA_OFFSET		28	// A,B,C,D   bank(28~31)
 #define BANK_EXTSGPIO_DATA_OFFSET	32	// E,F,G,H   bank(32~35)
 #define BANK_EXTSGPIO_1_DATA_OFFSET	36	// I,J   bank(36~37)
 #endif /* GROUP_AST2400 */
 #if defined(GROUP_AST2500)
 #define BANK_SIMGPIO_3_DATA_OFFSET	    19	// U,V,W,X      bank(20~23)
 #define BANK_SIMGPIO_4_DATA_OFFSET	    23	// Y,Z,AA,AB    bank(24~27)
 #define BANK_SIMGPIO_5_DATA_OFFSET	    27	// AC,AD,AE,AF  bank(28~31)
 #define BANK_SGPIO_DATA_OFFSET		    32	// A,B,C,D      bank(32~35)
 #define BANK_EXTSGPIO_DATA_OFFSET	    36	// E,F,G,H      bank(36~39)
 #define BANK_EXTSGPIO_1_DATA_OFFSET	40	// I,J          bank(40~41)
 #endif /* GROUP_AST2500 */

#define BANK_EXTSGPIO			(BANK_SGPIO_DATA_OFFSET)
#endif

#if defined(GROUP_AST2500)
#define GPIO_SIZE			512
#define MAX_GPIO			336
#define SGPIO_START_PIN		256
#define MAX_GPIO_GRPS			28
#elif defined(GROUP_AST2400)
#define GPIO_SIZE			300
//To fix the BANK Y, BANK AB only had 4 pins,(shift 8 pins)
#define MAX_GPIO			304
#define SGPIO_START_PIN		224
#define MAX_GPIO_GRPS			19
#elif defined(GROUP_AST2300)
#define GPIO_SIZE			200
#define MAX_GPIO			216
#define SGPIO_START_PIN		152
#define MAX_GPIO_GRPS			19
#else
#define GPIO_SIZE			100
#define MAX_GPIO			64
#define SGPIO_START_PIN		NULL
#endif


/* SoC Register */
#define GPIO_BASE_OFFS			AST_GPIO_BASE

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
#define AST_SCU_MULTI_FUNC_2		0x84
#define AST_SCU_MULTI_PIN_SGPIO		0x00000f00
#define AST_SCU_UNLOCK_MAGIC		0x1688A8A8
#endif

// Standard (A,B,C,D)
#define GPIO_DATA_OFFSET		0x00
#define GPIO_PIN_DIR_OFFSET		0x04
#define GPIO_INT_EN_OFFSET		0x08
#define GPIO_INT_SEN_TYPE0_OFFSET	0x0C
#define GPIO_INT_SEN_TYPE1_OFFSET	0x10
#define GPIO_INT_SEN_TYPE2_OFFSET	0x14
#define GPIO_INT_STATUS_OFFSET		0x18
#define GPIO_RESET_TOLERANT_OFFSET	0x1C
// Extended (E,F,G,H)
#define EXTGPIO_DATA_OFFSET		0x20
#define EXTGPIO_PIN_DIR_OFFSET		0x24
#define EXTGPIO_INT_EN_OFFSET		0x28
#define EXTGPIO_INT_SEN_TYPE0_OFFSET	0x2C
#define EXTGPIO_INT_SEN_TYPE1_OFFSET	0x30
#define EXTGPIO_INT_SEN_TYPE2_OFFSET	0x34
#define EXTGPIO_INT_STATUS_OFFSET	0x38
#define EXTGPIO_RESET_TOLERANT_OFFSET	0x3C
// Configuration
#define GPIO_DEBOUNCE_SET1		0x40
#define GPIO_DEBOUNCE_SET2		0x44
#define EXTGPIO_DEBOUNCE_SET1		0x48
#define EXTGPIO_DEBOUNCE_SET2		0x4C
#define GPIO_DEBOUNCE_TIME1		0x50
#define GPIO_DEBOUNCE_TIME2		0x54
#define GPIO_DEBOUNCE_TIME3		0x58
#define GPIO_CMD_SOURCE0		0x60
#define GPIO_CMD_SOURCE1		0x64
#define EXTGPIO_CMD_SOURCE0		0x68
#define EXTGPIO_CMD_SOURCE1		0x6C

#if defined(GROUP_AST2300) || defined(GROUP_AST2400) || defined(GROUP_AST2500)
// Data Read
#define GPIO_DATA_READ_OFFSET		0x0C0
#define EXTGPIO_DATA_READ_OFFSET	0x0C4
#define SIMGPIO_0_DATA_READ_OFFSET	0x0C8
#define SIMGPIO_1_DATA_READ_OFFSET	0x0CC
#define SIMGPIO_2_DATA_READ_OFFSET	0x0D0
// Simple#0 (I,J,K,L)
#define SIMGPIO_0_DATA_OFFEST		0x070
#define SIMGPIO_0_PIN_DIR_OFFEST	0x074
#define SIMGPIO_0_CMD_SOURCE0		0x090
#define SIMGPIO_0_CMD_SOURCE1		0x094
#define SIMGPIO_0_INT_EN_OFFSET		0x098
#define SIMGPIO_0_INT_SEN_TYPE0_OFFSET	0x09C
#define SIMGPIO_0_INT_SEN_TYPE1_OFFSET	0x0A0
#define SIMGPIO_0_INT_SEN_TYPE2_OFFSET	0x0A4
#define SIMGPIO_0_INT_STATUS_OFFSET	0x0A8
// Simple#1 (M,N,O,P)
#define SIMGPIO_1_DATA_OFFEST		0x078
#define SIMGPIO_1_PIN_DIR_OFFEST	0x07C
#define SIMGPIO_1_CMD_SOURCE0		0x0E0
#define SIMGPIO_1_CMD_SOURCE1		0x0E4
#define SIMGPIO_1_INT_EN_OFFSET		0x0E8
#define SIMGPIO_1_INT_SEN_TYPE0_OFFSET	0x0EC
#define SIMGPIO_1_INT_SEN_TYPE1_OFFSET	0x0F0
#define SIMGPIO_1_INT_SEN_TYPE2_OFFSET	0x0F4
#define SIMGPIO_1_INT_STATUS_OFFSET	0x0F8
// Simple#2 (Q,R,S), (T) for AST2400
#define SIMGPIO_2_DATA_OFFEST		0x080
#define SIMGPIO_2_PIN_DIR_OFFEST	0x084
#define SIMGPIO_2_CMD_SOURCE0		0x110
#define SIMGPIO_2_CMD_SOURCE1		0x114
#define SIMGPIO_2_INT_EN_OFFSET		0x118
#define SIMGPIO_2_INT_SEN_TYPE0_OFFSET	0x11C
#define SIMGPIO_2_INT_SEN_TYPE1_OFFSET	0x120
#define SIMGPIO_2_INT_SEN_TYPE2_OFFSET	0x124
#define SIMGPIO_2_INT_STATUS_OFFSET	0x128
// Serial GPIO
#define SGPIO_DATA_OFFSET		0x200
#define SGPIO_INT_EN_OFFSET		0x204
#define SGPIO_INT_SEN_TYPE0_OFFSET	0x208
#define SGPIO_INT_SEN_TYPE1_OFFSET	0x20C
#define SGPIO_INT_SEN_TYPE2_OFFSET	0x210
#define SGPIO_INT_STATUS_OFFSET		0x214
#define SGPIO_RESET_TOLERANT_OFFSET	0x218
#define EXTSGPIO_DATA_OFFSET		0x21C
#define EXTSGPIO_INT_EN_OFFSET		0x220
#define EXTSGPIO_INT_SEN_TYPE0_OFFSET	0x224
#define EXTSGPIO_INT_SEN_TYPE1_OFFSET	0x228
#define EXTSGPIO_INT_SEN_TYPE2_OFFSET	0x22C
#define EXTSGPIO_INT_STATUS_OFFSET	0x230
#define EXTSGPIO_RESET_TOLERANT_OFFSET	0x234
// EXT SGPIO #1 for AST2400
#define EXTSGPIO_1_DATA_OFFSET		0x238
#define EXTSGPIO_1_INT_EN_OFFSET		0x23C
#define EXTSGPIO_1_INT_SEN_TYPE0_OFFSET	0x240
#define EXTSGPIO_1_INT_SEN_TYPE1_OFFSET	0x244
#define EXTSGPIO_1_INT_SEN_TYPE2_OFFSET	0x248
#define EXTSGPIO_1_INT_STATUS_OFFSET	0x24C
#define EXTSGPIO_1_RESET_TOLERANT_OFFSET	0x250
#define SGPIO_CONF_OFFSET		0x254
#define SGPIO_DATA_READ_OFFSET		0x270
#define EXTSGPIO_DATA_READ_OFFSET	0x274
#define EXTSGPIO_1_DATA_READ_OFFSET	0x278

#if defined(GROUP_AST2400) || defined(GROUP_AST2500)
#define SIMGPIO_0_RESET_TOLERANT_OFFSET	0x0AC
#define SIMGPIO_0_DEBOUNCE_SET1		0x0B0
#define SIMGPIO_0_DEBOUNCE_SET2		0x0B4
#define SIMGPIO_0_INPUT_MASK		0x0B8
#define SIMGPIO_1_RESET_TOLERANT_OFFSET	0x0FC
#define SIMGPIO_1_DEBOUNCE_SET1		0x100
#define SIMGPIO_1_DEBOUNCE_SET2		0x104
#define SIMGPIO_1_INPUT_MASK		0x108
#define SIMGPIO_2_RESET_TOLERANT_OFFSET	0x12C
#define SIMGPIO_2_DEBOUNCE_SET1		0x130
#define SIMGPIO_2_DEBOUNCE_SET2		0x134
#define SIMGPIO_2_INPUT_MASK		0x138
#define GPIO_INPUT_MASK			0x1D0
#define EXTGPIO_INPUT_MASK		0x1D4
#define SIMGPIO_3_DATA_READ_OFFSET	0x0D4
#define SIMGPIO_4_DATA_READ_OFFSET	0x0D8
// Simple#3 (U,V,W,X)
#define SIMGPIO_3_DATA_OFFEST		0x088
#define SIMGPIO_3_PIN_DIR_OFFEST	0x08C
#define SIMGPIO_3_CMD_SOURCE0		0x140
#define SIMGPIO_3_CMD_SOURCE1		0x144
#define SIMGPIO_3_INT_EN_OFFSET		0x148
#define SIMGPIO_3_INT_SEN_TYPE0_OFFSET	0x14C
#define SIMGPIO_3_INT_SEN_TYPE1_OFFSET	0x150
#define SIMGPIO_3_INT_SEN_TYPE2_OFFSET	0x154
#define SIMGPIO_3_INT_STATUS_OFFSET	0x158
#define SIMGPIO_3_RESET_TOLERANT_OFFSET	0x15C
#define SIMGPIO_3_DEBOUNCE_SET1		0x160
#define SIMGPIO_3_DEBOUNCE_SET2		0x164
#define SIMGPIO_3_INPUT_MASK		0x168
// Simple#4 (Y,Z,AA,AB)
#define SIMGPIO_4_DATA_OFFEST		0x1E0
#define SIMGPIO_4_PIN_DIR_OFFEST	0x1E4
#define SIMGPIO_4_CMD_SOURCE0		0x170
#define SIMGPIO_4_CMD_SOURCE1		0x174
#define SIMGPIO_4_INT_EN_OFFSET		0x178
#define SIMGPIO_4_INT_SEN_TYPE0_OFFSET	0x17C
#define SIMGPIO_4_INT_SEN_TYPE1_OFFSET	0x180
#define SIMGPIO_4_INT_SEN_TYPE2_OFFSET	0x184
#define SIMGPIO_4_INT_STATUS_OFFSET	0x188
#define SIMGPIO_4_RESET_TOLERANT_OFFSET	0x18C
#define SIMGPIO_4_DEBOUNCE_SET1		0x190
#define SIMGPIO_4_DEBOUNCE_SET2		0x194
#define SIMGPIO_4_INPUT_MASK		0x198
#endif /* GROUP_AST2400 || GROUP_AST2500 */

#if defined(GROUP_AST2500)
// Simple#4 (AC,AD,AE,AF)
#define SIMGPIO_5_DATA_OFFEST		0x1E8
#define SIMGPIO_5_PIN_DIR_OFFEST	0x1EC
#define SIMGPIO_5_CMD_SOURCE0		0x1A0
#define SIMGPIO_5_CMD_SOURCE1		0x1A4
#define SIMGPIO_5_INT_EN_OFFSET		0x1A8
#define SIMGPIO_5_INT_SEN_TYPE0_OFFSET	0x1AC
#define SIMGPIO_5_INT_SEN_TYPE1_OFFSET	0x1B0
#define SIMGPIO_5_INT_SEN_TYPE2_OFFSET	0x1B4
#define SIMGPIO_5_INT_STATUS_OFFSET	0x1B8
#define SIMGPIO_5_RESET_TOLERANT_OFFSET	0x1BC
#define SIMGPIO_5_DEBOUNCE_SET1		0x1C0
#define SIMGPIO_5_DEBOUNCE_SET2		0x1C4
#define SIMGPIO_5_INPUT_MASK		0x1C8

#endif /* GROUP_AST2500 */

#endif /* GROUP_AST2300 || GROUP_AST2400 || GROUP_AST2500 */


/* Driver Info */
#define MAX_GPIO_IDX			(MAX_GPIO - 1)
#define GPIO_MAJOR_NO			101
#define AST_GPIO_DEV_NAME		"ast-gpio"
#define AST_SGPIO_DEV_NAME		"ast-sgpio"

#define GPIO_HW_DEV_NAME		"gpio_hw"
#define MAX_GPIO_HW_PORT		1

#ifdef DEBUG
#define ERR(format, args...)	printk(PREFIX CERR  "ERR  " format CNORM "\n", ##args)
#define WARN(format, args...)	printk(PREFIX CWARN "WARN " format CNORM "\n", ##args)
#define INFO(format, args...)	printk(PREFIX CINFO "INFO " format CNORM "\n", ##args)
#define DBG(format, args...)	printk(PREFIX CDBG  "DBG  " format CNORM "\n", ##args)
#else
#define ERR(format, args...)
#define INFO(format, args...)
#define DBG(format, args...)
#endif

/* Gpio Device Private data */
typedef struct {
	void	*ast2050_gpio_addr;
}  __attribute__((packed)) gpio_dev_t;


/* GPIO pull up/down resistor data */
typedef struct
{
	int group;
	int bit;
} __attribute__((packed)) gpio_pull_up_down_Info_t;

/*  function declarations */
int ast2050_readgpio      ( void *gpin_data  );
int ast2050_writegpio     ( void *gpin_data  );
int ast2050_getdir        ( void *gpin_data  );
int ast2050_setdir        ( void *gpin_data  );
int ast2050_getpol        ( void *gpin_data  );
int ast2050_setpol        ( void *gpin_data  );
int ast2050_getpull_up_down    ( void *gpin_data  );
int ast2050_setpull_up_down    ( void *gpin_data  );
int ast2050_regsensorints      ( void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr );
int ast2050_unregsensorints    ( void *gpin_data  );
int ast2050_regchassisints     ( void* gpin_data, uint32_t total_chassis_interrupts,  void* chassis_itr );
int ast2050_unregchassisints   ( void *gpin_data  );
void ast2300_enable_serial_gpio( void );
int ast2050_readgpios  (void *gpio_list_data_info, unsigned int count);
int ast2050_writegpios (void *gpio_list_data_info, unsigned int count);
int ast2050_getgpiosdir  (void *gpio_list_data_info, unsigned int count);
int ast2050_setgpiosdir (void *gpio_list_data_info, unsigned int count);
int ast2050_read_debounce      ( void *gpin_data  ); //Quanta
int ast2050_write_debounce     ( void *gpin_data  ); //Quanta
#endif /* __GPIO_MOD__ */

