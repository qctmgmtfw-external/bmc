/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/


#include <coreTypes.h>
#include "coreusb.h"

#include "ether.h"
#include "rndis.h"
#include "eth_mod.h"
#include "dbgout.h"


static uint32 OIDListSupported[] = 
{
	OID_GEN_SUPPORTED_LIST,
	OID_GEN_CURRENT_PACKET_FILTER,
	OID_802_3_PERMANENT_ADDRESS,
	OID_802_3_CURRENT_ADDRESS,
	OID_GEN_PHYSICAL_MEDIUM,
	OID_GEN_MEDIA_SUPPORTED,
	OID_GEN_MEDIA_IN_USE,
	OID_GEN_HARDWARE_STATUS,
	OID_GEN_LINK_SPEED,
	OID_GEN_MAXIMUM_FRAME_SIZE,
	OID_GEN_TRANSMIT_BLOCK_SIZE,
	OID_GEN_RECEIVE_BLOCK_SIZE,
	OID_GEN_VENDOR_ID,
	OID_GEN_VENDOR_DESCRIPTION,
	OID_GEN_VENDOR_DRIVER_VERSION,
	OID_GEN_CURRENT_PACKET_FILTER,
	OID_GEN_MAXIMUM_TOTAL_SIZE,
	OID_GEN_MEDIA_CONNECT_STATUS,
	OID_GEN_XMIT_OK,
	OID_GEN_RCV_OK,
	OID_GEN_XMIT_ERROR,
	OID_GEN_RCV_ERROR,
	OID_GEN_RCV_NO_BUFFER,
	OID_GEN_DIRECTED_BYTES_XMIT,
	OID_GEN_DIRECTED_FRAMES_XMIT,
	OID_GEN_MULTICAST_BYTES_XMIT,
	OID_GEN_MULTICAST_FRAMES_XMIT,
	OID_GEN_BROADCAST_BYTES_XMIT,
	OID_GEN_BROADCAST_FRAMES_XMIT,
	OID_GEN_DIRECTED_BYTES_RCV,
	OID_GEN_DIRECTED_FRAMES_RCV,
	OID_GEN_MULTICAST_BYTES_RCV,
	OID_GEN_MULTICAST_FRAMES_RCV,
	OID_GEN_BROADCAST_BYTES_RCV,
	OID_GEN_BROADCAST_FRAMES_RCV,
	OID_GEN_RCV_CRC_ERROR,
	OID_802_3_RCV_ERROR_ALIGNMENT,
	OID_802_3_XMIT_DEFERRED,
	OID_802_3_XMIT_MAX_COLLISIONS,
	OID_802_3_RCV_OVERRUN,
	OID_802_3_XMIT_UNDERRUN,
	OID_802_3_XMIT_HEARTBEAT_FAILURE,
	OID_802_3_XMIT_TIMES_CRS_LOST,
	OID_802_3_XMIT_LATE_COLLISIONS,
	OID_GEN_TRANSMIT_QUEUE_LENGTH,
	OID_802_3_MULTICAST_LIST,
	OID_802_3_MAC_OPTIONS,
	OID_802_3_MAXIMUM_LIST_SIZE
};

static RNDIS_MSG *QueueResponse(NET_DEVICE *pDev, RNDIS_MSG *pMsg, uint32 MessageLength)
{
	RNDIS_INFO *Info = (RNDIS_INFO *)pDev->Priv;
	RNDIS_MSG_QUEUE *Response;
	RNDIS_MSG *Message;
	
	Response = kmalloc(sizeof(Response->Prev) + 
	                   sizeof(Response->Next) + 
	                   MessageLength, GFP_ATOMIC);
	if(Response == NULL)
	{
		TCRIT("QueueResponse() : Allocation Failed\n");
		return NULL;
	}
	
	Response->Prev = Info->ResponseTail;
	Response->Next = NULL;
	
	if(Info->ResponseTail)
		Info->ResponseTail->Next = Response;
	else 
		Info->ResponseHead = Response; // initialise head since the list is empty
	
	Info->ResponseTail = Response;
	
	Message = &Response->Message;
	Message->MessageType = pMsg->MessageType | 0x80000000;
	Message->MessageLength = MessageLength;
	
	switch(Message->MessageType)
	{
	case REMOTE_NDIS_INITIALIZE_CMPLT:
		Info->State = RNDIS_INITIALIZED;
		Message->MessageType = REMOTE_NDIS_INITIALIZE_MSG | 0x80000000;
		Message->InitCmplt.RequestId = pMsg->Init.RequestId;
		break;
		
	case REMOTE_NDIS_QUERY_CMPLT:
		Message->QueryCmplt.RequestId = pMsg->Query.RequestId;
		Message->QueryCmplt.InformationBufferOffset = 16;
		Message->QueryCmplt.InformationBufferLength = Message->MessageLength - 24;
		break;
		
	case REMOTE_NDIS_SET_CMPLT:
		Message->SetCmplt.RequestId = pMsg->Set.RequestId;
		break;
		
	case REMOTE_NDIS_RESET_CMPLT:
		break;
		
	case REMOTE_NDIS_KEEPALIVE_CMPLT:
		Message->KeepAliveCmplt.RequestId = pMsg->KeepAlive.RequestId;
		break;
		
	default:
		TCRIT("QueueResponse() : Unknown response added to list\n");
		break;
	}
	return Message;
}

