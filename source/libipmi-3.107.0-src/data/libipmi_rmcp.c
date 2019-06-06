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
* Filename: libipmi_rmcp.c
*
* Descriptions: Contains implementation of RMCP layer API required
*   session layer for establishing connection with BMC
*
* Author: Rajasekhar
*
******************************************************************/
#include "libipmi_session.h"
#include "libipmi_network.h"
#include "libipmi_rmcp.h"
#include "libipmi_errorcodes.h"

#include <string.h>

/*-----------------------------------------------------------------------
RMCP_Connect

Connect to RMCP layer of BMC
------------------------------------------------------------------------*/
uint8 RMCP_Connect(LAN_CHANNEL_T *LanInfo, char *szIPAddress, uint16 wPort)
{
    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
        return UDP_Connect(LanInfo, szIPAddress, wPort);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
        return TCP_Connect(LanInfo, szIPAddress, wPort);
    }
    else
    {
        DEBUG_PRINTF("RMCP.c : Undefined LAN protocol\n");
        return LIBIPMI_MEDIUM_E_UNSUPPORTED;
    }
}

/*-----------------------------------------------------------------------
RMCP_Close

Close connection to RMCP layer of BMC
------------------------------------------------------------------------*/
void RMCP_Close(LAN_CHANNEL_T *LanInfo )
{
    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
        UDP_Close(LanInfo);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
        TCP_Close(LanInfo);
    }
}

/*-----------------------------------------------------------------------
RMCP_SendData

Fills RMCP Header and sends to session layer network layer
------------------------------------------------------------------------*/
uint8 RMCP_SendData(LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 dwSize, int timeout)
{
    RMCPHdr_T	*pRMCPHeader;

    /* Fill RMCP Header */

    pRMCPHeader = (RMCPHdr_T	*)szBuffer;
    pRMCPHeader->Version		= 0x06;		/* RMCP Version 1.0 */
    pRMCPHeader->Reserved		= 0x00;		/* Reserverd */
    pRMCPHeader->SeqNumber		= 0xFF;		/* always FFh for IPMI */
    pRMCPHeader->MsgClass		= 0x07;		/* always 07h for IPMI */

    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
        return UDP_SendData(LanInfo, szBuffer, dwSize, timeout);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
        return TCP_SendData(LanInfo, szBuffer, dwSize, timeout);
    }
    else
    {
        DEBUG_PRINTF("RMCP.c : Undefined LAN protocol\n");
        return LIBIPMI_MEDIUM_E_UNSUPPORTED;
    }
}

/*-----------------------------------------------------------------------
RMCP_ReceiveData

Receives data from RMCP layer and Validates RMCP Header
------------------------------------------------------------------------*/
uint8 RMCP_ReceiveData(LAN_CHANNEL_T *LanInfo, char *szBuffer, uint32 *pdwSize, int timeout)
{
	RMCPHdr_T	*pRMCPHeader;
	uint8 		nRetval;

    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
	    nRetval = UDP_ReceiveData(LanInfo, szBuffer, pdwSize, timeout);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
	    nRetval = TCP_ReceiveData(LanInfo, szBuffer, pdwSize, timeout);
    }
    else
    {
        DEBUG_PRINTF("RMCP.c : Undefined LAN protocol\n");
        return LIBIPMI_MEDIUM_E_UNSUPPORTED;
    }

	if(nRetval == 0 )
	{
		/* Validate RMCP Header */
		pRMCPHeader = (RMCPHdr_T	*)szBuffer;
		if( pRMCPHeader->Version	  != 0x06		/* RMCP Version 1.0 */
			|| pRMCPHeader->Reserved  != 0x00		/* Reserverd */
			|| pRMCPHeader->SeqNumber != 0xFF		/* always FFh for IPMI */
			|| pRMCPHeader->MsgClass  != 0x07		/* always 07h for IPMI */
			)
		{
			DEBUG_PRINTF("Invalid RMCP Packet received \n");
			return LIBIPMI_RMCP_E_INVALID_PACKET;
		}
	}

	return nRetval;
}

