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
* Filename: libipmi_ipmb_sensor.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dbgout.h>


#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "sensor_helpers.h"
#include "libipmi_ipmb_sensor.h"

#include "libipmi_sdr.h"
#include "libipmi_StorDevice.h"
#include "IPMBIfc.h"


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




/**************************************************************************************/
LIBIPMI_API uint16
IPMICMD_IPMB_SendMsg( IPMI20_SESSION_T *pSession, uint8 *pReq, int size, int timeout)
{
    uint16	wRet;
    uint32	dwResLen;
	SendMsgRes_T pRes;

    dwResLen = sizeof(pRes);
    
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_APP, CMD_SEND_MSG,
											(uint8*)pReq, size,
                                            (uint8*)&pRes, &dwResLen,
                                            timeout);
    return wRet;
}
/**************************************************************************************/
LIBIPMI_API uint16
IPMICMD_IPMB_GetMsg( IPMI20_SESSION_T *pSession, uint8 *pRes, uint32 *size, int timeout)
{
    uint16	wRet;
    SendMsgReq_T pReq;
    
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_APP, CMD_GET_MSG,
											(uint8*)&pReq, 0,
                                            (uint8*)pRes, size,
                                            timeout);
    return wRet;
}
/**************************************************************************************/

LIBIPMI_API uint16
LIBIPMI_HL_IPMB_SendMsg( IPMI20_SESSION_T *pSession, uint8 slave, uint8 NetFnLUN, uint8 rqLun, uint8 cmd,
						  uint8 *rqdata, size_t rqdata_len,
						  uint8 *rsdata, uint32 *rsdata_len, int timeout)
{
    SendMsgHeader* SendMsg;
    uint8  *pReq;
    uint8  chksum;
	uint16 i,j;
	uint16 wRet;
	uint32 dwResLen;
	uint8  *rstmp;

    rstmp = malloc(0xff);
    usleep(100000);	wRet = IPMICMD_IPMB_GetMsg( pSession, rstmp, &dwResLen, timeout );
    usleep(100000);	wRet = IPMICMD_IPMB_GetMsg( pSession, rstmp, &dwResLen, timeout );
    usleep(50000);	wRet = IPMICMD_IPMB_GetMsg( pSession, rstmp, &dwResLen, timeout );
	usleep(25000);	wRet = IPMICMD_IPMB_GetMsg( pSession, rstmp, &dwResLen, timeout );

    pReq = malloc(sizeof(SendMsgHeader) + rqdata_len + 1);
    
    SendMsg = (SendMsgHeader *)pReq;
    
    // {0x00, 0x20, 0x28, 0xb8, 0x20, 0x01, 0x20, 0xbf};  
    SendMsg->CompletionCode = 0x00;
    SendMsg->resSlaveAddr	= slave;
    SendMsg->netFnTargetLUN = NetFnLUN;
    SendMsg->Checksum1		= 0x100 - SendMsg->resSlaveAddr - SendMsg->netFnTargetLUN;
    SendMsg->reqSlaveAddr	= BMC_SLAVE_ADDRESS;
    SendMsg->reqLUN			= rqLun;
    SendMsg->Command		= cmd;
	memcpy(pReq+sizeof(SendMsgHeader), rqdata, rqdata_len);
	
	chksum = 0;
	for (i=4; i<sizeof(SendMsgHeader) + rqdata_len; i++) {
		chksum += *(pReq + i);
	//	printf("[%d] %02x %x\n", i, *(pReq+i), chksum);
	}
	*(pReq+i) = 0x100-chksum;


	j = 1;
	while (j--)
	{
		/* Send Message Request */
	    wRet = IPMICMD_IPMB_SendMsg( pSession, pReq, sizeof(SendMsgHeader)+rqdata_len+1, timeout );
		if (wRet != LIBIPMI_E_SUCCESS) {
			TCRIT ("IPMICMD_IPMB_SendMsg ... failed\n");
			continue;
		}

		// Get Message Response information
		i = 200;
		while (i--)
		{
			usleep(100000);
			dwResLen = *rsdata_len;
			wRet = IPMICMD_IPMB_GetMsg( pSession, rsdata, &dwResLen, timeout );
			if (wRet != LIBIPMI_E_SUCCESS) {
			//	TCRIT ("IPMICMD_IPMB_GetMsg ... failed\n");
				continue;
			} else {
				break;
			}
		}
		if (wRet == LIBIPMI_E_SUCCESS)
			break;
	}
	*rsdata_len = dwResLen;

	free( pReq );	
	free( rstmp );

    return( wRet );
}
/**************************************************************************************/

