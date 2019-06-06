/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2008, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* Filename: libipmi_sensor.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dbgout.h"



#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_sensor.h"
#include "libipmi_AppDevice.h"
#include "libipmi_AMIOEM.h"

#if 0
/* A bunch of defines that SDRRecord needs.  We don't care about the actual */
/* values, since we don't really use them, but I pulled these from the      */
/* Rules.make for the IPMI stack                                            */
#define MAX_NUM_USERS           ( 20 )
#define NUM_USER_PER_CHANNEL    ( 10 )
#define SDR_DEVICE_SIZE         ( 3 )
#include "SDRRecord.h"
#endif

#ifdef __GNUC__
/** \brief gcc method for designating a packed struct */
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#pragma pack( 1 )
#endif

#include "IPMI_SDRRecord.h"

#undef PACKED
#ifndef __GNUC__
#pragma pack( )
#endif


#include "IPMI_SensorEvent.h"

#include "sensor_helpers.h"

/* 35.2 Get Device SDR Info Command */
uint16	IPMICMD_GetSDRInfo( IPMI20_SESSION_T *pSession,
							uint8	*pReqGetSDRInfo,
							GetSDRInfoRes_T *pResGetSDRInfo,
							int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = MAX_RESPONSE_SIZE;
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_DEV_SDR_INFO,
											(uint8*)pReqGetSDRInfo, sizeof(uint8),
											(uint8 *)pResGetSDRInfo, &dwResLen,
											timeout);

	return wRet;
}

/* 35.3 Get Device SDR Command */
LIBIPMI_API uint16	IPMICMD_GetDevSDR( IPMI20_SESSION_T *pSession,
										GetDevSDRReq_T	*pReqDevSDR,
										GetDevSDRRes_T	*pResDevSDR,
										uint32			*pOutBuffLen,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetDevSDRRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_DEV_SDR,
											(uint8*)pReqDevSDR, sizeof(GetDevSDRReq_T),
											(uint8 *)pResDevSDR, &dwResLen,
											timeout);

	*pOutBuffLen = dwResLen;

	return wRet;
}

/* 35.4 Reserve Device SDR Repository Command */
LIBIPMI_API uint16	IPMICMD_ReserveDevSDR( IPMI20_SESSION_T *pSession,
										ReserveDevSDRRes_T *pResReserveDevSDR,
										int timeout)
{
	char	Req[20];
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(ReserveDevSDRRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_RESERVE_DEV_SDR_REPOSITORY,
											(uint8*)Req, 0,
											(uint8 *)pResReserveDevSDR, &dwResLen,
											timeout);

	return wRet;
}

/* 35.5 Get Sensor Reading Factors Command */
LIBIPMI_API uint16	IPMICMD_GetSensorReadingFactor( IPMI20_SESSION_T *pSession,
										GetSensorReadingFactorReq_T	*pReqGetSensorReadingFactor,
										GetSensorReadingFactorRes_T *pResGetSensorReadingFactor,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorReadingFactorRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_READING_FACTORS,
											(uint8*)pReqGetSensorReadingFactor, sizeof(GetSensorReadingFactorReq_T),
											(uint8 *)pResGetSensorReadingFactor, &dwResLen,
											timeout);

	return wRet;
}

/* 35.6 Set Sensor Hysteresis Command */
LIBIPMI_API uint16	IPMICMD_SetSensorHysterisis( IPMI20_SESSION_T *pSession,
										SetSensorHysterisisReq_T *pReqSetSensorHysterisis,
										SetSensorHysterisisRes_T *pResSetSensorHysterisis,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetSensorHysterisisRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_SET_SENSOR_HYSTERISIS,
											(uint8*)pReqSetSensorHysterisis, sizeof(SetSensorHysterisisReq_T),
											(uint8 *)pResSetSensorHysterisis, &dwResLen,
											timeout);

	return wRet;
}

/* 35.7 Get Sensor Hysteresis Command */
LIBIPMI_API uint16	IPMICMD_GetSensorHysterisis( IPMI20_SESSION_T *pSession,
										GetSensorHysterisisReq_T *pReqGetSensorHysterisis,
										GetSensorHysterisisRes_T *pResGetSensorHysterisis,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorHysterisisRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_HYSTERISIS,
											(uint8*)pReqGetSensorHysterisis, sizeof(GetSensorHysterisisReq_T),
											(uint8 *)pResGetSensorHysterisis, &dwResLen,
											timeout);

	return wRet;
}