static int DequeueResponse(NET_DEVICE *pDev, uint8 *pBuffer, uint16 *pLength)
{
	RNDIS_INFO *Info = (RNDIS_INFO *)pDev->Priv;
	RNDIS_MSG_QUEUE *Head;
	RNDIS_MSG *Response;
	
	if((Head = Info->ResponseHead) == NULL)
		return 1;
	
	Response = &Head->Message;
	if(*pLength > Response->MessageLength)
		*pLength = Response->MessageLength;
	
	memcpy(pBuffer, Response, *pLength);
	
	if((Info->ResponseHead = Info->ResponseHead->Next) == NULL)
		Info->ResponseTail = NULL;
	else 
		Info->ResponseHead->Prev = NULL;
	
	kfree(Head);
	
	return 0;
}

static int HandleRndisInitializeMsg(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_INITIALIZE_CMPLT *Cmplt;
	
	if((Cmplt = &(QueueResponse(pDev, pMsg, 52)->InitCmplt)) == NULL)
		return 1;
	
	Cmplt->Status = RNDIS_STATUS_SUCCESS;
	Cmplt->MajorVersion = RNDIS_MAJOR_VERSION;
	Cmplt->MinorVersion = RNDIS_MINOR_VERSION;
	Cmplt->DeviceFlags = RNDIS_DF_CONNECTIONLESS;
	Cmplt->Medium = RNDIS_MEDIUM_802_3;
	Cmplt->MaxPacketsPerTransfer = 1;
	Cmplt->MaxTransferSize = 1600;
	Cmplt->PacketAlignmentFactor = 0;
	Cmplt->AFListOffset = 0;
	Cmplt->AFListSize = 0;
	
	return 0;
}