LIBIPMI_API uint16
LIBIPMI_HL_IPMB_GetSensorCount( IPMI20_SESSION_T *pSession, int *sdr_count, uint8 I2CAddress, int timeout )
{
	uint16 wRet;
	uint8 *rqMsg=0;
	uint8 *rsMsg;
	uint32 dwResLen;
	SDRRepositoryInfo_T repo_info;

    rsMsg = malloc(sizeof(SDRRepositoryInfo_T)+9);
   	dwResLen = sizeof(SDRRepositoryInfo_T)+7;


	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_STORAGE, 0x01,
									CMD_GET_SDR_REPOSITORY_INFO,
									rqMsg, 0,
									rsMsg, &dwResLen, timeout);

	memcpy(&repo_info, rsMsg+7, sizeof(SDRRepositoryInfo_T));

    /* Determine total number of SDRs stored */
    *sdr_count = ipmitoh_u16( repo_info.RecCt );
    
    free( rsMsg );
    return( wRet );
}
/**************************************************************************************/
/* Get SDR Repository Allocation Info Command */
LIBIPMI_API uint16 
LIBIPMI_HL_IPMB_GetSDRRepositoryAllocInfo( IPMI20_SESSION_T *pSession, 
											SDRRepositoryAllocInfo_T *pResGetSDRRepositoryAllocInfo,
											uint8 I2CAddress, int timeout)
{
	uint16 wRet;
	uint8 *rqMsg=0;
	uint8 *rsMsg;
	uint32 dwResLen;

    rsMsg = malloc(sizeof(SDRRepositoryAllocInfo_T)+9);
	dwResLen = sizeof(SDRRepositoryAllocInfo_T)+7;
	
	
	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_STORAGE, 0x01,
									CMD_GET_SDR_REPOSITORY_ALLOCATION_INFO,
									rqMsg, 0,
									rsMsg, &dwResLen, timeout);
	
	memcpy(pResGetSDRRepositoryAllocInfo, rsMsg+7, sizeof(SDRRepositoryAllocInfo_T));

    
    free( rsMsg );
    return( wRet );
}
/**************************************************************************************/
/* Reserve SDR Repository Command */
LIBIPMI_API uint16
LIBIPMI_HL_IPMB_ReserveSDRRepository( IPMI20_SESSION_T *pSession, 
										ReserveSDRRepositoryRes_T *pResReserveSDRRepository,
										uint8 I2CAddress, int timeout)
{
    uint16 wRet;
	uint8 *rqMsg=0;
	uint8 *rsMsg;
	uint32 dwResLen;

    rsMsg = malloc(sizeof(ReserveSDRRepositoryRes_T)+9);
	dwResLen = sizeof(ReserveSDRRepositoryRes_T)+7;
	
	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_STORAGE, 0x01,
									CMD_RESERVE_SDR_REPOSITORY,
									rqMsg, 0,
									rsMsg, &dwResLen, timeout);
	
	memcpy(pResReserveSDRRepository, rsMsg+7, sizeof(ReserveSDRRepositoryRes_T));

    
    free( rsMsg );
    return( wRet );
}
/**************************************************************************************/
/* Get SDR Command */
LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSDR( IPMI20_SESSION_T *pSession,
											GetSDRReq_T *pReqGetSDR,
											uint8 		*pResGetSDR,
											uint32      *pdwOutBuffLen,
											uint8 		I2CAddress,  int timeout)
{
	GetSDRRes_T pGetSDRRes;
	SDRRecHdr_T pSDRRecHdr;
    uint16		wRet;
	uint8		*rsMsg;
	uint8		nLen;
	uint32		dwResLen;
//	int			i;
	
	/****** 
	00 00 2d b3 20 00 23 							IPMB Head
	00 03 00 										GetSDRRes_T
	02 00 51 01 35 									SDRRecHdr_T
	20 00 01 07 01 7f d8 01 01 cf 33 cf 33 1b 1b 00	SDR Data
	01 00 00 01 00 00 00 00 00 07 1e 32 14 ff 00 ff
	23 1e 00 15 19 01 01 00 00 00 ca 4c 6f 63 61 6c
	20 54 65 6d 70 
	2e 00 											IPMB tail
	*****/

    rsMsg = malloc(*pdwOutBuffLen+9);
    memset(rsMsg, '\0', *pdwOutBuffLen+9);
    
	pReqGetSDR->Offset = 0;
	pReqGetSDR->Size = 5;

  	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_STORAGE, 0x01,
  									CMD_GET_SDR,
  									(uint8 *)pReqGetSDR, sizeof(GetSDRReq_T),
  									(uint8 *)rsMsg, &dwResLen, timeout);
	
	memcpy(&pGetSDRRes, rsMsg+7						, sizeof(GetSDRRes_T));
	memcpy(&pSDRRecHdr, rsMsg+7+sizeof(GetSDRRes_T)	, sizeof(SDRRecHdr_T));
	memcpy(pResGetSDR , rsMsg+7, dwResLen-7-2);
	
	*pdwOutBuffLen = pSDRRecHdr.Len + sizeof(GetSDRRes_T) + sizeof(SDRRecHdr_T);

	printf("pdwOutBuffLen:[%ld]\n\n", *pdwOutBuffLen);
	printf("GetSDRRes.CompletionCode:[%x]\n", pGetSDRRes.CompletionCode );
	printf("GetSDRRes.NextRecID     :[%x]\n", pGetSDRRes.NextRecID	 	);
	printf("SDRRecHdr.ID            :[%x]\n", pSDRRecHdr.ID			    );
	printf("SDRRecHdr.Version       :[%x]\n", pSDRRecHdr.Version		);
	printf("SDRRecHdr.Type          :[%x]\n", pSDRRecHdr.Type			);
	printf("SDRRecHdr.Len           :[%x]\n", pSDRRecHdr.Len			);

	nLen = 0;
	for (; nLen<pSDRRecHdr.Len; )
	{
		pReqGetSDR->Offset += pReqGetSDR->Size;	// Each time GetSDRRes_T will response
		if (pSDRRecHdr.Len-nLen > 25)			//Because MAX_SDR_LEN is 5
			pReqGetSDR->Size = 25;
		else 
			pReqGetSDR->Size = pSDRRecHdr.Len-nLen;

	  	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_STORAGE, 0x01,
	  									CMD_GET_SDR,
	  									(uint8 *)pReqGetSDR, sizeof(GetSDRReq_T),
	  									(uint8 *)rsMsg, &dwResLen, timeout);

		dwResLen = dwResLen-7-3-2; // Head and GetSDRRes_T and Tail
		memcpy(pResGetSDR+nLen+sizeof(GetSDRRes_T)+sizeof(SDRRecHdr_T)
				, rsMsg+7+3, dwResLen);

		nLen += dwResLen;

		printf("pReqGetSDR->Offset[%x]\n", pReqGetSDR->Offset);
		printf("pReqGetSDR->Size  [%x]\n", pReqGetSDR->Size  );
		printf("dwResLen          [%lx]\n", dwResLen);
		printf("nLen              [%x]\n" , nLen);

	}

