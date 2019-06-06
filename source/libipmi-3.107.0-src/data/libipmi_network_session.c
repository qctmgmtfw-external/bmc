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
* Filename: libipmi_network_session.c
*
* Descriptions: Contains implementation of session establishment
*   functions to BMC thru network medium
*
* Author: Rajasekhar
*
******************************************************************/
#include <stdlib.h>
#include <string.h>

#include "libipmi_session.h"
#include "libipmi_rmcp.h"
#include "libipmi_helper.h"
#include "IPMI_AMIDevice.h"
#include "std_macros.h"
#include "dbgout.h"
#if LIBIPMI_IS_OS_WINDOWS()
#include <windows.h>
#endif
#ifdef ICC_OS_WINDOWS
#include <Wincrypt.h>
#else
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#endif
#if ( _WIN64 || _WIN32 ) 
#define snprintf sprintf_s
#endif


#define ENCRYPT_MASK									0x80
#define AUTH_MASK										0x40

#define MAX_REQUEST_SIZE								1024 * 60
#define MAX_IPMI_MESSAGE_SIZE							1024 * 60

#define	AES_CBC_INIT_VECTOR_SIZE 						16
#define	MAX_AES_BLOCK_SIZE								16

#define	MAX_AUTH_CODE_SIZE								12

/* IPMI_20_CONSTANT_VALUES */
#define	IPMI_20_NEXT_HEADER_CONSTANT					0x07

#define MAX_HMAC_SIZE									20
#define INT_CHK_VAL_SIZE								12
#define	MAX_KEY1_CONST_SIZE								20
#define	MAX_KEY2_CONST_SIZE								20

#define MAX_INIT_VECTOR_SIZE							16
#define	MAX_GUID_SIZE									16
#define MSG2_AUTH_CODE_LEN								20
#define MSG3_AUTH_CODE_LEN								20
#define MAX_SIK_SIZE									20
#define CURRENT_CHANNEL_NUM								0x0e

#define ROLE_LOOKUP_NAME_ONLY							0x1
#define ROLE_LOOKUP_NAME_AND_PRIV						0x0

#define IP_ADDR_LEN 										4



uint16 Send_RAW_IPMI2_0_Network_Command(IPMI20_SESSION_T *pSession, uint8 byPayLoadType,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pszReqData, uint32 dwReqDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										int	timeout);
uint8 Session_IPMI2_0_Send_Command(IPMI20_SESSION_T *pSession, uint8 *pbyPayLoadType,
										uint8 *pbyIPMIData, uint32 dwIPMIDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										int	timeout);
/*** Helper Functions ***/
void FillNetworkSessionHeader(IPMI20_SESSION_T *pSession, uint8 byPayLoadType,
										SessionHdr2_T*	pSessionHeader);
void FillNetworkSessionTrailer(IPMI20_SESSION_T *pSession,
								SessionHdr2_T*	pSessionHeader,
							   uint8 *pbyIPMISessionTrailer, uint32 dwIPMIPktLen,
							   uint32 *pdwTotalPktLen);
uint32 FillNetworkIPMIMessage(IPMI20_SESSION_T *pSession, uint8 *Buffer,
							uint8 byNetFnLUN, uint8 byCommand,
							uint8 *pbyReqData, uint32 dwReqDataLen);

uint8 IsPreSessionMessage(uint8 byPayLoadType, uint8 byResPayLoadType );


/*** Authentication Helper Functions ***/
void AuthAlg_HMAC_SHA1(uint8 *szKey, uint16 wKeyLen,
						uint8 *pbyHMAC, uint16 wHMACLen,
						uint8 *pResultAuthCode, uint16 wAuthCodeLen);

/*** Encryption Helper Functions ***/
uint8 EncryptIPMIPacket(IPMI20_SESSION_T *pSession, uint8 *pbyInputBuffer, uint32 dwInputDataLen, uint32 dwMaxInputBuffSize,
							uint8 *pbyOutputBuffer, uint16 *pwOutputBuffLen);

uint8 AES_CBC_Encrypt(uint8 *pbyInputBuffer, uint32 dwInputDataLen,
						uint32 dwMaxInputBuffSize, uint8 *pbyOutputBuffer,
						uint16 *pwOutputBuffLen, uint8 *pKey);

/*** Decryption Helper Functions ***/
uint8 DecryptIPMIPacket(IPMI20_SESSION_T* pSession, uint8 *pbyInputBuffer,
						 uint32 dwInputDataLen,	uint8 *pbyOutputBuffer,
						 uint32 *pdwOutputBuffLen);
uint8 AES_CBC_Decrypt(uint8 *pbyInputBuffer, uint32 dwInputDataLen,
					 uint8 *pbyOutputBuffer, uint32 *pdwOutputBuffLen,
					 uint8 *pKey);

/*** Send Command Functions ***/
uint16 GetChannelAuthCapabilities(IPMI20_SESSION_T *pSession,
								  uint8 byPrivLevel, uint8 *pbyUseKG, int timeout);
uint16 SendOpenSessionRequest(IPMI20_SESSION_T *pSession, uint8 byAuthAlg,
							  uint8 byIntegrityAlg, uint8 byConfAlg,
							  uint8 *pbyResPkt, uint32 *pdwIPMIResLen,
							  int timeout);
uint16 SendRAKPMessage_1(IPMI20_SESSION_T *pSession, uint8 *pbyResPkt,
							uint32 *pdwIPMIResLen, int timeout);
uint16 SendRAKPMessage_3(IPMI20_SESSION_T *pSession, uint8 *pbyResPkt,
                            uint32 *pdwIPMIResLen, int timeout, uint8 RakpMsg2status);

/*** Validate Message Functions ***/
uint8 ValidationSessionHeader(IPMI20_SESSION_T *pSession,
								SessionHdr2_T*	pSessionHeader);
uint8 ValidationSessionTrailer(IPMI20_SESSION_T* pSession, uint8 *pbyResData);
uint16 ValidateOpenSessionResponse(IPMI20_SESSION_T *pSession,
								    uint8 *pbyIPMIRes, uint32 dwIPMIResLen);
uint16 ValidateRAKPMessage_2(IPMI20_SESSION_T *pSession, uint8 *pbyIPMIRes,
								uint32 dwIPMIResLen);
uint16 ValidateRAKPMessage_4(IPMI20_SESSION_T *pSession, uint8 *pbyIPMIRes,
							 uint32 dwIPMIResLen, uint8 *byKG,
							 uint16 wKGLen);


