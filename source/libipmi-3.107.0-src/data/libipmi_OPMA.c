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
* Filename: libipmi_OPMA.c
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbgout.h"

#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "std_macros.h"
#include "libipmi_OPMA.h"

/**
 * OPMAIPMICMD_SetSensorReadingOffset
 * libipmi API for SetSensorReadingOffset
 *
 **/
uint16
OPMAIPMICMD_SetSensorReadingOffset ( IPMI20_SESSION_T *pSession,
                                     SetSensorReadingOffsetReq_T  *pSetSensorStateReq ,
                                     SetSensorReadingOffsetRes_T  *pSetSensorStateRes,
                                     int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetSensorReadingOffsetRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA1 << 2 ) + 0x00 ), CMD_OPMA_SET_SENSOR_RD_OFFSET ,
                                           (uint8 *)pSetSensorStateReq ,sizeof (SetSensorReadingOffsetReq_T),
                                           (uint8 *)pSetSensorStateRes, &dwResLen, timeout );
    return( wRet );

}

/**
  * OPMAIPMICMD_GetSensorReadingOffset
  * libipmi API for GetSensorReadingOffset
  *
 **/
uint16 OPMAIPMICMD_GetSensorReadingOffset ( IPMI20_SESSION_T *pSession,
                                            GetSensorReadingOffsetReq_T  *pGetSensorStateReq ,
                                            GetSensorReadingOffsetRes_T  *pGetSensorStateRes,
                                            int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSensorReadingOffsetRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA1 << 2 ) + 0x00 ), CMD_OPMA_GET_SENSOR_RD_OFFSET ,
                                           (uint8 *)pGetSensorStateReq ,sizeof (GetSensorReadingOffsetReq_T),
                                           (uint8 *)pGetSensorStateRes, &dwResLen, timeout );
    return( wRet );


}
/**
  * OPMAIPMICMD_SetSystemTypeIdentifier
  * libipmi API for SetSystemTypeIdentifier OPAM Command
  *
 **/
uint16 OPMAIPMICMD_SetSystemTypeIdentifier  ( IPMI20_SESSION_T *pSession,
                                            SetSystemTypeIdentifierReq_T  *pSetSysIdReq,
                                            SetSystemTypeIdentifierRes_T  *pSetSysIdRes,
                                            int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetSystemTypeIdentifierRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_SET_SYS_TYPE_ID ,
                                           (uint8 *)pSetSysIdReq ,sizeof (SetSystemTypeIdentifierReq_T),
                                           (uint8 *)pSetSysIdRes, &dwResLen, timeout );
    return( wRet );


}

/**
  * OPMAIPMICMD_GetystemTypeIdentifier
  * libipmi API for GetSystemTypeIdentifier OPMA Command
  *
 **/
 uint16 OPMAIPMICMD_GetSystemTypeIdentifier  ( IPMI20_SESSION_T *pSession,
                                               GetSystemTypeIdentifierRes_T  *pGetSysIdRes,
                                               int timeout )
{

    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSystemTypeIdentifierRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_GET_SYS_TYPE_ID ,
                                           (uint8 *)NULL ,0,
                                           (uint8 *)pGetSysIdRes, &dwResLen, timeout );
    return( wRet );

}

/**
  * OPMAIPMICMD_GetmCardCapabilities
  * libipmi API for GetmCardCapabilities
  *
**/
uint16 OPMAIPMICMD_GetmCardCapabilities  ( IPMI20_SESSION_T *pSession,
                                         GetmCardCapabilitiesRes_T  *pmCardRes,
                                         int timeout )
{

    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetmCardCapabilitiesRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_GET_MCARD_CAP ,
                                           (uint8 *)NULL ,0,
                                           (uint8 *)pmCardRes, &dwResLen, timeout );
    return( wRet );

}

/**
  * OPMAIPMICMD_ClearCMOS
  * libipmi API for ClearCMOS OPMA Command
  *
 **/
 uint16 OPMAIPMICMD_ClearCMOS ( IPMI20_SESSION_T *pSession, ClearCMOSRes_T *pClearCMOS,int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( ClearCMOSRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_CLR_CMOS ,
                                           (uint8 *)NULL ,0,
                                           (uint8 *)pClearCMOS, &dwResLen, timeout );
    return( wRet );
}


/**
  * OPMAIPMICMD_SetLocalAccessLock
  * libipmi API for SetLocalAccessLock Local Access
  *
 **/
uint16 OPMAIPMICMD_SetLocalAccessLock ( IPMI20_SESSION_T *pSession,
                                      SetLocalAccessLockOutStateReq_T  *pLockstateReq ,
                                      SetLocalAccessLockOutStateRes_T  *pLockstateRes,
                                      int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( SetLocalAccessLockOutStateRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_SET_LOCAL_LOCKOUT,
                                           (uint8 *)pLockstateReq, sizeof( SetLocalAccessLockOutStateReq_T ),
                                           (uint8 *)pLockstateRes, &dwResLen, timeout );
    return( wRet );
}


/**
  * OPMAIPMICMD_GetLocalAccessLock
  * libipmi API for GetLocalAccessLock OPMA command
  *
**/
uint16 OPMAIPMICMD_GetLocalAccessLock ( IPMI20_SESSION_T *pSession,GetLocalAccessLockOutStateRes_T  *pLockstateRes,
                                        int timeout )
{
    uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetLocalAccessLockOutStateRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_GET_LOCAL_LOCKOUT,
                                           (uint8 *)NULL ,0,
                                           (uint8 *)pLockstateRes, &dwResLen, timeout );
    return( wRet );
}

/**
 * OPMAIPMICMD_GetSupportedHostIDs
 * libipmi API for GetSupportedHostIDs OPMA command
 *
**/
 uint16 OPMAIPMICMD_GetSupportedHostIDs ( IPMI20_SESSION_T *pSession,
                                        GetSupportedHostIDsRes_T  *pGetSuppHostId, int timeout )
{

  uint16 wRet;
    uint32 dwResLen;

    dwResLen = sizeof( GetSupportedHostIDsRes_T );
    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command( pSession, PAYLOAD_TYPE_IPMI,
                                           ( ( NETFN_OPMA2 << 2 ) + 0x00 ), CMD_OPMA_GET_SUPPORTED_HOST_IDS ,
                                           (uint8 *)NULL ,0,
                                           (uint8 *)pGetSuppHostId, &dwResLen, timeout );
    return( wRet );
}