/*
	printf("******* ");
	for (i=0;i<nLen+3+5;i++)
		printf("%02x ", *(pResGetSDR+i));
	printf("\n");
*/

    free( rsMsg );
    return( wRet );
}

/**************************************************************************************/
/* Get Sensor Reading Factors Command */
LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSensorReading( IPMI20_SESSION_T *pSession,
											GetSensorReadingReq_T *pReqGetSensorReading,
											uint8 *pResGetSensorReading,
											uint8 I2CAddress, int timeout)
{
	uint16 wRet;
	uint8 *rsMsg;
	uint32 dwResLen;
	

    rsMsg = malloc(sizeof(GetSensorReadingRes_T)+9);
	dwResLen = sizeof(GetSensorReadingRes_T)+7;

  	wRet = LIBIPMI_HL_IPMB_SendMsg( pSession, I2CAddress, NETFNLUN_IPMI_SENSOR, 0x01,
  									CMD_GET_SENSOR_READING,
									(uint8*)pReqGetSensorReading, sizeof(GetSensorReadingReq_T),
									(uint8*)rsMsg, &dwResLen, timeout);
  
	memcpy(pResGetSensorReading, rsMsg+7, sizeof(GetSensorReadingRes_T));

    free( rsMsg );
    return( wRet );
}