/* 35.8 Set Sensor Thresholds Command */
LIBIPMI_API uint16	IPMICMD_SetSensorThreshold( IPMI20_SESSION_T *pSession,
										SetSensorThresholdReq_T *pReqSetSensorThreshold,
										SetSensorThresholdRes_T *pResSetSensorThreshold,int OwnerLUN,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetSensorThresholdRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(NETFNLUN_IPMI_SENSOR|OwnerLUN),CMD_SET_SENSOR_THRESHOLDS,
											(uint8*)pReqSetSensorThreshold, sizeof(SetSensorThresholdReq_T),
											(uint8 *)pResSetSensorThreshold, &dwResLen,
											timeout);

	return wRet;
}

/* 35.9 Get Sensor Thresholds Command */
LIBIPMI_API uint16	IPMICMD_GetSensorThreshold( IPMI20_SESSION_T *pSession,
										GetSensorThresholdReq_T *pReqGetSensorThreshold,
										GetSensorThresholdRes_T *pResGetSensorThreshold,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorThresholdRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_THRESHOLDS,
											(uint8*)pReqGetSensorThreshold, sizeof(GetSensorThresholdReq_T),
											(uint8 *)pResGetSensorThreshold, &dwResLen,
											timeout);

	return wRet;
}

/* 35.10 Set Sensor Event Enable Command */
LIBIPMI_API uint16	IPMICMD_SetSensorEventEnable( IPMI20_SESSION_T *pSession,
										SetSensorEventEnableReq_T *pReqSetSensorEventEnable,
										SetSensorEventEnableRes_T *pResSetSensorEventEnable,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetSensorEventEnableRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_SET_SENSOR_EVENT_ENABLE,
											(uint8*)pReqSetSensorEventEnable, sizeof(SetSensorEventEnableReq_T),
											(uint8 *)pResSetSensorEventEnable, &dwResLen,
											timeout);

	return wRet;
}

/* 35.11 Get Sensor Event Enable Command */
LIBIPMI_API uint16	IPMICMD_GetSensorEventEnable( IPMI20_SESSION_T *pSession,
										GetSensorEventEnableReq_T *pReqGetSensorEventEnable,
										GetSensorEventEnableRes_T *pResGetSensorEventEnable,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorEventEnableRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_EVENT_ENABLE,
											(uint8*)pReqGetSensorEventEnable, sizeof(GetSensorEventEnableReq_T),
											(uint8 *)pResGetSensorEventEnable, &dwResLen,
											timeout);

	return wRet;
}


/* 35.12 ReArm Sensor Events Command */
LIBIPMI_API uint16	IPMICMD_ReArmSensorEvents( IPMI20_SESSION_T *pSession,
										ReArmSensorReq_T *pReArmSensorReq,
										ReArmSensorRes_T *pReArmSensorRes,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(ReArmSensorRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_REARM_SENSOR_EVENTS,
											(uint8*)pReArmSensorReq, sizeof(ReArmSensorReq_T),
											(uint8 *)pReArmSensorRes, &dwResLen,
											timeout);

	return wRet;
}


/* 35.13 Get Sensor Event Status Command */
LIBIPMI_API uint16	IPMICMD_GetSensorEventStatus( IPMI20_SESSION_T *pSession,
										GetSensorEventStatusReq_T *pReqGetSensorEventStatus,
										GetSensorEventStatusRes_T *pResGetSensorEventStatus,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorEventStatusRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_EVENT_STATUS,
											(uint8*)pReqGetSensorEventStatus, sizeof(GetSensorEventEnableReq_T),
											(uint8 *)pResGetSensorEventStatus, &dwResLen,
											timeout);

	return wRet;
}

/* 35.14 Get Sensor Reading Command */
LIBIPMI_API uint16	IPMICMD_GetSensorReading( IPMI20_SESSION_T *pSession,
										GetSensorReadingReq_T *pReqGetSensorReading,
										GetSensorReadingRes_T *pResGetSensorReading,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorReadingRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_READING,
											(uint8*)pReqGetSensorReading, sizeof(GetSensorReadingReq_T),
											(uint8 *)pResGetSensorReading, &dwResLen,
											timeout);

	return wRet;
}

