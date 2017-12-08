#ifndef __LIBIPMI_SENSOR_DEVICE_H__
#define __LIBIPMI_SENSOR_DEVICE_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"
#include "libsensor.h"

/* command specific header files */
#pragma pack(1)
#include "IPMI_Sensor.h"
#pragma pack()
#include "IPMI_SensorEvent.h"
#include "IPMI_AMI.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* 35.2 Get Device SDR Info Command */
LIBIPMI_API uint16 IPMICMD_GetSDRInfo( IPMI20_SESSION_T *pSession,
                                       uint8 *pReqGetSDRInfo,
                                       GetSDRInfoRes_T *pResGetSDRInfo,
                                       int timeout);
    
/* 35.3 Get Device SDR Command */
LIBIPMI_API uint16 IPMICMD_GetDevSDR( IPMI20_SESSION_T *pSession,
                                      GetDevSDRReq_T *pReqDevSDR,
                                      GetDevSDRRes_T *pResDevSDR,
                                      uint32 *pOutBuffLen,
                                      int timeout);

/* 35.4 Reserve Device SDR Repository Command */
LIBIPMI_API uint16 IPMICMD_ReserveDevSDR( IPMI20_SESSION_T *pSession,
                                          ReserveDevSDRRes_T *pResReserveDevSDR,
                                          int timeout);

/* 35.5 Get Sensor Reading Factors Command */
LIBIPMI_API uint16 IPMICMD_GetSensorReadingFactor( IPMI20_SESSION_T *pSession,
                                                   GetSensorReadingFactorReq_T *pReqGetSensorReadingFactor,
                                                   GetSensorReadingFactorRes_T *pResGetSensorReadingFactor,
                                                   int timeout);

/* 35.6 Set Sensor Hysteresis Command */
LIBIPMI_API uint16 IPMICMD_SetSensorHysterisis( IPMI20_SESSION_T *pSession,
                                                SetSensorHysterisisReq_T *pReqSetSensorHysterisis,
                                                SetSensorHysterisisRes_T *pResSetSensorHysterisis,
                                                int timeout);
/* 35.7 Get Sensor Hysteresis Command */
LIBIPMI_API uint16 IPMICMD_GetSensorHysterisis( IPMI20_SESSION_T *pSession,
                                                GetSensorHysterisisReq_T *pReqGetSensorHysterisis,
                                                GetSensorHysterisisRes_T *pResGetSensorHysterisis,
                                                int timeout);

/* 35.8 Set Sensor Thresholds Command */
LIBIPMI_API uint16 IPMICMD_SetSensorThreshold( IPMI20_SESSION_T *pSession,
                                               SetSensorThresholdReq_T *pReqSetSensorThreshold,
                                               SetSensorThresholdRes_T *pResSetSensorThreshold,
                                               int timeout);

/* 35.9 Get Sensor Thresholds Command */
LIBIPMI_API uint16 IPMICMD_GetSensorThreshold( IPMI20_SESSION_T *pSession,
                                               GetSensorThresholdReq_T *pReqGetSensorThreshold,
                                               GetSensorThresholdRes_T *pResGetSensorThreshold,
                                               int timeout);

/* 35.10 Set Sensor Event Enable Command */
LIBIPMI_API uint16 IPMICMD_SetSensorEventEnable( IPMI20_SESSION_T *pSession,
                                                 SetSensorEventEnableReq_T *pReqSetSensorEventEnable,
                                                 SetSensorEventEnableRes_T *pResSetSensorEventEnable,
                                                 int timeout);

/* 35.11 Get Sensor Event Enable Command */
LIBIPMI_API uint16 IPMICMD_GetSensorEventEnable( IPMI20_SESSION_T *pSession,
                                                 GetSensorEventEnableReq_T *pReqGetSensorEventEnable,
                                                 GetSensorEventEnableRes_T *pResGetSensorEventEnable,
                                                 int timeout);

/* 35.12 ReArm Sensor Events Command */
LIBIPMI_API uint16	IPMICMD_ReArmSensorEvents( IPMI20_SESSION_T *pSession,
                                                 ReArmSensorReq_T *pReArmSensorReq,
                                                 ReArmSensorRes_T *pReArmSensorRes,
                                                 int timeout);

