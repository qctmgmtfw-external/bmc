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
* Filename: libipmi_kcs_session.c
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_kcs.h"
#include "kcs_driver.h"

#if LIBIPMI_IS_OS_LINUX()
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif


uint8 SendDataToKCSDriver(HANDLE hKCSDevice, 
						  uint8	byResAddr,
						  uint8 *pbyInBuffer, uint32 dwInBuffLen, 
						  uint8 *pbyOutBuffer, uint32 *pdwOutBuffLen,
						  int timeout);
uint8	OpenKCSDriver(IPMI20_SESSION_T *pSession, char *pszDevice);
uint8	CloseKCSDriver(IPMI20_SESSION_T *pSession);

uint16 Create_IPMI20_KCS_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
								 uint8 byResAddr)
{
	uint8			byRet;

	/* initialize to zero */
	memset(pSession, 0, sizeof(IPMI20_SESSION_T) );

	/* allocate memory for serial session */
	pSession->hKCSSession = (IPMI20_KCS_SESSION_T*)malloc( sizeof(IPMI20_KCS_SESSION_T) );

	/* Open Serial Port */
	byRet = OpenKCSDriver(pSession, pszDevice);

	/* return if error */
	if(byRet != LIBIPMI_STATUS_SUCCESS)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
	
	pSession->bySessionStarted				= SESSION_STARTED;
	pSession->byMediumType					= KCS_MEDIUM;
	pSession->hKCSSession->byResAddr		= byResAddr;

	return LIBIPMI_E_SUCCESS;	
}

uint16 Close_IPMI20_KCS_Session( IPMI20_SESSION_T *pSession )
{
	return LIBIPMI_E_SUCCESS;
}

uint16 Send_RAW_IPMI2_0_KCS_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout)
{
	uint8				byRet;
	char				byReq[MAX_REQUEST_SIZE];
	IPMIKCSRequest_T*	pReqPkt;

	pReqPkt = (IPMIKCSRequest_T*)byReq;
	
	pReqPkt->byNetFnLUN = byNetFnLUN;
	pReqPkt->byCmd = byCommand;
	memcpy(pReqPkt->byData, pbyReqData, dwReqDataLen);


	/* Send KCS Driver */
	byRet = SendDataToKCSDriver(pSession->hKCSSession->hKCSDevice, 
							pSession->hKCSSession->byResAddr,
						  byReq, sizeof(IPMIKCSRequest_T) + dwReqDataLen - 1, 
						  pbyResData, pdwResDataLen, timeout);
	
	return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
}