static int HandleRndisQueryMsg(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_INFO *Info = (RNDIS_INFO *) pDev->Priv;
	RNDIS_QUERY_CMPLT *Cmplt = NULL;
	
#define AllocCmplt(size) (Cmplt = &(QueueResponse(pDev, pMsg, 24 + (size))->QueryCmplt))
#define pDATA8 (((uint8 *)Cmplt) + 24)
#define pDATA16 ((uint16 *)pDATA8)
#define pDATA32 ((uint32 *)pDATA8)
	

	TDBG_FLAGGED(eth, DEBUG_ETH,"HandleRndisQueryMsg(%0x) called\n", pMsg->Query.Oid);

	switch(pMsg->Query.Oid)
	{
	/* Moved OID_802_3_PERMANENT_ADDRESS and OID_GEN_PHYSICAL_MEDIUM to the
	   top of the switch-case table due to their higher probability of hit */
	case OID_802_3_PERMANENT_ADDRESS:
	case OID_802_3_CURRENT_ADDRESS:
		if(AllocCmplt(ETH_ALEN) != NULL)
			memcpy(pDATA8, EthGetHostMac(), ETH_ALEN);
		break;
		
	case OID_GEN_PHYSICAL_MEDIUM:
	case OID_GEN_MEDIA_SUPPORTED:
	case OID_GEN_MEDIA_IN_USE:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(NDIS_MEDIUM_802_3);
		break;
		
	case OID_GEN_SUPPORTED_LIST:
		{
			int count;
			
			count = sizeof(OIDListSupported) / sizeof(uint32);
			if(AllocCmplt(count * sizeof(uint32)) == NULL)
				break;
			while(count--)
				pDATA32[count] = cpu_to_le32(OIDListSupported[count]);
		}
		break;
	
	case OID_GEN_HARDWARE_STATUS:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(0);
		break;
		
	case OID_GEN_LINK_SPEED:
		if(AllocCmplt(sizeof(uint32)) != NULL)
//		*pDATA32 = cpu_to_le32(Info->Speed);
		*pDATA32 = cpu_to_le32(100000);
		break;
		
	case OID_GEN_MAXIMUM_FRAME_SIZE:
	case OID_GEN_TRANSMIT_BLOCK_SIZE:
	case OID_GEN_RECEIVE_BLOCK_SIZE:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(pDev->Mtu);
		break;
		
	case OID_GEN_VENDOR_ID:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(Info->VendorID);
		break;
		
	case OID_GEN_VENDOR_DESCRIPTION:
		if(AllocCmplt(strlen(Info->VendorDesc)) != NULL)
			strcpy(pDATA8, Info->VendorDesc);
		break;
		
	case OID_GEN_VENDOR_DRIVER_VERSION:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(1);
		break;
		
	case OID_GEN_CURRENT_PACKET_FILTER:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(Info->Filter);
		break;
		
	case OID_GEN_MAXIMUM_TOTAL_SIZE:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(pDev->Mtu + 58);
		break;
		
	case OID_GEN_MEDIA_CONNECT_STATUS:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(Info->MediaState);
		break;
		
	case OID_GEN_XMIT_OK:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(Info->TxPackets);
		break;
		
	case OID_GEN_RCV_OK:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(Info->RxPackets);
		break;
		
	case OID_GEN_XMIT_ERROR:
	case OID_GEN_RCV_ERROR:
	case OID_GEN_RCV_NO_BUFFER:
	case OID_GEN_DIRECTED_BYTES_XMIT:
	case OID_GEN_DIRECTED_FRAMES_XMIT:
	case OID_GEN_MULTICAST_BYTES_XMIT:
	case OID_GEN_MULTICAST_FRAMES_XMIT:
	case OID_GEN_BROADCAST_BYTES_XMIT:
	case OID_GEN_BROADCAST_FRAMES_XMIT:
	case OID_GEN_DIRECTED_BYTES_RCV:
	case OID_GEN_DIRECTED_FRAMES_RCV:
	case OID_GEN_MULTICAST_BYTES_RCV:
	case OID_GEN_MULTICAST_FRAMES_RCV:
	case OID_GEN_BROADCAST_BYTES_RCV:
	case OID_GEN_BROADCAST_FRAMES_RCV:
	case OID_GEN_RCV_CRC_ERROR:
		/* 802.3 OIDs */
	case OID_802_3_RCV_ERROR_ALIGNMENT:
		// Don't know the values of the following two :
		//case OID_802_3_XMIT_ONE_COLLISIONS:
		//case OID_802_3_MORE_COLLISIONS:
	case OID_802_3_XMIT_DEFERRED:
	case OID_802_3_XMIT_MAX_COLLISIONS:
	case OID_802_3_RCV_OVERRUN:
	case OID_802_3_XMIT_UNDERRUN:
	case OID_802_3_XMIT_HEARTBEAT_FAILURE:
	case OID_802_3_XMIT_TIMES_CRS_LOST:
	case OID_802_3_XMIT_LATE_COLLISIONS:
		/* we just don't say that we don't support these */
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(0);
		break;
		
	case OID_GEN_TRANSMIT_QUEUE_LENGTH:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(1);
		break;
		
	case OID_802_3_MULTICAST_LIST:
		/* No multi-cast support as of now */
	case OID_802_3_MAC_OPTIONS:
		/*Do not really know about this one too */
		AllocCmplt(0);
		break;
		
	case OID_802_3_MAXIMUM_LIST_SIZE:
		if(AllocCmplt(sizeof(uint32)) != NULL)
			*pDATA32 = cpu_to_le32(0);
		break;
		
		/* Unimplemented OIDs */
	case OID_PNP_CAPABILITIES:
	case OID_PNP_QUERY_POWER:
	default:
		TWARN("HandleRndisQueryMsg() : Querying Unknown Oid : %d\n", pMsg->Query.Oid);
		if(AllocCmplt(0) == NULL)
			return 1;
		
		Cmplt->Status = RNDIS_STATUS_NOT_SUPPORTED;
		return 0;
	}
	
	if(Cmplt == NULL)
		return 1;
	
#undef AllocCmplt
#undef pDATA8
#undef pDATA16
#undef pDATA32
	
	Cmplt->Status = RNDIS_STATUS_SUCCESS;
	return 0;
}

