/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * hal_defs.h
 * HAL Definitions
 *
 * Author: Vinoth kumar S <vinothkumars@ami.com>
 *
 *****************************************************************/
#ifndef HAL_DEFS_H
#define HAL_DEFS_H

#include "Types.h"
#include "SensorMonitor.h"

#define MAX_INT_HNDLR_NAME  25
#define MAX_NM_HNDLR_NAME  25

/**
 * @brief HAL Error code definitions
**/
typedef enum
{
    /* HAL Error codes  */
    HAL_ERR_NONE,           /* No error */
    HAL_ERR_READ,           /* Error in reading device */
    HAL_ERR_WRITE,          /* Error in writing to device*/
    HAL_ERR_FUNC_NOT_SUPPORTED,     /* Function called is not supported */

} hal_status_codes_e;

/**
 * @brief HAL Supported Hardware functions
**/
typedef enum
{
    HAL_DEVICE_READ,        /* General Read function */
    HAL_DEVICE_WRITE,       /* General Write function */

    /* Special I2C  functions  */
    HAL_I2C_READ_REG,       /* Read I2C register */
    HAL_I2C_READ_WRITE,     /* Read/Write I2C */

    /* Special GPIO  functions  */
    HAL_SET_GPIO_DIR,       /* GPIO Set direction */
    HAL_GET_GPIO_DIR,       /* GPIO Get direction */
    HAL_SET_GPIO_POL,       /* GPIO Set Polarity */
    HAL_GET_GPIO_POL,       /* GPIO Get Polarity */
    HAL_SET_GPIO_OUTPUT_REG,       /* GPIO Set output data register */

} hal_func_codes_e;

/**
 * @brief HAL Supported Device Types
**/
typedef enum
{
    HAL_DEVICE_I2C,         /* I2C Device */
    HAL_DEVICE_GPIO,        /* GPIO Device */
    HAL_DEVICE_MMAP,        /* Memory mapped device */
    HAL_DEVICE_FANPWM,   /* FanPwm Device */

} hal_dev_type_e;

/** 
 * @brief HAL Error code definitions 
**/ 
typedef enum 
{ 
    HAL_SOURCE_PORT_UART,         /* Source uart port */ 
    HAL_SOURCE_PORT_COM,          /* Source com port */ 
    HAL_SOURCE_PORT_BOTH,         /* Source both port */ 
     
} hal_lpcuart_type_e; 

/**
 * @brief This structure holds device properties information
**/
typedef struct
{
    int     handle;     /* Device handle */
    u8      sensor_num; /* sensor number if device is a sensor */
    int     len;        /* Device property length */
    u8      *pData;     /* Device properties */

} hal_dev_properties_t;


/**
 * @brief This structure holds I2C device information
**/
typedef struct
{
    u8      bus;        /* Bus number in which the device resides */
    u8      slave_addr; /* Slave address of the device */
    u8      reg_num;    /* Register to read/write */

    /* I2C read/write functionality uses the read/write buffer
           present in the HAL structure */

} hal_i2c_t;

/**
 * @brief This structure holds GPIO device information
**/
typedef struct
{
    u8      pin;    /* GPIO number */
    u8      dir;    /* direction for this gpio */
    u8      pol;    /* polarity for this gpio */

} hal_gpio_t;

/**
 * @brief This structure holds Memory mapped device information
**/
typedef struct
{
    u32     addr;   /* memory map to access */
    u8      mask;   /* memory map mask to read or write data */

} hal_mmap_t;

/**
 * @brief This structure holds ADC device information
**/
typedef struct
{
    int channel; /* adc channel to read */
    int raw;     /* non-0 means driver returns raw data */

} hal_adc_t;


/**
 * @brief This structure necessary hardware information to access any device
**/

typedef struct
{
	u8	     fannum;
	unsigned int rpmvalue;
	unsigned int fanpwm;
	unsigned int fantach;

}hal_fanpwm_t;
/**
 * @brief This structure necessary hardware information to access any device
**/