/**************************************************************************************/

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_ReadSensor( IPMI20_SESSION_T *pSession, uint8 *sdr_buffer,
										uint8 *raw_reading, double *reading,
										uint8 *discrete, uint8 I2CAddress, int timeout )
{
    SDRRecHdr_T *header = (SDRRecHdr_T *)sdr_buffer;
    GetSensorReadingReq_T get_reading_req;
    GetSensorReadingRes_T get_reading_res;
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
        get_reading_req.SensorNum = record->SensorNum;
    }
    else if( record_type == 0x02 )
    {
        CompactSensorRec_T *record;

        /* Determine if sensor is discrete, and pull out the sensor number */
        record = (CompactSensorRec_T *)sdr_buffer;
        if( record->EventTypeCode > 0x01 )
            *discrete = record->EventTypeCode;
        get_reading_req.SensorNum = record->SensorNum;
    }
    else
    {
        /* We only know how to read sensors with records */
        /* of type full or compact                       */
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_INVALID_SDR_ENTRY ) );
    }

   
    /* Get the sensor reading */
    wRet = LIBIPMI_HL_IPMB_GetSensorReading( pSession, &get_reading_req,
                                     (uint8 *)&get_reading_res, I2CAddress, timeout );
                                   

    if( wRet != LIBIPMI_E_SUCCESS )
	{
	//TWARN("falied when readinf sensor no %d\n",get_reading_req.SensorNum);
        return( wRet );
	}

    /* Pass the raw reading out to the caller. */
    /* We need this for discrete sensors.      */
    *raw_reading = get_reading_res.SensorReading;

    /* If sensor scanning is disabled... */
    if( !( get_reading_res.Flags & 0x40 ) )
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

/**************************************************************************************/

LIBIPMI_API uint16 LIBIPMI_HL_IPMB_GetSensorData(IPMI20_SESSION_T *pSession, uint8 *sdr_buffer, int SDRCount,
												 int MaxSDRLen, uint16 reservation_id, uint8 I2CAddress, int timeout)
{
	GetSDRRes_T *get_sdr_res;
	GetSDRReq_T  get_sdr_req;
	
    int i;
    int wRet = 0;
	uint8 *temp_buffer;
	uint16 record_id = 0;
	uint32 dwDataLen;
	
	
    temp_buffer = malloc( MaxSDRLen + sizeof( GetSDRRes_T ));
        
    if( temp_buffer == NULL )
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ));

	get_sdr_res = (GetSDRRes_T *)temp_buffer;

	for (i=0; i<SDRCount; i++)
    {
		get_sdr_req.ReservationID = htoipmi_u16(reservation_id);
		get_sdr_req.RecID = htoipmi_u16( record_id );
		get_sdr_req.Offset = 0;
		get_sdr_req.Size = 0xff;

		dwDataLen = MaxSDRLen + sizeof( GetSDRRes_T );


		wRet = LIBIPMI_HL_IPMB_GetSDR( pSession, &get_sdr_req, temp_buffer, &dwDataLen,  I2CAddress, timeout );

	    if (wRet != LIBIPMI_E_SUCCESS) {
	    	TCRIT("Error GetSDR Number[%d]\n", i);
	    	continue;
		}
		record_id = ipmitoh_u16( get_sdr_res->NextRecID );

	    memcpy(sdr_buffer+i*MaxSDRLen, temp_buffer+sizeof(GetSDRRes_T), dwDataLen );

	}
	free( temp_buffer );
	
	return wRet;
}
/**************************************************************************************/
#define SDR_FILE_PATH_EXTERNALBMC   "/tmp/sdr_data_ExtBMC"

