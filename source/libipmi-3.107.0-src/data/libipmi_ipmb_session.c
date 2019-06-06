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
* Filename: libipmi_ipmb_session.c
*
******************************************************************/
#include "libipmi_session.h"
//#include "libipmi_kcs.h"
//#include "kcs_driver.h"

#if LIBIPMI_IS_OS_LINUX()
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include "dbgout.h"
#include "ipmb_utils.h"
#include "ipmb_interface.h"
#include "libi2c.h"
#include "ipmb_structs.h"

#define SAFE_FREE(ptr) { if((ptr)) { free(ptr); ptr=NULL; } }


uint16 Create_IPMI20_IPMB_Session(IPMI20_SESSION_T *pSession, uint8 bySlaveAddr, uint8 bus)
{
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )

	/* initialize to zero */
	memset(pSession, 0, sizeof(IPMI20_SESSION_T) );

	/* allocate memory for serial session */
	pSession->hIPMBSession = (IPMI20_IPMB_SESSION_T*)malloc( sizeof(IPMI20_IPMB_SESSION_T) );

	if (NULL == pSession->hIPMBSession)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_NO_ENOUGH_MEMORY);

	pSession->bySessionStarted				= SESSION_STARTED;
	pSession->byMediumType					= IPMB_MEDIUM;
	pSession->hIPMBSession->busnumber		= bus;
	pSession->hIPMBSession->bySlaveAddr		= bySlaveAddr;

	return LIBIPMI_E_SUCCESS;	
#else
	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_OS_UNSUPPORTED);
#endif
}

uint16 Close_IPMI20_IPMB_Session( IPMI20_SESSION_T *pSession )
{
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )
	SAFE_FREE(pSession->hIPMBSession);
	return LIBIPMI_E_SUCCESS;
#else
	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_OS_UNSUPPORTED);
#endif
}

uint16 Send_RAW_IPMI2_0_IPMB_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout)
{
#if LIBIPMI_IS_OS_LINUX() && (LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_ARM() )
	uint8			pbyRequestData[MAX_IPMB_BUFFER];
	uint8			pbyResponseData[MAX_IPMB_BUFFER];
	size_t			response_len = MAX_IPMB_BUFFER;
	int			data_len;
	
	uint16			wRet;
	if(0)
	{
		timeout=timeout; /*-Wextra: Flag added for strict compilation */
	}
	if( dwReqDataLen > MAX_IPMB_DATA_SIZE )
	{
		TCRIT("Request packet size(%d) exceeds maximum IPMB data size(%d)", dwReqDataLen, MAX_IPMB_DATA_SIZE);
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_REQ_BUFF_TOO_BIG);
	}
	
	if( *pdwResDataLen > MAX_IPMB_DATA_SIZE )
	{
		TCRIT("Response data size(%d) cannot exceed maximum IPMB data size(%d)", *pdwResDataLen, MAX_IPMB_DATA_SIZE);
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_RES_BUFF_TOO_BIG);
	}

	memset(pbyRequestData,0x00,MAX_IPMB_BUFFER);
	
	/* First byte always command number */
	pbyRequestData[0] = byCommand;
	
	/* follows request data */
	if (dwReqDataLen)
		memcpy(&pbyRequestData[1], pbyReqData, dwReqDataLen);


	memset(pbyResponseData,0x00,MAX_IPMB_BUFFER);
	wRet = ipmi_generic_command( pSession->hIPMBSession->busnumber,pSession->hIPMBSession->bySlaveAddr,
					byNetFnLUN,
					pbyRequestData, dwReqDataLen+1,
					pbyResponseData, &response_len );
	if(( wRet == LIBIPMI_E_SUCCESS) || IS_IPMI_ERROR(wRet))	//Should Return  Complection Code with error data if IPMI Error Occur
	{
		/* IPMB MESSAGE FORMAT
		
		   IPMB MESSAGE HEADER	( 5 BYTES )
		   RESPONSE DATA	( N BYTES )
		   CHECKSUM		( 1 BYTE )
		*/
		

		if (response_len > MAX_IPMB_BUFFER) {
			TCRIT("Send_RAW_IPMI2_0_IPMB_Command: illegal response len %d", response_len);
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_RES_BUFF_TOO_BIG);
		}

		data_len = response_len - ( sizeof(IPMBMsg) + 1 + 1);

		if (data_len < 0) {
			TCRIT("Send_RAW_IPMI2_0_IPMB_Command: illegal response(data) len %d", 
					response_len);
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_IPMB_COMM_FAILURE);
		}
#if 0	
		if(*pdwResDataLen < data_len)
		{
			TWARN("Send_RAW_IPMI2_0_IPMB_Command : Supplied buffer size(%d bytes) is less than the actual IPMI response buffer size(%d bytes)\n",
				*pdwResDataLen, data_len);
		}
#endif

		
		/* In some cases where we dont know the response size*/
		//if( *pdwResDataLen == 0 )
			memcpy(pbyResData, &pbyResponseData[1/*Responce contain Slave Addr also*/ +sizeof(IPMBMsg)], data_len);
		//else
			//memcpy(pbyResData, &pbyResponseData[1/*Responce contain Slave Addr also*/ + sizeof(IPMBMsg)], *pdwResDataLen);

		*pdwResDataLen = data_len;
	}

	return wRet;
#else
	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_OS_UNSUPPORTED);
#endif
}