/* 35.15 Set Sensor Type Command */
LIBIPMI_API uint16	IPMICMD_SetSensorType( IPMI20_SESSION_T *pSession,
										SetSensorTypeReq_T *pReqSetSensorType,
										SetSensorTypeRes_T *pResSetSensorType,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetSensorTypeRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_READING,
											(uint8*)pReqSetSensorType, sizeof(SetSensorTypeReq_T),
											(uint8 *)pResSetSensorType, &dwResLen,
											timeout);

	return wRet;
}


/* 35.16 Get Sensor Type Command */
LIBIPMI_API uint16	IPMICMD_GetSensorType( IPMI20_SESSION_T *pSession,
										GetSensorTypeReq_T *pReqGetSensorType,
										GetSensorTypeRes_T *pResGetSensorType,
										int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetSensorTypeRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											NETFNLUN_IPMI_SENSOR, CMD_GET_SENSOR_READING,
											(uint8*)pReqGetSensorType, sizeof(GetSensorTypeReq_T),
											(uint8 *)pResGetSensorType, &dwResLen,
											timeout);

	return wRet;
}
LIBIPMI_API uint16 IPMICMD_SetSensorReading( IPMI20_SESSION_T *pSession,
                                            SetSensorReadingReq_T *pReqSetSensorReading,
                                            SetSensorReadingRes_T *pResSetSensorReading,
                                            int timeout)
{
    uint16	wRet;
    uint32	dwResLen;

    dwResLen = sizeof(SetSensorReadingRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_SENSOR,CMD_SET_SENSOR_READING,
                                            (uint8*)pReqSetSensorReading, sizeof(SetSensorReadingReq_T),
                                            (uint8 *)pResSetSensorReading , &dwResLen,
                                             timeout);

    return wRet;
}



/* ------------------ High level functions ------------------ */
#define SDR_FILE_PATH   "/tmp/sdr_data"

LIBIPMI_API uint16
LIBIPMI_HL_ReadSensorFromSDR( IPMI20_SESSION_T *pSession, uint8 *sdr_buffer,
							  uint8 *raw_reading, double *reading,
							  uint8 *discrete, int timeout )
{
    SDRRecHdr_T *header = (SDRRecHdr_T *)sdr_buffer;
    BOOL SendMsg= FALSE;
    GetSensorReadingReq_T get_reading_req;
    GetSensorReadingRes_T get_reading_res;
    AMIGetChNumRes_T pAMIGetChNumRes;
    uint8 SendMsgReq[5];
    uint32 SendMsgReqLen = 0;
    uint8 record_type;
    uint16 wRet;

    *discrete = (u8)0;

    record_type = header->Type;
	if( record_type == 0x01 )
    {
        FullSensorRec_T *record;

        /* Determine if sensor is discrete, and pull out the sensor number */
        record = (FullSensorRec_T *)sdr_buffer;
        if( record->EventTypeCode > 0x01 )
            *discrete = record->EventTypeCode;
        if(record->OwnerID != 0x20)
        {
            wRet = IPMICMD_AMIGetChNum (pSession,&pAMIGetChNumRes,
                                                                    timeout);
            SendMsg = TRUE;
            SendMsgReq[0] = 0x40 | pAMIGetChNumRes.ChannelNum;
            SendMsgReq[1] = record->OwnerID;
            SendMsgReq[2] = NETFN_SENSOR;
            SendMsgReq[3] = CMD_GET_SENSOR_READING;
            SendMsgReq[4] = record->SensorNum;
            SendMsgReqLen = 5;
        }
        else
        {
            get_reading_req.SensorNum = record->SensorNum;
        }
    }
    else if( record_type == 0x02 )
    {
        CompactSensorRec_T *record;

        /* Determine if sensor is discrete, and pull out the sensor number */
        record = (CompactSensorRec_T *)sdr_buffer;
        if( record->EventTypeCode > 0x01 )
            *discrete = record->EventTypeCode;
        if(record->OwnerID != 0x20)
        {
            wRet = IPMICMD_AMIGetChNum (pSession,&pAMIGetChNumRes,
                                                                    timeout);
            SendMsg = TRUE;
            SendMsgReq[0] = 0x40 | pAMIGetChNumRes.ChannelNum;
            SendMsgReq[1] = record->OwnerID;
            SendMsgReq[2] = NETFN_SENSOR;
            SendMsgReq[3] = CMD_GET_SENSOR_READING;
            SendMsgReq[4] = record->SensorNum;
            SendMsgReqLen = 5;
        }
        else
        {
            get_reading_req.SensorNum = record->SensorNum;
        }
    }
    else
    {
        /* We only know how to read sensors with records */
        /* of type full or compact                       */
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_INVALID_SDR_ENTRY ) );
    }

    if(SendMsg == TRUE)
    {
        /*SendMessage Command to Get Sensor Reading when sensor Owner ID is not 0x20 */
        wRet = IPMICMD_SendMessage(pSession,SendMsgReq,(uint8 *)&get_reading_res,
                                                           SendMsgReqLen,timeout);

        if(get_reading_res.CompletionCode == CC_TIMEOUT)
        {
             get_reading_res.Flags &= 0x20;
        }
        else if( wRet != LIBIPMI_E_SUCCESS)
        {
            return( wRet );
        }

    }
    else
    {
        /* Get the sensor reading */
        wRet = IPMICMD_GetSensorReading( pSession, &get_reading_req,
                                     &get_reading_res, timeout );
        if( wRet != LIBIPMI_E_SUCCESS )
        {
            //TWARN("falied when readinf sensor no %d\n",get_reading_req.SensorNum);
            return( wRet );
        }
    }
    /* Pass the raw reading out to the caller. */
    /* We need this for discrete sensors.      */
    *raw_reading = get_reading_res.SensorReading;

    /* If sensor scanning is disabled or the reading is unavailable... */
    if( !( get_reading_res.Flags & 0x40 )  ||  ( get_reading_res.Flags & 0x20) )
    {
        /* Return an error code to let the caller know */
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_SENSOR_DISABLED ) );
    }

    if( *discrete )
    {
        /* Pass the discrete state information back in the reading variable */
        *reading = (double)( get_reading_res.ComparisonStatus |
                            ( get_reading_res.OptionalStatus << 8 ) );
    }
    else
    {
        /* Convert raw sensor reading to final form */
        ipmi_convert_reading( sdr_buffer, *raw_reading, reading );
    }

    return( LIBIPMI_E_SUCCESS );
}


