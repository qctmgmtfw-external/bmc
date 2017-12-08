/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
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
 * SensorAPI.h
 * Sensor Access API.
 *
 *  Author: Vinoth kumar <vinothkumars@ami.com>
 *
 ******************************************************************/
#ifndef SENSORAPI_H
#define SENSORAPI_H
#include "Types.h"
#include "SensorMonitor.h"
#include "IPMI_Sensor.h"


/*-----------------------------------------------------------------
 * @fn API_ReInitSensor
 * @brief Reinitialize sensor.
 * @param u8SensorNum - Sensor number.
 * @return 	0	if Success
 *		-1	if reinit failed.
 *-----------------------------------------------------------------*/
extern int API_ReInitSensor (INT8U u8SensorNum,int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_SetScanningInterval
 * @brief Set the scanning interval for a sensor.
 * @param u8SensorNum - Sensor number.
 * @param ScanningInt - Scanning interval.
 * @return 	0	if Success
 *		-1	if set failed.
 *-----------------------------------------------------------------*/
extern int API_SetSensorScanning (INT8U u8SensorNum, INT8U EnableFlag,int BMCInst);
/*-----------------------------------------------------------------
 * @fn API_SetScanningInterval
 * @brief Set the internal scanning enable/disable for a sensor.
 * @param u8SensorNum - Sensor number.
 * @param Enable / Disable flag.
 * @return 	0	if Success
 *		-1	if set failed.
 *-----------------------------------------------------------------*/

extern int API_SetSensorInternalScanning (INT8U u8SensorNum, INT8U EnableFlag,int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_SetSensorState
 * @brief Sets the following sensor state.
 * @param u8SensorNum - Sensor number.
 * @param State - Sensor states with the following bit descriptions.
    				  Bit 0 -  Initialization Done      
				  Bit 1 -  Update in Progress       
				  Bit 2 -  reserved                 
				  Bit 3 -  reserved                 
				  Bit 4 -  reserved                 
				  Bit 5 -  Unable to read           
				  Bit 6 -  Sensor Scanning disabled 
				  Bit 7 -  Event Message Disabled   
 * @return 	0	if Success
 *		-1	if set failed.
 *-----------------------------------------------------------------*/
extern int API_SetSensorState (INT8U u8SensorNum, INT8U State,int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GetSensorInfo
 * @brief Provides all the sensor information.
 * @param u8SensorNum - Sensor number.
 * @return 	pSensorInfo - Sensor Information.
 * 		0	if Success
 *		-1	if set failed.
 *-----------------------------------------------------------------*/
extern SensorInfo_T* API_GetSensorInfo (INT8U SensorNum,int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GlobalSensorScanningEnable
 * @brief Controls the the sensor scanning.
 * @param bEnable - TRUE to enable and FALSE to disable.
 * @return 	0   if success
 *		-1	if failed.
 *-----------------------------------------------------------------*/
extern int  API_GlobalSensorScanningEnable (BOOL bEnable,int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_GlobalSensorScanningEnable
 * @brief Returns the status of the sensor scanning.
 * @param 
 * @return  bool - TRUE : Enabled FALSE : disabled
 *
 *-----------------------------------------------------------------*/
extern int  API_GetGlobalSensorScanningEnable (int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_RegSensorCB
 * @brief Registers the callback function for sensor.
 * @param pCB - callback function
 * @param Hooktype - this can be PRE_MONITOR or POST_MONITOR.
 * @return 	0   if success
 *		-1	if failed.
 *-----------------------------------------------------------------*/
extern int API_RegSensorCB (INT8U SensorNum, void *pCB, HookType_e HookType,int BMCInst);
void API_SensorAverage(INT8U SensorNum, INT16U* SensorReading, int BMCInst);

#define	N0_OF_READINGS_TO_BE_AVERAGED 4
typedef struct
{
    INT16U   Cnt;
    INT16U Reading;
    INT16U Average;

} Average_T;


extern int API_ReArmSensor (ReArmSensorRes_T* pReArmSensorRes, int BMCInst);

/*------------------------------------------------------------------
 * @fn API_InitPowerOnTick
 * @brief Initialize the power on tick counter
 *         should be invoked when ever power on occurs.
 *------------------------------------------------------------------*/
extern void API_InitPowerOnTick (int BMCInst);

/*------------------------------------------------------------------
 * @fn API_InitSysResetTick
 * @brief Initialize the System Reset tick counter
 *        should be invoked when ever power reset occurs.
 *------------------------------------------------------------------*/
extern void API_InitSysResetTick (int BMCInst);


/*-----------------------------------------------------------------
 * @fn API_SetSensorReadingOffset  
 * @brief Set the sensor Reading Offset 
 * @param - SensorLUN - Owner's LUN  INT8U SensorNo - Sensor Number  
 * Offset  - Sendor Reading Offset 
 * @return  bool - -1 failed to set the Offset    0 - Reading is set 
 *
 *-----------------------------------------------------------------*/
extern int API_SetSensorReadingOffset ( INT8U SensorLUN ,  INT8U SensorNo ,  INT8S Offset, int BMCInst );

/*-----------------------------------------------------------------
 * @fn API_GetSensorReadingOffset  
 * @brief Get the sensor Reading Offset 
 * @param - SensorLUN - Owner's LUN  INT8U SensorNo - Sensor Number  
 * Offset  - Sendor Reading Offset 
 * @return  bool - -1 No match for the SensorNum and Lun is present 
 *
 *-----------------------------------------------------------------*/

extern int API_GetSensorReadingOffset ( INT8U SensorLUN ,  INT8U SensorNo ,  INT8S * pOffset ,int BMCInst);


#endif //SENSORAPI_H