/******************* Session Establishment functions ******************/
/*---------------------------------------------------------------------
Create_IPMI20_Network_Session

Establishes a connection with BMC using Network medium.
----------------------------------------------------------------------*/
uint16 Create_IPMI20_Network_Session(IPMI20_SESSION_T *pSession, char *pszIPAddress, uint16 wPort,
							char *pszUserName, char *pszPassword,  uint8 bProtocolType,
							uint8 byMaxRetries,
							uint8 byAuthType,
							uint8 byEncryption, uint8 byIntegrity, uint8 *byPrivLevel,
							uint8 byReqAddr, uint8 byResAddr,
							uint8 *byKG, uint16 wKGLen,
							int timeout)
{
	uint8					pbyIPMIRes [MAX_RESPONSE_SIZE];
	uint32					dwIPMIResLen;
	uint8					byRetvalue;
	uint16					wRetvalue;
	uint8					byUseKG=0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	

	/* Allocate memory for holding network session information */
	pSession->hNetworkSession = (IPMI20_NETWORK_SESSION_T*)malloc( sizeof(IPMI20_NETWORK_SESSION_T) );
	if(pSession->hNetworkSession == NULL)
	{
        	DEBUG_PRINTF("Error in allocating memory \n");
        	return -1;
	}

	/*initialize the allocated memory to zero */
	memset(pSession->hNetworkSession, 0, sizeof(IPMI20_NETWORK_SESSION_T) );

	pSession->hNetworkSession->hLANInfo.bProtocol	= bProtocolType;
	if(0)
	{
		byMaxRetries=byMaxRetries;    /*-Wextra: Flag added for strict compilation.*/
	}

    /* supports only RMCP PLUS FORMAT */
	if(byAuthType != AUTHTYPE_RMCP_PLUS_FORMAT )
	{
		DEBUG_PRINTF("Authentication type not supported\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTHTYPE_NOT_SUPPORTED);
	}

	/* Validate length of username and password */
	if (pszUserName)
	{
		if(strlen(pszUserName)>= MAX_USER_NAME_LEN)
		{
			DEBUG_PRINTF("User Name too long\n");
			return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_NAME);
		}
	}
	if (pszPassword)
	{
		if(strlen(pszPassword)>= MAX_USER_PWD_LEN)
		{
			DEBUG_PRINTF("Password too long\n");
			return STATUS_CODE(0, LIBIPMI_E_INVALID_USER_NAME);
		}
	}

	/* Connect to BMC */
	if( (byRetvalue=RMCP_Connect(&pSession->hNetworkSession->hLANInfo, pszIPAddress, wPort)) == LIBIPMI_STATUS_SUCCESS )
	{
		/* Succesfully connected so send Ping packet to BMC */
		if( (byRetvalue=RMCP_Ping(&pSession->hNetworkSession->hLANInfo, timeout)) == LIBIPMI_STATUS_SUCCESS )
		{
			/* Ping packet successfully sent so expect Pong message */
			byRetvalue=RMCP_Pong(&pSession->hNetworkSession->hLANInfo, timeout);
		}
	}

	/*Return if any error occured during RMCP session establishment */

	if(byRetvalue != LIBIPMI_STATUS_SUCCESS )
	{
		DEBUG_PRINTF("Unable to ping\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRetvalue);
	}
	else
	{
	//	DEBUG_PRINTF("Ping Successfull\n");
	}

	pSession->hNetworkSession->byPreSession			= TRUE;
	pSession->hNetworkSession->dwSessionID			= 0;
	pSession->hNetworkSession->dwSessionSeqNum		= 0;
	pSession->hNetworkSession->byAuthType			= AUTHTYPE_RMCP_PLUS_FORMAT;
	pSession->hNetworkSession->byRole				= *byPrivLevel;
	pSession->hNetworkSession->byIsEncryptionReq	= FALSE;
	pSession->hNetworkSession->byReqAddr			= byReqAddr;
	pSession->hNetworkSession->byResAddr			= byResAddr;
	
	/* Exact medium type can be found using bProtocol member from LanInfo struct.
	 It can be either NETWORK_MEDIUM_TCP or NETWORK_MEDIUM_UDP */
	pSession->byMediumType = pSession->hNetworkSession->hLANInfo.bProtocol;

	if (pszUserName)
	{
		tempret=snprintf (pSession->hNetworkSession->szUserName,MAX_USER_NAME_LEN,"%s", pszUserName);
		if((tempret<0)||(tempret>=MAX_USER_NAME_LEN))
                {
                    DEBUG_PRINTF("BUFFER OVERFLOW");
                    return -1;
                }
		if( pszPassword )
		{
                  tempret=snprintf (pSession->hNetworkSession->szPwd, MAX_USER_PWD_LEN,"%s",pszPassword);
                  if((tempret<0)||(tempret>=MAX_USER_PWD_LEN))
                  {
                      DEBUG_PRINTF("BUFFER OVERFLOW");
                      return -1;
                  }
		}
		else
			memset (pSession->hNetworkSession->szPwd,0,MAX_USER_PWD_LEN);
		
	}
	else	
	{
		memset (pSession->hNetworkSession->szUserName,0,MAX_USER_NAME_LEN);
		memset (pSession->hNetworkSession->szPwd,0,MAX_USER_PWD_LEN);
	}

	/* Get Authentication Capabilities */
	if( (wRetvalue=GetChannelAuthCapabilities(pSession, *byPrivLevel, &byUseKG, timeout)) != LIBIPMI_E_SUCCESS )
	{
		DEBUG_PRINTF("Unable to GetChannelAuthCapabilities\n");
		return wRetvalue;
	}
	else
	{
		//DEBUG_PRINTF("libipmi:GetChannelAuthCapabilities Successfull\n");
	}

	/* Send Open Session Request */

	if( (wRetvalue=SendOpenSessionRequest(pSession, AUTH_ALG_RAKP_HMAC_SHA1,
											byIntegrity, byEncryption,
											pbyIPMIRes, &dwIPMIResLen, timeout)) == LIBIPMI_E_SUCCESS )
		/* Validate Open Session Response */
		wRetvalue = ValidateOpenSessionResponse(pSession, pbyIPMIRes, dwIPMIResLen);

	/* Return if Error*/

	if(wRetvalue != LIBIPMI_E_SUCCESS)
	{
		DEBUG_PRINTF("Open Session Request Failed \n");
		return wRetvalue;
	}
	else
	{
		//DEBUG_PRINTF("Open Session Request Successful \n");
	}

	/* Send RAKP Message 1*/
	if( (wRetvalue=SendRAKPMessage_1(pSession, pbyIPMIRes,
										&dwIPMIResLen, timeout)) == LIBIPMI_E_SUCCESS )
		/* Validate RAKP Message 2 */
		wRetvalue = ValidateRAKPMessage_2(pSession, pbyIPMIRes, dwIPMIResLen);

	/* Return if Error*/
	if(wRetvalue != LIBIPMI_E_SUCCESS)
	{
            if ((wRetvalue & 0xFF)==LIBIPMI_E_AUTH_CODE_INVALID)
            {
                /*In IPMI Spec, We need to send the RAKP3 message if the RAKP2 message's status code receives any error code*/
                SendRAKPMessage_3(pSession, pbyIPMIRes, &dwIPMIResLen, timeout,SC_UNAUTHORISED_ROLE);
            }

		DEBUG_PRINTF("RAKP 1&2 failed \n");
		return wRetvalue;
	}
	else
	{
//		DEBUG_PRINTF("RAKP 1&2 successful \n");
	}

	/* Send RAKP Message 3*/
    if( (wRetvalue=SendRAKPMessage_3(pSession, pbyIPMIRes, &dwIPMIResLen, timeout,SC_NO_ERROR)) == LIBIPMI_E_SUCCESS )
	{
		/* Validate RAKP Message 4 */
		if(byUseKG)
			wRetvalue = ValidateRAKPMessage_4(pSession, pbyIPMIRes, dwIPMIResLen, byKG, wKGLen);
		else
			wRetvalue = ValidateRAKPMessage_4(pSession, pbyIPMIRes, dwIPMIResLen, 
												(uint8*)&pSession->hNetworkSession->szPwd[0], 
												(uint16)strlen(pSession->hNetworkSession->szPwd));
	}

	/* Return if Error*/
	if(wRetvalue != LIBIPMI_E_SUCCESS)
	{
		DEBUG_PRINTF("RAKP 3&4 failed \n");
		return wRetvalue;
	}
	else
	{
//		DEBUG_PRINTF("RAKP 3&4 successful \n");
	}

	/* Set the params which will be useful for later transactions */
	pSession->hNetworkSession->byPerMsgAuthentication 	= byIntegrity;
	pSession->hNetworkSession->byIsEncryptionReq 		= byEncryption;
	pSession->hNetworkSession->byPreSession 			= 0;
	pSession->bySessionStarted 							= SESSION_STARTED;
	pSession->hNetworkSession->byDefTimeout				= timeout;


	return wRetvalue;
}

/*---------------------------------------------------------------------
Close_IPMI20_Network_Session

Closes a connection with BMC using Network medium.
----------------------------------------------------------------------*/
uint16 Close_IPMI20_Network_Session( IPMI20_SESSION_T *pSession )
{
	uint16	wRet;
	uint32	Req;
	uint8	byRes[MAX_RESPONSE_SIZE];
	uint32	dwResLen;

	Req = pSession->hNetworkSession->dwSessionID;
       pSession->hNetworkSession->dwSessionSeqNum++;
	wRet = Send_RAW_IPMI2_0_Network_Command(pSession, PAYLOAD_TYPE_IPMI,
										DEFAULT_NET_FN_LUN, CMD_CLOSE_SESSION,
										(uint8 *)&Req, sizeof(uint32),
										byRes, &dwResLen,
										pSession->hNetworkSession->byDefTimeout);
	return wRet;
}
/******************* End Session Establishment functions ******************/

/******************* RAW IPMI Command Functions ***************************/
/*---------------------------------------------------------------------
Send_RAW_IPMI2_0_Network_Command

Sends a RAW IPMI command using Network Medium
----------------------------------------------------------------------*/
uint16 Send_RAW_IPMI2_0_Network_Command(IPMI20_SESSION_T *pSession, uint8 byPayLoadType,
										uint8 byNetFnLUN, uint8 byCommand,
										uint8 *pszReqData, uint32 dwReqDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										int	timeout)
{
	uint8			pbyLanReqPacket[MAX_REQUEST_SIZE] = {0};
	uint8			pbyLanResPacket[MAX_RESPONSE_SIZE] = {0};
	uint8			pbyIPMIMsg_Buffer[MAX_IPMI_MESSAGE_SIZE] = {0};
	uint32			dwIPMBDataLen;
	uint32			dwIPMBResLen;
	uint16			*pwIPMIMsgLen;
	uint8			*pbyIPMIMsg;
	uint8			byRet;
	uint8 			byCompletionCode;
	uint32			dwDecryptLen;
	uint8			byResPayLoadType;
	SessionHdr2_T	*pSessionHeader;


	pSessionHeader = (SessionHdr2_T	*) (pbyLanReqPacket + sizeof(RMCPHdr_T) );
	pwIPMIMsgLen = (uint16 *) &(pSessionHeader->IPMIMsgLen);
	pbyIPMIMsg =  (uint8*) (pSessionHeader+1);


	/********** Payload Data *************/
	if(byPayLoadType == PAYLOAD_TYPE_IPMI)

	{
		dwIPMBDataLen = FillNetworkIPMIMessage(pSession, pbyIPMIMsg_Buffer, byNetFnLUN,
												byCommand, pszReqData, dwReqDataLen);
	}
	else
	{
        if(dwReqDataLen<=MAX_IPMI_MESSAGE_SIZE)
        {
            memcpy (pbyIPMIMsg_Buffer, pszReqData, dwReqDataLen);
            dwIPMBDataLen = dwReqDataLen;
        }
        else
        {
        	TCRIT("Buffer overflow in %s\n",__FUNCTION__);
        	return -1;
        }
	}
	/* Encrypt the packet if required */
	if ( pSession->hNetworkSession->byIsEncryptionReq &&
		 !pSession->hNetworkSession->byPreSession )
	{
		byRet = EncryptIPMIPacket(pSession, pbyIPMIMsg_Buffer, dwIPMBDataLen, MAX_IPMI_MESSAGE_SIZE,
							pbyIPMIMsg, pwIPMIMsgLen);

		if(byRet != LIBIPMI_STATUS_SUCCESS)
			return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
		dwIPMBDataLen = (uint32)*pwIPMIMsgLen;
	}
	else
	{
		if(dwIPMBDataLen <= (MAX_REQUEST_SIZE - sizeof(RMCPHdr_T) - sizeof(SessionHdr2_T)))
		{
			memcpy (pbyIPMIMsg, pbyIPMIMsg_Buffer, dwIPMBDataLen);
			*pwIPMIMsgLen = (uint16)dwIPMBDataLen;
		}
		else
		{
			TCRIT("Buffer overflow");
			return -1;
		}
	}

	byResPayLoadType = byPayLoadType;

	/* Send the command to session layer */

	byRet = Session_IPMI2_0_Send_Command(pSession, &byResPayLoadType,
									pbyLanReqPacket, dwIPMBDataLen,
									pbyLanResPacket, &dwIPMBResLen,
									timeout);


	if(byRet !=	LIBIPMI_STATUS_SUCCESS)
		return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);

	pSessionHeader = (SessionHdr2_T	*) (pbyLanResPacket + sizeof(RMCPHdr_T) );
	pwIPMIMsgLen = (uint16 *) &(pSessionHeader->IPMIMsgLen);
	pbyIPMIMsg =  (uint8*) (pSessionHeader+1);

	/* Validate if Pre Session Message */
	if(	IsPreSessionMessage(byPayLoadType, byResPayLoadType) )
	{
		*pdwResDataLen = *pwIPMIMsgLen;
		memcpy (pszResData, pbyIPMIMsg, *pdwResDataLen);

		byCompletionCode = *(pbyIPMIMsg + 1);

		if(byCompletionCode != CC_NORMAL )
			return STATUS_CODE(IPMI_ERROR_FLAG, byCompletionCode);
		
		return LIBIPMI_E_SUCCESS;
	}

	/* Decrypt the IPMI Message if required */

	if(byResPayLoadType & ENCRYPT_MASK )

	{
		memset(pbyIPMIMsg_Buffer, 0, MAX_IPMI_MESSAGE_SIZE);
		if( (byRet=DecryptIPMIPacket(pSession, pbyIPMIMsg, *pwIPMIMsgLen,
										pbyIPMIMsg_Buffer, &dwDecryptLen)) != LIBIPMI_STATUS_SUCCESS)

			return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
		
		/* Validate IPMI Message Header 
		byRet = ValidateIPMIMessage(pSession, pbyIPMIMsg_Buffer, dwDecryptLen);
		
		if(  byRet != LIBIPMI_STATUS_SUCCES)
			return STATUS_CODE(MEDIUM_ERROR_FLAG, byRet);
		*/
		byCompletionCode = *(pbyIPMIMsg_Buffer + sizeof (IPMIMsgHdr_T));
		*pdwResDataLen = dwDecryptLen - (sizeof (IPMIMsgHdr_T) + sizeof (INT8U) );
		memcpy (pszResData, pbyIPMIMsg_Buffer + sizeof (IPMIMsgHdr_T) , *pdwResDataLen);
	}
	else
	{
		byCompletionCode = *(pbyIPMIMsg + sizeof (IPMIMsgHdr_T));

		/*Actual Response data size = Toatl Size - (sizeof(IPMB Request Header) + sizeof(Completion Code))*/
		*pdwResDataLen = *pwIPMIMsgLen - (sizeof (IPMIMsgHdr_T) + sizeof (INT8U) );
		memcpy (pszResData, pbyIPMIMsg + sizeof (IPMIMsgHdr_T) , *pdwResDataLen);
	}
      
	if(( byCompletionCode != CC_NORMAL)
            &&(((AMIYAFUNotAck *)pszResData)->NotAck.YafuCmd != CMD_AMI_YAFU_COMMON_NAK))
		return STATUS_CODE(IPMI_ERROR_FLAG, byCompletionCode);

	return LIBIPMI_E_SUCCESS;
	
}

