#ifndef __LIBIPMI_IPMB_SENSOR_DEVICE_H__
#define __LIBIPMI_IPMB_SENSOR_DEVICE_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"
#include "libsensor.h"

#include "libipmi_sdr.h"


/* command specific header files */
#pragma pack(1)
#include "IPMI_Sensor.h"
#pragma pack()
#include "IPMI_SensorEvent.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Think */
LIBIPMI_API uint16 IPMICMD_IPMB_SendMsg(IPMI20_SESSION_T *pSession, uint8 *pReq, int szie, int timeout);
LIBIPMI_API uint16 IPMICMD_IPMB_GetMsg(IPMI20_SESSION_T *pSession, uint8 *pReq, uint32 *szie, int timeout);


typedef struct _IPMBHeader
{
    /* The target slave address is supplied by the driver here */
    uint8 CompletionCode;
    uint8 resSlaveAddr;
    uint8 netFnTargetLUN;
	uint8 Checksum1;
	uint8 reqSlaveAddr;
	uint8 reqLUN;
	uint8 Command;
} PACKED SendMsgHeader;


/*------------------------- HL sensor functions -----------------------------*/
LIBIPMI_API uint16 LIBIPMI_HL_IPMB_SendMsg( IPMI20_SESSION_T *pSession, uint8 slave, uint8 NetFnLUN, uint8 rqLun, uint8 cmd,
									 		uint8 *rqdata, size_t rqdata_len,
											uint8 *rsdata, uint32 *rsdata_len, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSensorCount( IPMI20_SESSION_T *pSession, int *sdr_count, uint8 I2CAddress, int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_ReserveSDRRepository( IPMI20_SESSION_T *pSession, ReserveSDRRepositoryRes_T *pResReserveSDRRepository, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSDRRepositoryAllocInfo( IPMI20_SESSION_T *pSession, SDRRepositoryAllocInfo_T *pResGetSDRRepositoryAllocInfo, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSDR( IPMI20_SESSION_T *pSession, GetSDRReq_T *pReqGetSDR, uint8 *pResGetSDR, uint32 *pdwOutBuffLen, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSensorReading( IPMI20_SESSION_T *pSession,
											GetSensorReadingReq_T *pReqGetSensorReading,
											uint8 *pResGetSensorReading,
											uint8 I2CAddress, int timeout);


LIBIPMI_API uint16 LIBIPMI_HL_IPMB_ReadSensor( IPMI20_SESSION_T *pSession, uint8 *sdr_buffer, uint8 *raw_reading, float *reading, uint8 *discrete, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSensorData(IPMI20_SESSION_T *pSession,uint8 *sdr_buffer, int SDRCount, int MaxSDRLen, uint16 reservation_id, uint8 I2CAddress, int timeout);

int IPMB_read_sensor_sdrs( IPMI20_SESSION_T *pSession, struct sensor_info **sensor_list, int SDRCount, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_LoadSensorSDRs( IPMI20_SESSION_T *pSession, uint8 **sdr_buffer, int *SDRCount, uint8 I2CAddress, int timeout);

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetAllSensorReadings( IPMI20_SESSION_T *pSession, struct sensor_data *sensor_list, uint8 I2CAddress, int *nSensors, int timeout );


/*@external@*/extern int
extract_sdr_id( u8 *sdr_buffer, /*@out@*/char *description )
/*@modifies *description@*/;

#ifdef  __cplusplus
}
#endif

#endif
