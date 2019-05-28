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

#ifndef RNDIS_H
#define RNDIS_H

#include "ndis.h"

#define RNDIS_MAJOR_VERSION  1
#define RNDIS_MINOR_VERSION  0

#define USB_RNDIS_SUBCLASS                  0x02
#define USB_RNDIS_PROTOCOL                  0xFF

/* Various class requests used by RNDIS */
#define CDC_SEND_ENCAPSULATED_COMMAND       0x00
#define CDC_GET_ENCAPSULATED_RESPONSE       0x01
#define CDC_REQ_SET_LINE_CODING             0x20
#define CDC_REQ_GET_LINE_CODING             0x21
#define CDC_REQ_SET_CONTROL_LINE_STATE      0x22
#define CDC_REQ_SEND_BREAK                  0x23
#define CDC_SET_ETHERNET_MULTICAST_FILTERS  0x40
#define CDC_SET_ETHERNET_PM_PATTERN_FILTER  0x41
#define CDC_GET_ETHERNET_PM_PATTERN_FILTER  0x42
#define CDC_SET_ETHERNET_PACKET_FILTER      0x43
#define CDC_GET_ETHERNET_STATISTIC          0x44

/* Various RNDIS status values */
#define RNDIS_STATUS_SUCCESS             0x00000000
#define RNDIS_STATUS_FAILURE             0xC0000001
#define RNDIS_STATUS_INVALID_DATA        0xC0010015
#define RNDIS_STATUS_NOT_SUPPORTED       0xC00000BB
#define RNDIS_STATUS_MEDIA_CONNECT       0x4001000B
#define RNDIS_STATUS_MEDIA_DISCONNECT    0x4001000C

/* MessageType values */
#define REMOTE_NDIS_PACKET_MSG           0x00000001
#define REMOTE_NDIS_INITIALIZE_MSG       0x00000002
#define REMOTE_NDIS_HALT_MSG             0x00000003
#define REMOTE_NDIS_QUERY_MSG            0x00000004
#define REMOTE_NDIS_SET_MSG              0x00000005
#define REMOTE_NDIS_RESET_MSG            0x00000006
#define REMOTE_NDIS_INDICATE_STATUS_MSG  0x00000007
#define REMOTE_NDIS_KEEPALIVE_MSG        0x00000008
#define REMOTE_NDIS_INITIALIZE_CMPLT     0x80000002
#define REMOTE_NDIS_QUERY_CMPLT          0x80000004
#define REMOTE_NDIS_SET_CMPLT            0x80000005
#define REMOTE_NDIS_RESET_CMPLT          0x80000006
#define REMOTE_NDIS_KEEPALIVE_CMPLT      0x80000008

#define RNDIS_DF_CONNECTIONLESS          0x00000001
#define RNDIS_DF_CONNECTION_ORIENTED     0x00000002

#define RNDIS_MEDIUM_802_3               NDIS_MEDIUM_802_3

/* Non-NDIS specific OIDs */
#define OID_PNP_CAPABILITIES             0xFD010100
#define OID_PNP_SET_POWER                0xFD010101
#define OID_PNP_QUERY_POWER              0xFD010102
#define OID_PNP_ADD_WAKE_UP_PATTERN      0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN   0xFD010104
#define OID_PNP_ENABLE_WAKE_UP           0xFD010106

#define RNDIS_RESPONSE_AVAILABLE         0x00000001

#define RNDIS_TIMEOUT_CONTROL    10
#define RNDIS_TIMEOUT_KEEPALIVE  5

typedef struct
{
	uint32 MessageType;
	uint32 MessageLength;
} __attribute__((packed)) RNDIS_MSG_HEADER;