LIBIPMI_API uint16
LIBIPMI_HL_LoadSensorSDRs( IPMI20_SESSION_T *pSession, uint8 **sdr_buffer,
                           int *count, int timeout )
{
    FILE *sdrfile;
    uint8 *local_sdr_buffer = NULL;

    /* Open the SDR file */
    sdrfile = fopen( SDR_FILE_PATH, "r" );
    if( sdrfile == NULL )
    {
        /* If sdr file does not exist...    */
        if( errno == ENOENT )
        {
            /* ...create it */
            sdrfile = fopen( SDR_FILE_PATH, "w+" );
            if( sdrfile == NULL )
            {
                /* Uh-oh, something bad happened. */
                TWARN( "Cannot create SDR file: %s\n", strerror( errno ) );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            /* Read the SDRs from the BMC (Note: this function calls malloc) */
            *count = read_sensor_sdrs( pSession,
                                       (struct sensor_info **)&local_sdr_buffer,
                                       timeout );
            if( *count < 0 )
            {
                TWARN( "Cannot read SDRs from BMC: %s\n", strerror( errno ) );
                fclose( sdrfile );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            /* Write the SDR data to file */
            if( (signed)fwrite( local_sdr_buffer, (size_t)1,
                        sizeof( struct sensor_info ) * *count, sdrfile ) < 0 )
            {
                TWARN( "Cannot write to SDR file: %s\n", strerror( errno ) );
                fclose( sdrfile );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            fclose( sdrfile );
        }
        else
        {
            /* Uh-oh, something bad happened. */
            TWARN( "Cannot open SDR file: %s\n", strerror( errno ) );
            return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
        }
    }
    else
    {
        int retval;
        size_t filelen;

        /* Determine the length of the file */
        retval = fseek( sdrfile, 0, SEEK_END );
        if( retval < 0 )
        {
            TWARN( "Cannot seek SDR file: %s\n", strerror( errno ) );
            fclose( sdrfile );
            return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
        }
        filelen = (size_t)ftell( sdrfile );

        /* If the caller actually wants the sdrs... */
        if( sdr_buffer != NULL )
        {
            /* Allocate some memory for 'em, and copy them in */
            local_sdr_buffer = malloc( filelen );
            if( local_sdr_buffer == NULL )
            {
                TWARN( "Cannot allocate memory for SDR file: %s\n", strerror( errno ) );
                fclose( sdrfile );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ) );
            }

            /* Back to the beginning of the file (after seeking to the end above) */
            retval = fseek( sdrfile, 0, SEEK_SET );
            if( retval < 0 )
            {
                TWARN( "Cannot seek SDR file: %s\n", strerror( errno ) );
                fclose( sdrfile );
                free( local_sdr_buffer );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            /* Read the entire file into the buffer we allocated */
            if( fread( local_sdr_buffer, (size_t)1, filelen, sdrfile ) != filelen )
            {
                TWARN( "Cannot read from SDR file: %s\n", strerror( errno ) );
                fclose( sdrfile );
                free( local_sdr_buffer );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }
        }

        fclose( sdrfile );

        /* Size of sensor info structs is regular, so it's easy to */
        /* calculate the number of SDRs in the file                */
        *count = filelen / sizeof( struct sensor_info );
    }

    /* If sdr_buffer is not null, pass a pointer to the allocated */
    /* storage that contains the sdr data.                        */
    if( sdr_buffer != NULL )
        *sdr_buffer = local_sdr_buffer;
    else
    {
        if( local_sdr_buffer != NULL )
            free( local_sdr_buffer );
    }

    return( LIBIPMI_E_SUCCESS );
}


LIBIPMI_API uint16
LIBIPMI_HL_GetSensorCount( IPMI20_SESSION_T *pSession, int *sdr_count, int timeout )
{
    uint16 wRet;
    SDRRepositoryInfo_T pResSDRRepoInfo;

    wRet =  IPMICMD_GetSDRRepositoryInfo( pSession,
                                                        &pResSDRRepoInfo,
                                                        timeout);
    if(wRet != LIBIPMI_E_SUCCESS)
    {
        TWARN("Error in Getting Num of Sensor infon\n");
        return wRet;
    }

    *sdr_count = pResSDRRepoInfo.RecCt;
    return wRet;
}

#if 0
LIBIPMI_API uint16
LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
                                 struct sensor_data *sensor_list, int timeout )
{
    uint16 wRet;
    uint8 *sdr_buffer;
    int sdr_count;
    int i;
    GetSensorThresholdReq_T * pReqGetSensorThreshold;
    GetSensorThresholdRes_T * pResGetSensorThreshold;

    /* Load the SDRs related to sensors into memory */
    wRet = LIBIPMI_HL_LoadSensorSDRs( pSession, &sdr_buffer, &sdr_count, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        TWARN( "Cannot load sensor SDRs\n" );
        return( wRet );
    }

    /* For each SDR entry... */
    for( i = 0; i < sdr_count; i++ )
    {
        uint8 raw_reading;
        float reading;
        uint8 discrete;
        struct sensor_info *sensor = &( ( (struct sensor_info *)sdr_buffer )[ i ] );
        FullSensorRec_T *record = (FullSensorRec_T *)( sensor->sdr_buffer );

        /* Get the reading from this sensor */
        wRet = LIBIPMI_HL_ReadSensorFromSDR( pSession, sensor->sdr_buffer,
                                             &raw_reading, &reading, &discrete, 15 );

        //assume sensor is accessible till we find out otherwise
        sensor_list[i].SensorAccessibleFlags = 0;

        if( wRet != LIBIPMI_E_SUCCESS )
        {
            if(GET_ERROR_CODE(wRet) == 0xD5)
            {
                //sensor is not accessbile
                TWARN("Sensor is not accessible..server maybe powered off\n");
                sensor_list[i].SensorAccessibleFlags = GET_ERROR_CODE(wRet);
            }
            else if(GET_ERROR_CODE(wRet) == OEMCC_SENSOR_DISABLED)
            {
                sensor_list[i].SensorAccessibleFlags = 0xD5;
            }
            raw_reading = 0x00;
            reading = 0x00;
            discrete = 0x00;
        }

        /*Get Updated Threshold Values*/
        pReqGetSensorThreshold = malloc(sizeof(GetSensorThresholdReq_T));
        if(NULL == pReqGetSensorThreshold)
            return -1;
        pResGetSensorThreshold = malloc(sizeof(GetSensorThresholdRes_T));
        if(NULL == pResGetSensorThreshold)
        {
            free(pReqGetSensorThreshold);
            return -1;
        }
        memset(pResGetSensorThreshold, 0, sizeof(GetSensorThresholdRes_T));

        pReqGetSensorThreshold->SensorNum = record->SensorNum;
        wRet = IPMICMD_GetSensorThreshold(pSession, pReqGetSensorThreshold,
                                                                pResGetSensorThreshold, 15);

        if( wRet == LIBIPMI_E_SUCCESS )
        {
            record->UpperNonRecoverable = pResGetSensorThreshold->UpperNonRecoverable;
            record->UpperNonCritical = pResGetSensorThreshold->UpperNonCritical;
            record->UpperCritical= pResGetSensorThreshold->UpperCritical;
            record->LowerNonRecoverable  = pResGetSensorThreshold->LowerNonRecoverable;
            record->LowerNonCritical = pResGetSensorThreshold->LowerNonCritical;
            record->LowerCritical = pResGetSensorThreshold->LowerCritical;
        }

        /* Copy the relevant sensor data into caller's array */
        strcpy( sensor_list[ i ].sensor_name, sensor->description );
        sensor_list[ i ].sensor_num = record->SensorNum;
        sensor_list[ i ].owner_id = record->OwnerID;
        sensor_list[ i ].owner_lun = record->OwnerLUN;
        sensor_list[ i ].sensor_type = record->SensorType;
        sensor_list[ i ].sensor_units[ 0 ] = record->Units1;
        sensor_list[ i ].sensor_units[ 1 ] = record->Units2;
        sensor_list[ i ].sensor_units[ 2 ] = record->Units3;
        sensor_list[ i ].sensor_reading = reading;
        sensor_list[ i ].raw_reading = raw_reading;

        /* If we have a discrete sensor... */
        if( discrete )
        {
            /* ...copy just the discrete state */
            sensor_list[ i ].sensor_state = 0;
            sensor_list[ i ].discrete_state = discrete;
        }
        else
        {
            int retval;

            /* ...if we have thresholds, get the threshold state and copy */
            /* all the threshold information to the caller's array.       */
            retval = GetSensorState( pSession, raw_reading, sensor->sdr_buffer, & (sensor_list[ i ].sensor_state) , 15);
            if( retval < 0 )
                TWARN( "get_sdr_sensor_state failed\n" );
            sensor_list[ i ].discrete_state = 0;
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->LowerNonRecoverable,
            &sensor_list[ i ].low_non_recov_thresh);
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->LowerCritical,
            &sensor_list[ i ].low_crit_thresh);
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->LowerNonCritical,
            &sensor_list[ i ].low_non_crit_thresh);
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->UpperNonCritical,
            &sensor_list[ i ].high_non_crit_thresh);
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->UpperCritical,
            &sensor_list[ i ].high_crit_thresh);
        ipmi_convert_reading(sensor->sdr_buffer,
            pResGetSensorThreshold->UpperNonRecoverable,
            &sensor_list[ i ].high_non_recov_thresh);
        }
        free(pReqGetSensorThreshold);
        free(pResGetSensorThreshold);
    }

    /* The sdr list was allocated by sdr_load, and we must free it */
    free( sdr_buffer );
    return( 0 );
}
#endif

