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
* Filename: libipmi_sdr.c
*
******************************************************************/
#include <string.h>
#include <stdlib.h>

#include "dbgout.h"

#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmi_sdr.h"
#include "IPMI_Storage.h"
#include "std_macros.h"

#include "libipmi_sal_session.h"
#include "libipmi_AMIOEM.h"

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



/* Get SDR Repository Info Command */
LIBIPMI_API uint16    IPMICMD_GetSDRRepositoryInfo( IPMI20_SESSION_T *pSession,
                                        SDRRepositoryInfo_T *pResGetSDRRepositoryInfo,
                                        int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(SDRRepositoryInfo_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_GET_SDR_REPOSITORY_INFO,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResGetSDRRepositoryInfo, &dwResLen,
                                            timeout);

    return wRet;
}

/* Get SDR Repository Allocation Info Command */
LIBIPMI_API uint16    IPMICMD_GetSDRRepositoryAllocInfo( IPMI20_SESSION_T *pSession,
                                        SDRRepositoryAllocInfo_T *pResGetSDRRepositoryAllocInfo,
                                        int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(SDRRepositoryAllocInfo_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_GET_SDR_REPOSITORY_ALLOCATION_INFO,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResGetSDRRepositoryAllocInfo, &dwResLen,
                                            timeout);

    return wRet;
}

/* Reserve SDR Repository Command */
LIBIPMI_API uint16    IPMICMD_ReserveSDRRepository( IPMI20_SESSION_T *pSession,
                                        ReserveSDRRepositoryRes_T *pResReserveSDRRepository,
                                        int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(ReserveSDRRepositoryRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_RESERVE_SDR_REPOSITORY,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResReserveSDRRepository, &dwResLen,
                                            timeout);

    return wRet;
}


/* Get SDR Command */
LIBIPMI_API uint16    IPMICMD_GetSDR( IPMI20_SESSION_T *pSession,
                                    GetSDRReq_T *pReqGetSDR,
                                    GetSDRRes_T *pResGetSDR,
                                    uint32        *pdwOutBuffLen,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = *pdwOutBuffLen;
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_GET_SDR,
                                            (uint8*)pReqGetSDR, sizeof(GetSDRReq_T),
                                            (uint8 *)pResGetSDR, &dwResLen,
                                            timeout);

    *pdwOutBuffLen = dwResLen;

    return wRet;
}


/* Add SDR Command */
LIBIPMI_API uint16    IPMICMD_AddSDR( IPMI20_SESSION_T *pSession,
                                    uint8        *pReqAddSDR,
                                    uint32        dwInBuffLen,
                                    AddSDRRes_T *pResAddSDR,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(AddSDRRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_ADD_SDR,
                                            (uint8*)pReqAddSDR, dwInBuffLen,
                                            (uint8 *)pResAddSDR, &dwResLen,
                                            timeout);

    return wRet;
}


/* Partial Add SDR Command */
LIBIPMI_API uint16    IPMICMD_PartialAddSDR( IPMI20_SESSION_T *pSession,
                                    PartialAddSDRReq_T *pReqPartialAddSDR,
                                    uint32        dwInBuffLen,
                                    PartialAddSDRRes_T *pResPartialAddSDR,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(PartialAddSDRRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_PARTIAL_ADD_SDR,
                                            (uint8*)pReqPartialAddSDR, dwInBuffLen,
                                            (uint8 *)pResPartialAddSDR, &dwResLen,
                                            timeout);

    return wRet;
}


/* Delete SDR Command */
LIBIPMI_API uint16    IPMICMD_DeleteSDR( IPMI20_SESSION_T *pSession,
                                    DeleteSDRReq_T *pReqDeleteSDR,
                                    DeleteSDRReq_T *pResDeleteSDR,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(DeleteSDRReq_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_DELETE_SDR,
                                            (uint8*)pReqDeleteSDR, sizeof(DeleteSDRReq_T),
                                            (uint8 *)pResDeleteSDR, &dwResLen,
                                            timeout);

    return wRet;
}