/*-----------------------------------------------------------------------
RMCP_Ping

Sends RMCP Ping message to BMC
------------------------------------------------------------------------*/
uint8 RMCP_Ping(LAN_CHANNEL_T *LanInfo, int timeout)
{
	RMCPPing_T	pingMsg;

	/* IPMI2.0 Specification Table 13-6 */

	/* Fill Ping Packet */
	/* ASF IANA 4542 = 0x11BE */
	pingMsg.IANANum[0] = 0x00;			/* IANA Number */
	pingMsg.IANANum[1] = 0x00;			/* IANA Number */
	pingMsg.IANANum[2] = 0x11;			/* IANA Number */
	pingMsg.IANANum[3] = 0xBE;			/* IANA Number */	
	pingMsg.MsgType			= 0x80;				/* Presence Ping */
	pingMsg.MsgTag			= 0x00;				/* Mesage tag*/
	pingMsg.Reserved1		= 0x00;				/* Reserved */
	pingMsg.DataLen			= 0x00;				/* DataLength */


	/* Create RMCP Header */
	pingMsg.Hdr.Version			= 0x06;
	pingMsg.Hdr.Reserved		= 0x00;
	pingMsg.Hdr.SeqNumber		= 0xFF;
	pingMsg.Hdr.MsgClass		= 0x06;

    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
	    return UDP_SendData(LanInfo, (char *)&pingMsg, sizeof(RMCPPing_T), timeout);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
	    return TCP_SendData(LanInfo, (char *)&pingMsg, sizeof(RMCPPing_T), timeout);
    }
    else
    {
        DEBUG_PRINTF("RMCP.c : Undefined LAN protocol\n");
        return LIBIPMI_MEDIUM_E_UNSUPPORTED;
    }

}

/*-----------------------------------------------------------------------
RMCP_Pong

Receives RMCP Pong message from BMC
------------------------------------------------------------------------*/
uint8 RMCP_Pong(LAN_CHANNEL_T *LanInfo, int timeout)
{
	uint8		pbyLanResPacket[MAX_RESPONSE_SIZE] = {0};
	RMCPPong_T	*pongMsg = (RMCPPong_T*)&pbyLanResPacket[0];
	uint32		dwSize;
	uint8 nRetval;

    if(LanInfo->bProtocol == NETWORK_MEDIUM_UDP)
    {
	    nRetval = UDP_ReceiveData(LanInfo, (char *)pongMsg, &dwSize, timeout);
    }
    else if(LanInfo->bProtocol == NETWORK_MEDIUM_TCP)
    {
	    nRetval = TCP_ReceiveData(LanInfo, (char *)pongMsg, &dwSize, timeout);
    }
    else
    {
        DEBUG_PRINTF("RMCP.c : Undefined LAN protocol\n");
        return LIBIPMI_MEDIUM_E_UNSUPPORTED;
    }


	if(nRetval == LIBIPMI_STATUS_SUCCESS)
	{
		/* Validate Pong message Header */
		if( pongMsg->Hdr.Version	  != 0x06		/* RMCP Version 1.0 */
			|| pongMsg->Hdr.Reserved  != 0x00		/* Reserverd */
			|| pongMsg->Hdr.SeqNumber != 0xFF		/* always FFh for IPMI */
			|| pongMsg->Hdr.MsgClass  != 0x06		/* always 07h for IPMI */
			)
		{
			DEBUG_PRINTF("Invalid Pong Message \n");
			return LIBIPMI_RMCP_E_INVALID_PONG;
		}

		/* IPMI2.0 Specification Table 13-7 */
		if( pongMsg->IANANum[0]!=0x00
			||pongMsg->IANANum[1]!=0x00
			||pongMsg->IANANum[2]!=0x11
			||pongMsg->IANANum[3]!=0xBE
			|| pongMsg->MsgType != 0x40					/* Message Type shd be 40h */
			|| pongMsg->Support != 0x81
			)
		{
			DEBUG_PRINTF("Invalid Pong Message \n");
			return LIBIPMI_RMCP_E_INVALID_PONG;
		}

		DEBUG_PRINTF("RMCP Pong received successfully \n");
	}

	return nRetval;
}