/**
 * @fn LIBIPMI_HL_GetAllSensorReadings
 * @brief High Level API to get all the sensor Info
 * @params Session, sensor_list, nNumSensor, timout
 * @return proper completion code on success
 * nNumSensor This argument is used for letting know this API that for much sensor record the memory allocated by caller
 * So that it will not lead to memory corruption during SDR FILL/ERASE is in progress. 
 * The caller will make use of this value in oder to know the valid records in sensor_list
 */

LIBIPMI_API uint16
LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
                                 struct sensor_data *sensor_list, uint32* nNumSensor,int timeout )
{
    uint16 wRet;
    int sdr_count;
    int i ;
    SenInfo_T * pSensorBuff = NULL;
    unsigned char* pSensor = NULL;

    uint32 num = 0;

    /*Expects the number of record value from caller for which it would have allocated memory*/
    num = *nNumSensor;
    /* Load the SDRs related to sensors into memory */
    wRet = LIBIPMI_HL_GetSensorCount( pSession,&sdr_count, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        TWARN( "Cannot load sensor SDRs\n" );
        return( wRet );
    }

    pSensorBuff = (SenInfo_T*) malloc(sdr_count* sizeof(SenInfo_T));
    if(pSensorBuff == NULL)
    {
        TWARN("Error in Allocating the Memory\n");
        return -1;
    }
    memset(pSensorBuff, 0, sdr_count* sizeof(SenInfo_T));
    pSensor = (unsigned char *)pSensorBuff;

