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
 * ipmi_hal.h
 * This header file chooses either softprocessor apis or hal apis
 * for hardware 
 *
 * Author: Vinothkumar S <vinothkumars@ami.com> 
 * 
 *****************************************************************/
#ifndef IPMI_HAL_H
#define IPMI_HAL_H

#include "hal_api.h"

#define    IPMI_HAL_INIT(dl_handle, BMCInst)		hal_init (dl_handle, BMCInst)
#define    IPMI_HAL_INIT_SENSOR(BMCInst)     		hal_init_sensor_tbl(BMCInst)
#define    IPMI_HAL_INIT_DEVICES(BMCInst)    		hal_init_device(BMCInst)
#define    IPMI_HAL_GET_SENSOR_PROPERTIES(SEN_NUM, PROP,BMCInst) hal_get_sensor_properties(SEN_NUM, (sensor_properties_t*)PROP,BMCInst)
#define    IPMI_HAL_GET_SENSOR_READING(SEN_NUM, READING,BMCInst)  hal_get_sensor_reading(SEN_NUM, READING,BMCInst)



#endif //IPMI_HAL_H