/*---------------------------------------------------------------------
Session_IPMI2_0_Send_Command

Fills Session Header, Session Trailer and sends the data to BMC
using RMCP Layer. After receiving the response data from BMC, it
validates Session header, Session Trailer and returns the result.
----------------------------------------------------------------------*/
uint8 Session_IPMI2_0_Send_Command(IPMI20_SESSION_T *pSession, uint8 *pbyPayLoadType,
										uint8 *pbyIPMIData, uint32 dwIPMIDataLen,
										uint8 *pszResData, uint32 *pdwResDataLen,
										int	timeout)
{
	SessionHdr2_T*	pSessionHeader;

	uint32			dwIPMIPktLen;
	uint32			dwTotalPktLen;
	uint8			byRet;
	uint8*			pbyIPMISessionTrailer;
    RAKPMsg3Req_T *pbyRAKPMsg3;


	/* Find Session Header Field */
	pSessionHeader = (SessionHdr2_T*) (pbyIPMIData + sizeof(RMCPHdr_T));


	/* Fill Session Header */
	FillNetworkSessionHeader(pSession, *pbyPayLoadType, pSessionHeader);

	/* Find Session Trailer */
	pbyIPMISessionTrailer = (uint8 *)(pbyIPMIData + sizeof(RMCPHdr_T) +
								sizeof(SessionHdr2_T) + dwIPMIDataLen);
	dwIPMIPktLen = pbyIPMISessionTrailer - (uint8*)pbyIPMIData;
	dwTotalPktLen = dwIPMIPktLen;

	/* Fill Session Trailer */

	if ( *pbyPayLoadType == PAYLOAD_TYPE_IPMI )
	{
		FillNetworkSessionTrailer(pSession, pSessionHeader, pbyIPMISessionTrailer,
									dwIPMIPktLen, &dwTotalPktLen);
	}

	/* In PPC machines, we have to convert big to little */
	pSessionHeader->IPMIMsgLen = TO_LITTLE_SHORT(pSessionHeader->IPMIMsgLen);

	/* Send the Packet to RMCP Layer */
	byRet = RMCP_SendData(&pSession->hNetworkSession->hLANInfo, (char *)pbyIPMIData, dwTotalPktLen, 0);

	if(byRet != LIBIPMI_STATUS_SUCCESS)
		return byRet;

        /*If RAKP3 message sends the status code to BMC with any error code means, BMC will not respond to that message.So it results in 
            libipmi to wait in select function call infinitely. So, we no need to wait for receiving packet, instead return with proper Error Flag
            to indicate the session failure*/
        if (PAYLOAD_TYPE_RAKP_MSG_3 == *pbyPayLoadType)
        {
            pSessionHeader = (SessionHdr2_T*)(pbyIPMIData + sizeof(RMCPHdr_T) );
            pbyRAKPMsg3 = (RAKPMsg3Req_T *)(pSessionHeader+1);
            if(pbyRAKPMsg3->StatusCode != SC_NO_ERROR)
                return RMCP_RAKP_ERROR_FLAG;
        }

	/* Receive Packet from RMCP Layer */
	byRet = RMCP_ReceiveData(&pSession->hNetworkSession->hLANInfo, (char *)pszResData, pdwResDataLen, timeout);

	if(byRet ==	LIBIPMI_STATUS_SUCCESS)
	{/* Support Added for SEND_MSG command */
		IPMIMsgHdr_T *pbyIPMIHdr;
		uint8	byCompletionCode;

		pSessionHeader = (SessionHdr2_T	*) (pszResData + sizeof(RMCPHdr_T) );
		pbyIPMIHdr =  (IPMIMsgHdr_T *) (pSessionHeader+1);
		if (CMD_SEND_MSG == pbyIPMIHdr->Cmd && (NETFN_APP==(NET_FN(pbyIPMIHdr->NetFnLUN) & 0xFE)))
		{
			byCompletionCode = *(uint8 *)(pbyIPMIHdr + 1);
			if (CC_NORMAL == byCompletionCode)
				byRet = RMCP_ReceiveData(&pSession->hNetworkSession->hLANInfo, (char *)pszResData, pdwResDataLen, timeout);
			/* else failed Send Message, dont have to wait for response, return CC */
		}
	}
	if(byRet != LIBIPMI_STATUS_SUCCESS)
		return byRet;

	/* Packet received Successfully */

	/* Find Session Header */
	pSessionHeader = (SessionHdr2_T*)(pszResData + sizeof(RMCPHdr_T));

	/* In PPC machines, we have to convert big to little */
	pSessionHeader->IPMIMsgLen = TO_LITTLE_SHORT(pSessionHeader->IPMIMsgLen);


	/* Validate Session Header */
	if( (byRet = ValidationSessionHeader(pSession, pSessionHeader)) != LIBIPMI_STATUS_SUCCESS)
		return byRet;

	/* If not PreSession messages */
	if(pSessionHeader->PayloadType & AUTH_MASK)
	{
		/* Validate Session Trailer */
		if( (byRet = ValidationSessionTrailer(pSession, (uint8 *)pSessionHeader)) != LIBIPMI_STATUS_SUCCESS)
			return byRet;
	}

	/* update the payload status */
	*pbyPayLoadType = pSessionHeader->PayloadType;

	return LIBIPMI_STATUS_SUCCESS;
}
/******************* End RAW IPMI Command Functions ***************************/

/************************** Helper Functions ****************************/
/*---------------------------------------------------------------------
FillNetworkSessionHeader

Fills the Network Session Header information
----------------------------------------------------------------------*/
void FillNetworkSessionHeader(IPMI20_SESSION_T *pSession, uint8 byPayLoadType,
										SessionHdr2_T*	pSessionHeader)
{
	pSessionHeader->AuthType = pSession->hNetworkSession->byAuthType;	/* AuthType */
	pSessionHeader->PayloadType = byPayLoadType;

	if( ! pSession->hNetworkSession->byPreSession)
	{
		if( pSession->hNetworkSession->byIsEncryptionReq )
			pSessionHeader->PayloadType |= ENCRYPT_MASK;

		if( pSession->hNetworkSession->byPerMsgAuthentication )
			pSessionHeader->PayloadType |= AUTH_MASK;					/*PayLoadType*/

		pSessionHeader->SessionSeqNum =
					pSession->hNetworkSession->dwSessionSeqNum;			/* SeqNum */
#if LIBIPMI_IS_PLATFORM_PPC() || LIBIPMI_IS_PLATFORM_MIPS()
        if(memcmp(pSession->hNetworkSession->hLANInfo.szIPAddress,"127.0.0.1",sizeof(pSession->hNetworkSession->hLANInfo.szIPAddress)))
        {
            pSessionHeader->SessionSeqNum = ipmitoh_u32 (pSessionHeader->SessionSeqNum);
        }
#endif
		pSessionHeader->SessionID =
					pSession->hNetworkSession->dwSessionID;				/* SessionID */
	}
	else
	{
		pSessionHeader->SessionSeqNum = 0;
		pSessionHeader->SessionID = 0;
	}

	return;
}

/*---------------------------------------------------------------------
FillNetworkSessionTrailer

Fills the Network Session Trailer information
----------------------------------------------------------------------*/
void FillNetworkSessionTrailer(IPMI20_SESSION_T *pSession, SessionHdr2_T*	pSessionHeader,
							   uint8 *pbyIPMISessionTrailer, uint32 dwIPMIPktLen,
							   uint32 *pdwTotalPktLen)
{
	uint16	wPadLen;
	uint8  	*pbyPadLength;
	uint8	*pbyNextHeader;
	uint8	*pbyAuthCode;
       uint16       AuthCodeLen;

#ifdef ICC_OS_WINDOWS
    // hCryptProv:      Handle to a cryptographic service provider (CSP). 
    //                  This example retrieves the default provider for  
    //                  the PROV_RSA_FULL provider type.  
    // hKey:            Handle to a symmetric key. This example creates a 
    //                  key for the RC4 algorithm.
    // hHmacHash:       Handle to an HMAC hash.
    // dwDataLen:       Length, in bytes, of the hash.
    // HmacInfo:        Instance of an HMAC_INFO structure that contains 
    //                  information about the HMAC hash.
    HCRYPTPROV  hCryptProv  = NULL;
    HCRYPTKEY   hKey        = NULL;
    HCRYPTHASH  hHmacHash   = NULL;
    DWORD       dwDataLen   = 0;
    HMAC_INFO   HmacInfo;
    BYTE RC2KeyBlob[32] = {
        0x08,0x02,0x00,0x00,0x02,0x66,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_RC2 0x00006602)
        0x00,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 // SHA1 key with parity
        };
#endif

	/* Find the total pad length */
	wPadLen = (uint16)( sizeof(uint32) - ( (dwIPMIPktLen + 2) % sizeof(uint32) ) ) % sizeof(uint32);

	/*  Find pad length field */
	pbyPadLength = pbyIPMISessionTrailer + wPadLen;

	/* Find the next header field */
	pbyNextHeader = pbyIPMISessionTrailer + wPadLen + 1;

	/* Find Auth Code field */
	pbyAuthCode = pbyIPMISessionTrailer + wPadLen + 2;


	/* Fill the pad bytes with 0xFF */
	memset(pbyIPMISessionTrailer, 0xFF, wPadLen);

	/* Fill pad length */
       *pbyPadLength =  wPadLen;

	/* Fill Next Header field */
	*pbyNextHeader = IPMI_20_NEXT_HEADER_CONSTANT;

	*pdwTotalPktLen = dwIPMIPktLen + 2 + wPadLen;
       AuthCodeLen = *pdwTotalPktLen  - sizeof(RMCPHdr_T);

	/* Generate Auth Code */
