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
* Filename: libipmi_serial_session.c
*
* Description: Contains implementation of session establishment
*   functions to BMC thru serial medium
*
* Author: Rajasekhar
*
******************************************************************/
#include <stdlib.h>
#include <string.h>

#include "libipmi_session.h"
#include "libipmi_serial.h"
#include "libipmi_helper.h"

#if LIBIPMI_IS_OS_LINUX()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#endif

#if ( _WIN64 || _WIN32 ) 
#define snprintf sprintf_s
#endif


#define		START_BYTE					0xA0
#define		STOP_BYTE					0xA5
#define		HAND_SHAKE_BYTE				0xA6
#define		DATA_ESCAPE					0xAA
#define		BYTE_ESCAPE					0x1B

#define		ENCODED_START_BYTE			0xB0
#define		ENCODED_STOP_BYTE			0xB5
#define		ENCODED_HAND_SHAKE_BYTE		0xB6
#define		ENCODED_DATA_ESCAPE			0xBA
#define		ENCODED_BYTE_ESCAPE			0x3B


uint16 Send_RAW_IPMI2_0_Serial_Command(IPMI20_SESSION_T *pSession, 
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout);

uint8 WriteSerialPort(IPMI20_SESSION_T *pSession, uint8 *pbyData, 
						uint32 dwDataLen, int timeout);
uint8 ReadByte(IPMI20_SESSION_T *pSession, uint8 *pbyData, int timeout);
uint8 ReadSerialPort(IPMI20_SESSION_T *pSession, uint8 *pbyData, 
					 uint32 *pdwDataLen, int timeout);
uint8 OpenSerialPort(IPMI20_SESSION_T *pSession, char *pszDevice);
void  ByteEscapeEncode(uint8	*pbyInputData,	uint32 dwInputDataLen,
						 uint8	*pbyOutputData,	uint32 *pdwOutputDataLen);
uint8 ByteEscapeDecode(uint8	*pbyInputData,	uint32 dwInputDataLen,
						 uint8	*pbyOutputData,	uint32 *pdwOutputDataLen);
uint8 ValidateCheckSum(uint8	*pbyData, uint32 dwDataLen);
uint16 ChangeSerialPortSettings(IPMI20_SESSION_T *pSession);

extern uint8	CalculateCheckSum(uint8 *pbyStart, uint8 *pbyEnd);