static int HandleRndisSetMsg(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_INFO *Info = (RNDIS_INFO *)pDev->Priv;
	RNDIS_SET_CMPLT *Cmplt;
	uint32 Filter;
	
	if((Cmplt = &(QueueResponse(pDev, pMsg, 16)->SetCmplt)) == NULL)
		return 1;
	
	Cmplt->Status = RNDIS_STATUS_SUCCESS;
	
	TDBG_FLAGGED(eth, DEBUG_ETH,"HandleRndisSetMsg(%0x) called\n", pMsg->Set.Oid);
	
	switch(pMsg->Set.Oid)
	{
	case OID_GEN_CURRENT_PACKET_FILTER:
		if(pMsg->Set.InformationBufferLength != 4)
		{
			Cmplt->Status = RNDIS_STATUS_INVALID_DATA;
			break;
		}
		
		Filter = *((uint32 *)
				(((uint8 *)Cmplt) + pMsg->Set.InformationBufferOffset));
		Info->Filter = le32_to_cpu(Filter);
		printk ("Filter = %x\n", (unsigned int)Filter);
		Info->State = (Info->Filter) ? 
			RNDIS_DATA_INITIALIZED : RNDIS_INITIALIZED;
		break;
		
		/* Unimplemented OIDs */
	case OID_802_3_MULTICAST_LIST:
	case OID_GEN_RNDIS_CONFIG_PARAMETER:
	case OID_PNP_SET_POWER:
	case OID_PNP_ENABLE_WAKE_UP:
	case OID_PNP_ADD_WAKE_UP_PATTERN:
	case OID_PNP_REMOVE_WAKE_UP_PATTERN:
	default:
		TWARN("HandleRndisSetMsg() : Trying to set Unknown Oid : %d\n", pMsg->Set.Oid);
		Cmplt->Status = RNDIS_STATUS_NOT_SUPPORTED;
	}
	
	return 0;
}

static int HandleRndisResetMsg(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_RESET_CMPLT *Cmplt;
	
	if((Cmplt = &(QueueResponse(pDev, pMsg, 16)->ResetCmplt)) == NULL)
		return 1;
	
	Cmplt->Status = RNDIS_STATUS_SUCCESS;
	Cmplt->AddressingReset = 0;
	
	return 0;
}

static int HandleRndisKeepAliveMsg(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_KEEPALIVE_CMPLT *Cmplt;
	
	if((Cmplt = &(QueueResponse(pDev, pMsg, 16)->KeepAliveCmplt)) == NULL)
		return 1;
	
	Cmplt->Status = RNDIS_STATUS_SUCCESS;
	
	return 0;
}


static int HandleRndisCommand(NET_DEVICE *pDev, RNDIS_MSG *pMsg)
{
	RNDIS_INFO *Info = (RNDIS_INFO *)pDev->Priv;
	
	TDBG_FLAGGED(eth, DEBUG_ETH,"HandleRndisCommand(%0x) called\n", pMsg->MessageType);
	
	switch(pMsg->MessageType)
	{
	case REMOTE_NDIS_INITIALIZE_MSG:
		return HandleRndisInitializeMsg(pDev, pMsg);
		
	case REMOTE_NDIS_QUERY_MSG:
		return HandleRndisQueryMsg(pDev, pMsg);
		
	case REMOTE_NDIS_SET_MSG:
		printk ("**************REMOTE_NDIS_SET_MSG**************\n");
		return HandleRndisSetMsg(pDev, pMsg);
		
	case REMOTE_NDIS_HALT_MSG:
		ClearRndisPendingResponse ();
		printk ("**************REMOTE_NDIS_HALT_MSG ???**************\n");		
		Info->State = RNDIS_UNINITIALIZED;
		break;
	
	case REMOTE_NDIS_RESET_MSG:
		printk ("**************REMOTE_NDIS_RESET_MSG ???**************\n");	
		ClearRndisPendingResponse ();
		return HandleRndisResetMsg(pDev, pMsg);
		
	case REMOTE_NDIS_KEEPALIVE_MSG:
		printk ("**************REMOTE_NDIS_KEEPALIVE_MSG ???**************\n");						
		return HandleRndisKeepAliveMsg(pDev, pMsg);
		
	case REMOTE_NDIS_INDICATE_STATUS_MSG:
		TWARN("HandleRndisCommand(): Unimplemented MessageType : %d\n", pMsg->MessageType);
		return 1;
		
	default:
		TWARN("HandleRndisCommand(): Unknown MessageType : %d\n", pMsg->MessageType);
		return 1;
	}
	
	return 0;
}

int HandleRndisRequest(NET_DEVICE *pDev, DEVICE_REQUEST *pRequest)
{
	int ret = 0;

	RNDIS_MSG *pMsg;
	
	TDBG_FLAGGED(eth, DEBUG_ETH,"HandleRndisRequest(%0x) called\n", pRequest->bRequest);
	
	switch(pRequest->bRequest)
	{
	case CDC_SEND_ENCAPSULATED_COMMAND:
		pRequest->Status = 0;
		pMsg = (RNDIS_MSG *) pRequest->Data;
		if((ret = HandleRndisCommand(pDev, pMsg)) == 0)
			SetRndisPendingResponse();
		break;
		
	case CDC_GET_ENCAPSULATED_RESPONSE:
		pRequest->Status = DequeueResponse(pDev,
		                                   pRequest->Data, &pRequest->wLength);

		break;
		
	default:
		TWARN("HandleRndisRequest() : Unknown request : %d\n", pRequest->bRequest);
		ret = 1;
	}
	
	return ret;
}
