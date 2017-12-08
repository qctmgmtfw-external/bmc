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
 * sensor.h
 * Sensor functions.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *          
 ******************************************************************/
#ifndef SENSOR_H
#define SENSOR_H

#include "Types.h"
#include "IPMI_Sensor.h"
#include "SDRRecord.h"
#include "SensorMonitor.h"

/**
 * @brief Sensor Types
 */
#define PROCESSOR				0x07
#define POWER_SUPPLY				0x08
#define MEMORY					0x0c
#define ENTITY_PRESENCE				0x25
#define BATTERY					0x29


/**
 * @brief Sensor presence bit
 */
#define PROCESSOR_PRESENCE_DETECTED				0x07
#define	POWER_SUPPLY_PRESENCE_DETECTED				0x00
#define POWER_SUPPLY_OUT_OF_RANGE_PRESENT			0x05
#define MEMORY_PRESENCE_DETECTED				0x06
#define	ENTITY_PRESENCE_ENTITY_PRESENT				0x00
#define	BATTERY_PRESENCE_DETECTED				0x02

/**
 * @brief Event type codes
 */
#define GENERIC_EVENT_TYPE_DEV_PRESENCE		0x08
#define GENERIC_EVENT_TYPE_DEV_AVAILABLE	0x09
#define EVENT_TYPE_SENSOR_SPECIFIC		0x6f



#define EVENT_AND_SCANNING_ENABLE (BIT7|BIT6)
#define READING_UNAVAILABLE (BIT5)

/**
 * @brief Sensor Reading for Device
 */
#define DEVICE_PRESENT		0x01
#define DEVICE_ENABLED		0x01

/*** External Definitions ***/
#define THRESHOLD_SENSOR_CLASS      0x01

#define GET_SETTABLE_SENSOR_BIT(FLAGS)  (FLAGS & 0x80)
#define GET_EVENT_DATA_OP(OP)      ( ((OP) >> 6))
#define GET_ASSERT_EVT_OP(OP)      ( ((OP) >> 4) & 0x03)
#define GET_DEASSERT_EVT_OP(OP)    ( ((OP) >> 2) & 0x03)
#define GET_SETSENSOR_OP(OP)       ( (OP) & 0x03)


#define CLEAR_ASSERT_BITS            0x3
#define SET_ASSERT_BITS              0x2
#define WRITE_ASSERT_BITS            0x1

#define CLEAR_DEASSERT_BITS            0x3
#define SET_DEASSERT_BITS              0x2
#define WRITE_DEASSERT_BITS            0x1


#define WRITE_NO_EVTDATA1             0x02
#define WRITE_EVTDATA1                0x01
#define USE_SM_EVTDATA               0x00


#define DCMI_INLET_TEMP_ENTITY_ID           0x40
#define DCMI_CPU_TEMP_ENTITY_ID             0x41
#define DCMI_BASEBOARD_TEMP_ENTITY_ID       0x42

#define IPMI_INLET_TEMP_ENTITY_ID           0x37
#define IPMI_CPU_TEMP_ENTITY_ID             0x03
#define IPMI_BASEBOARD_TEMP_ENTITY_ID       0x07

/**
 * @brief Initialize Sensor information.
 * @return 0 if success, -1 if error
**/
extern int InitSensor (int BMCInst);

/**
 * @brief Initialize the scanning bit of each sensor
 * @return 0 if success, -1 if error
 */
extern int InitSensorScanningBit (int BMCInst);

/**
 * @brief Get sensor's SDR record.
 * @param SensorNum - Sensor number.
 * @return the sensor's SDR record.
**/
extern _FAR_ SDRRecHdr_T* SR_GetSensorSDR (INT8U SensorNum,int BMCInst);


extern int GetRecordIdsforDCMISensor (INT8U EntityID, INT8U EntityInstance, 
		INT8U StartingEntityInstance, INT16U* pBuf, INT8U* pTotalValidInstances,int BMCInst);


/**
 * @defgroup sdc Sensor Device Commands
 * @ingroup senevt
 * IPMI Sensor Device interface command handlers. These commands are 
 * used for configuring hysterisis and thresholds and reading
 * sensor values.
 * @{
**/
extern int GetDevSDRInfo (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetDevSDR (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ReserveDevSDRRepository (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSensorType (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorType (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ReArmSensor (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorEventStatus (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSensorHysterisis (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorHysterisis (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSensorThresholds (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorThresholds (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorReadingFactors (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSensorEventEnable (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorEventEnable (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSensorReading (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSensorReading (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
int CompareValues(BOOL isSigned, INT8U val1, INT8U val2);
/** @} */

/**
 * @brief Get Sensor Hysterisis.
 * @param SensorNum - Sensor number.
 * @param Res       - Response data.
 * @return 0 if success, -1 if error.
**/
extern int SR_GetSensorHysterisis  (INT8U SensorNum, _NEAR_ GetSensorHysterisisRes_T* Res);

/**
 * @brief Get Sensor Threshold.
 * @param SensorNum - Sensor number.
 * @param Res       - Response data.
 * @return 0 if success, -1 if error.
**/
extern int SR_GetSensorThreshold   (INT8U SensorNum, _NEAR_ GetSensorThresholdRes_T* Res);

/**
 * @brief Get Sensor Event Enables.
 * @param SensorNum - Sensor number.
 * @param Res       - Response data.
 * @return 0 if success, -1 if error.
**/
extern int SR_GetSensorEventEnable (INT8U  SensorNum, _NEAR_ GetSensorEventEnableRes_T* Res);

/**
 * @brief Get Sensor Reading.
 * @param SensorNum - Sensor number.
 * @return the Sensor reading.
**/
extern INT16U SM_GetSensorReading (INT8U SensorNum, INT16U *pSensorReading);

/**
 * @brief SR_LoadSDRDefaults.
 * @param sr - SDR Record , pSensorInfo - Sensor information.
 * @return none.
**/

extern void SR_LoadSDRDefaults (SDRRecHdr_T* sr, SensorInfo_T* pSensorInfo,int BMCInst);


/**
 * @brief SR_FindSDR.
 * @param SensorNum - Finds SDR for this sensor number.
 * @return none.
**/

extern SDRRecHdr_T* SR_FindSDR (INT8U SensorNum, int BMCInst);


#endif  /* SENSOR_H */