#ifdef ICC_OS_WINDOWS
    //--------------------------------------------------------------------
    // Zero the HMAC_INFO structure and use the SHA1 algorithm for hashing.
    memset(&HmacInfo, 0, sizeof(HmacInfo));
    HmacInfo.HashAlgid = CALG_SHA1;
    //--------------------------------------------------------------------
    // Acquire a handle to the default RSA cryptographic service provider.
    if (!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, NULL))
        if (GetLastError() == NTE_BAD_KEYSET)
            if(!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
                printf(" Error in AcquireContext 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Import a symmetric key from a hash object
    // Secure Hash Algorithm (SHA1) 160 bit
    RC2KeyBlob[8] = MAX_KEY1_SIZE;
    memcpy(&RC2KeyBlob[12], pSession->hNetworkSession->byKey1, MAX_KEY1_SIZE);

    if (!CryptImportKey(hCryptProv, &RC2KeyBlob, sizeof(RC2KeyBlob), 0, CRYPT_IPSEC_HMAC_KEY | CRYPT_EXPORTABLE, &hKey ))
        printf("Error in Import Key 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Create an HMAC
    if (!CryptCreateHash( hCryptProv, CALG_HMAC, hKey, 0, &hHmacHash))
        printf("Error in CryptCreateHash 0x%08x \n", GetLastError());

    if (!CryptSetHashParam( hHmacHash, HP_HMAC_INFO, (BYTE*)&HmacInfo, 0))
        printf("Error in CryptSetHashParam 0x%08x \n", GetLastError());
    
    if (!CryptHashData( hHmacHash, (uint8*)pSessionHeader, AuthCodeLen, 0))
        printf("Error in CryptHashData 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Retrieve the hash value.
    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, NULL, &dwDataLen, 0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, pbyAuthCode, &dwDataLen,0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if(hHmacHash)
        CryptDestroyHash(hHmacHash);
    if(hKey)
        CryptDestroyKey(hKey);
    if(hCryptProv)
        CryptReleaseContext(hCryptProv, 0);
#else
    ((unsigned char(*)(const EVP_MD *,const void *,int,const unsigned char *,size_t,unsigned char *,unsigned int*))g_CryptoHandle[CRYPTO_HMAC])( ((const EVP_MD *(*)(void))g_CryptoHandle[CRYPTO_EVP_SHA1])(), 
                   (INT8U *)pSession->hNetworkSession->byKey1, MAX_KEY1_SIZE
                   , (_FAR_ INT8U*)pSessionHeader, AuthCodeLen
                   , (INT8U *)pbyAuthCode, NULL);
#endif

	/*	Update the total data length */

	*pdwTotalPktLen += MAX_AUTH_CODE_SIZE;


	return;
}

/*---------------------------------------------------------------------
FillNetworkIPMIMessage

Fills the NetworkIPMI Message.
----------------------------------------------------------------------*/
uint32 FillNetworkIPMIMessage(IPMI20_SESSION_T *pSession, uint8 *Buffer,
							uint8 byNetFnLUN, uint8 byCommand,
							uint8 *pbyReqData, uint32 dwReqDataLen)
{
	IPMIMsgHdr_T *pbyIPMIHdr;

	uint8 *pbyIPMIData;
	IPMIMsgHdr_T *pbyBridgeIPMIHdr;
	uint8 *cs2;

	pbyIPMIHdr = (IPMIMsgHdr_T *)&Buffer[0];
	pbyIPMIData = &Buffer[sizeof(IPMIMsgHdr_T)];

	/*	IPMI Message Header */
	pbyIPMIHdr->ResAddr 		= pSession->hNetworkSession->byResAddr;
	pbyIPMIHdr->ReqAddr 		= pSession->hNetworkSession->byReqAddr;
	pbyIPMIHdr->NetFnLUN 		= byNetFnLUN;
	pbyIPMIHdr->RqSeqLUN		= pSession->hNetworkSession->byIPMBSeqNum << 2;
	pbyIPMIHdr->Cmd				= byCommand;
	pbyIPMIHdr->ChkSum			= LIBIPMI_CalculateCheckSum( (uint8 *)pbyIPMIHdr, (uint8 *)&(pbyIPMIHdr->ChkSum) ); /* cheksum_1 */

	/* Increment the sequence number */
	pSession->hNetworkSession->byIPMBSeqNum = (pSession->hNetworkSession->byIPMBSeqNum+1) % 0x40;

	if ( CMD_SEND_MSG == byCommand )
	{ /* Send Message Support */
	/* Add the second IPMI Hdr wrt Send Message */
	/* Original Message Ex.. CMD_SEND_MSG 0x40 0x6 0x1 */
	/* Transform it to "resaddr nefn/lun chsum reqaddr seq/lun cmd  data addr netfn/ln chsum reqaddr seq/lun cmd chksum2 chksum2" */
	
		pbyIPMIData[0] 				= pbyReqData[0];
		pbyBridgeIPMIHdr 				= (IPMIMsgHdr_T *)(pbyIPMIData + 1);
		pbyBridgeIPMIHdr->ResAddr 	= pbyReqData[1];
		pbyBridgeIPMIHdr->NetFnLUN 	= pbyReqData[2] << 2; 
		pbyBridgeIPMIHdr->ChkSum	= LIBIPMI_CalculateCheckSum( (uint8 *)pbyBridgeIPMIHdr, (uint8 *)&(pbyBridgeIPMIHdr->ChkSum) ); /* cheksum_1 */
		pbyBridgeIPMIHdr->ReqAddr 	= pSession->hNetworkSession->byResAddr;
		pbyBridgeIPMIHdr->RqSeqLUN	= pbyIPMIHdr->RqSeqLUN;
		pbyBridgeIPMIHdr->Cmd		= pbyReqData[3];

		/* Taking the first 4 bytes from the Req Packet and tranferring them to 1 + IPMIMsgHdr size */
		/* Copy the rest of the Request bytes at the end */
		memcpy (pbyIPMIData+1+sizeof(IPMIMsgHdr_T), pbyReqData+4, dwReqDataLen -4);

		/* Add the two checksums */
		cs2 = (uint8*)(pbyIPMIData + 1 + sizeof(IPMIMsgHdr_T)+dwReqDataLen -4);

		*cs2 = LIBIPMI_CalculateCheckSum( (&(pbyBridgeIPMIHdr->ChkSum))+1, cs2);
		cs2++;
		*cs2 = LIBIPMI_CalculateCheckSum( (&(pbyIPMIHdr->ChkSum))+1, cs2);

		return ( 2*sizeof(IPMIMsgHdr_T) + dwReqDataLen -1);
	}
	else
	{
	/* IPMI Data */
	memcpy(pbyIPMIData, pbyReqData, dwReqDataLen);

	/* checksum_2*/
	pbyIPMIData[dwReqDataLen] = LIBIPMI_CalculateCheckSum( (&(pbyIPMIHdr->ChkSum))+1, &pbyIPMIData[dwReqDataLen] );

	return ( sizeof(IPMIMsgHdr_T) + dwReqDataLen + 1 );
	}
}

/*---------------------------------------------------------------------
IsPreSessionMessage

Checks if a message is a pre session message based on its PayLoadType.
----------------------------------------------------------------------*/
uint8 IsPreSessionMessage(uint8 byPayLoadType, uint8 byResPayLoadType )
{
	switch(byPayLoadType)

	{
		case PAYLOAD_TYPE_RSSP_OPEN_SES_REQ:
		case PAYLOAD_TYPE_RSSP_OPEN_SES_RES:
		case PAYLOAD_TYPE_RAKP_MSG_1:
		case PAYLOAD_TYPE_RAKP_MSG_2:
		case PAYLOAD_TYPE_RAKP_MSG_3:
		case PAYLOAD_TYPE_RAKP_MSG_4:
			if( byPayLoadType == (byResPayLoadType-1) )
				return (uint8)1;
	}

	return (uint8)0;
}
/************************ End Helper Functions *************************/

/******************* Authentication Helper Functions *******************/
/*---------------------------------------------------------------------
AuthAlg_HMAC_SHA1

Calculates the HMAC for given data and the key string
----------------------------------------------------------------------*/
void AuthAlg_HMAC_SHA1(uint8 *szKey, uint16 wKeyLen,
						uint8 *pbyHMAC, uint16 wHMACLen,
						uint8 *pResultAuthCode, uint16 wAuthCodeLen)
{
#if LIBIPMI_IS_OS_LINUX()
    if(0)
    {
	wAuthCodeLen=wAuthCodeLen; /* -Wextra: Flag added for strict compilation.*/
    }
#endif
#ifdef ICC_OS_WINDOWS
    // hCryptProv:      Handle to a cryptographic service provider (CSP). 
    //                  This example retrieves the default provider for  
    //                  the PROV_RSA_FULL provider type.  
    // hKey:            Handle to a symmetric key. This example creates a 
    //                  key for the RC4 algorithm.
    // hHmacHash:       Handle to an HMAC hash.
    // dwDataLen:       Length, in bytes, of the hash.
    // HmacInfo:        Instance of an HMAC_INFO structure that contains 
    //                  information about the HMAC hash.
    HCRYPTPROV  hCryptProv  = NULL;
    HCRYPTKEY   hKey        = NULL;
    HCRYPTHASH  hHmacHash   = NULL;
    DWORD       dwDataLen   = 0;
    HMAC_INFO   HmacInfo;
    BYTE RC2KeyBlob[32] = {
        0x08,0x02,0x00,0x00,0x02,0x66,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_RC2 0x00006602)
        0x00,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 // SHA1 key with parity
        };
    BYTE RC4KeyBlob[17] = {
        0x08,0x02,0x00,0x00,0x01,0x68,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_RC4 0x00006801)
        0x00,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00                 // RC4 key with parity
        };

    //--------------------------------------------------------------------
    // Zero the HMAC_INFO structure and use the SHA1 algorithm for hashing.
    memset(&HmacInfo, 0, sizeof(HmacInfo));
    HmacInfo.HashAlgid = CALG_SHA1;
    //--------------------------------------------------------------------
    // Acquire a handle to the default RSA cryptographic service provider.
    if (!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, NULL))
        if (GetLastError() == NTE_BAD_KEYSET)
            if(!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
                printf(" Error in AcquireContext 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Import a symmetric key from a hash object
    if (wKeyLen == 5)
    { // RSA Data Security 4 (RC4) 40 bit
        RC4KeyBlob[8] = wKeyLen;
        memcpy(&RC4KeyBlob[12], szKey, wKeyLen);

        if (!CryptImportKey(hCryptProv, &RC4KeyBlob, sizeof(RC4KeyBlob), 0, CRYPT_EXPORTABLE, &hKey ))
            printf("Error in Import Key 0x%08x \n", GetLastError());
    }
    else
    { // Secure Hash Algorithm (SHA1) 160 bit
        RC2KeyBlob[8] = wKeyLen;
        memcpy(&RC2KeyBlob[12], szKey, wKeyLen);

        if (!CryptImportKey(hCryptProv, &RC2KeyBlob, sizeof(RC2KeyBlob), 0, CRYPT_IPSEC_HMAC_KEY | CRYPT_EXPORTABLE, &hKey ))
            printf("Error in Import Key 0x%08x \n", GetLastError());
    }

    //--------------------------------------------------------------------
    // Create an HMAC
    if (!CryptCreateHash( hCryptProv, CALG_HMAC, hKey, 0, &hHmacHash))
        printf("Error in CryptCreateHash 0x%08x \n", GetLastError());

    if (!CryptSetHashParam( hHmacHash, HP_HMAC_INFO, (BYTE*)&HmacInfo, 0))
        printf("Error in CryptSetHashParam 0x%08x \n", GetLastError());
    
    if (!CryptHashData( hHmacHash, pbyHMAC, wHMACLen, 0))
        printf("Error in CryptHashData 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Retrieve the hash value.
    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, NULL, &dwDataLen, 0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, pResultAuthCode, &dwDataLen,0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if(hHmacHash)
        CryptDestroyHash(hHmacHash);
    if(hKey)
        CryptDestroyKey(hKey);
    if(hCryptProv)
        CryptReleaseContext(hCryptProv, 0);
#else
    ((unsigned char(*)(const EVP_MD *,const void *,int,const unsigned char *,size_t,unsigned char *,unsigned int*))g_CryptoHandle[CRYPTO_HMAC])(((const EVP_MD *(*)(void))g_CryptoHandle[CRYPTO_EVP_SHA1])()
                   ,(INT8U *)szKey, wKeyLen
                   , (_FAR_ INT8U*)pbyHMAC, wHMACLen
                   , (INT8U *)pResultAuthCode, NULL);
#endif

	return;
}
/****************** End Authentication Helper Functions ****************/

/********************* Encryption Helper Functions ****************************/
/*---------------------------------------------------------------------
EncryptIPMIPacket

Encrypts the given packet with the given Encryption algorithm
----------------------------------------------------------------------*/
uint8 EncryptIPMIPacket(IPMI20_SESSION_T *pSession, uint8 *pbyInputBuffer, uint32 dwInputDataLen, uint32 dwMaxInputBuffSize,
							uint8 *pbyOutputBuffer, uint16 *pwOutputBuffLen)
{
	uint8 byRetValue = LIBIPMI_STATUS_SUCCESS;
	/* IPMI 2.0 Specification Page 181 */

	if(  pSession->hNetworkSession->byEncryptAlgthm == CONFIDENTIALITY_ALG_AES_CBC_128 )
		byRetValue = AES_CBC_Encrypt(pbyInputBuffer, dwInputDataLen, dwMaxInputBuffSize,
										pbyOutputBuffer, pwOutputBuffLen,
										&pSession->hNetworkSession->byKey2[0]);
	else
	{
		DEBUG_PRINTF("Encryption not supported");
		byRetValue = LIBIPMI_ENCRYPTION_UNSUPPORTED;
	}

	return byRetValue;
}

/*---------------------------------------------------------------------
AES_CBC_Encrypt

Encrypts the given packet with the AES_CBC_128 Encryption algorithm
----------------------------------------------------------------------*/
uint8 AES_CBC_Encrypt(uint8 *pbyInputBuffer, uint32 dwInputDataLen, uint32 dwMaxInputBuffSize,
							uint8 *pbyOutputBuffer, uint16 *pwOutputBuffLen,
							uint8 *pKey)
{
	int 	i=0;

	uint8 	pbyInitVector[AES_CBC_INIT_VECTOR_SIZE]={0};
	uint32	dwPadLen=0;
	uint32	dwNewDataLen=0;
        #if LIBIPMI_IS_OS_WINDOWS()
	HCRYPTPROV hProvider=0;
//	BYTE pbBuffer = 0;
	#endif

#ifdef ICC_OS_WINDOWS
    #define KEYLENGTH  0x00800000
    HCRYPTPROV hCryptProv;
    HCRYPTKEY sessionKey;
    WORD padding = PKCS5_PADDING;
    DWORD mode = CRYPT_MODE_CBC;
    DWORD encryptedMessageLen = 0;
    BYTE encryptedMessage[1024 * 20];
    BYTE Aes128KeyBlob[28] = {
        0x08,0x02,0x00,0x00,0x0e,0x66,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_AES_128 0x0000660e)
        0x10,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  // AES 128 key with parity
        };
#else
    EVP_CIPHER_CTX ctx;
    uint32 tmplen;
#endif
	/* Create vector */
	for(i=0; i < AES_CBC_INIT_VECTOR_SIZE; i++)
        {
             	#if LIBIPMI_IS_OS_LINUX()
		Generate_random_number(&pbyInitVector[i],1);
		#elif LIBIPMI_IS_OS_WINDOWS()	
		CryptGenRandom(hProvider, 1, &pbyInitVector[i]);
		#endif
        }
		//Generate_random_number(&pbyInitVector[i],1);
	dwPadLen =  ( MAX_AES_BLOCK_SIZE - ( (dwInputDataLen  + 1) % MAX_AES_BLOCK_SIZE ) ) % MAX_AES_BLOCK_SIZE;
	dwNewDataLen = dwInputDataLen + dwPadLen + 1;

	if( dwNewDataLen > dwMaxInputBuffSize)
	{
		DEBUG_PRINTF("No enough memory for encryption \n");
		return LIBIPMI_AES_CBC_E_NO_ENOUGH_MEMORY;
	}

	/* Fill the padded fields*/

	for(i=0; i< (int)dwPadLen; i++)

		pbyInputBuffer[dwInputDataLen + i] = (uint8) (i+1);

	/* Fill pad length */

	pbyInputBuffer[dwNewDataLen - 1] = (uint8)dwPadLen;


	memcpy(pbyOutputBuffer, pbyInitVector, AES_CBC_INIT_VECTOR_SIZE);

#ifdef ICC_OS_WINDOWS
    if(!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, NULL))
        if (GetLastError() == NTE_BAD_KEYSET)
            if(!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_NEWKEYSET))
                printf("Error in AcquireContext 0x%08x \n", GetLastError());

    memcpy(&Aes128KeyBlob[12], pKey, 16);

    if (!CryptImportKey(hCryptProv, &Aes128KeyBlob, sizeof(Aes128KeyBlob), 0, CRYPT_EXPORTABLE, &sessionKey ))
        printf("Error in Import Key 0x%08x \n", GetLastError());

    if(!CryptSetKeyParam(sessionKey, KP_IV, pbyInitVector, 0))
        printf("Error in Import Initization Vector 0x%08x \n", GetLastError());

    if(!CryptSetKeyParam(sessionKey, KP_MODE, (BYTE*)&mode, 0))
        printf("Error in Setting cipher mode 0x%08x \n", GetLastError());

    // Encrypt message
    memcpy(encryptedMessage, pbyInputBuffer, dwNewDataLen);
    encryptedMessageLen = dwNewDataLen;
    if(!CryptEncrypt(sessionKey, NULL, FALSE, 0, encryptedMessage, &encryptedMessageLen, sizeof(encryptedMessage)))
        printf("Error in Encryption 0x%08x \n", GetLastError());
    memcpy(&pbyOutputBuffer[AES_CBC_INIT_VECTOR_SIZE], encryptedMessage, encryptedMessageLen);
    
    // destroy session key
    CryptDestroyKey(sessionKey);
    CryptReleaseContext(hCryptProv, 0);
#else
    ((void(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_INIT])(&ctx);
    ((int(*)(EVP_CIPHER_CTX *,const EVP_CIPHER *,ENGINE *, const unsigned char*, const unsigned char*))g_CryptoHandle[CRYPTO_EVP_ENCRYPTINIT_EX])(&ctx, ((const EVP_CIPHER *(*)(void))g_CryptoHandle[CRYPTO_EVP_AES_128_CBC])()
               , NULL, pKey, pbyInitVector);
    ((int(*)(EVP_CIPHER_CTX *,int))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_SET_PADDING])(&ctx, 0);
    if(!((int(*)(EVP_CIPHER_CTX *,unsigned char *,int *, const unsigned char *,int))g_CryptoHandle[CRYPTO_EVP_ENCRYPTUPDATE])(&ctx, &pbyOutputBuffer[AES_CBC_INIT_VECTOR_SIZE], (int *)&tmplen, pbyInputBuffer, dwNewDataLen))
    {
        DEBUG_PRINTF ("AES CBC 128 Encrypt updated in Confidentiality Trailer Fail\n");
        ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
        return LIBIPMI_AES_CBC_E_FAILURE;
    } 
    else
    {
        uint32 tmpfinallen;

        if(!((int(*)(EVP_CIPHER_CTX *,unsigned char *,int *))g_CryptoHandle[CRYPTO_EVP_ENCRYPTFINAL_EX])(&ctx, &pbyOutputBuffer[AES_CBC_INIT_VECTOR_SIZE] + tmplen, (int *)&tmpfinallen))
        {
            DEBUG_PRINTF ("Confidentiality Trailer AES CBC 128 Encryption Fail\n");
            ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
            return LIBIPMI_AES_CBC_E_FAILURE;
        }
        ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
    }
#endif

	*pwOutputBuffLen = (uint16)(dwNewDataLen + AES_CBC_INIT_VECTOR_SIZE);

	return LIBIPMI_STATUS_SUCCESS;
}
/**************** End Encryption Helper Functions ******************************/

