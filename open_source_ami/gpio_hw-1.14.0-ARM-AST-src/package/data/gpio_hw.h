/*****************************************************
 **                                                            				**
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            				**
 **            All Rights Reserved.                            			**
 **                                                            				**
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             	**
 **                                                            				**
 **        Georgia - 30093, USA. Phone-(770)-246-8600.          **
 **                                                            				**
 *****************************************************/


#ifndef __GPIO_MOD__
#define __GPIO_MOD__

#ifdef SOC_AST2300
#define GPIO_SIZE				200
#define MAX_GPIO 				216
#define SGPIO_START_PIN			152
#define BANK_EXTSGPIO					19
#define BANK_GPIO_DIR_Q_S				15
#define BANK_GPIO_DIR_M_P				11
#define BANK_GPIO_DATA_I_L				7
#define AST_SCU_MULTI_FUNC_2	0x84
#define AST_SCU_MULTI_PIN_SGPIO	0x00000f00
#define AST_SCU_UNLOCK_MAGIC	0x1688A8A8
#else
#define GPIO_SIZE				100
#define MAX_GPIO 				64
#endif

#define GPIO_BASE_OFFS 			AST_GPIO_BASE
#define GPIO_DATA_OFFSET				0x00
#define GPIO_PIN_DIR_OFFSET				0x04
#define GPIO_INT_EN_OFFSET				0x08
#define GPIO_INT_SEN_TYPE0_OFFSET		0x0C
#define GPIO_INT_SEN_TYPE1_OFFSET		0x10
#define GPIO_INT_SEN_TYPE2_OFFSET		0x14
#define GPIO_INT_STATUS_OFFSET			0x18
#define EXTGPIO_DATA_OFFSET				0x20
#define EXTGPIO_PIN_DIR_OFFSET			0x24
#define EXTGPIO_INT_EN_OFFSET			0x28
#define EXTGPIO_INT_SEN_TYPE0_OFFSET	0x2C
#define EXTGPIO_INT_SEN_TYPE1_OFFSET	0x30
#define EXTGPIO_INT_SEN_TYPE2_OFFSET	0x34
#define EXTGPIO_INT_STATUS_OFFSET		0x38

#ifdef SOC_AST2300
#define GPIO_DATA_I_L					0x70
#define GPIO_DIR_I_L					0x74
#define GPIO_DATA_M_P					0x78
#define GPIO_DIR_M_P					0x7C
#define GPIO_DATA_Q_S					0x80
#define GPIO_DIR_Q_S					0x84
#define SIMGPIO_0_INT_EN_OFFSET			0x098
#define SIMGPIO_0_INT_SEN_TYPE0_OFFSET	0x09C
#define SIMGPIO_0_INT_SEN_TYPE1_OFFSET	0x0A0
#define SIMGPIO_0_INT_SEN_TYPE2_OFFSET	0x0A4
#define SIMGPIO_0_INT_STATUS_OFFSET		0x0A8
#define SIMGPIO_1_INT_EN_OFFSET			0x0E8
#define SIMGPIO_1_INT_SEN_TYPE0_OFFSET	0x0EC
#define SIMGPIO_1_INT_SEN_TYPE1_OFFSET	0x0F0
#define SIMGPIO_1_INT_SEN_TYPE2_OFFSET	0x0F4
#define SIMGPIO_1_INT_STATUS_OFFSET		0x0F8
#define SIMGPIO_2_INT_EN_OFFSET			0x118
#define SIMGPIO_2_INT_SEN_TYPE0_OFFSET	0x11C
#define SIMGPIO_2_INT_SEN_TYPE1_OFFSET	0x120
#define SIMGPIO_2_INT_SEN_TYPE2_OFFSET	0x124
#define SIMGPIO_2_INT_STATUS_OFFSET		0x128
#define SGPIO_DATA_OFFSET				0x200
#define SGPIO_INT_EN_OFFSET				0x204
#define SGPIO_INT_SEN_TYPE0_OFFSET		0x208
#define SGPIO_INT_SEN_TYPE1_OFFSET		0x20C
#define SGPIO_INT_SEN_TYPE2_OFFSET		0x210
#define SGPIO_INT_STATUS_OFFSET			0x214
#define EXTSGPIO_DATA_OFFSET			0x21C
#define EXTSGPIO_INT_EN_OFFSET			0x220
#define EXTSGPIO_INT_SEN_TYPE0_OFFSET	0x224
#define EXTSGPIO_INT_SEN_TYPE1_OFFSET	0x228
#define EXTSGPIO_INT_SEN_TYPE2_OFFSET	0x22c
#define EXTSGPIO_INT_STATUS_OFFSET		0x230
#define SGPIO_CONF_OFFSET				0x254
#define SGPIO_DATA_READ_OFFSET			0x270
#define EXTSGPIO_DATA_READ_OFFSET		0x274
#endif


#define BANK_EXTGPIO_DATA_OFFSET		3

#define MAX_GPIO_IDX            MAX_GPIO - 1
#define GPIO_MAJOR_NO			101
#define NAME					"gpio-ast"

#define GPIO_HW_DEV_NAME        "gpio_hw"
#define MAX_GPIO_HW_PORT		1


#if 0
#define ERR(format, args...)    printk(PREFIX CERR  "ERR  " format CNORM "\n", ##args)
#define WARN(format, args...)   printk(PREFIX CWARN "WARN " format CNORM "\n", ##args)
#define INFO(format, args...)   printk(PREFIX CINFO "INFO " format CNORM "\n", ##args)
#define DBG(format, args...)    printk(PREFIX CDBG  "DBG  " format CNORM "\n", ##args)
#else
#define ERR(format, args...)
#define INFO(format, args...)
#define DBG(format, args...)
#endif

/* Gpio Device Private data */
typedef struct {
    void     *ast2050_gpio_addr;
}  __attribute__((packed)) gpio_dev_t;


/*  function declarations */
int ast2050_readgpio      ( void *gpin_data  );
int ast2050_writegpio     ( void *gpin_data  );
int ast2050_getdir        ( void *gpin_data  );
int ast2050_setdir        ( void *gpin_data  );
int ast2050_getpol        ( void *gpin_data  );
int ast2050_setpol        ( void *gpin_data  );
int ast2050_getpull_up_down        ( void *gpin_data  );
int ast2050_setpull_up_down        ( void *gpin_data  );
int ast2050_regsensorints  ( void* gpin_data, uint32_t total_interrupt_sensors, void* sensor_itr );
int ast2050_unregsensorints   ( void *gpin_data  );
int ast2050_regchassisints   ( void* gpin_data, uint32_t total_chassis_interrupts,  void* chassis_itr );
int ast2050_unregchassisints  ( void *gpin_data  );
void ast2300_enable_serial_gpio(void);



#endif //__GPIO_MOD__






