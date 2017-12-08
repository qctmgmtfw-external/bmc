
/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2010-2011, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 *****************************************************************
 ******************************************************************
 *
 * PDKsensor.h
 * sensor hook functions.
 *
 *  Author:Pavithra S (pavithras@amiindia.co.in)
 *
 ******************************************************************/
#ifndef PDK_SENSOR_H
#define PDK_SENSOR_H

#include "SharedMem.h"
#include "SensorAPI.h"

typedef struct  {
	INT8U						SensorNum;			/* SensorNumber for which the pre-monitor and post-monitor 
													  hook is to be replaced by default hook defined in Init_Sensor.
													*/
	pPDK_MonitorHook_T			pPreMonitor;		/* Pre monitor hook   */
	pPDK_MonitorHook_T			pPostMonitor;		/* Post Monitor hook */
	
}SensorHooks_T;

/** The below defined sensor numbers are for testing purpose.
  *   For the below sensors the default pre-monitor and post-monitor hooks
  *   are replaced by the hooks defined in PDK_RegisterSensorHooks
  */
#define SENSOR_NUMBER_1 0x01
#define SENSOR_NUMBER_2 0x02
#define SENSOR_NUMBER_3 0x03

/***
*@brief The pre -monitor hook function for first sensor.
*@	The default pre-monitor hook function is overridden by this for the first sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PreMonitorSensor_1 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);

/***
*@brief The pre -monitor hook function for second sensor.
*@	The default pre-monitor hook function is overridden by this for the second sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PreMonitorSensor_2 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);

/***
*@brief The pre -monitor hook function for third sensor.
*@	The default pre-monitor hook function is overridden by this for the third sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PreMonitorSensor_3 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);

/***
*@brief The post -monitor hook function for first sensor.
*@	The default post-monitor hook function is overridden by this for the first sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PostMonitorSensor_1 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);
/***
*@brief The post -monitor hook function for second sensor.
*@	The default post-monitor hook function is overridden by this for the second sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PostMonitorSensor_2 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);

/***
*@brief The post -monitor hook function for third sensor.
*@	The default post-monitor hook function is overridden by this for the third sensor.
*@[in]pSensorInfo - The SensorInfo structure
*@[in]pReadFlags - Flags to be used if any as in internal_sensor
*@[in]BMCInst  - The particular BMCInst
**/
int PDK_PostMonitorSensor_3 (void*  pSenInfo,INT8U *pReadFlags,int BMCInst);


/***
* PDK_RegisterSensorHooks
*@brief Pre-Monitor and Post Monitor Sensor Hooks are initialized in this function
*@[in]BMCInst  - The particular BMCInst
**/

int PDK_RegisterSensorHooks(int BMCInst);



#endif