/**************** Decryption Helper Functions ***********************************/
/*---------------------------------------------------------------------
DecryptIPMIPacket

Decrypts the given packet with the given decryption algorithm
----------------------------------------------------------------------*/
uint8 DecryptIPMIPacket(IPMI20_SESSION_T* pSession, uint8 *pbyInputBuffer,
						 uint32 dwInputDataLen,	uint8 *pbyOutputBuffer,
						 uint32 *pdwOutputBuffLen)
{
	uint8 byRetValue = LIBIPMI_STATUS_SUCCESS;


	/* IPMI 2.0 Specification Page 181 */
	if(  pSession->hNetworkSession->byEncryptAlgthm == CONFIDENTIALITY_ALG_AES_CBC_128 )
		byRetValue = AES_CBC_Decrypt(pbyInputBuffer, dwInputDataLen,
										pbyOutputBuffer, pdwOutputBuffLen,
										&pSession->hNetworkSession->byKey2[0]);
	else
	{
		DEBUG_PRINTF("Encryption not supported");
		byRetValue = LIBIPMI_ENCRYPTION_UNSUPPORTED;
	}

	return byRetValue;
}

/*---------------------------------------------------------------------
AES_CBC_Decrypt

Decrypts the given packet with AES_CBC_128 decryption algorithm
----------------------------------------------------------------------*/
uint8 AES_CBC_Decrypt(uint8 *pbyInputBuffer, uint32 dwInputDataLen,
					 uint8 *pbyOutputBuffer, uint32 *pdwOutputBuffLen,
					 uint8 *pKey)
{
	uint8		*pEncryptData;
	uint8		pbyInitVector[AES_CBC_INIT_VECTOR_SIZE];
	uint8		byConfPadLen;

#ifdef ICC_OS_WINDOWS
    #define KEYLENGTH  0x00800000
    HCRYPTPROV hCryptProv;
    HCRYPTKEY sessionKey;
    WORD padding = PKCS5_PADDING;
    DWORD mode = CRYPT_MODE_CBC;
    DWORD encryptedMessageLen = 0;
    BYTE encryptedMessage[1024];
    BYTE Aes128KeyBlob[28] = {
        0x08,0x02,0x00,0x00,0x0e,0x66,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_AES_128 0x0000660e)
        0x10,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  // AES 128 key with parity
        };
#else
    EVP_CIPHER_CTX ctx;
    uint32 tmplen;
#endif

	/* Get the initialization vector array	*/

	memcpy (pbyInitVector, pbyInputBuffer, MAX_INIT_VECTOR_SIZE);
       pEncryptData = (pbyInputBuffer  + MAX_INIT_VECTOR_SIZE);

	dwInputDataLen -= MAX_INIT_VECTOR_SIZE;

	/* Do the decryption */
#ifdef ICC_OS_WINDOWS
    if(!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, NULL))
         if (GetLastError() == NTE_BAD_KEYSET)
            if(!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_NEWKEYSET))
                printf("Error in AcquireContext 0x%08x \n", GetLastError());

    memcpy(&Aes128KeyBlob[12], pKey, 16);

    if (!CryptImportKey(hCryptProv, &Aes128KeyBlob, sizeof(Aes128KeyBlob), 0, CRYPT_EXPORTABLE, &sessionKey ))
        printf("Error in Import Key 0x%08x \n", GetLastError());

    if(!CryptSetKeyParam(sessionKey, KP_IV, pbyInitVector, 0))
        printf("Error in Import Initization Vector 0x%08x \n", GetLastError());

    if(!CryptSetKeyParam(sessionKey, KP_MODE, (BYTE*)&mode, 0))
        printf("Error in Setting cipher mode 0x%08x \n", GetLastError());

    // Decrypt message
    memcpy(encryptedMessage, pEncryptData, dwInputDataLen);
    encryptedMessageLen = dwInputDataLen;
    if(!CryptDecrypt(sessionKey, NULL, FALSE, 0, encryptedMessage, &encryptedMessageLen))
        printf("Error in Decryption 0x%08x \n", GetLastError());
    memcpy(pbyOutputBuffer, encryptedMessage, encryptedMessageLen);

    // destroy session key
    CryptDestroyKey(sessionKey);
    CryptReleaseContext(hCryptProv, 0);