/*
 * RNDIS Control Messages
 */

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 MajorVersion;
	uint32 MinorVersion;
	uint32 MaxTransferSize;
} __attribute__((packed)) RNDIS_INITIALIZE_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Status;
	uint32 MajorVersion;
	uint32 MinorVersion;
	uint32 DeviceFlags;
	uint32 Medium;
	uint32 MaxPacketsPerTransfer;
	uint32 MaxTransferSize;
	uint32 PacketAlignmentFactor;
	uint32 AFListOffset;
	uint32 AFListSize;
}__attribute__((packed)) RNDIS_INITIALIZE_CMPLT;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
} __attribute__((packed)) RNDIS_HALT_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Oid;
	uint32 InformationBufferLength;
	uint32 InformationBufferOffset;
	uint32 DeviceVcHandle;
}__attribute__((packed)) RNDIS_QUERY_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Status;
	uint32 InformationBufferLength;
	uint32 InformationBufferOffset;
	union{
		uint8 mac[6];
	};
}__attribute__((packed)) RNDIS_QUERY_CMPLT;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Oid;
	uint32 InformationBufferLength;
	uint32 InformationBufferOffset;
	uint32 DeviceVcHandle;
}__attribute__((packed)) RNDIS_SET_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Status;
}__attribute__((packed)) RNDIS_SET_CMPLT;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 Reserved;
}__attribute__((packed)) RNDIS_RESET_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 Status;
	uint32 AddressingReset;
}__attribute__((packed)) RNDIS_RESET_CMPLT;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 Status;
	uint32 StatusBufferLength;
	uint32 StatusBufferOffset;
	struct Rndis_Diagnostic_Info
	{
		uint32 DiagStatus;
		uint32 ErrorOffset;
	} Diag;
}__attribute__((packed)) RNDIS_INDICATE_STATUS_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
}__attribute__((packed)) RNDIS_KEEPALIVE_MSG;

typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 RequestId;
	uint32 Status;
}__attribute__((packed)) RNDIS_KEEPALIVE_CMPLT;

/* RNDIS Data Message */
typedef struct
{
	RNDIS_MSG_HEADER Header;
	uint32 DataOffset;
	uint32 DataLength;
	uint32 OOBDataOffset;
	uint32 OOBDataLength;
	uint32 NumOOBDataElements;
	uint32 PerPacketInfoOffset;
	uint32 PerPacketInfoLength;
	uint32 VcHandle;
	uint32 Reserved;
}__attribute__((packed)) RNDIS_PACKET_MSG;

typedef union
{
	struct
	{
		uint32 MessageType;
		uint32 MessageLength;
	};
	RNDIS_INITIALIZE_MSG Init;
	RNDIS_INITIALIZE_CMPLT InitCmplt;
	RNDIS_QUERY_MSG Query;
	RNDIS_QUERY_CMPLT QueryCmplt;
	RNDIS_SET_MSG Set;
	RNDIS_SET_CMPLT SetCmplt;
	RNDIS_RESET_MSG Reset;
	RNDIS_RESET_CMPLT ResetCmplt;
	RNDIS_KEEPALIVE_MSG KeepAlive;
	RNDIS_KEEPALIVE_CMPLT KeepAliveCmplt;
	RNDIS_INDICATE_STATUS_MSG Status;
	RNDIS_PACKET_MSG Packet;
}__attribute__((packed)) RNDIS_MSG ;

typedef enum {
	RNDIS_UNINITIALIZED,
	RNDIS_INITIALIZED,
	RNDIS_DATA_INITIALIZED
} RNDIS_STATE;

typedef struct RNDIS_MSG_QUEUE
{
	struct RNDIS_MSG_QUEUE *Prev, *Next;
	RNDIS_MSG Message;
}  RNDIS_MSG_QUEUE;

typedef struct 
{
	uint32 Notification;
	uint32 Reserved;
} RNDIS_NOTIFICATION;

typedef struct
{
	RNDIS_STATE State;
	uint32 Speed;
	uint32 MediaState;
	uint32 VendorID;
	const char *VendorDesc;
	uint32 Filter;
	RNDIS_MSG_QUEUE *ResponseHead;
	RNDIS_MSG_QUEUE *ResponseTail;
	uint32 TxPackets;
	uint32 RxPackets;
} RNDIS_INFO;

int HandleRndisRequest(NET_DEVICE *pDev, DEVICE_REQUEST *pRequest);

#endif