/*---------------------------------------------------------------------
Create_IPMI20_Serial_Session

Establishes a connection with BMC using Serial medium.
----------------------------------------------------------------------*/
uint16 Create_IPMI20_Serial_Session(IPMI20_SESSION_T *pSession, char *pszDevice,
							char *pszUserName, char *pszPassword, 
							uint8 byAuthType,
							uint8 *byPrivLevel,
							uint8 byReqAddr, uint8 byResAddr,
							int timeout)
{
	uint8						byRet;
	uint16						wRet;
	uint32						dwResLen;
	GetSessionChallengeReq_T	SessChallReq;
	GetSessionChallengeRes_T	SessChallRes;
	ActivateSessionReq_T		ActvSessReq;
	ActivateSessionRes_T		ActvSessRes;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	
	/* supports only none and straight password authentication types */
	if(byAuthType != AUTHTYPE_NONE && byAuthType != AUTHTYPE_STRAIGHT_PASSWORD)
	{
		DEBUG_PRINTF("Authentication type not supported\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED);
	}

	/* allocate memory for serial session */
	pSession->hSerialSession = (IPMI20_SERIAL_SESSION_T*)malloc( sizeof(IPMI20_SERIAL_SESSION_T) );
	if(pSession->hSerialSession == NULL)
	{
		DEBUG_PRINTF("Error in allocating memory \n");
		return -1;
	}
	memset(pSession->hSerialSession, 0, sizeof(IPMI20_SERIAL_SESSION_T) );

	/* Open Serial Port */
	byRet = OpenSerialPort(pSession, pszDevice);

	/* return if error */
	if(byRet != LIBIPMI_STATUS_SUCCESS)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
	
	/* Set the default settings if nothing is set */
	if(pSession->Settings.bySettings != SETTINGS_DEFAULT &&
		pSession->Settings.bySettings != SETTINGS_USER )
		LIBIPMI_SetDefaultSerialPortSettings(pSession);
	
	/* Set Serial Port Settings */
	wRet = ChangeSerialPortSettings(pSession);
	
	/* return if error */
	if(wRet != LIBIPMI_E_SUCCESS)
		return wRet;

	/* Fill the parameters required for later requests to BMC */
	tempret=snprintf (pSession->hSerialSession->szUserName,MAX_USER_NAME_LEN,"%s", pszUserName);
        if((tempret<0)||(tempret>=MAX_USER_NAME_LEN))
        {
             DEBUG_PRINTF("BUFFER OVERFLOW");
             return -1;
        }
	tempret=snprintf (pSession->hSerialSession->szPwd, MAX_USER_PWD_LEN,"%s",pszPassword);
        if((tempret<0)||(tempret>=MAX_USER_PWD_LEN))
        {
            DEBUG_PRINTF("BUFFER OVERFLOW");
            return -1;
        }
	pSession->hSerialSession->byPrivLevel		= *byPrivLevel;
	pSession->hSerialSession->byAuthType		= byAuthType;
	pSession->hSerialSession->byReqAddr			= byReqAddr;
	pSession->hSerialSession->byResAddr			= byResAddr;
	pSession->hSerialSession->byIPMBSeqNum		= 1;
	pSession->byMediumType						= SERIAL_MEDIUM;

	
	/***** GetSessionChallengeRequest ******/
	memset(&SessChallReq, 0, sizeof(GetSessionChallengeReq_T));
	SessChallReq.AuthType = byAuthType;
	memcpy(SessChallReq.UserName, pszUserName, strlen(pszUserName) );

	dwResLen = sizeof(GetSessionChallengeRes_T);
	
	wRet = Send_RAW_IPMI2_0_Serial_Command(pSession,
										DEFAULT_NET_FN_LUN, CMD_GET_SESSION_CHALLENGE,
										(uint8 *)&SessChallReq, 
										sizeof(GetSessionChallengeReq_T),
										(uint8 *)&SessChallRes, 
										&dwResLen,
										timeout);
	if( wRet != LIBIPMI_E_SUCCESS)
	{
		DEBUG_PRINTF("GetSessionChallengeRequest failed\n");
		return wRet;
	}

	/*	Activate Session Request */
	memset(&ActvSessReq, 0, sizeof(ActivateSessionReq_T) );
	ActvSessReq.AuthType = byAuthType;
	ActvSessReq.PrivLevel = *byPrivLevel;
	tempret=snprintf((char*)&ActvSessReq.Challenge[0],sizeof(ActvSessReq.Challenge),"%s",(char*)pszPassword);
	if((tempret<0)||(tempret>=(signed)sizeof(ActvSessReq.Challenge)))
        {
            DEBUG_PRINTF("BUFFER OVERFLOW");
            return -1;
        }
	ActvSessReq.OutboundSeqNum = 1;

	dwResLen = sizeof(ActivateSessionReq_T);
	
	wRet = Send_RAW_IPMI2_0_Serial_Command(pSession,
										DEFAULT_NET_FN_LUN, CMD_ACTIVATE_SESSION,
										(uint8 *)&ActvSessReq, 
										sizeof(ActivateSessionReq_T),
										(uint8 *)&ActvSessRes, 
										&dwResLen,
										timeout);

	if( wRet != LIBIPMI_E_SUCCESS)
	{
		DEBUG_PRINTF("ActivateSessionRequest failed\n");
		return wRet;
	}

	pSession->hSerialSession->dwSessionID	= ActvSessRes.SessionID;
	pSession->bySessionStarted				= SESSION_STARTED;


	/* Serial port opened successfully */
	return LIBIPMI_E_SUCCESS;
}

/*---------------------------------------------------------------------
Close_IPMI20_Serial_Session

Closes connection with BMC using Serial medium.
----------------------------------------------------------------------*/
uint16 Close_IPMI20_Serial_Session( IPMI20_SESSION_T *pSession )
{
	uint16	wRet;
	uint32	Req;
	uint8	byRes[MAX_RESPONSE_SIZE];
	uint32	dwResLen;

	Req = pSession->hSerialSession->dwSessionID;
	wRet = Send_RAW_IPMI2_0_Serial_Command(pSession, 
										DEFAULT_NET_FN_LUN, CMD_CLOSE_SESSION,
										(uint8 *)&Req, sizeof(uint32),
										byRes, &dwResLen,
										pSession->hSerialSession->byDefTimeout);
	return wRet;
}

/*---------------------------------------------------------------------
Send_RAW_IPMI2_0_Serial_Command

Sends RAW IPMI command to BMC using serial medium
----------------------------------------------------------------------*/
uint16 Send_RAW_IPMI2_0_Serial_Command(IPMI20_SESSION_T *pSession,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pbyReqData, uint32 dwReqDataLen,
										uint8 *pbyResData, uint32 *pdwResDataLen,
										int	timeout)
{
	uint8			pbyRequestData[MAX_REQUEST_SIZE];
	uint8			pbyResponseData[MAX_RESPONSE_SIZE];
	uint8			EncryptBuf[MAX_REQUEST_SIZE];
	uint32			dwReqLen;
	uint32			dwResLen;
	uint32			dwEncryptBuffLen;
	IPMIMsgHdr_T	*pbyIPMIHdr;
	uint8			*pbyIPMIData;
	uint8			byRet;
	uint8			byCompletionCode;
	
	/* Find the IPMI Header and IPMI Data Part */
	pbyIPMIHdr = (IPMIMsgHdr_T	*)&pbyRequestData[ 0 ];
	pbyIPMIData = &pbyRequestData[ sizeof(IPMIMsgHdr_T) ];
	

	/* Fill the packet */
	pbyIPMIHdr->ResAddr 	= pSession->hSerialSession->byResAddr;
	pbyIPMIHdr->ReqAddr 	= pSession->hSerialSession->byReqAddr;
	pbyIPMIHdr->NetFnLUN 	= byNetFnLUN;
	pbyIPMIHdr->RqSeqLUN	= pSession->hSerialSession->byIPMBSeqNum << 2;
	pbyIPMIHdr->Cmd		= byCommand;
	pbyIPMIHdr->ChkSum	= LIBIPMI_CalculateCheckSum( (uint8 *)pbyIPMIHdr, (uint8 *)&(pbyIPMIHdr->ChkSum) ); /* cheksum_1 */

	//printf("libipmi_raw_serial: req len : %ld\n",dwReqDataLen);
	/* Increment the sequence number */
	pSession->hSerialSession->byIPMBSeqNum = (pSession->hSerialSession->byIPMBSeqNum+1) % 0x40;

	/* Fill the Data part  */
    if(dwReqDataLen<=(MAX_REQUEST_SIZE-sizeof(IPMIMsgHdr_T) - 1))
    {
        memcpy(pbyIPMIData, pbyReqData, dwReqDataLen);
    }
    else
    {
        return -1;
    }
	
	/* Calculate CheckSum-2*/
	pbyIPMIData[dwReqDataLen] = LIBIPMI_CalculateCheckSum( (&(pbyIPMIHdr->ChkSum))+1, &pbyIPMIData[dwReqDataLen] );

	/* Update the request data length */
	dwReqLen = sizeof(IPMIMsgHdr_T) + dwReqDataLen + 1;
	//printf("libipmi_raw_serial: sizeof(IPMIMsgHdr_T) : %d\n",sizeof(IPMIMsgHdr_T));

	//printf("libipmi_raw_serial: before bytes escape encode : %ld\n",dwReqLen);
	/* Byte Escape Encoding */
	ByteEscapeEncode(&pbyRequestData[0], dwReqLen, &EncryptBuf[0], &dwEncryptBuffLen);

	//printf("libipmi_raw_serial: total bytes written: %ld\n",dwEncryptBuffLen);
	/* Send the data to BMC */
	byRet = WriteSerialPort(pSession, &EncryptBuf[0], dwEncryptBuffLen, timeout);

	/* Get response */
	if(byRet == LIBIPMI_STATUS_SUCCESS)
		byRet = ReadSerialPort(pSession, &pbyResponseData[0], &dwResLen, timeout);
	
	/* Return on error */
	if(byRet != LIBIPMI_STATUS_SUCCESS)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);

	/* Byte Escape Decoding */
	ByteEscapeDecode(&pbyResponseData[0], dwResLen, &EncryptBuf[0], &dwEncryptBuffLen);

	/* Validate CheckSum */
	if(!LIBIPMI_ValidateCheckSum(&EncryptBuf[0], dwEncryptBuffLen))
		return	STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_CHKSUM_MISMATCH);

	/* Completion Code */
	byCompletionCode = EncryptBuf[ sizeof(IPMIMsgHdr_T) ];
	if( byCompletionCode != CC_NORMAL )
		return	STATUS_CODE(IPMI_ERROR_FLAG, byCompletionCode );
	
	/* Calculate response data len */
	*pdwResDataLen = dwEncryptBuffLen - sizeof(IPMIMsgHdr_T) - 1;
		
	memcpy(pbyResData, &EncryptBuf[ sizeof(IPMIMsgHdr_T) ], *pdwResDataLen);

	return LIBIPMI_E_SUCCESS;
}