    *nNumSensor = sdr_count;
    wRet = LIBIPMI_HL_AMIGetSensorInfo(pSession, pSensorBuff, nNumSensor, timeout);
    if ( (wRet & ~(1<<8)) != LIBIPMI_E_SUCCESS )
    {
        free(pSensorBuff);
        if( wRet == LIBIPMI_E_SENSOR_INFO_EMPTY)
        {
            TINFO("Sensor Info is empty\n");
        }
        else
        {
            TWARN("Error in getting sensor info\n");
        }
        return (wRet);
    }

    if(num < *nNumSensor)
        *nNumSensor = num;

    for(i = 0; i < (signed)*nNumSensor; i++)
    {
        sensor_list[ i ].sensor_num = pSensorBuff->SensorNumber;
        sensor_list[ i ].owner_id = pSensorBuff->OwnerID;
        sensor_list[ i ].owner_lun = pSensorBuff->OwnerLUN;
        sensor_list[ i ].sensor_type = pSensorBuff->SensorTypeCode;
        sensor_list[ i ].sensor_units[ 0 ] = pSensorBuff->Units1;
        sensor_list[ i ].sensor_units[ 1 ] = pSensorBuff->Units2;
        sensor_list[ i ].sensor_units[ 2 ] = pSensorBuff->Units3;
        sensor_list[ i ].raw_reading = pSensorBuff->SensorReading;
		sensor_list[ i ].sdr_type = pSensorBuff->hdr.Type;//Quanta+
        strncpy(sensor_list[ i ].sensor_name,pSensorBuff->SensorName,sizeof(pSensorBuff->SensorName));
        sensor_list[i].SensorAccessibleFlags = 0;

        if(pSensorBuff->EventTypeCode > 1)
        {
            /*Updating the discrete sensor details*/
            sensor_list[i].discrete_state = pSensorBuff->EventTypeCode;
            sensor_list[i].sensor_state = 0;
            sensor_list[ i ].sensor_reading = (double)(pSensorBuff->ComparisonStatus | (pSensorBuff->OptionalStatus << 8));
        }
        else
        {
            sensor_list[ i ].discrete_state = 0;
            sensor_list[ i ].Settable_Readable_ThreshMask=pSensorBuff->Settable_Readable_ThreshMask;

            if(pSensorBuff->SensorReading>0)
            /*Convert Sensor Reading*/
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->SensorReading, &sensor_list[i].sensor_reading, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert LowerNonRecoverable*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT2)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerNonRecoverable, &sensor_list[i].low_non_recov_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert LowerCritical*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT1)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerCritical, &sensor_list[i].low_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert LowerNonCritical*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT0)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerNonCritical, &sensor_list[i].low_non_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert UpperNonCritical*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT3)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperNonCritical, &sensor_list[i].high_non_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert UpperCritical*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT4)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperCritical, &sensor_list[i].high_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Convert UpperNonRecoverable*/
            if(sensor_list[ i ].Settable_Readable_ThreshMask & BIT5)
            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperNonRecoverable, &sensor_list[i].high_non_recov_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1,
                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);

            /*Get the Sensor state*/
            SensorState(pSensorBuff->SensorReading,&sensor_list[i].sensor_state,pSensorBuff->hdr.Type,pSensorBuff->Linearization,pSensorBuff->Units1,pSensorBuff->EventTypeCode,pSensorBuff->AssertionEventByte1,pSensorBuff->AssertionEventByte2,pSensorBuff->DeassertionEventByte1,pSensorBuff->DeassertionEventByte2);
        }

        /* If sensor scanning is disabled or the reading is unavailable... */
        if( !( pSensorBuff->Flags & SENSOR_SCANNING_BIT)  || (pSensorBuff->Flags & UNABLE_TO_READ_SENSOR) )
        {
            /* Return an error code to let the caller know */
            sensor_list[i].SensorAccessibleFlags = SENSOR_NOT_AVAILABLE;
            sensor_list[i].sensor_reading = 0;
            sensor_list[i].discrete_state = 0;
            sensor_list[i].raw_reading = 0;
        }
        pSensorBuff++;
    }
    free(pSensor);
    return wRet ;
}