int IPMB_read_sensor_sdrs( IPMI20_SESSION_T *pSession, struct sensor_info **sensor_list, 
						int SDRCount, uint8 I2CAddress, int timeout )
{
    uint16 wRet;
    SDRRepositoryAllocInfo_T	SDRAllocationInfo;
    ReserveSDRRepositoryRes_T	reservation;
    uint8 *sdr_buffer;
    struct sensor_info *sensor_buffer = NULL;
    struct sensor_info *sensor_buffer_temp;
	int MaxSDRLen = 0;
    int sensor_count = 0;
    int i;


	wRet = LIBIPMI_HL_IPMB_GetSDRRepositoryAllocInfo(pSession, &SDRAllocationInfo, I2CAddress, timeout);
	if( wRet != 0 )
	{
		TCRIT("Error getting SDR Repository Allocation Info\n");
		return wRet;
	}

    /* First get a reservation */
    wRet = LIBIPMI_HL_IPMB_ReserveSDRRepository( pSession, &reservation, I2CAddress, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
	{
		TCRIT("Error Reserve SDR Repository\n");
		return wRet;
	}

	/* Determine max SDR entry length */
	MaxSDRLen = SDRAllocationInfo.MaxRecSize * ipmitoh_u16( SDRAllocationInfo.AllocUnitSize );    

    /* Allocate some space to store the SDRs */
    sdr_buffer = malloc(MaxSDRLen * SDRCount);
    printf("buffer len is [%d] x [%d] = %d\n", SDRCount, MaxSDRLen, MaxSDRLen * SDRCount);
    if (sdr_buffer == NULL)
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ) );


	/* Read all the SDRs */
  	wRet = 	LIBIPMI_HL_IPMB_GetSensorData(pSession, sdr_buffer, SDRCount, MaxSDRLen,
  										 ipmitoh_u16(reservation.ReservationID), I2CAddress, timeout);
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        TWARN( "Failure getting all SDRs\n" );
        free( sdr_buffer );
        return( wRet );
    }


    /* For each SDR... */
    for( i = 0; i < SDRCount; i++ )
    {
        SDRRecHdr_T *header;

        header = (SDRRecHdr_T *)( sdr_buffer + ( i * MaxSDRLen ) );

        /* If the SDR is for a sensor... */
        if( ( header->Type == FULL_SDR_REC ) || ( header->Type == COMPACT_SDR_REC ) )
        {
            /* We've found another sensor */
            sensor_count++;

            /* Get more memory for this sensor entry */
            sensor_buffer_temp = realloc( sensor_buffer, sizeof( struct sensor_info ) * sensor_count );
            if( sensor_buffer_temp == NULL )
            {
                TWARN( "Cannot allocate memory for SDR entry, failing...\n" );
                if( sensor_buffer != NULL )
                    free( sensor_buffer );
                free( sdr_buffer );
                return( -1 );
            }
            sensor_buffer = sensor_buffer_temp;

            /* Copy the SDR data into the newly allocated structure */
            memcpy( &sensor_buffer[sensor_count-1].sdr_buffer,
                    ( sdr_buffer + ( i * MaxSDRLen ) ),
                    sizeof( SDRRecHdr_T ) + header->Len );

            /* Save sensor ID */
            if( extract_sdr_id( sensor_buffer[sensor_count-1].sdr_buffer,
                                sensor_buffer[sensor_count-1].description ) < 0 )
                sprintf( sensor_buffer[sensor_count-1].description, "Unknown" );

            /* Only full records contain sensor conversion data */
            if( header->Type == FULL_SDR_REC )
            {
                bool inverse_linear = false;
                FullSensorRec_T *record = (FullSensorRec_T *)( sdr_buffer + ( i * MaxSDRLen ) );
                double converted_threshold;

                if( record->Linearization == 7 )
                inverse_linear = true;

                /* Save Low Non-recoverable sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->LowerNonRecoverable, &converted_threshold );
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].high_non_recov_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].low_non_recov_thresh = converted_threshold;

                /* Save Low Critical sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->LowerCritical, &converted_threshold );
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].high_crit_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].low_crit_thresh = converted_threshold;

                /* Save Low Non-Critical sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->LowerNonCritical, &converted_threshold );
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].high_non_crit_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].low_non_crit_thresh = converted_threshold;

                /* Save Upper Non-Recoverable sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->UpperNonRecoverable, &converted_threshold );
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].low_non_recov_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].high_non_recov_thresh = converted_threshold;

                /* Save Upper Critical sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->UpperCritical, &converted_threshold );
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].low_crit_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].high_crit_thresh = converted_threshold;

                /* Save Upper Non-Critical sensor threshold */
                ipmi_convert_reading( (u8 *)record, record->UpperNonCritical, &converted_threshold );
                
                if( inverse_linear )
                    sensor_buffer[sensor_count-1].low_non_crit_thresh = converted_threshold;
                else
                    sensor_buffer[sensor_count-1].high_non_crit_thresh = converted_threshold;
            }
        }
    }

    /* We don't need the complete SDR list now that we've got the */
    /* SDRs that are sensor related.                              */
    free( sdr_buffer );

    /* Pass the pointer to the sensor data we allocated back */
    /* Freeing this memory is the caller's responsibility    */
    *sensor_list = sensor_buffer;

    return( sensor_count );
}