/* Clear SDR Repository Command */
LIBIPMI_API uint16    IPMICMD_ClearSDRRepository( IPMI20_SESSION_T *pSession,
                                    ClearSDRReq_T *pReqClearSDR,
                                    ClearSDRRes_T *pResClearSDR,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(ClearSDRRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_CLEAR_SDR_REPOSITORY,
                                            (uint8*)pReqClearSDR, sizeof(ClearSDRReq_T),
                                            (uint8 *)pResClearSDR, &dwResLen,
                                            timeout);

    return wRet;
}


/* Get SDR Repository Time Command */
LIBIPMI_API uint16    IPMICMD_GetSDRRepositoryTime( IPMI20_SESSION_T *pSession,
                                    GetSDRRepositoryTimeRes_T *pResGetSDRRepositoryTime,
                                    int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(GetSDRRepositoryTimeRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_GET_SDR_REPOSITORY_TIME,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResGetSDRRepositoryTime, &dwResLen,
                                            timeout);

    return wRet;
}


/* Set SDR Repository Time Command */
LIBIPMI_API uint16    IPMICMD_SetSDRRepositoryTime( IPMI20_SESSION_T *pSession,
                                    SetSDRRepositoryTimeReq_T *pReqSetSDRRepositoryTime,
                                    uint8 *pResSetSDRRepositoryTime,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(uint8);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_SET_SDR_REPOSITORY_TIME,
                                            (uint8*)pReqSetSDRRepositoryTime, sizeof(SetSDRRepositoryTimeReq_T),
                                            (uint8 *)pResSetSDRRepositoryTime, &dwResLen,
                                            timeout);

    return wRet;
}


/* Enter SDR Repository Update Mode Command */
LIBIPMI_API uint16    IPMICMD_EnterSDRUpdateMode( IPMI20_SESSION_T *pSession,
                                    EnterSDRUpdateModeRes_T *pResEnterSDRUpdateMode,
                                    int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(EnterSDRUpdateModeRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_ENTER_SDR_REPOSITORY_UPDATE_MODE,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResEnterSDRUpdateMode, &dwResLen,
                                            timeout);

    return wRet;
}

/* Exit SDR Repository Update Mode Command */
LIBIPMI_API uint16    IPMICMD_ExitSDRUpdateMode( IPMI20_SESSION_T *pSession,
                                    ExitSDRUpdateModeRes_T *pResExitSDRUpdateMode,
                                    int timeout)
{
    uint8    Req[20];
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(ExitSDRUpdateModeRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_EXIT_SDR_REPOSITORY_UPDATE_MODE,
                                            (uint8*)Req, 0,
                                            (uint8 *)pResExitSDRUpdateMode, &dwResLen,
                                            timeout);

    return wRet;
}

/* Run Initialization Agent Command */
LIBIPMI_API uint16    IPMICMD_RunInitAgent( IPMI20_SESSION_T *pSession,
                                    RunInitAgentReq_T *pReqRunInitAgent,
                                    RunInitAgentRes_T *pResRunInitAgent,
                                    int timeout)
{
    uint16    wRet;
    uint32    dwResLen;

    dwResLen = sizeof(RunInitAgentRes_T);
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                                            NETFNLUN_IPMI_STORAGE, CMD_RUN_INITIALIZATION_AGENT,
                                            (uint8*)pReqRunInitAgent, sizeof(RunInitAgentReq_T),
                                            (uint8 *)pResRunInitAgent, &dwResLen,
                                            timeout);

    return wRet;
}


