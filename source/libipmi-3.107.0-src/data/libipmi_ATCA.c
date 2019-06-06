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
* Filename: libipmi_ATCA.c
*
* Descriptions: Contains implementation of FRU LED
*   specific IPMC command functions
*
* Author: Balakumar
*
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dbgout.h>

#include "libipmi_ATCA.h"
#include "libipmi_session.h"
#include "libipmi_errorcodes.h"
#include "IPMI_PICMG.h"
#include "IPMI_AMI.h"

/**
   \brief	Internal function to Get FRU LED Properties Command
   @param	pSession					[in]Session handle
   @param	pGetFRULedPropReq			[in]Get FRU LED property Request Data
   @param	pGetFRULedPropRes	   		[out]Get FRU LED property Response Data
   @param	timeout						[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMCCMD_GetFRULedProperties( IPMI20_SESSION_T *pSession,
							GetFRULedPropReq_T *pGetFRULedPropReq,
							GetFRULedPropRes_T *pGetFRULedPropRes,
							int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetFRULedPropRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(( NETFN_PICMG << 2 ) + 0x00 ), CMD_GET_FRU_LED_PROPERTIES,
											(uint8*)pGetFRULedPropReq, sizeof(GetFRULedPropReq_T),
											(uint8 *)pGetFRULedPropRes, &dwResLen,
											timeout);

	return wRet;
}

/**
   \brief	Internal function to Get FRU LED Color Capabilities Command
   @param	pSession					[in]Session handle
   @param	pGetLedColorCapsReq			[in]Get FRU LED Color Capability Request Data
   @param	pGetLedColorCapsRes	   		[out]Get FRU LED Color Capability Response Data
   @param	timeout						[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMCCMD_GetLedColorCaps( IPMI20_SESSION_T *pSession,
								GetLedColorCapsReq_T *pGetLedColorCapsReq,
								GetLedColorCapsRes_T *pGetLedColorCapsRes,
								int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetLedColorCapsRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(( NETFN_PICMG << 2 ) + 0x00 ), CMD_GET_LED_COLOR_CAPS,
											(uint8*)pGetLedColorCapsReq, sizeof(GetLedColorCapsReq_T),
											(uint8 *)pGetLedColorCapsRes, &dwResLen,
											timeout);

	return wRet;
}

/**
   \brief	Internal function to Set FRU LED State Command
   @param	pSession					[in]Session handle
   @param	pSetFRULedStateReq			[in]Set FRU LED State Request Data
   @param	pSetFRULedStateRes	   		[out]Set FRU LED State Response Data
   @param	timeout						[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMCCMD_SetFRULedState( IPMI20_SESSION_T *pSession,
							SetFRULedStateReq_T	*pSetFRULedStateReq,
							SetFRULedStateRes_T *pSetFRULedStateRes,
							int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(SetFRULedStateRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(( NETFN_PICMG << 2 ) + 0x00 ), CMD_SET_FRU_LED_STATE,
											(uint8*)pSetFRULedStateReq, sizeof(SetFRULedStateReq_T),
											(uint8 *)pSetFRULedStateRes, &dwResLen,
											timeout);

	return wRet;
}

/**
   \brief	Internal function to Get FRU LED State Command
   @param	pSession					[in]Session handle
   @param	pGetFRULedStateReq			[in]Get FRU LED State Request Data
   @param	pGetFRULedStateRes	   		[out]Get FRU LED State Response Data
   @param	timeout						[in]timeout value in seconds.

   @retval Returns LIBIPMI_STATUS_SUCCESS on success and error codes on failure
*/
uint16	IPMCCMD_GetFRULedState( IPMI20_SESSION_T *pSession,
							GetFRULedStateReq_T	*pGetFRULedStateReq,
							GetFRULedStateRes_T *pGetFRULedStateRes,
							int timeout)
{
	uint16	wRet;
	uint32	dwResLen;

	dwResLen = sizeof(GetFRULedStateRes_T);
	wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
											(( NETFN_PICMG << 2 ) + 0x00 ), CMD_GET_FRU_LED_STATE,
											(uint8*)pGetFRULedStateReq, sizeof(GetFRULedStateReq_T),
											(uint8 *)pGetFRULedStateRes, &dwResLen,
											timeout);

	return wRet;
}