typedef struct
{
	unsigned char target;
	unsigned char dev_id;
	unsigned char write_len;
	unsigned char write_buffer[20];
	unsigned char read_len;
	unsigned char read_buffer[20];
}hal_peci_t;

/** 
 * @brief This structure necessary hardware information to access any device 
**/ 
 
typedef struct 
{ 
    unsigned char source_port_type; 
    unsigned short source_port; 
    unsigned short destination_port; 
}hal_lpcuart_t; 

typedef struct
{
    unsigned char IsIntConfig;
    char IntrHndlrName[MAX_INT_HNDLR_NAME];
    unsigned char Int_num;
    unsigned char Intsource;
    unsigned char Intsensornum;
    unsigned char SensorType;
    unsigned char TriggerMethod;
    unsigned char TriggerType;
    unsigned char int_type;
    unsigned char int_input_data;
}hal_intsensor_t;

#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
typedef struct
{
    unsigned char IsNMconfig;
    unsigned char Sensor_Num;
    unsigned char SensorType;
    unsigned char NetFnLUN;
    unsigned char cmd;
    u16 NM_SensorID;
    unsigned char res_par;
    char FillHndlrName[MAX_NM_HNDLR_NAME];
    char ParseHndlrName[MAX_NM_HNDLR_NAME];
}hal_nmsensor_t;
#endif
/**
 * @brief This structure necessary hardware information to access any device
**/

typedef struct
{
    hal_status_codes_e  status_code;    /* HAL api's status codes */
    hal_func_codes_e    func;           /* Hardware function */

    union {
        u8              *pbyte;         /* All HAL apis return data
                                        in bytes using this pointer */
        u16             *pword;         /* All HAL apis return data accesed
                                        in words using this pointer */
        u8              *pread_buf;     /* All HAL apis return data
                                        in bytes using this pointer */
    };

    int                 read_len;       /* bytes to read */
    u8                  *pwrite_buf;    /* All HAL apis write data
                                        from this buffer */
    int                 write_len;      /* bytes to write */

    hal_dev_properties_t dev_prop;      /* Device properties information */
    hal_dev_type_e      dev_type;       /* Type of the device */

    hal_i2c_t           i2c;            /* Hold I2C information */
    hal_gpio_t          gpio;           /* Hold GPIO information */
    hal_mmap_t          mmap;           /* Hold memory mapped information */
    hal_adc_t           adc;            /* Hold ADC information */

    hal_fanpwm_t     fan; 	/* Hold FanPwm information Added by winston for fanpwm */
    hal_peci_t  peci;            /* Hold PECI information Added by winston for peci */
    hal_lpcuart_t lpcuart;            /* Hold LPCUART information Added by jcchiu for lpcuart */
    hal_intsensor_t intsensor;     /* Hold Interrupt Sensor information */

#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
    hal_nmsensor_t nmsensor;     /* Hold Node Manager Sensor information */
#endif
} hal_t;



/**
 * @brief HAL read/write/init function type definition
**/
typedef int (*phal_hndlr_t) (hal_t *phal);


/**
 * @brief HAL Device Table
**/
typedef struct
{
//   u16          device_enum;
   u16          device_instance;
   u16          sensor_number;
   u16          device_id;
   phal_hndlr_t device_read;
   phal_hndlr_t device_write;
   phal_hndlr_t device_init;
   void         *device_properties;
   int          properties_len;

} device_tbl_t;

/**
 * @brief HAL Sensor Properties Table
**/
typedef struct
{
   u8  monitor_interval;
   u8  monitor_state;
   u16 normal_value;     		
   u8  poweron_delay;  // Delay after chassis power on in seconds. 0  don’t delay
   u8  sysreset_delay; // Delay after chassis reset    in seconds. 0  don’t delay

} sensor_properties_t;

typedef int (*preturn_total_t) (void);
typedef device_tbl_t* (*preturn_tbl_t) (int);
typedef struct
{
    preturn_total_t  pget_total_sensors;
    preturn_tbl_t     pget_sen_tbl_entry;
    preturn_total_t  pget_total_devices;
    preturn_tbl_t     pget_dev_tbl_entry;

} par_fn_t;


#endif //#define HAL_DEFS_H