/***************** High level SDR commands *********************/
static uint16 ipmb_get_sdr( IPMI20_SESSION_T *pSession,
                                              uint16 reservation_id,
                                              uint16 record_id,
					      GetSDRRes_T *pGetSDRRes,
					      uint32 *dwRecordLen,
                                              int timeout )
{
#define IPMB_SDR_MAX_SINGLE_READ	(16)
	uint8 offset = (u8)0;
	uint8 remaining_bytes;
	uint8 bytes_to_read;
	uint32 dwRetLen;
	uint16 wRet;
	
	uint8	data_buffer[255];
	GetSDRReq_T get_sdr_req;
	SDRRecHdr_T	*sdr_record;
	
	uint8	*output_buffer;
	uint32	output_buffer_ix=0;
	
	output_buffer = (uint8 *)pGetSDRRes;
	
	/* Find the record length first */
	get_sdr_req.ReservationID = htoipmi_u16(reservation_id);
	get_sdr_req.RecID = htoipmi_u16( record_id );
	get_sdr_req.Offset = 0;
	get_sdr_req.Size = sizeof(SDRRecHdr_T);
	dwRetLen = sizeof(SDRRecHdr_T) + sizeof(GetSDRRes_T);
	wRet = IPMICMD_GetSDR( pSession, &get_sdr_req, (GetSDRRes_T *)&data_buffer[0],
			&dwRetLen, timeout );
	
	if(wRet != LIBIPMI_E_SUCCESS)
		return wRet;
		
	/* We got record length. Now we read the record */
	sdr_record = (SDRRecHdr_T*)&data_buffer[sizeof(GetSDRRes_T)];
	offset = sizeof(SDRRecHdr_T);
	remaining_bytes = sdr_record->Len;
	
	/* copy to output buffer */
	memcpy(&output_buffer[output_buffer_ix], data_buffer, dwRetLen);
	output_buffer_ix += dwRetLen;
	
	do {
		bytes_to_read = (remaining_bytes > IPMB_SDR_MAX_SINGLE_READ)? IPMB_SDR_MAX_SINGLE_READ : remaining_bytes;
		
		get_sdr_req.ReservationID = htoipmi_u16(reservation_id);
		get_sdr_req.RecID = htoipmi_u16( record_id );
		get_sdr_req.Offset = offset;
		get_sdr_req.Size = bytes_to_read;
		dwRetLen = bytes_to_read + sizeof(GetSDRRes_T);
		wRet = IPMICMD_GetSDR( pSession, &get_sdr_req, (GetSDRRes_T*)&data_buffer[0],
				&dwRetLen, timeout );
		
		if(wRet != LIBIPMI_E_SUCCESS)
			return wRet;
		
		memcpy(&output_buffer[output_buffer_ix], &data_buffer[sizeof(GetSDRRes_T)], bytes_to_read);
		output_buffer_ix += bytes_to_read;
		
		offset += bytes_to_read;
		remaining_bytes -= bytes_to_read;
	
	}while( remaining_bytes > 0 );
	
	sdr_record = (SDRRecHdr_T*)&output_buffer[sizeof(GetSDRRes_T)];
	*dwRecordLen = sdr_record->Len + sizeof(SDRRecHdr_T);

	return LIBIPMI_E_SUCCESS;	
}

LIBIPMI_API uint16 LIBIPMI_HL_GetSDR( IPMI20_SESSION_T *pSession,
                                              uint16 reservation_id,
                                              uint16 record_id,
					      GetSDRRes_T *pGetSDRRes,
					      uint32 *dwDataLen,
                                              int timeout )
{
	uint16	wRet=LIBIPMI_E_SUCCESS;
	
	
	if( pSession->byMediumType == IPMB_MEDIUM )
	{
		wRet = ipmb_get_sdr(pSession, reservation_id, 
					record_id,
					pGetSDRRes,
					dwDataLen,
					timeout );
	}
	else
	{
		GetSDRReq_T get_sdr_req;
			
		get_sdr_req.ReservationID = htoipmi_u16(reservation_id);
		get_sdr_req.RecID = htoipmi_u16( record_id );
		get_sdr_req.Offset = 0;
		get_sdr_req.Size = 0xff;
    		wRet = IPMICMD_GetSDR( pSession, &get_sdr_req, pGetSDRRes,
                           dwDataLen, timeout );
	}

	return wRet;	
}