#else
    ((void(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_INIT])(&ctx);
    ((int(*)(EVP_CIPHER_CTX *,const EVP_CIPHER *,ENGINE *, const unsigned char*, const unsigned char*))g_CryptoHandle[CRYPTO_EVP_DECRYPTINIT_EX])(&ctx, ((const EVP_CIPHER *(*)(void))g_CryptoHandle[CRYPTO_EVP_AES_128_CBC])()
             , NULL, pKey, pbyInitVector);
    ((int(*)(EVP_CIPHER_CTX *,int))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_SET_PADDING])(&ctx, 0);
    if(!((int(*)(EVP_CIPHER_CTX *,unsigned char *,int *, const unsigned char *,int))g_CryptoHandle[CRYPTO_EVP_DECRYPTUPDATE])(&ctx, pbyOutputBuffer, (int *)&tmplen, pEncryptData, dwInputDataLen))
    {
        DEBUG_PRINTF ("AES CBC 128 Decrypt Fail\n");
        ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
        return LIBIPMI_AES_CBC_E_FAILURE;
    } 
    else
    {
        uint32 tmpfinallen;

        if(!((int(*)(EVP_CIPHER_CTX *,unsigned char *,int *))g_CryptoHandle[CRYPTO_EVP_DECRYPTFINAL_EX])(&ctx, pbyOutputBuffer + tmplen, (int *)&tmpfinallen))
        {
            DEBUG_PRINTF ("AES CBC 128 Decrypt final Fail\n");
            ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
            return LIBIPMI_AES_CBC_E_FAILURE;
        }
        ((int(*)(EVP_CIPHER_CTX *))g_CryptoHandle[CRYPTO_EVP_CIPHER_CTX_CLEANUP])(&ctx);
    }
#endif


	/* Find the pad field */

	byConfPadLen = pbyOutputBuffer[dwInputDataLen - 1];


	/* Remove Padding bytes including padding field */
	dwInputDataLen -= byConfPadLen + 1;


	/* Return the remaining IPMI Message length */
	*pdwOutputBuffLen = dwInputDataLen;



	return LIBIPMI_STATUS_SUCCESS;
}
/**************** End Decryption Helper Functions ******************************/

/************************** Send Command Functions ****************************/
/*---------------------------------------------------------------------
GetChannelAuthCapabilities

Sends GetChannelAuthenticationCapabilities command to BMC 
using network medium
----------------------------------------------------------------------*/
uint16 GetChannelAuthCapabilities(IPMI20_SESSION_T *pSession, uint8 byPrivLevel, 
									uint8 *pbyUseKG, int timeout)
{
	uint8					pbyIPMIReq [MAX_REQUEST_SIZE];

	uint8					pbyIPMIRes [MAX_RESPONSE_SIZE];
	uint32					dwIPMIResLen;
	uint16					wRetvalue;
	GetChAuthCapReq_T		*pChAuthCap;
	GetChAuthCapRes_T		*pChAuthCapRes;

	pChAuthCap = (GetChAuthCapReq_T	*)&pbyIPMIReq[0];

	/* initialize to 0 */
	memset(pChAuthCap, 0, sizeof (GetChAuthCapReq_T));


	if(0==memcmp( pSession->hNetworkSession->hLANInfo.szIPAddress,"127.0.0.1",IP_ADDR_LEN))
		{

	
		/* Initialize channel and privilege level */
		pChAuthCap->ChannelNum = pSession->EthIndex|0x80;
		}else
		{
			pChAuthCap->ChannelNum = 0x80 | CURRENT_CHANNEL_NUM;
		}
	pChAuthCap->PrivLevel = byPrivLevel;


	wRetvalue = Send_RAW_IPMI2_0_Network_Command (pSession, PAYLOAD_TYPE_IPMI, DEFAULT_NET_FN_LUN,
													CMD_GET_CH_AUTH_CAP, pbyIPMIReq,
													sizeof (GetChAuthCapReq_T),
							 	 					pbyIPMIRes, &dwIPMIResLen, timeout);
	if(wRetvalue == LIBIPMI_E_SUCCESS)
	{
		pChAuthCapRes	= (GetChAuthCapRes_T	*)&pbyIPMIRes[0];

		if( !(pChAuthCapRes->AuthType & 0x80) )
		{
			DEBUG_PRINTF("Auth Type RMCP PLUS Not supported\n");
		}

		/* If this bit is set, we have to use password instead of default KG */
		*pbyUseKG =  (pChAuthCapRes->PerMsgUserAuthLoginStatus & 0x20);
				
		pSession->hNetworkSession->byChannelNum 				= pChAuthCapRes->ChannelNum;
		pSession->hNetworkSession->byPerMsgAuthentication 	= (pChAuthCapRes->PerMsgUserAuthLoginStatus & 0x10) >> 4;
	}
	else
	{
		if ( IS_IPMI_ERROR(wRetvalue) )
		{
			uint8 byErr = GET_ERROR_CODE(wRetvalue);
			if( byErr > 0 && byErr <= LAST_RMCP_RAKP_STATUS_CODE )
				wRetvalue = STATUS_CODE( RMCP_RAKP_ERROR_FLAG, byErr );
		}
	}


	return wRetvalue;
}

/*---------------------------------------------------------------------
SendOpenSessionRequest

Sends OpenSessionRequest command to BMC using network medium
----------------------------------------------------------------------*/
uint16 SendOpenSessionRequest(IPMI20_SESSION_T *pSession, uint8 byAuthAlg, uint8 byIntegrityAlg,
								uint8 byConfAlg, uint8 *pbyResPkt, uint32 *pdwIPMIResLen, int timeout)
{
	uint8					pbyIPMIReq [MAX_REQUEST_SIZE]={0};
	uint16					wRetvalue=0;
	RSSPOpenSessionReq_T	*pOpenSessionReq;
        unsigned char rand_num=0;
	#if LIBIPMI_IS_OS_WINDOWS()
	HCRYPTPROV hProvider=0;
	BYTE pbBuffer = 0;
	#endif

	pOpenSessionReq = (RSSPOpenSessionReq_T	*)&pbyIPMIReq[0];


	memset(pOpenSessionReq, 0, sizeof(RSSPOpenSessionReq_T) );



	/* fill Open Session request packet */
	#if LIBIPMI_IS_OS_LINUX()
	Generate_random_number(&rand_num,1);
	pSession->hNetworkSession->byMsgTag = pOpenSessionReq->MsgTag = rand_num;
	#elif LIBIPMI_IS_OS_WINDOWS()
	/*const unsigned long dwLength=1;
	HCRYPTPROV hProvider=0;
	BYTE pbBuffer[dwLength] = {0};*/
	CryptGenRandom(hProvider, 1, &pbBuffer);
	pSession->hNetworkSession->byMsgTag = pOpenSessionReq->MsgTag = pbBuffer;
	#endif
	pOpenSessionReq->Role					= pSession->hNetworkSession->byRole;
	pOpenSessionReq->Reserved [0] =pSession->EthIndex ;
	pOpenSessionReq->Reserved [1] = 0;
	#if LIBIPMI_IS_OS_LINUX()
	pOpenSessionReq->RemConSessionID = pSession->hNetworkSession->dwRemConSessID = rand_num;
	#elif LIBIPMI_IS_OS_WINDOWS()
	pOpenSessionReq->RemConSessionID = pSession->hNetworkSession->dwRemConSessID = pbBuffer;
	#endif
    

	/* Authentication Algorithm */
    memset(&pOpenSessionReq->Auth, 0, sizeof(RSSPPayloadInfo_T) );
    pOpenSessionReq->Auth.Algorithm = byAuthAlg;
    pOpenSessionReq->Auth.PayloadType = 0x00;
    pOpenSessionReq->Auth.PayloadLen  = sizeof (RSSPPayloadInfo_T);

    /* Integrity Algorithm */
    memset(&pOpenSessionReq->Integrity, 0, sizeof(RSSPPayloadInfo_T) );
    pOpenSessionReq->Integrity.Algorithm = byIntegrityAlg;
    pOpenSessionReq->Integrity.PayloadType = 0x01;
    pOpenSessionReq->Integrity.PayloadLen  = sizeof (RSSPPayloadInfo_T);

    /* Confidentiality Algorithm */
    memset(&pOpenSessionReq->Confidentiality, 0, sizeof(RSSPPayloadInfo_T) );
    pOpenSessionReq->Confidentiality.Algorithm = byConfAlg;
    pOpenSessionReq->Confidentiality.PayloadType = 0x02;
    pOpenSessionReq->Confidentiality.PayloadLen  = sizeof (RSSPPayloadInfo_T);

	/* Send the packet */

	wRetvalue = Send_RAW_IPMI2_0_Network_Command (pSession, PAYLOAD_TYPE_RSSP_OPEN_SES_REQ, 0,
													0, pbyIPMIReq,	sizeof (RSSPOpenSessionReq_T),
							 	 					pbyResPkt, pdwIPMIResLen, timeout);
	if( wRetvalue != LIBIPMI_E_SUCCESS )
	{
		if ( IS_IPMI_ERROR(wRetvalue) )
		{
			uint8 byErr = GET_ERROR_CODE(wRetvalue);
			if( byErr > 0 && byErr <= LAST_RMCP_RAKP_STATUS_CODE )
				wRetvalue = STATUS_CODE( RMCP_RAKP_ERROR_FLAG, byErr );
		}
	}

	return wRetvalue;
}