/*---------------------------------------------------------------------
LIBIPMI_SetSerialPortSettings

Sets serial port settings 
----------------------------------------------------------------------*/
uint16 ChangeSerialPortSettings(IPMI20_SESSION_T *pSession)
{

#if LIBIPMI_IS_OS_WINDOWS()
	DCB				PortDCB;
	
	PortDCB.DCBlength = sizeof (DCB);

	GetCommState (pSession->hSerialSession->hSerialPort, &PortDCB);

	/* BaudRate */
	PortDCB.BaudRate		= pSession->Settings.dwBaudRate;
	
	/* Byte Size */
	PortDCB.ByteSize		= pSession->Settings.byByteSize;
	
	/* Parity */
	switch(pSession->Settings.byParity)
	{
		case EVEN_PARITY:
			PortDCB.Parity	= EVENPARITY; break;
		case ODD_PARITY:
			PortDCB.Parity	= ODDPARITY; break;
		case NO_PARITY:
			PortDCB.Parity	= NOPARITY; break;
		case MARK_PARITY:
			PortDCB.Parity	= MARKPARITY; break;
		case SPACE_PARITY:
			PortDCB.Parity	= SPACEPARITY; break;
		default:
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);
	}

	/* Stop Bits */
	switch(pSession->Settings.byStopBit)
	{
		case STOPBIT_ONE:
			PortDCB.StopBits = ONESTOPBIT; break;
		case STOPBIT_ONE5:
			PortDCB.StopBits = ONE5STOPBITS; break;
		case STOPBIT_TWO:
			PortDCB.StopBits = TWOSTOPBITS; break;
		default:
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);
	}
		
	if (!SetCommState (pSession->hSerialSession->hSerialPort, &PortDCB))
	{
		DEBUG_PRINTF("Unable to set params\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);
	}

#elif LIBIPMI_IS_OS_LINUX()
	/* For linux */
	struct termios ttysettings;
	//speed_t spd;
	
	/* Get comm port settings */
	if (tcgetattr (pSession->hSerialSession->hSerialPort, &ttysettings) < 0)
	{
		DEBUG_PRINTF("Unable to get serial port settings\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);	
	}

#if 0
	
	ttysettings.c_iflag = 0;
	ttysettings.c_oflag = 0;
	ttysettings.c_cflag = CS8 | CLOCAL | CREAD;
	ttysettings.c_lflag = 0;
	ttysettings.c_cc[VMIN] = 0;
	ttysettings.c_cc[VTIME] = 0;

	/* baud rate */
	switch (pSession->Settings.dwBaudRate)
	{
		case 300:
			spd = B300;	break;
		case 600:
			spd = B600; break;
		case 1200:
			spd = B1200;break;
		case 2400:
			spd = B2400;break;
		case 9600:
			spd = B9600;break;
		case 19200:
			spd = B19200;break;
		case 38400:
			spd = B38400;break;
		case 57600:
			spd = B57600;break;
		case 115200:
			spd = B115200;break;
		case 230400:
			spd = B230400;break;
		default:
			DEBUG_PRINTF("Inavlid BaudRate %lu\n", pSession->Settings.dwBaudRate);
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);
	}

	/* Byte Size */
	if (pSession->Settings.byByteSize == 7)
	{
		ttysettings.c_cflag &= ~CS8;
		ttysettings.c_cflag |= CS7;
	}

	/* Stop Bits */
	if (pSession->Settings.byStopBit == STOPBIT_TWO )
		ttysettings.c_cflag |= CSTOPB;
		
	/* Parity Bits */
	if( pSession->Settings.byParity != NO_PARITY )
	{
		ttysettings.c_cflag |= PARENB;
		if (pSession->Settings.byParity != EVEN_PARITY )
				ttysettings.c_cflag |= PARODD;
	}
//    ttysettings.c_cflag |= CRTSCTS;            /* Enable RTS/CTS flow  */

	/* set the serial port settings */
	if (cfsetospeed (&ttysettings, spd) < 0)
	{
		DEBUG_PRINTF("Unable to set serial port settings\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);	
	}
	if (cfsetispeed (&ttysettings, spd) < 0)
	{
		DEBUG_PRINTF("Unable to set serial port settings\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);	
	}
	if (tcsetattr(pSession->hSerialSession->hSerialPort, 
					TCSANOW, &ttysettings) < 0)
	{
		DEBUG_PRINTF("Unable to get serial port settings\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_INVALID_PARAMS);	
	}
#endif
	cfsetospeed(&ttysettings, B115200);
	cfsetispeed(&ttysettings, B115200);
	
	ttysettings.c_cflag |= CS8;              /* Set 8bits/charecter          */
	ttysettings.c_cflag &= ~CSTOPB;          /* set framing to 1 stop bits   */
	ttysettings.c_cflag &= ~(PARENB);        /* set parity to NONE           */
	ttysettings.c_iflag &= ~(INPCK);


    ttysettings.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);     /* Raw input mode */
    ttysettings.c_oflag &= ~(OCRNL | ONLCR | ONOCR | ONLRET);
    ttysettings.c_iflag &= ~(IXON | IXOFF);     /* no sw flow ctrl */
    ttysettings.c_cflag |= CRTSCTS;            /* Enable RTS/CTS flow  */
    ttysettings.c_iflag &= ~INLCR;
    ttysettings.c_iflag &= ~ICRNL;
    ttysettings.c_iflag &= ~IGNCR;



     tcflush(pSession->hSerialSession->hSerialPort, TCIFLUSH);

    /* set the new attributes in the tty driver */
    tcsetattr(pSession->hSerialSession->hSerialPort, TCSANOW, &ttysettings);
#else

	/* For any other OS, return error */
	return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_MEDIUM_E_OS_UNSUPPORTED);

#endif
	
	/* Maximum retries
	pSession->hSerialSession->byMaxRetries = byMaxRetries;
	*/

	return LIBIPMI_E_SUCCESS;
}

/*---------------------------------------------------------------------
WriteSerialPort

Writes data to the serial port between A0 and A5
----------------------------------------------------------------------*/
uint8 WriteSerialPort(IPMI20_SESSION_T *pSession, uint8 *pbyData, 
					  uint32 dwDataLen, int timeout)
{
	uint8	byTempData=0;
	uint8	byRet;

#if LIBIPMI_IS_OS_WINDOWS()	
	uint32	dwBytesWritten;
	COMMTIMEOUTS	CommTimeOuts;
	/* Win32 */
	
	/* Get the timeouts */
   	GetCommTimeouts (pSession->hSerialSession->hSerialPort, &CommTimeOuts);

	CommTimeOuts.ReadIntervalTimeout		= 1000;
	CommTimeOuts.ReadTotalTimeoutMultiplier = timeout ;
	CommTimeOuts.ReadTotalTimeoutConstant	= 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier= timeout;
	CommTimeOuts.WriteTotalTimeoutConstant	= 2 ;

	/* Set the timeouts */
	if (!SetCommTimeouts (pSession->hSerialSession->hSerialPort, &CommTimeOuts))
	{
		DEBUG_PRINTF("Unable to set params\n");
		return LIBIPMI_MEDIUM_E_INVALID_PARAMS;
	}

	/* Flush the port before writing */
	if (0 == PurgeComm (pSession->hSerialSession->hSerialPort, 
							PURGE_RXCLEAR | PURGE_TXCLEAR) ) 
		DEBUG_PRINTF("Error flushing the buffer\n");
		
	
	/* write to the port  */
	if (!WriteFile (pSession->hSerialSession->hSerialPort,
						pbyData, dwDataLen, &dwBytesWritten, NULL))
	{
		DEBUG_PRINTF("Error writing Bytes\n");
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
	}

	if(dwDataLen != dwBytesWritten)
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;

#elif LIBIPMI_IS_OS_LINUX()
	fd_set				wdfs;
	struct timeval		wait_time;
	int					nRet;

	FD_ZERO(&wdfs);
	FD_SET(pSession->hSerialSession->hSerialPort, &wdfs);
	
	/* set the timeout */
	wait_time.tv_sec=timeout;
	wait_time.tv_usec=0;
	nRet = select(pSession->hSerialSession->hSerialPort+1, NULL, 
						&wdfs, NULL, &wait_time);
	
	/* error */
	if(nRet == -1)
	{
		DEBUG_PRINTF("Select failed while waiting for write pipe \n");
		return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
	}
	/* timeout happened */
	else if(nRet == 0)
	{
		DEBUG_PRINTF("Connection timedout: Write Pipe is full \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}


	//printf("LIBIPMI: datalen = %ld\n",dwDataLen);
#if 1
	/* write pipe is free. write the data */
	nRet = write(pSession->hSerialSession->hSerialPort, pbyData, dwDataLen);
	if(nRet != (int)dwDataLen)
	{
		printf("Error writing to serial: bytes written : %d != %d\n",nRet,dwDataLen);
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
	}
#endif
#if 0
	numBytes = dwDataLen;
	while(numBytes)
	{
		if(numBytes > 4000)
		{
			wBytes = 4000;
		//	sleep(7);
		}
		else
			wBytes = numBytes;

		//if(numBytes > 1000)
		//	sleep(7);
		//printf("writing %ld\n",wBytes);
		/* write pipe is free. write the data */
		nRet = write(pSession->hSerialSession->hSerialPort, pbyData, wBytes);
		if(nRet != (int)wBytes)
		{
			//printf("LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE %d, %s\n",nRet,strerror(errno));
			return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
		}
			//printf("bytes written %d\n",nRet);
		numBytes -= wBytes;
		pbyData += wBytes;
	}
#endif



#else

	/* For any other OS, return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif

	/* Receive Hand Shake */
	if( (byRet = ReadByte(pSession, &byTempData, timeout)) != LIBIPMI_STATUS_SUCCESS )
	{
		return byRet;
	}

	if(byTempData != HAND_SHAKE_BYTE)
	{
		return LIBIPMI_SESSION_E_HANDSHAKE_NOT_RECVD;
	}
	//printf("LIBIPMI_SESSION_E_HANDSHAKE_RECVD\n");
	return LIBIPMI_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------
ReadByte

Reads a byte from Serial Port
----------------------------------------------------------------------*/
uint8 ReadByte(IPMI20_SESSION_T *pSession, uint8 *pbyData, int timeout)
{
	
#if LIBIPMI_IS_OS_LINUX()
	if(0)
	{
		timeout=timeout; /* -Wextra: Flag added for strict compilation. */
	}
#endif
#if LIBIPMI_IS_OS_WINDOWS()	
	DWORD	dwBytesRead;
	
	/* write to the port  */
	if (!ReadFile (pSession->hSerialSession->hSerialPort,
						pbyData, 1, &dwBytesRead, NULL))
	{
		DEBUG_PRINTF("Error reading Bytes\n");
		return LIBIPMI_MEDIUM_E_RECV_DATA_FAILURE;
	}

	if(dwBytesRead == 0)
	{
		//DEBUG_PRINTF("Read pipe is empty - timedout\n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}


#elif LIBIPMI_IS_OS_LINUX()
	fd_set				rdfs;
	struct timeval		wait_time;
	int					nRet;


	FD_ZERO(&rdfs);
	FD_SET(pSession->hSerialSession->hSerialPort, &rdfs);
	
	/* set the timeout */
	//wait_time.tv_sec=timeout;
	wait_time.tv_sec=30;
	wait_time.tv_usec=0;
	nRet = select(pSession->hSerialSession->hSerialPort+1, &rdfs, 
						NULL, NULL, &wait_time);
	
	/* error */
	if(nRet == -1)
	{
		DEBUG_PRINTF("Select failed while waiting for read pipe \n");
		printf("Select failed while waiting for read pipe \n");
		return LIBIPMI_MEDIUM_E_INVALID_SOCKET;
	}
	/* timeout happened */
	else if(nRet == 0)
	{
		DEBUG_PRINTF("Connection timedout: Read Pipe is empty \n");
		printf("Connection timedout: Read Pipe is empty \n");
		return LIBIPMI_MEDIUM_E_TIMED_OUT;
	}

	/* read pipe has some data. read the data */
	nRet = read(pSession->hSerialSession->hSerialPort, pbyData, 1);

	if(nRet != 1)
		return LIBIPMI_MEDIUM_E_SEND_DATA_FAILURE;
#else

	/* For any other OS return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif
	
	return LIBIPMI_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------
ReadSerialPort

Reads data from Serial Port
----------------------------------------------------------------------*/
uint8 ReadSerialPort(IPMI20_SESSION_T *pSession, uint8 *pbyData, 
					 uint32 *pdwDataLen, int timeout)
{
	uint8	byTempData;
	uint8	byRet;
	
#if LIBIPMI_IS_OS_WINDOWS()	
	uint32	dwBytesWritten=0;
	COMMTIMEOUTS	CommTimeOuts;
	/* Win32 */
	
	/* Get the timeouts */
   	GetCommTimeouts (pSession->hSerialSession->hSerialPort, &CommTimeOuts);

	CommTimeOuts.ReadIntervalTimeout		= 1000;
	CommTimeOuts.ReadTotalTimeoutMultiplier = timeout ;
	CommTimeOuts.ReadTotalTimeoutConstant	= 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier= timeout;
	CommTimeOuts.WriteTotalTimeoutConstant	= 2 ;

	/* Set the timeouts */
	if (!SetCommTimeouts (pSession->hSerialSession->hSerialPort, &CommTimeOuts))
	{
		DEBUG_PRINTF("Unable to set params\n");
		return LIBIPMI_MEDIUM_E_INVALID_PARAMS;
	}

#endif
	byTempData = 0x00;

	/* Receive A0 from serial port */
	if( (byRet = ReadByte(pSession, &byTempData, timeout)) != LIBIPMI_STATUS_SUCCESS )
		return byRet;
		
	if(byTempData != START_BYTE)
		return LIBIPMI_MEDIUM_E_INVALID_DATA;
	
	*pdwDataLen = 0;
	
	/* Now Receive data till A5 */
	while( (byRet = ReadByte(pSession, &byTempData, timeout)) == LIBIPMI_STATUS_SUCCESS )
	{
		if(byTempData != STOP_BYTE)
			pbyData[(*pdwDataLen)++] = byTempData;
		else
			break;
	}

	/* Data received. Check if any error occured in between */
	if( byRet != LIBIPMI_STATUS_SUCCESS)
		return byRet;

		
	return LIBIPMI_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------
OpenSerialPort

Opens the serial port
----------------------------------------------------------------------*/
uint8	OpenSerialPort(IPMI20_SESSION_T *pSession, char *pszDevice)
{
	HANDLE		hSerialPort;

#if LIBIPMI_IS_OS_WINDOWS()
	
	/* Open Serial port for read and write */
	hSerialPort = CreateFile (pszDevice,
								GENERIC_READ | GENERIC_WRITE,
								0,
								NULL,
								OPEN_EXISTING,
			  					FILE_ATTRIBUTE_NORMAL,
								NULL);
	
	/* Error occured while opening the device */
	if(hSerialPort == INVALID_HANDLE_VALUE ) 
	{
		DEBUG_PRINTF("Unable to open Serial Port %s\n", pszDevice);
		return LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
	}

#elif LIBIPMI_IS_OS_LINUX()
	
	/* Open Serial port for read and write */
	hSerialPort = open(pszDevice, O_RDWR);

	/* Error occured while opening the device */
	if(hSerialPort == -1 ) 
	{
		DEBUG_PRINTF("Unable to open Serial Port %s\n", pszDevice);
		return LIBIPMI_MEDIUM_E_CONNECT_FAILURE;
	}

#else
	
	/* For any other OS return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif
	

	/* handle got successfully */
	pSession->hSerialSession->hSerialPort = hSerialPort;
	
	return LIBIPMI_STATUS_SUCCESS;
}


/*---------------------------------------------------------------------
ByteEscapeEncode

Encodes the data using byte Escape
IPMI Spec Page-204 Table 14-5 & 14-6
----------------------------------------------------------------------*/
void ByteEscapeEncode(uint8	*pbyInputData,	uint32 dwInputDataLen,
						 uint8	*pbyOutputData,	uint32 *pdwOutputDataLen)
{
	uint32 i;

	*pdwOutputDataLen = 0;
	pbyOutputData[(*pdwOutputDataLen)++] = START_BYTE;
	for(i = 0; i < dwInputDataLen; i++)
	{
		switch(pbyInputData[i])
		{
			case START_BYTE:
				pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
				pbyOutputData[(*pdwOutputDataLen)++] = ENCODED_START_BYTE;
				break;
			case STOP_BYTE:
				pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
				pbyOutputData[(*pdwOutputDataLen)++] = ENCODED_STOP_BYTE;
				break;
			case HAND_SHAKE_BYTE:
				pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
				pbyOutputData[(*pdwOutputDataLen)++] = ENCODED_HAND_SHAKE_BYTE;
				break;
			case DATA_ESCAPE:
				pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
				pbyOutputData[(*pdwOutputDataLen)++] = ENCODED_DATA_ESCAPE;
				break;
			case BYTE_ESCAPE:
				pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
				pbyOutputData[(*pdwOutputDataLen)++] = ENCODED_BYTE_ESCAPE;
				break;
			default:
				pbyOutputData[(*pdwOutputDataLen)++] = pbyInputData[i];
				break;
		}
	}
	pbyOutputData[(*pdwOutputDataLen)++] = STOP_BYTE;

	return;
}

/*---------------------------------------------------------------------
ByteEscapeDecode

Decodes the data using byte Escape
IPMI Spec Page-204 Table 14-5 & 14-6
----------------------------------------------------------------------*/
uint8 ByteEscapeDecode(uint8	*pbyInputData,	uint32 dwInputDataLen,
						 uint8	*pbyOutputData,	uint32 *pdwOutputDataLen)
{
	uint32	i;
	uint8	byESCByteRecvd=0;

	*pdwOutputDataLen = 0;
	for(i = 0; i < dwInputDataLen; i++)
	{
		/* ESC byte already received so decode the next byte */
		if( byESCByteRecvd )
		{
			switch(pbyInputData[i])
			{
				case ENCODED_START_BYTE:
					pbyOutputData[(*pdwOutputDataLen)++] = START_BYTE;
					break;
				case ENCODED_STOP_BYTE:
					pbyOutputData[(*pdwOutputDataLen)++] = STOP_BYTE;
					break;
				case ENCODED_HAND_SHAKE_BYTE:
					pbyOutputData[(*pdwOutputDataLen)++] = HAND_SHAKE_BYTE;
					break;
				case ENCODED_DATA_ESCAPE:
					pbyOutputData[(*pdwOutputDataLen)++] = DATA_ESCAPE;
					break;
				case ENCODED_BYTE_ESCAPE:
					pbyOutputData[(*pdwOutputDataLen)++] = BYTE_ESCAPE;
					break;
				default:
					return LIBIPMI_MEDIUM_E_INVALID_DATA;					
			}
			
			byESCByteRecvd = 0;
		}
		else if( pbyInputData[i] == DATA_ESCAPE ) /* ESC received just now */
			byESCByteRecvd = 1;
		else /* Normal data */
			pbyOutputData[(*pdwOutputDataLen)++] = pbyInputData[i];
	}

	return LIBIPMI_STATUS_SUCCESS;
}
						
/*---------------------------------------------------------------------
CloseSerialPort

Closes the Serial Port
----------------------------------------------------------------------*/
uint8 CloseSerialPort(IPMI20_SESSION_T *pSession)
{

#if LIBIPMI_IS_OS_WINDOWS()
	/* Win32*/
	CloseHandle(pSession->hSerialSession->hSerialPort);

#elif LIBIPMI_IS_OS_LINUX()
	/* Linux */
	close(pSession->hSerialSession->hSerialPort);

#else
	
	/* For any other OS return error */
	return LIBIPMI_MEDIUM_E_OS_UNSUPPORTED;

#endif

	return LIBIPMI_STATUS_SUCCESS;
}