uint8	OpenKCSDriver(IPMI20_SESSION_T *pSession, char *pszDevice)
{

#if LIBIPMI_IS_OS_WINDOWS()
	/* For Win32 */
	pSession->hKCSSession->hKCSDevice = CreateFile (pszDevice, 
										GENERIC_READ | GENERIC_WRITE, 
										FILE_SHARE_READ | FILE_SHARE_WRITE,	
										NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 
										NULL);
	
	if ( pSession->hKCSSession->hKCSDevice == NULL )
	{
		DEBUG_PRINTF("Cannot Open KCS Driver\n");
		return LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
	}

#elif LIBIPMI_IS_OS_LINUX()
	/* Linux */
	
	pSession->hKCSSession->hKCSDevice = open(pszDevice, O_RDWR);
	
	if ( pSession->hKCSSession->hKCSDevice == -1 )
	{
		DEBUG_PRINTF("Cannot Open KCS Driver\n");
		return LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
	}
#else

	/* For any other OS, return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif

	return LIBIPMI_STATUS_SUCCESS;
}


uint8	CloseKCSDriver(IPMI20_SESSION_T *pSession)
{
#if LIBIPMI_IS_OS_WINDOWS()
	/* Win32 */
	CloseHandle(pSession->hKCSSession->hKCSDevice);
#elif LIBIPMI_IS_OS_LINUX()
	/* Linux */
	close(pSession->hKCSSession->hKCSDevice);
#else

	/* For any other OS, return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif

	return LIBIPMI_STATUS_SUCCESS;
}

uint8 SendDataToKCSDriver(HANDLE hKCSDevice, 
						  uint8	byResAddr,
						  uint8 *pbyInBuffer, uint32 dwInBuffLen, 
						  uint8 *pbyOutBuffer, uint32 *pdwOutBuffLen,
						  int timeout)
{
	uint8				byReq[64];
	uint8				byRes[64];
	ImbRequestBuffer*	pReqPkt;
	ImbResponseBuffer*	pResPkt;
	IPMIKCSRequest_T*	pIPMIReq;
	
	
	pReqPkt = (ImbRequestBuffer *) byReq;
	pResPkt = (ImbResponseBuffer *) byRes;
	pIPMIReq = (IPMIKCSRequest_T*) pbyInBuffer;

	pReqPkt->req.rsSa           = byResAddr;
	pReqPkt->req.cmd            = pIPMIReq->byCmd;
	pReqPkt->req.netFn          = ((pIPMIReq->byNetFnLUN >> 2) & 0x3F);
	pReqPkt->req.rsLun          = (pIPMIReq->byNetFnLUN & 0x03);
	pReqPkt->req.dataLength     = (uint8)(dwInBuffLen - sizeof(IPMIKCSRequest_T) + 1);
	memcpy ( pReqPkt->req.data, pIPMIReq->byData, dwInBuffLen - sizeof(IPMIKCSRequest_T) + 1);
	pReqPkt->flags              = 0;
	pReqPkt->timeOut			= 5000 * 1000; // convert to uSec units


#if LIBIPMI_IS_OS_WINDOWS()
{
	HANDLE				hEvt;
	BOOL				status;
	DWORD				respLength = sizeof(byRes);
	OVERLAPPED			ovl ;

	hEvt = CreateEvent (NULL, TRUE, FALSE, NULL) ;
	if (!hEvt)
	{ 
		DEBUG_PRINTF("Not able to create Event\n");
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE; 
	}
	ovl.hEvent = hEvt ;
	ResetEvent (hEvt) ;

	/* Device IO Control */
	status	= DeviceIoControl(  hKCSDevice,
								IOCTL_IMB_SEND_MESSAGE,
								byReq,
								sizeof( byReq ),
								& byRes,
								sizeof( byRes ),
								& respLength,
								&ovl);
	
	if (!status && (GetLastError() == ERROR_IO_PENDING)) 
	{
		WaitForSingleObject (hEvt, INFINITE) ;
		ResetEvent (hEvt) ;
	} 
	else 
	{
	    if (!status && (GetLastError() != ERROR_IO_PENDING))
		{
			CloseHandle (hEvt);
			return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
		}
	}

	/* Get the number of returned bytes */
	if ( ( GetOverlappedResult(hKCSDevice, &ovl, (LPDWORD)&respLength, TRUE ) == 0 ) 
		 || (respLength <= 0))
	{
		CloseHandle (hEvt);
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
	}
	
	/* Supplied memory is not sufficient to accomodate the response data */
	if( *pdwOutBuffLen < respLength )
		return LIBIPMI_MEDIUM_E_INVALID_PARAMS;

	/* Copy the data to output buffer and return */
	memcpy (pbyOutBuffer, byRes, respLength);
	
	*pdwOutBuffLen = respLength;
}
#elif LIBIPMI_IS_OS_LINUX()
{
	fd_set				fs;
	int					nDes,ret;
	struct timeval		wait_time, *pwait_time;
	int					nTotalBytesSent;
	int					nBytesSent;
	int					nBytesRecvd;

	/* Wait for the socket to get ready */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

	FD_ZERO(&fs);
	FD_SET(hKCSDevice, &fs);
	nDes = hKCSDevice + 1;
	
	/* wait for specified amount of time */
	ret = select (nDes, NULL, &fs, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
		DEBUG_PRINTF("Select failed while waiting for write pipe \n");
		return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
	}
	/* timed out */
	else if(ret == 0)
	{
		DEBUG_PRINTF("Connection timedout: Write Pipe is full \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}
	
	/* socket is ready to send data */
	nTotalBytesSent = 0;

	/* Till the last byte is sent */
	while( nTotalBytesSent < (int)dwInBuffLen )
	{
		/* send data */
		nBytesSent = write (hKCSDevice, pbyInBuffer, dwInBuffLen);
								
		/* send failure */
		if (nBytesSent == -1)
		{
			DEBUG_PRINTF("Error while writing data to KCS driver\n");
			return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
		}
		else
			nTotalBytesSent += nBytesSent;
	}

	/* Now receive the data */
	if(timeout >= 0)
	{
		wait_time.tv_sec = timeout;
		wait_time.tv_usec = 0;
		pwait_time = &wait_time;
	}
	else
		pwait_time = NULL;

	FD_ZERO(&fs);
	FD_SET(hKCSDevice, &fs);
	nDes = hKCSDevice + 1;
	
	/* wait for specified amount of time */
	ret = select (nDes, NULL, &fs, NULL, pwait_time);

	/* socket error */
	if(ret == SOCKET_ERROR)
	{
		DEBUG_PRINTF("Select failed while waiting for read pipe \n");
		return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
	}
	/* timed out */
	else if(ret == 0)
	{
		DEBUG_PRINTF("Connection timedout: read pipe is empty \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}
	
	/* socket is ready to receive data */
	nBytesRecvd = read (hKCSDevice, byRes, MAX_RESPONSE_SIZE);

	/* Error while receiving data */
	if(nBytesRecvd == SOCKET_ERROR)
	{
		DEBUG_PRINTF("Error while receiving data\n");
		return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
	}

	/* Supplied memory is not sufficient to accomodate the response data */
	if( *pdwOutBuffLen < (uint32)nBytesRecvd)
		return LIBIPMI_MEDIUM_E_INVALID_PARAMS;
	
	memcpy(pbyOutBuffer, byRes, nBytesRecvd);
	*pdwOutBuffLen = (uint32)nBytesRecvd;
}
#else

	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif
	
	return LIBIPMI_STATUS_SUCCESS;
}