LIBIPMI_API uint16 LIBIPMI_HL_GetCompleteSDR( IPMI20_SESSION_T *pSession,
                                              uint16 record_id,
                                              uint16 *next_record_id,
                                              uint8 *sdr_buffer,
                                              size_t buffer_len,
                                              int timeout )
{
    uint16 wRet;
    ReserveSDRRepositoryRes_T reservation;
    SDRRepositoryAllocInfo_T alloc_info;
    uint8 *temp_buffer;
    GetSDRRes_T *get_sdr_res;
    uint32 blen = 0;
    int max_sdr_len;

    /* Get repository allocation information */
    wRet = IPMICMD_GetSDRRepositoryAllocInfo( pSession, &alloc_info, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
        return( wRet );
    
    reservation.ReservationID = 0; //Setting reservation ID as zero as we are trying to get complete SDR.
    
    /* Determine max SDR entry length */
    max_sdr_len = alloc_info.MaxRecSize * ipmitoh_u16( alloc_info.AllocUnitSize );

    /* Make sure the calling function has space for this thing */
    if( buffer_len < (size_t) max_sdr_len )
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ) );

    
    temp_buffer = malloc( max_sdr_len + sizeof( GetSDRRes_T ) );
    if( temp_buffer == NULL )
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ) );
    
    get_sdr_res = (GetSDRRes_T *)temp_buffer;
	if(pSession->byMediumType == NETWORK_MEDIUM_UDP_SAL)
    {
        blen = max_sdr_len + sizeof(GetSDRRes_T);
    }

#if 0        
    get_sdr_req.ReservationID = reservation.ReservationID;
    get_sdr_req.RecID = htoipmi_u16( record_id );
    get_sdr_req.Offset = 0;
    get_sdr_req.Size = 0xff;

    /* Now try to get the whole record */
    wRet = IPMICMD_GetSDR( pSession, &get_sdr_req, (GetSDRRes_T *)temp_buffer,
                           &blen, timeout );
#endif
    wRet = LIBIPMI_HL_GetSDR(pSession, ipmitoh_u16(reservation.ReservationID),
    					record_id, (GetSDRRes_T *)temp_buffer, &blen, timeout);
    if( wRet != LIBIPMI_E_SUCCESS )
    {
        free( temp_buffer );
        return( wRet );
    }

    *next_record_id = ipmitoh_u16( get_sdr_res->NextRecID );

    memcpy( sdr_buffer, temp_buffer + sizeof( GetSDRRes_T ), blen );
    free( temp_buffer );

    return( wRet );
}


