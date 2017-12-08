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
 * hal_hw.h
 * HAL hardware  functions
 *
 * Author: Vinoth kumar S <vinothkumars@ami.com> 
 * 
 *****************************************************************/
#ifndef HAL_HW_H
#define HAL_HW_H
 
#include "Types.h"
#include "hal_defs.h"
#include "Debug.h"

#define I2C_LIB "/usr/local/lib/libi2c.so"
#define GPIO_LIB "/usr/local/lib/libgpio.so"
#define ADC_LIB "/usr/local/lib/libadc.so"
#define FANPWM_LIB "/usr/local/lib/libpwmtach.so"
#define PECI_LIB "/usr/local/lib/libpeci.so"
#define LPCUART_LIB "/usr/local/lib/liblpcuart.so"
#define NM_LIB "/usr/local/lib/libipminmsupport.so"
#define MAX_HAL_HANDLE_NAME 256
#define FAN_MAX_NO 16

typedef enum
{
    HAL_I2C_RW = 0,
    HAL_I2C_MR,
    HAL_I2C_MW,
    HAL_I2C_SET_HOST_ADDRESS,
    HAL_I2C_SLAVE_READ_ON_FD,
    HAL_I2C_MASTER_WRITE_ON_FD,
    HAL_I2C_GET_RECOVERY_INFO,
    HAL_I2C_SET_RECOVERY_INFO,
    MAX_I2C_HANDLE
}HAL_I2C_HANDLE;

typedef enum
{
    HAL_GPIO_GET_DIR = 0,
    HAL_GPIO_GET_POL,
    HAL_GPIO_GET_DATA,
    HAL_GPIO_SET_DIR_OUT,
    HAL_GPIO_SET_DIR_IN,
    HAL_GPIO_SET_POL_HIGH,
    HAL_GPIO_SET_POL_LOW,
    HAL_GPIO_SET_DATA_HIGH,
    HAL_GPIO_SET_DATA_LOW,
    HAL_GPIO_REGISTER_SENSOR_INTERRUPTS,
    HAL_GPIO_WAIT_FOR_SENSOR_INTERRUPTS,
    HAL_GPIO_UNREGISTER_SENSOR_INTERRUPTS,
    HAL_GPIO_REGISTER_CHASSIS_INTERRUPTS,
    HAL_GPIO_WAIT_FOR_CHASSIS_INTERRUPTS,
    HAL_GPIO_UNREGISTER_CHASSIS_INTERRUPTS, 
    MAX_GPIO_HANDLE
}HAL_GPIO_HANDLE;

typedef enum
{
    HAL_ADC_READ = 0,
    MAX_ADC_HANDLE
}HAL_ADC_HANDLE;

typedef enum
{
    HAL_FANPWM_ENABLEFAN = 0,
    HAL_FANPWM_ENABLESTATUSREAD,
    HAL_FANPWM_GETFANSPEED,
    HAL_FANPWM_SETFANSPEED,
    HAL_FANPWM_CONFIG_MAP_TBL,
    HAL_FANPWM_SET_PWM_DUTYCYCLE,
    HAL_FANPWM_GET_PWM_DUTYCYCLE,
    HAL_FANPWM_ENABLEALLFAN,
    MAX_FANPWM_HANDLE
}HAL_FANPWM_HANDLE;

typedef enum
{
    HAL_PECI_COMMAND = 0,
    HAL_PECI_READTEMP,    
    HAL_PECI_GETDIB,
    HAL_PECI_RDPKGCONFIG,
    HAL_PECI_WRPKGCONFIG,
    HAL_PECI_RDIAMSR,
    HAL_PECI_WRIAMSR,
    HAL_PECI_RDPCICONFIG,
    HAL_PECI_WRPCICONFIG,
    HAL_PECI_ENABLEAWFCS,
    MAX_PECI_HANDLE
}HAL_PECI_HANDLE;

typedef enum 
{ 
    HAL_LPCUART_ENABLE_UART_PORT = 0, 
    HAL_LPCUART_DISABLE_UART_PORT, 
    HAL_LPCUART_ROUTE_UART_TO_COM, 
    HAL_LPCUART_ROUTE_COM_TO_UART, 
    HAL_LPCUART_ROUTE_UART_TO_UART, 
    MAX_LPCUART_HANDLE 
}HAL_LPCUART_HANDLE; 
#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
typedef enum
{
    HAL_NM_SENSOR_REGISTER = 0,
    MAX_NM_HANDLE
}HAL_NM_HANDLE;
#endif
typedef struct
{
    INT16U HALHandleNum;
    INT8U HALHandleName[MAX_HAL_HANDLE_NAME];
}HAL_Init;



typedef void (*phalfunc) (void );


struct fan_map_entry_t {
unsigned int   fan_num;
unsigned int   pwm_num;
unsigned int   tach_num;
};

extern phalfunc g_HALI2CHandle[MAX_I2C_HANDLE];
extern phalfunc g_HALGPIOHandle[MAX_GPIO_HANDLE];
extern phalfunc g_HALADCHandle[MAX_ADC_HANDLE];
extern phalfunc g_HALFanPwmHandle[MAX_FANPWM_HANDLE];
extern phalfunc g_HALPECIHandle[MAX_PECI_HANDLE];
extern phalfunc g_HALLPCUARTHandle[MAX_LPCUART_HANDLE];



extern int i2c_read_reg (hal_t *phal);
extern int i2c_read_regData (hal_t *phal);
extern int i2c_write_reg (hal_t *phal);
extern int i2c_write_read (hal_t *phal);
extern int gpio_read (hal_t *phal);
extern int gpio_write (hal_t *phal);
extern int mmap_read (hal_t *phal);
extern int mmap_write (hal_t *phal);
extern int adc_read (hal_t *phal);
extern int fan_speed_read (hal_t *phal);  
extern int fan_speed_set (hal_t *phal); 
extern int fan_set_connect (hal_t *phal);
extern int peci_read_temp(hal_t *phal); 
extern int lpcuart_route_set(hal_t *phal);
#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
extern int nm_register_sensor(hal_t *phal);
#endif

#endif //#define HAL_HW_H

