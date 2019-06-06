#ifndef __LIBIPMI_SDR_H__
#define __LIBIPMI_SDR_H__

/* LIIPMI core header files */
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "libipmiifc.h"

/* command specific header files */
#pragma pack(1)
#include "IPMI_SDR.h"
#pragma pack()

#include "IPMI_SensorEvent.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* Get SDR Repository Info Command */
LIBIPMI_API uint16	IPMICMD_GetSDRRepositoryInfo( IPMI20_SESSION_T *pSession,
										SDRRepositoryInfo_T *pResGetSDRRepositoryInfo,
										int timeout);

/* Get SDR Repository Allocation Info Command */
LIBIPMI_API uint16	IPMICMD_GetSDRRepositoryAllocInfo( IPMI20_SESSION_T *pSession,
										SDRRepositoryAllocInfo_T *pResGetSDRRepositoryAllocInfo,
										int timeout);

/* Reserve SDR Repository Command */
LIBIPMI_API uint16	IPMICMD_ReserveSDRRepository( IPMI20_SESSION_T *pSession,
										ReserveSDRRepositoryRes_T *pResReserveSDRRepository,
										int timeout);

/* Get SDR Command */
LIBIPMI_API uint16	IPMICMD_GetSDR( IPMI20_SESSION_T *pSession,
									GetSDRReq_T *pReqGetSDR,
									GetSDRRes_T *pResGetSDR,
									uint32		*pdwOutBuffLen,
									int timeout);

/* Add SDR Command */
LIBIPMI_API uint16	IPMICMD_AddSDR( IPMI20_SESSION_T *pSession,
									uint8		*pReqAddSDR,
									uint32		dwInBuffLen,
									AddSDRRes_T *pResAddSDR,
									int timeout);

/* Partial Add SDR Command */
LIBIPMI_API uint16	IPMICMD_PartialAddSDR( IPMI20_SESSION_T *pSession,
									PartialAddSDRReq_T *pReqPartialAddSDR,
									uint32		dwInBuffLen,
									PartialAddSDRRes_T *pResPartialAddSDR,
									int timeout);

/* Delete SDR Command */
LIBIPMI_API uint16	IPMICMD_DeleteSDR( IPMI20_SESSION_T *pSession,
									DeleteSDRReq_T *pReqDeleteSDR,
									DeleteSDRReq_T *pResDeleteSDR,
									int timeout);

/* Clear SDR Repository Command */
LIBIPMI_API uint16	IPMICMD_ClearSDRRepository( IPMI20_SESSION_T *pSession,
									ClearSDRReq_T *pReqClearSDR,
									ClearSDRRes_T *pResClearSDR,
									int timeout);

/* Get SDR Repository Time Command */
LIBIPMI_API uint16	IPMICMD_GetSDRRepositoryTime( IPMI20_SESSION_T *pSession,
									GetSDRRepositoryTimeRes_T *pResGetSDRRepositoryTime,
									int timeout);

/* Set SDR Repository Time Command */
LIBIPMI_API uint16	IPMICMD_SetSDRRepositoryTime( IPMI20_SESSION_T *pSession,
									SetSDRRepositoryTimeReq_T *pReqSetSDRRepositoryTime,
									uint8 *pResSetSDRRepositoryTime,
									int timeout);

/* Enter SDR Repository Update Mode Command */
LIBIPMI_API uint16	IPMICMD_EnterSDRUpdateMode( IPMI20_SESSION_T *pSession,
									EnterSDRUpdateModeRes_T *pResEnterSDRUpdateMode,
									int timeout);

/* Exit SDR Repository Update Mode Command */
LIBIPMI_API uint16	IPMICMD_ExitSDRUpdateMode( IPMI20_SESSION_T *pSession,
									ExitSDRUpdateModeRes_T *pResExitSDRUpdateMode,
									int timeout);

/* Run Initialization Agent Command */
LIBIPMI_API uint16	IPMICMD_RunInitAgent( IPMI20_SESSION_T *pSession,
									RunInitAgentReq_T *pReqRunInitAgent,
									RunInitAgentRes_T *pResRunInitAgent,
									int timeout);

/*---------- LIBIPMI Higher level routines -----------*/
LIBIPMI_API uint16 LIBIPMI_HL_GetCompleteSDR( IPMI20_SESSION_T *pSession,
                                              uint16 record_id,
                                              uint16 *next_record_id,
                                              uint8 *sdr_buffer,
                                              size_t buffer_len,
                                              int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_GetAllSDRs( IPMI20_SESSION_T *pSession,
                                          uint8 *sdr_buffer, size_t buffer_len,
                                          int timeout );


LIBIPMI_API uint16 LIBIPMI_HL_GetSDR( IPMI20_SESSION_T *pSession,
                                            uint16 reservation_id,
                                            uint16 record_id,
                                            GetSDRRes_T *pGetSDRRes,
                                            uint32 *dwDataLen,
                                            int timeout );
#define SDR_CACHE_PATH "/var/sdrcache"

#define ERR_SDRCACHE_SUCCESS 		0
#define ERR_SDRCACHE_FILEOP  		1
#define ERR_SDRCACHE_INVALID_ARGS	2
#define ERR_SDRCACHE_BUFFSIZE		3


LIBIPMI_API uint16 LIBIPMI_HL_GetAllSDRs_Cached( IPMI20_SESSION_T *pSession,
                                          uint8 *sdr_buffer, 
										  uint32* sdr_buff_size,
										  uint32* max_sdr_len,
										  uint32 * sdr_count,
                                          int timeout );

LIBIPMI_API uint16 LIBIPMI_HL_GetSpecificSDR( IPMI20_SESSION_T *pSession,
										uint8 *sdr_buffer, 
										uint32* sdr_buff_size,
										int SensorNumber,
										int OwnerLUN,
										int SensorType,
										int timeout );


#ifdef  __cplusplus
}
#endif

#endif







