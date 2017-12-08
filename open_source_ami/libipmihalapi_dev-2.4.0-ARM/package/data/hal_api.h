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
 * hal_api.h
 * HAL API functions
 *
 * Author: Vinoth kumar S <vinothkumars@ami.com> 
 * 
 *****************************************************************/
#ifndef HAL_API_H
#define HAL_API_H
 
#include "Types.h"
#include "hal_defs.h"
#include "Debug.h"

#define DEVICE_NOT_PRESENT      0xFFFF
#define INVALID_HANDLE          -1



/* Platform Access Routines Library macros */
#define HAL_PAR_LIB          		libpar.so

/* Platform Access Routines Library API macros */
#define HAL_GET_TOTAL_SENSORS_API  	hal_get_total_sensors
#define HAL_GET_SENSOR_TBL_ENTRY_API  	hal_get_sensor_table_entry
#define HAL_GET_TOTAL_DEVICES_API  	hal_get_total_devices
#define HAL_GET_DEVICE_TBL_ENTRY_API  	hal_get_device_table_entry
#define HAL_GET_DEVICE_INIT_ENTRY_API  	hal_get_device_init_entry


typedef struct
{
	par_fn_t        par ;
	device_tbl_t    *pdevice_tbl;
	u16             num_device;
	u8              is_hal_initilized ;
	u16             sensor_handle_tbl [MAX_SENSOR_NUMBERS];
}HAL_T;


extern int HALHandlesInit();

extern device_tbl_t* get_sensor_table (u16 sensor, int BMCInst);

extern int hal_init (void  *lib_handle, int BMCInst); 

extern int hal_init_sensor_tbl (int BMCInst);

extern int hal_init_device (int BMCInst);

extern int hal_get_sensor_reading (u16 sensor_num, u16 *p_reading,int BMCInst);

extern int  hal_get_sensor_properties (u16 sensor_num,  sensor_properties_t* psensor_properties,int BMCInst);

extern int hal_write_sensor (u16 sensor_num, u8* pwritebuf, u8 writelen,int BMCInst);

extern int hal_init_sensor (u16 sensor_num,int BMCInst);

extern int hal_get_device_handle (INT16U deviceid,int BMCInst);

extern device_tbl_t* get_device_table (int handle,int BMCInst);

extern int hal_device_read  (int handle, u8* preadbuf, int* preadlen,int BMCInst);

extern int hal_device_write  (int handle , u8* pwritebuf, u8 writelen,int BMCInst);

extern int hal_device_init  (int handle,int BMCInst);

extern int hal_device_get_properties  (int handle , u8* properties,int BMCInst);

extern int handle_check_device_presence  (u16 deviceid ,int BMCInst);

extern int hal_device_sensor_num  (int handle , u8* psensornum,int BMCInst);

extern int hal_gpio_get_dir  (int handle , u8* dir,int BMCInst);

extern int hal_gpio_set_dir  (int handle, u8 dir,int BMCInst);

extern int hal_gpio_get_pol  (int handle, u8* pol,int BMCInst);

extern int hal_gpio_set_pol  (int handle, u8 pol,int BMCInst);

extern int hal_rw_i2c  (int handle, u8* preadbuf, u8 readlen, u8* pwritebuf, u8 writelen,int BMCInst);

extern int hal_get_next_device_handle (int prev_handle, INT16U device_id,int BMCInst);

extern int hal_gpio_set_output_reg  (int handle, u8 val,int BMCInst);


extern int hal_get_total_sensors (void);
extern device_tbl_t* hal_get_sensor_table_entry (int table_index);
extern int hal_get_total_devices (void);
extern device_tbl_t* hal_get_device_table_entry (int table_index);
extern phal_hndlr_t hal_get_device_init_entry (int table_index);


#endif //#define HAL_API_H