/*---------------------------------------------------------------------
SendRAKPMessage_1

Sends RAKP message_1 command to BMC using network medium
----------------------------------------------------------------------*/
uint16 SendRAKPMessage_1(IPMI20_SESSION_T *pSession, uint8 *pbyResPkt,
							uint32 *pdwIPMIResLen, int timeout)
{
	uint8					pbyIPMIReq [MAX_REQUEST_SIZE]={0};
	uint16					wRetvalue=0;
	RAKPMsg1Req_T			*pRAKPMsg1;
	int						i=0;
	uint32					dwPktLen;
	unsigned char rand_num=0;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
	#if LIBIPMI_IS_OS_WINDOWS()
	HCRYPTPROV hProvider = 0;
	BYTE pbBuffer=0;
	#endif
	pRAKPMsg1 = (RAKPMsg1Req_T	*)&pbyIPMIReq[0];

	/* initialize to 0 */

	memset(pRAKPMsg1, 0, sizeof(RAKPMsg1Req_T) );

	/* SIDC, RM, RoleM, ULengthM, < UNameM > */



	/* Fill RAKP Message Packet */
	#if LIBIPMI_IS_OS_LINUX()
	Generate_random_number(&rand_num,1);
	pRAKPMsg1->MsgTag			=  pSession->hNetworkSession->byMsgTag	= rand_num;
	#elif LIBIPMI_IS_OS_WINDOWS()
	CryptGenRandom(hProvider, 1, &pbBuffer);
	pRAKPMsg1->MsgTag			=  pSession->hNetworkSession->byMsgTag	= pbBuffer;
	#endif
	pRAKPMsg1->ManSysSessionID	=  pSession->hNetworkSession->dwSessionID;
	//we have to send user name lookup rather than user and role lookup
	pRAKPMsg1->Role				=  pSession->hNetworkSession->byRole | (ROLE_LOOKUP_NAME_ONLY << 4); 
	pRAKPMsg1->UsrNameLen		= strlen (pSession->hNetworkSession->szUserName);


	for(i=0; i<MAX_RAND_NO_LEN; i++)
	{
	#if LIBIPMI_IS_OS_LINUX()
	pRAKPMsg1->RemConRandomNo[i] = pSession->hNetworkSession->byRemConRandomNo[i] = rand_num;
	#elif LIBIPMI_IS_OS_WINDOWS()
	pRAKPMsg1->RemConRandomNo[i] = pSession->hNetworkSession->byRemConRandomNo[i] = pbBuffer;
	#endif
	}


	tempret=(signed)snprintf ((char*)pRAKPMsg1->UsrName,sizeof(pRAKPMsg1->UsrName),"%s", pSession->hNetworkSession->szUserName);
	if((tempret<0)||(tempret>=(signed)sizeof(pRAKPMsg1->UsrName)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
	dwPktLen = sizeof(RAKPMsg1Req_T) - sizeof(pRAKPMsg1->UsrName) + pRAKPMsg1->UsrNameLen;

	/* Send the packet*/
	wRetvalue = Send_RAW_IPMI2_0_Network_Command (pSession, PAYLOAD_TYPE_RAKP_MSG_1, 0,
													0, pbyIPMIReq,
													dwPktLen,
							 	 					pbyResPkt, pdwIPMIResLen, timeout);
	if( wRetvalue != LIBIPMI_E_SUCCESS )
	{
		if ( IS_IPMI_ERROR(wRetvalue) )
		{
			uint8 byErr = GET_ERROR_CODE(wRetvalue);
			if( byErr > 0 && byErr <= LAST_RMCP_RAKP_STATUS_CODE )
				wRetvalue = STATUS_CODE( RMCP_RAKP_ERROR_FLAG, byErr );
		}
	}
	
	return wRetvalue;
}

/*---------------------------------------------------------------------
SendRAKPMessage_3

Sends RAKP message_3 command to BMC using network medium
----------------------------------------------------------------------*/
uint16 SendRAKPMessage_3(IPMI20_SESSION_T *pSession, uint8 *pbyResPkt,
	                 uint32 *pdwIPMIResLen, int timeout,uint8 RakpMsg2Status)
{
	uint8					pbyIPMIReq [MAX_REQUEST_SIZE];
	uint16					wRetvalue;
	RAKPMsg3Req_T			*pRAKPMsg3;
	Msg3hmac_T				Msg3hmac;
	uint16					wMsg3hmacLen;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/
#if LIBIPMI_IS_OS_LINUX()
        time_t t;
#endif

	pRAKPMsg3 = (RAKPMsg3Req_T	*)&pbyIPMIReq[0];

	/* initialize to 0 */
	memset(pRAKPMsg3, 0, sizeof(RAKPMsg3Req_T) );

#if LIBIPMI_IS_OS_LINUX()
	time(&t);
	pRAKPMsg3->MsgTag		   = pSession->hNetworkSession->byMsgTag	= (INT8U)rand_r ((INT32U*)&t);
#else
	pRAKPMsg3->MsgTag		   = pSession->hNetworkSession->byMsgTag	= (INT8U)rand();
#endif
	pRAKPMsg3->ManSysSessionID = pSession->hNetworkSession->dwSessionID;
//  	pRAKPMsg3->StatusCode      = SC_NO_ERROR;
	pRAKPMsg3->Reserved1 [0]   = pRAKPMsg3->Reserved1 [1] = 0;

	
	pRAKPMsg3->StatusCode      =    RakpMsg2Status;
	/* SIDC, HMACK[UID] (RC, SIDM, RoleM, ULengthM, < UNameM >) */
	/* Load Msg3 HMAC */
	memcpy (Msg3hmac.MgdSysRandNo, pSession->hNetworkSession->byMgdSysRandomNo, MAX_RAND_NO_LEN);
	Msg3hmac.RemConSessionID   = pSession->hNetworkSession->dwRemConSessID;
	Msg3hmac.Role			   = pSession->hNetworkSession->byRole | (ROLE_LOOKUP_NAME_ONLY << 4);
        Msg3hmac.UsrNameLen		   = strlen(pSession->hNetworkSession->szUserName);
	tempret=snprintf ((char*)Msg3hmac.UsrName,sizeof(Msg3hmac.UsrName),"%s", pSession->hNetworkSession->szUserName);
	if((tempret<0)||(tempret>=(signed)sizeof(Msg3hmac.UsrName)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
	wMsg3hmacLen				   = sizeof (Msg3hmac) - 16 + Msg3hmac.UsrNameLen;


	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1((uint8*)&pSession->hNetworkSession->szPwd[0], (uint16)strlen (pSession->hNetworkSession->szPwd),
							(uint8*)&Msg3hmac, wMsg3hmacLen,
							(uint8*)(pRAKPMsg3 + 1), MSG3_AUTH_CODE_LEN);
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);

	/* Send the packet*/
	wRetvalue = Send_RAW_IPMI2_0_Network_Command (pSession, PAYLOAD_TYPE_RAKP_MSG_3, 0,
													0, pbyIPMIReq,	sizeof(RAKPMsg3Req_T) + MSG3_AUTH_CODE_LEN,
							 	 					pbyResPkt, pdwIPMIResLen, timeout);


	if( wRetvalue != LIBIPMI_E_SUCCESS )
	{
		if ( IS_IPMI_ERROR(wRetvalue) )
		{
			uint8 byErr = GET_ERROR_CODE(wRetvalue);
			if( byErr > 0 && byErr <= LAST_RMCP_RAKP_STATUS_CODE )
				wRetvalue = STATUS_CODE( RMCP_RAKP_ERROR_FLAG, byErr );
		}
	}

	return wRetvalue;
}

/************************** Validate Message Functions *************************/
/*---------------------------------------------------------------------
ValidationSessionHeader

Validates received session header
----------------------------------------------------------------------*/
uint8 ValidationSessionHeader(IPMI20_SESSION_T *pSession,
								SessionHdr2_T*	pSessionHeader)
{
	/* Validate Authentication Type */

	if( pSession->hNetworkSession->byAuthType != pSessionHeader->AuthType)
	{
		DEBUG_PRINTF("ValidationSessionHeader: Invalid Auth Type\n");
		return LIBIPMI_E_INVALID_AUTHTYPE;
	}

	if (pSessionHeader->SessionID != 0 &&
		pSessionHeader->SessionID != pSession->hNetworkSession->dwRemConSessID &&
		pSessionHeader->SessionID != pSession->hNetworkSession->dwSessionID )
	{
		DEBUG_PRINTF("ValidationSessionHeader: Invalid Session ID Type\n");
		return LIBIPMI_E_INVALID_SESSIONID;
	}

	return LIBIPMI_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------
ValidationSessionTrailer

Validates the received session Trailer
----------------------------------------------------------------------*/
uint8 ValidationSessionTrailer(IPMI20_SESSION_T* pSession, uint8 *pbyResData)
{
	uint8			byPadLen;
	uint8			*pbyPadLen;
	uint32			dwTotalLen;
	uint8			pbyAuthCode[20];
	uint8			*pbyResAuthCode;
       uint16                     AuthCodeLen;

#ifdef ICC_OS_WINDOWS
    // hCryptProv:      Handle to a cryptographic service provider (CSP). 
    //                  This example retrieves the default provider for  
    //                  the PROV_RSA_FULL provider type.  
    // hKey:            Handle to a symmetric key. This example creates a 
    //                  key for the RC4 algorithm.
    // hHmacHash:       Handle to an HMAC hash.
    // dwDataLen:       Length, in bytes, of the hash.
    // HmacInfo:        Instance of an HMAC_INFO structure that contains 
    //                  information about the HMAC hash.
    HCRYPTPROV  hCryptProv  = NULL;
    HCRYPTKEY   hKey        = NULL;
    HCRYPTHASH  hHmacHash   = NULL;
    DWORD       dwDataLen   = 0;
    HMAC_INFO   HmacInfo;
    BYTE RC2KeyBlob[32] = {
        0x08,0x02,0x00,0x00,0x02,0x66,0x00,0x00, // BLOB header (PLAINTEXTKEYBLOB 0x8, CUR_BLOB_VERSION (2), CALG_RC2 0x00006602)
        0x00,0x00,0x00,0x00,                     // key length, in bytes
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 // SHA1 key with parity
        };
#endif


	/* Total IPMI Response data length including the pad bytes */
	dwTotalLen = sizeof (SessionHdr2_T) + (((SessionHdr2_T*)pbyResData)->IPMIMsgLen);

	/* Calculate the padding bytes */

	byPadLen = (sizeof(uint32) - ( (dwTotalLen+2) % sizeof(uint32))) % sizeof(uint32);

	/* Find the padding field in Response packet */

	pbyPadLen = (uint8*)(pbyResData + dwTotalLen + byPadLen);

	if(*pbyPadLen != byPadLen)
	{
		DEBUG_PRINTF("Padding bytes mismatch\n");
		return LIBIPMI_E_PADBYTES_MISMATCH;
	}

	/* AuthCode field starts after PadLength and NextHeader fields*/

	pbyResAuthCode = (pbyPadLen + 2);
       AuthCodeLen     = dwTotalLen + byPadLen + 2;

#ifdef ICC_OS_WINDOWS
    //--------------------------------------------------------------------
    // Zero the HMAC_INFO structure and use the SHA1 algorithm for hashing.
    memset(&HmacInfo, 0, sizeof(HmacInfo));
    HmacInfo.HashAlgid = CALG_SHA1;
    //--------------------------------------------------------------------
    // Acquire a handle to the default RSA cryptographic service provider.
    if (!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, NULL))
        if (GetLastError() == NTE_BAD_KEYSET)
            if(!CryptAcquireContext( &hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
                printf(" Error in AcquireContext 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Import a symmetric key from a hash object
    // Secure Hash Algorithm (SHA1) 160 bit
    RC2KeyBlob[8] = MAX_KEY1_SIZE;
    memcpy(&RC2KeyBlob[12], pSession->hNetworkSession->byKey1, MAX_KEY1_SIZE);

    if (!CryptImportKey(hCryptProv, &RC2KeyBlob, sizeof(RC2KeyBlob), 0, CRYPT_IPSEC_HMAC_KEY | CRYPT_EXPORTABLE, &hKey ))
        printf("Error in Import Key 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Create an HMAC
    if (!CryptCreateHash( hCryptProv, CALG_HMAC, hKey, 0, &hHmacHash))
        printf("Error in CryptCreateHash 0x%08x \n", GetLastError());

    if (!CryptSetHashParam( hHmacHash, HP_HMAC_INFO, (BYTE*)&HmacInfo, 0))
        printf("Error in CryptSetHashParam 0x%08x \n", GetLastError());
    
    if (!CryptHashData( hHmacHash, pbyResData, AuthCodeLen, 0))
        printf("Error in CryptHashData 0x%08x \n", GetLastError());

    //--------------------------------------------------------------------
    // Retrieve the hash value.
    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, NULL, &dwDataLen, 0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if (!CryptGetHashParam( hHmacHash, HP_HASHVAL, pbyAuthCode, &dwDataLen,0))
       printf("Error in CryptGetHashParam 0x%08x \n", GetLastError());

    if(hHmacHash)
        CryptDestroyHash(hHmacHash);
    if(hKey)
        CryptDestroyKey(hKey);
    if(hCryptProv)
        CryptReleaseContext(hCryptProv, 0);
#else
    ((unsigned char(*)(const EVP_MD *,const void *,int,const unsigned char *,size_t,unsigned char *,unsigned int*))g_CryptoHandle[CRYPTO_HMAC])( ((const EVP_MD *(*)(void))g_CryptoHandle[CRYPTO_EVP_SHA1])(), 
                   (INT8U *)pSession->hNetworkSession->byKey1, MAX_KEY1_SIZE
                   , (_FAR_ INT8U*)pbyResData, AuthCodeLen
                   , (INT8U *)pbyAuthCode, NULL);
#endif



	if ( memcmp (pbyResAuthCode, pbyAuthCode, MAX_AUTH_CODE_SIZE) != 0 )
	{
		DEBUG_PRINTF ("AuthCode Mismatch\n");
		return LIBIPMI_E_AUTHCODE_MISMATCH;
	}

	return LIBIPMI_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------
ValidateOpenSessionResponse

Validates Open Session Response packet received from BMC
----------------------------------------------------------------------*/
uint16 ValidateOpenSessionResponse(IPMI20_SESSION_T *pSession,
								    uint8 *pbyIPMIRes, uint32 dwIPMIResLen)
{
	RSSPOpenSessionRes_T*	pOpenSessionRes;

	pOpenSessionRes = (RSSPOpenSessionRes_T*)pbyIPMIRes;
	if(0)
	{
		dwIPMIResLen=dwIPMIResLen; /*-Wextra: Flag added for strict compilation*/
	}

	if ((pOpenSessionRes->MsgTag			!= pSession->hNetworkSession->byMsgTag) ||
		(pOpenSessionRes->RemConSessionID	!= pSession->hNetworkSession->dwRemConSessID) ||
		(pOpenSessionRes->StatusCode		!= SC_NO_ERROR))
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_INVALID_OPEN_SESSION_RESPONSE);


	pSession->hNetworkSession->byAuthAlgthm			= pOpenSessionRes->Auth.Algorithm;
	pSession->hNetworkSession->byIntegrityAlgthm	= pOpenSessionRes->Integrity.Algorithm;
	pSession->hNetworkSession->byEncryptAlgthm		= pOpenSessionRes->Confidentiality.Algorithm;
	pSession->hNetworkSession->dwSessionID			= pOpenSessionRes->MgdSysSessionID;

	if (pSession->hNetworkSession->byIntegrityAlgthm)
		pSession->hNetworkSession->byPerMsgAuthentication = true;
	if (pSession->hNetworkSession->byEncryptAlgthm)
		pSession->hNetworkSession->byIsEncryptionReq = true;

	return LIBIPMI_E_SUCCESS;
}

/*---------------------------------------------------------------------
ValidateRAKPMessage_2

Validates RAKP Message_2
----------------------------------------------------------------------*/
uint16 ValidateRAKPMessage_2(IPMI20_SESSION_T *pSession, uint8 *pbyIPMIRes,
								uint32 dwIPMIResLen)
{
	RAKPMsg2Res_T*	pRAKPMsg2;
	uint8 *			pAuthCode;
	uint8			pbyCalcAuthCode[32];
	uint8			byHmac[128];
	RAKPMsg1HMAC_T *pbyhmac;
	uint16			whmaclen;
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	pRAKPMsg2 = (RAKPMsg2Res_T *)pbyIPMIRes;
	pAuthCode = (uint8*) (pRAKPMsg2+1);
	pbyhmac = (RAKPMsg1HMAC_T *)&byHmac[0];


	if(0)
	{
		dwIPMIResLen=dwIPMIResLen; /*-Wextra: Flag added for strict compilation*/
	}

	if ((pRAKPMsg2->MsgTag			!= pSession->hNetworkSession->byMsgTag) ||
		(pRAKPMsg2->RemConSessionID	!= pSession->hNetworkSession->dwRemConSessID) ||
		(pRAKPMsg2->StatusCode		!= SC_NO_ERROR))
	{
			DEBUG_PRINTF("Validation failed\n");
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_INVALID_RAKP_MESSAGE_2);
	}

	/* SIDM, RC, GUIDC,

			HMACK[UID] (SIDM, SIDC, RM, RC, GUIDC, RoleM, ULengthM, < UNameM >) */


	memcpy (pSession->hNetworkSession->byMgdSysRandomNo, pRAKPMsg2->ManSysRandomNo, MAX_RAND_NO_LEN);
	memcpy (pSession->hNetworkSession->byGUID, pRAKPMsg2->ManSysGUID, MAX_GUID_SIZE);

	pbyhmac->RemConSessionID		= pSession->hNetworkSession->dwRemConSessID;
	pbyhmac->MgdSysSessionID		= pSession->hNetworkSession->dwSessionID;
	memcpy (pbyhmac->RemConRandNo, pSession->hNetworkSession->byRemConRandomNo, MAX_RAND_NO_LEN);
	memcpy (pbyhmac->MgdSysRandNo, pSession->hNetworkSession->byMgdSysRandomNo, MAX_RAND_NO_LEN);
	memcpy (pbyhmac->MgdSysGUID, pSession->hNetworkSession->byGUID, MAX_GUID_SIZE);
	pbyhmac->Role					= pSession->hNetworkSession->byRole | (ROLE_LOOKUP_NAME_ONLY << 4); 
	pbyhmac->UsrNameLen				= strlen (pSession->hNetworkSession->szUserName);
	tempret=snprintf ((char*)pbyhmac->UsrName,sizeof(pbyhmac->UsrName),"%s", pSession->hNetworkSession->szUserName);
	if((tempret<0)||(tempret>=(signed)sizeof(pbyhmac->UsrName)))
        {
                TCRIT("Buffer Overflow");
                return -1;
        }
	whmaclen					= sizeof (RAKPMsg1HMAC_T) -sizeof(pbyhmac->UsrName)+ pbyhmac->UsrNameLen;

	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1((uint8*)&pSession->hNetworkSession->szPwd[0], (uint16)strlen (pSession->hNetworkSession->szPwd),
							byHmac, whmaclen,
							pbyCalcAuthCode, MSG2_AUTH_CODE_LEN);
	else
	{
		DEBUG_PRINTF("Authentication algorithm not supported\n");
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);
	}

	/* Validate the AuthCode */
	if( memcmp(pbyCalcAuthCode, pAuthCode, MSG2_AUTH_CODE_LEN) != 0 )
	{
		DEBUG_PRINTF("Authode is invalid %d, %d\n", sizeof (RAKPMsg1HMAC_T), sizeof(RAKPMsg2Res_T));
		return STATUS_CODE(IPMI_ERROR_FLAG, LIBIPMI_E_AUTH_CODE_INVALID);
	}

	return LIBIPMI_E_SUCCESS;
}

/*---------------------------------------------------------------------
ValidateRAKPMessage_4

Validates RAKP Message_4
----------------------------------------------------------------------*/
uint16 ValidateRAKPMessage_4(IPMI20_SESSION_T *pSession, uint8 *pbyIPMIRes,
							 uint32 dwIPMIResLen, uint8 *byKG,
							 uint16 wKGLen)
{
	RAKPMsg4Res_T*		pRAKPMsg4;
	SIKhmac_T	    	SIKhmac;
	uint16				wSIKhmacLen;
	uint8				bySIK[MAX_SIK_SIZE];
	uint8				byHMAC[128];

	RAKPMsg4hmac_T		*pMsg4hmac;
	uint8				byHMACSIK[MAX_HMAC_SIZE];
	uint8				szKey[100];
	int tempret=0;/*To check return value of snprintf, it is not used any where else*/

	pRAKPMsg4 = (RAKPMsg4Res_T *)pbyIPMIRes;
	if(0)
	{
		dwIPMIResLen=dwIPMIResLen; /*-Wextra: Flag added for strict compilation*/
	}

	if ((pRAKPMsg4->MsgTag			!= pSession->hNetworkSession->byMsgTag) ||
		(pRAKPMsg4->RemConSessionID	!= pSession->hNetworkSession->dwRemConSessID) ||
		(pRAKPMsg4->StatusCode		!= SC_NO_ERROR))
			return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_INVALID_RAKP_MESSAGE_4);


	/* SIK = HMAC[KG] (RM | RC | RoleM | ULengthM | < UNameM >) */
	memcpy (SIKhmac.RemConRandNo, pSession->hNetworkSession->byRemConRandomNo, MAX_RAND_NO_LEN);
	memcpy (SIKhmac.MgdSysRandNo, pSession->hNetworkSession->byMgdSysRandomNo, MAX_RAND_NO_LEN);
	SIKhmac.Role				= pSession->hNetworkSession->byRole | (ROLE_LOOKUP_NAME_ONLY << 4); 
	SIKhmac.UsrNameLen			= strlen (pSession->hNetworkSession->szUserName);
	tempret=snprintf ((char *)SIKhmac.UsrName,sizeof(SIKhmac.UsrName),"%s", pSession->hNetworkSession->szUserName);
	if((tempret<0)||(tempret>=(signed)sizeof(SIKhmac.UsrName)))
        {
            TCRIT("Buffer Overflow");
            return -1;
        }
	wSIKhmacLen					= sizeof (SIKhmac_T) - 16 + SIKhmac.UsrNameLen;

	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1(byKG, wKGLen,
							(uint8*)&SIKhmac, wSIKhmacLen,
							(uint8*)bySIK, MAX_SIK_SIZE);
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);

	/* SIDM, HMAC[SIK] (RM, SIDC, GUIDC) */
	pMsg4hmac = (RAKPMsg4hmac_T*)&byHMAC[0];
	memcpy (pMsg4hmac->RemConRandNo, pSession->hNetworkSession->byRemConRandomNo, MAX_RAND_NO_LEN);
	memcpy (pMsg4hmac->MgdSysGUID, pSession->hNetworkSession->byGUID, MAX_GUID_SIZE);
	pMsg4hmac->MgdSysSessionID	= pSession->hNetworkSession->dwSessionID;

	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1(bySIK, MAX_SIK_SIZE,
							&byHMAC[0], sizeof (RAKPMsg4hmac_T),
							&byHMACSIK[0], MAX_HMAC_SIZE);
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);

	/* Compare the HMAC[SIK]*/
	if (memcmp (pRAKPMsg4 + 1, byHMACSIK, INT_CHK_VAL_SIZE))
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_INVALID_HMAC_SIK);

	/* Generate Key_1 K1 = HMAC[SIK] (const 1) */
	memset(szKey, 1, MAX_KEY1_CONST_SIZE);

	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1(bySIK, MAX_SIK_SIZE,
							&szKey[0], MAX_KEY1_CONST_SIZE,
							&pSession->hNetworkSession->byKey1[0], MAX_KEY1_SIZE);
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);

	/* Generate Key_2 K2 = HMAC[SIK] (const 2) */
	memset(szKey, 2, MAX_KEY1_CONST_SIZE);

	if(	pSession->hNetworkSession->byAuthAlgthm == AUTH_ALG_RAKP_HMAC_SHA1 )
		AuthAlg_HMAC_SHA1(bySIK, MAX_SIK_SIZE,
							&szKey[0], MAX_KEY2_CONST_SIZE,
							&pSession->hNetworkSession->byKey2[0], MAX_KEY2_SIZE);
	else
		return STATUS_CODE(MEDIUM_ERROR_FLAG, LIBIPMI_E_AUTH_ALG_UNSUPPORTED);



	return LIBIPMI_E_SUCCESS;
}
/****************End Validation Functions **************************************/