LIBIPMI_API uint16 LIBIPMI_HL_GetAllSDRs( IPMI20_SESSION_T *pSession,
                                          uint8 *sdr_buffer, size_t buffer_len,
                                          int timeout )
{
    uint16 wRet;
    uint16 record_id = 0;
    int max_sdr_len;
    int sdr_count;
    SDRRepositoryAllocInfo_T alloc_info;
    SDRRepositoryInfo_T repo_info;
    int i;

    /* Get repository allocation information */
    wRet = IPMICMD_GetSDRRepositoryAllocInfo( pSession, &alloc_info, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
        return( wRet );

    /* Get repository information */
    wRet = IPMICMD_GetSDRRepositoryInfo( pSession, &repo_info, timeout );
    if( wRet != LIBIPMI_E_SUCCESS )
        return( -1 );

    /* Determine max SDR entry length */
    max_sdr_len = alloc_info.MaxRecSize * ipmitoh_u16( alloc_info.AllocUnitSize );

    /* Determine total number of SDRs stored */
    sdr_count = ipmitoh_u16( repo_info.RecCt );

    /* Make sure the passed buffer is large enough */
    if( buffer_len < (size_t) ( max_sdr_len * sdr_count ) )
        return( STATUS_CODE( IPMI_ERROR_FLAG, OEMCC_NOMEM ) );

    /* Read all the SDRs in this loop */
    for( i = 0; i < sdr_count; i++ )
    {
        uint16 next_record_id;

        /* Read the full SDR into the buffer */
        wRet = LIBIPMI_HL_GetCompleteSDR( pSession, record_id, &next_record_id,
                                          &sdr_buffer[ i * max_sdr_len ],
                                          max_sdr_len, timeout );
        if( wRet != LIBIPMI_E_SUCCESS )
            return( wRet );

        record_id = next_record_id;
    }

    return( wRet );
}

int CacheGetAllSDRS(IPMI20_SESSION_T* pSession,uint8* sdr_buffer,uint32* buff_size,uint32* max_sdr_len,uint32* sdr_count);

LIBIPMI_API uint16 LIBIPMI_HL_GetAllSDRs_Cached( IPMI20_SESSION_T *pSession,
                                          uint8 *sdr_buffer, 
										  uint32* sdr_buff_size,
										  uint32* max_sdr_len,
										  uint32 * sdr_count,
                                          int timeout )
{
	uint16 wRet;
	if(0)
	{
		timeout=timeout; /*-Wextra: Flag added for strict compilation*/
	}

	wRet = CacheGetAllSDRS(pSession,sdr_buffer,sdr_buff_size,max_sdr_len,sdr_count);

    
	return wRet;
}

LIBIPMI_API uint16 LIBIPMI_HL_GetSpecificSDR( IPMI20_SESSION_T *pSession,
                                                uint8 *sdr_buffer, 
                                                uint32* sdr_buff_size,
                                                int SensorNumber,
                                                int OwnerLUN,
                                                int SensorType,
                                                int timeout )
{
    ReserveSDRRepositoryRes_T ResReserveSDRRepository;
    uint8 data_buffer[255],Sensor_256_Enabled=0;
    FullSensorRec_T	*sdr_record = (FullSensorRec_T*) &data_buffer[sizeof(GetSDRRes_T)];
    GetSDRReq_T ReqGetSDR;
    GetSDRRes_T *pResGetSDR = (GetSDRRes_T *) &data_buffer[0];
    uint32      dwRetLen = sizeof data_buffer;
    uint16 wRet = 0;
    wRet = LIBIPMI_HL_AMIGetFeatureStatus(pSession,(uint8*)"CONFIG_SPX_FEATURE_MORE_THAN_256_SENSORS_SUPPORT", &Sensor_256_Enabled,timeout);
	if (wRet != LIBIPMI_E_SUCCESS)
			return wRet;

    wRet = IPMICMD_ReserveSDRRepository(pSession, &ResReserveSDRRepository, timeout);
        
    pResGetSDR->NextRecID = 0;
    while (pResGetSDR->NextRecID != 0xFF)
    {
        ReqGetSDR.ReservationID = ResReserveSDRRepository.ReservationID;
        ReqGetSDR.RecID = pResGetSDR->NextRecID;
        ReqGetSDR.Offset = 0;
        ReqGetSDR.Size = 0xff;
        wRet = IPMICMD_GetSDR(pSession, &ReqGetSDR, pResGetSDR, &dwRetLen, timeout);
        if (wRet != LIBIPMI_E_SUCCESS)
            break;
        if(Sensor_256_Enabled)
        {
            if ((sdr_record->SensorNum == SensorNumber) &&
               (sdr_record->OwnerLUN == OwnerLUN) &&
               (sdr_record->SensorType == SensorType))
               break;  // found
        }
        else
        {
            if (sdr_record->SensorNum == SensorNumber)
               break;
        } 

    }
    
    dwRetLen -= sizeof(GetSDRRes_T);
    *sdr_buff_size = *sdr_buff_size >= dwRetLen ? dwRetLen : *sdr_buff_size;
    memcpy(sdr_buffer, sdr_record, *sdr_buff_size);
    return wRet;
}