/* 35.13 Get Sensor Event Status Command */
LIBIPMI_API uint16	IPMICMD_GetSensorEventStatus( IPMI20_SESSION_T *pSession,
                                                 GetSensorEventStatusReq_T *pReqGetSensorEventStatus,
                                                 GetSensorEventStatusRes_T *pResGetSensorEventStatus,
                                                 int timeout);

/* 35.14 Get Sensor Reading Command */
LIBIPMI_API uint16 IPMICMD_GetSensorReading( IPMI20_SESSION_T *pSession,
                                             GetSensorReadingReq_T *pReqGetSensorReading,
                                             GetSensorReadingRes_T *pResGetSensorReading,
                                             int timeout);

/* 35.15 Set Sensor Type Command */
LIBIPMI_API uint16 IPMICMD_SetSensorType( IPMI20_SESSION_T *pSession,
                                          SetSensorTypeReq_T *pReqSetSensorType,
                                          SetSensorTypeRes_T *pResSetSensorType,
                                          int timeout);

/* 35.16 Get Sensor Type Command */
LIBIPMI_API uint16 IPMICMD_GetSensorType( IPMI20_SESSION_T *pSession,
                                          GetSensorTypeReq_T *pReqGetSensorType,
                                          GetSensorTypeRes_T *pResGetSensorType,
                                          int timeout);
LIBIPMI_API uint16 IPMICMD_SetSensorReading( IPMI20_SESSION_T *pSession,
                                           SetSensorReadingReq_T *pReqSetSensorReading,
                                           SetSensorReadingRes_T *pResSetSensorReading,
                                           int timeout);


/*------- Structure definitions and defines for HL sensor functions ---------*/
#pragma pack( 1 )
struct sensor_info
{
    /* SDR Entry */
    u8 sdr_buffer[ 64 ];      

    /*! Sensor description as a null terminated string */
    char description[ 17 ]; 

    /*! True if this sensor returns a discrete state */
    bool discrete_sensor;

    /*! True if this sensor uses analog thresholds */
    bool thresholds;

    float low_non_recov_thresh;    /*!< Low non-recoverable threshold */
    float low_crit_thresh;         /*!< Low critical threshold */
    float low_non_crit_thresh;     /*!< Low non-critical threshold */
    float high_non_crit_thresh;    /*!< High non-critical threshold */
    float high_crit_thresh;        /*!< High critical threshold */
    float high_non_recov_thresh;   /*!< High non-recoverable threshold */
};

/* IPMI threshold state definitions for monitoring */
#define THRESH_UNINITIALIZED        ( (u16)0x00 )
#define THRESH_NORMAL               ( (u16)0x01 )
#define THRESH_UP_NONCRIT           ( (u16)0x02 )
#define THRESH_UP_CRITICAL          ( (u16)0x04 )
#define THRESH_LOW_NONCRIT          ( (u16)0x08 )
#define THRESH_LOW_CRITICAL         ( (u16)0x10 )
#define THRESH_ACCESS_FAILED        ( (u16)0x20 )
#define THRESH_UP_NON_RECOV         ( (u16)0x40 )
#define THRESH_LOW_NON_RECOV        ( (u16)0x80 )

#define SENSOR_NOT_AVAILABLE        0xD5
#define UNABLE_TO_READ_SENSOR       0x20
#define SENSOR_SCANNING_BIT             0x40

#pragma pack()


/*------------------------- HL sensor functions -----------------------------*/
LIBIPMI_API uint16
LIBIPMI_HL_ReadSensorFromSDR( IPMI20_SESSION_T *pSession, uint8 *sdr_buffer,
                              uint8 *raw_reading, float *reading,
                              uint8 *discrete, int timeout );

LIBIPMI_API uint16
LIBIPMI_HL_LoadSensorSDRs( IPMI20_SESSION_T *pSession, uint8 **sdr_buffer, int *count, int timeout );

LIBIPMI_API uint16
LIBIPMI_HL_GetSensorCount( IPMI20_SESSION_T *pSession, int *sdr_count, int timeout );

LIBIPMI_API uint16
LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
                                 struct sensor_data *sensor_list, uint32* nNumSensor,int timeout );

#ifdef  __cplusplus
}
#endif

#endif





