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
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * ipmi_sensor.h
 * IPMI Sensor Requests and Responses.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_SENSOR_H
#define IPMI_SENSOR_H

#include "Types.h"

/*** External Definitions ***/
#define SENSOR_TYPE_SECUIRTY_VIOLATION  0x06
#define SENSOR_TYPE_EVT_LOGGING         0x10
#define SENSOR_TYPE_SYSTEM_EVENT        0x12
#define SENSOR_TYPE_CRITICAL_INTERRUPT  0x13
#define SENSOR_TYPE_WATCHDOG2           0x23
#define SENSOR_TYPE_FRU_STATE           0x2C

#define FP_NMI_OFFSET                                  0x00
#define PW_VIOLATION_OFFSET                       0x05

#define WDT_SENSOR_NUMBER                             0xFE
#define SECUIRTY_VIOLATION_SENSOR_NUMBER    0xFD
#define NMI_SENSOR_NUMBER                              0xFC
#define SENSOR_SPECIFIC_READ_TYPE                  0x6F
#define PEF_ACTION_SEN_SPECIFIC_OFFSET 0xC4


#define HOT_SWAP_SENSOR_START_NUM       0xD0

#define SENSOR_TYPE_FRUSDR_COLLECTION   0xD0
#define FRUSDR_COLLECTION_SENSOR_NUMBER 0xFD

// 0 = Communication lost during FRU collection
// 1 = FRU Invalid checksum
// 2 = Communication lost during SDR collection
#define FRU_COLLECTION_COMMUNICATION_LOST   0
#define FRU_COLLECTION_INV_CHECKSUM         1
#define SDR_COLLECTION_COMMUNICATION_LOST   2
#define SDR_COLLECTION_INV_CC               3
#define INVALID_PICMG_VERSION               4

#pragma pack( 1 )

/* GetSDRInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   NumSensor;
    INT8U   Flags;
    INT32U  TimeStamp;

} PACKED  GetSDRInfoRes_T;


/* GetDevSDRReq_T */
typedef struct
{
    INT16U  ReservationID;
    INT16U  RecID;
    INT8U   Offset;
    INT8U   Size;

} PACKED  GetDevSDRReq_T;


/* GetDevSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  NextRecID;

} PACKED  GetDevSDRRes_T;


/* ReserveDevSDRRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT16U  ReservationID;

} PACKED  ReserveDevSDRRes_T;


/* GetSensorReadingFactorReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   ReadingByte;

} PACKED  GetSensorReadingFactorReq_T;


/* GetSensorReadingFactorRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   NextReading;
    INT8U   M_LSB;
    INT8U   M_MSB_Tolerance;
    INT8U   B_LSB;
    INT8U   B_MSB_Accuracy;
    INT8U   Accuracy_MSB_Exp;
    INT8U   RExp_BExp;

} PACKED  GetSensorReadingFactorRes_T;


/* SetSensorHysterisisReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   Reserved;
    INT8U   PositiveHysterisis;
    INT8U   NegativeHysterisis;

} PACKED  SetSensorHysterisisReq_T;


/* SetSensorHysterisisRes_T */
typedef struct 
{
    INT8U   CompletionCode;

} PACKED  SetSensorHysterisisRes_T;


/* GetSensorHysterisisReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   Reserved;

} PACKED  GetSensorHysterisisReq_T;


/* GetSensorHysterisisRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   PositiveHysterisis;
    INT8U   NegativeHysterisis;

} PACKED  GetSensorHysterisisRes_T;


/* SetSensorThresholdReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   SetFlags;
    INT8U   LowerNonCritical;
    INT8U   LowerCritical;
    INT8U   LowerNonRecoverable;
    INT8U   UpperNonCritical;
    INT8U   UpperCritical;
    INT8U   UpperNonRecoverable;

} PACKED  SetSensorThresholdReq_T;


/* SetSensorThresholdRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  SetSensorThresholdRes_T;


/* GetSensorThresholdReq_T */
typedef struct
{
    INT8U   SensorNum;

} PACKED  GetSensorThresholdReq_T;


/* GetSensorThresholdRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   GetFlags;
    INT8U   LowerNonCritical;
    INT8U   LowerCritical;
    INT8U   LowerNonRecoverable;
    INT8U   UpperNonCritical;
    INT8U   UpperCritical;
    INT8U   UpperNonRecoverable;

} PACKED  GetSensorThresholdRes_T;


/* SetSensorEventEnableReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   Flags;
    INT16U  AssertionMask;
    INT16U  DeAssertionMask;

} PACKED  SetSensorEventEnableReq_T;


/* SetSensorEventEnableRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  SetSensorEventEnableRes_T;


/* GetSensorEventEnableReq_T */
typedef struct
{
    INT8U   SensorNum;

} PACKED  GetSensorEventEnableReq_T;


/* GetSensorEventEnableRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Flags;
    INT16U  AssertionMask;
    INT16U  DeAssertionMask;

} PACKED  GetSensorEventEnableRes_T;


/* GetSensorReadingReq_T */
typedef struct
{
    INT8U   SensorNum;

} PACKED  GetSensorReadingReq_T;


/* GetSensorReadingRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SensorReading;
    INT8U   Flags;
    INT8U   ComparisonStatus;
    INT8U   OptionalStatus;

} PACKED  GetSensorReadingRes_T;


/* SetSensorTypeReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   SensorType;
    INT8U   EventTypeCode;

} PACKED  SetSensorTypeReq_T;


/* SetSensorTypeRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  SetSensorTypeRes_T;


/* GetSensorTypeReq_T */
typedef struct
{
    INT8U   SensorNum;

} PACKED  GetSensorTypeReq_T;


/* GetSensorTypeRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SensorType;
    INT8U   EventTypeCode;

} PACKED  GetSensorTypeRes_T;

/* ReArmSensorReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   ReArmAllEvents;
    INT8U   ReArmAssertionEvents1;
    INT8U   ReArmAssertionEvents2;
    INT8U   ReArmDeassertionEvents1;
    INT8U   ReArmDeassertionEvents2;

} PACKED ReArmSensorReq_T;

/* ReArmSensorRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED ReArmSensorRes_T;

/* GetSensorEventStatusReq_T */
typedef struct
{
    INT8U   SensorNum;

} PACKED GetSensorEventStatusReq_T;

/* GetSensorEventStatusRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Flags;
    INT8U   AssertionEvents1;
    INT8U   AssertionEvents2;
    INT8U   DeassertionEvents1;
    INT8U   DeassertionEvents2;

} PACKED GetSensorEventStatusRes_T;

/* SetSensorReadingReq_T */
typedef struct
{
    INT8U   SensorNum;
    INT8U   Operation;
    INT8U   SensorReading;
    INT8U   AssertionEventOccuredByte1;
    INT8U   AssertionEventOccuredByte2;
    INT8U   DeAssertionEventOccuredByte1;
    INT8U   DeAssertionEventOccuredByte2;
    INT8U   EvtData1;
    INT8U   EvtData2;
    INT8U   EvtData3;

} PACKED  SetSensorReadingReq_T;


/* SetSensorReadingRes_T */
typedef struct
{
    INT8U   CompletionCode;

} PACKED  SetSensorReadingRes_T;



#pragma pack( )

#endif /* IPMI_SENSOR_H */