LIBIPMI_API uint16
LIBIPMI_HL_IPMB_LoadSensorSDRs( IPMI20_SESSION_T *pSession, uint8 **sdr_buffer, int *SDRCount, uint8 I2CAddress, int timeout )
{
	int count;
    FILE *sdrfile;
    uint8 *local_sdr_buffer = NULL;
    
    /* Open the SDR file */
    sdrfile = fopen( SDR_FILE_PATH_EXTERNALBMC, "r" );
    if( sdrfile == NULL )
    {
        /* If sdr file does not exist...    */
        if( errno == ENOENT )
        {
            /* ...create it */
            sdrfile = fopen( SDR_FILE_PATH_EXTERNALBMC, "w+" );
            if( sdrfile == NULL )
            {
                /* Uh-oh, something bad happened. */
                TWARN( "Cannot create SDR_ExtBMC file: %s\n", strerror( errno ) );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            /* Read the SDRs from the BMC (Note: this function calls malloc) */
            count = IPMB_read_sensor_sdrs( pSession,
                                       (struct sensor_info **)&local_sdr_buffer,
                                       *SDRCount, I2CAddress, timeout );
            if( count < 0 )
            {
                TWARN( "Cannot read SDRs from BMC: %s\n", strerror( errno ) );
                fclose( sdrfile );
                return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_FILE_ERR ) );
            }

            /* Write the SDR data to file */
            if( fwrite( local_sdr_buffer, (size_t)1,
                        sizeof( struct sensor_info ) * count, sdrfile ) < 0 )
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
            TWARN( "Cannot open SDR_ExtBMC file: %s\n", strerror( errno ) );
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
            TWARN( "Cannot seek SDR_ExtBMC file: %s\n", strerror( errno ) );
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
        count = filelen / sizeof( struct sensor_info );
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
	*SDRCount =  count;
	
    return( LIBIPMI_E_SUCCESS );   
}


LIBIPMI_API uint16
LIBIPMI_HL_IPMB_GetAllSensorReadings( IPMI20_SESSION_T *pSession, struct sensor_data *sensor_list, uint8 I2CAddress, int *nSensors, int timeout )
{
	int i;
	int wRet = 0;
	int SDRCount = *nSensors;
	uint8 *sdr_buffer;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
   

    /* Load the SDRs related to sensors into memory */
    wRet = LIBIPMI_HL_IPMB_LoadSensorSDRs( pSession, &sdr_buffer, &SDRCount, I2CAddress, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        TWARN( "Cannot load sensor SDRs\n" );
        return( wRet );
    }

  	
    for (i = 0; i < SDRCount; i++)
    {
		uint8 raw_reading;
		double reading;
		uint8 discrete;
	    
        struct sensor_info *sensor = &( ( (struct sensor_info *)sdr_buffer )[ i ] );
        FullSensorRec_T *record = (FullSensorRec_T *)( sensor->sdr_buffer );
					     
    	/***************************************************************************************/
        record = (FullSensorRec_T *)( sensor->sdr_buffer );

        /* Get the reading from this sensor */
        wRet = LIBIPMI_HL_IPMB_ReadSensor( pSession, sensor->sdr_buffer, &raw_reading,
        								 &reading, &discrete, I2CAddress, 15 );

		//assume sensor is accessible till we find out otherwise
		sensor_list[i].SensorAccessibleFlags = 0;
	
        if( wRet != LIBIPMI_E_SUCCESS )
        {
            TWARN( "Cannot read sensor wRet is %x\n",wRet );
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

        /* Copy the relevant sensor data into caller's array */
        tempret=snprintf( sensor_list[ i ].sensor_name,sizeof(sensor_list[ i ].sensor_name),"%s", sensor->description );
        if((tempret<0)||(tempret>=sizeof(sensor_list[ i ].sensor_name)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
        sensor_list[ i ].sensor_type	    = record->SensorType;
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
            retval = get_sdr_sensor_state( raw_reading, sensor->sdr_buffer, &( sensor_list[ i ].sensor_state ) );
            if( retval < 0 )
                TWARN( "get_sdr_sensor_state failed\n" );
            sensor_list[ i ].discrete_state = 0;
            sensor_list[ i ].low_non_recov_thresh	= sensor->low_non_recov_thresh;
            sensor_list[ i ].low_crit_thresh 		= sensor->low_crit_thresh;
            sensor_list[ i ].low_non_crit_thresh	= sensor->low_non_crit_thresh;
            sensor_list[ i ].high_non_crit_thresh	= sensor->high_non_crit_thresh;
            sensor_list[ i ].high_crit_thresh		= sensor->high_crit_thresh;
            sensor_list[ i ].high_non_recov_thresh	= sensor->high_non_recov_thresh;
        }

    }
    /* The sdr list was allocated by sdr_load, and we must free it */
	*nSensors = SDRCount;

  	free( sdr_buffer );

    return( 0 );
}


/*

printf("[%d][%-16s][%x][%x][%x][%6.4fx][%x] [%6.4fx][%6.4fx][%6.4fx][%6.4fx][%6.4fx][%6.4fx]\n"
, sencnt
, sensor_buffer->description
, record->Units1
, record->Units2
, record->Units3
, reading
, raw_reading
, sensor_buffer->low_non_recov_thresh
, sensor_buffer->low_crit_thresh  
, sensor_buffer->low_non_crit_thresh
, sensor_buffer->high_non_crit_thresh
, sensor_buffer->high_crit_thresh
, sensor_buffer->high_non_recov_thresh);
*/

/* ------------------ High level functions ------------------ */
