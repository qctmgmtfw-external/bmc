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
#include "linux/spinlock.h"
#include "coreusb.h"
#include "usb_core.h"
#include "ether.h"
#include "cdc.h"
#include "rndis.h"
#include "linux.h"
#include "eth_mod.h"
#include "dbgout.h"

#define CONFIG_RNDIS  1
#define CONFIG_ECM    2
#define ETH_SPEED ((19 * 64 * 1 * 1000 * 8) / 100)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static DEFINE_SPINLOCK(Lock);
#else
static spinlock_t Lock = SPIN_LOCK_UNLOCKED;
#endif

static int EthNetTxHandler(NET_PACKET *Pkt);
static int EthNetTxTimeoutHandler(void);

static RNDIS_INFO RndisInfo = {
	.State = RNDIS_UNINITIALIZED,
	.Speed = ETH_SPEED,
	.MediaState = NDIS_MEDIA_STATE_CONNECTED,
};
static NET_DEVICE Dev = {
	.Mtu = ETH_DEV_MTU,
	.NetTxHandler = EthNetTxHandler,
	.NetTxTimeoutHandler = EthNetTxTimeoutHandler, 
	.Priv = &RndisInfo
};
static uint8 DeviceNumber;
static uint8 EthInitDone = 0;
static uint8 EthControlIfNum = 0; /* Should be initialized when creating descriptor */
static uint8 EthDataIfNum = 1; /* Should be initialized when creating descriptor */
static atomic_t RndisRequestPending = ATOMIC_INIT(0);
static NET_PACKET * volatile Current = NULL;


static inline int IsRndis(void)
{
	return UsbCore.CoreUsbDevGetCfgIndex(DeviceNumber) == CONFIG_RNDIS;
}

void RndisSetVendor(uint32 VID, const char *Desc)
{
	RndisInfo.VendorID = VID;
	RndisInfo.VendorDesc = Desc;
}

void EthSetIntf(uint8 Control, uint8 Data)
{
	EthControlIfNum = Control;
	EthDataIfNum = Data;
}

uint8 *EthGetHostMac(void)
{
	return Dev.HostMac;
}

uint8 *EthGetDevMac(void)
{
	return Dev.DevMac;
}

void EthSetHostMac(uint8 *Mac)
{
	memcpy(Dev.HostMac, Mac, ETH_ALEN);
}

void EthSetDevMac(uint8 *Mac)
{
	memcpy(Dev.DevMac, Mac, ETH_ALEN);
}

static void RndisNotifyHost(void)
{
	uint8 epnum;
	static RNDIS_NOTIFICATION notification = {
		.Notification = RNDIS_RESPONSE_AVAILABLE
	};
	
	epnum = NotifyEpNum [0];
	UsbCore.CoreUsbWriteDataNonBlock(DeviceNumber, epnum, (uint8 *)&notification, 8, NO_ZERO_LEN_PKT);
}

void ClearRndisPendingResponse(void)
{
	atomic_set(&RndisRequestPending, 0);
}

void SetRndisPendingResponse(void)
{
//	if(atomic_inc_return(&RndisRequestPending) == 1)
	if(atomic_inc_return(&RndisRequestPending) > 0)

		RndisNotifyHost();
	if (atomic_read (&RndisRequestPending) > 1)
	{
		TCRIT ("SetRndisPendingResponse(): RndisRequestPending value cannot be more than 1");
	}
}

void EthRxHandler(uint8 DevNo,uint8 ifnum, uint8 epnum)
{
	static int Length;
	static uint8 Data[ETH_DEV_MTU + 100];// or maybe [1514];
	int EpSize;
	uint8 CfgIndex;
	uint32 RxLen;
	uint8 *RxData;

	if(ifnum != EthDataIfNum)
	{
		TCRIT("EthRxHandler() : ifnum is %d\n", ifnum);
		return;
	}
	
	if(!EthInitDone)
	{
		TCRIT ("EthRxHandler(): EthInitDone should be non zero. This should not happen............\n");
		return;
	}
	
	/* Get the Received Data and Length */
	RxLen = UsbCore.CoreUsbGetRxDataLen(DevNo,epnum);	
	RxData  = UsbCore.CoreUsbGetRxData(DevNo,epnum);

	if (Length + RxLen > sizeof (Data))
	{
		TCRIT ("EthRxHandler() : The Packet received was bigger than %d\n", sizeof (Data));
		Length = 0;
		return;
	}
	memcpy(Data + Length, RxData, RxLen);
	Length += RxLen;
	CfgIndex = UsbCore.CoreUsbDevGetCfgIndex(DevNo);
	EpSize = UsbCore.CoreUsbGetMaxPacketSize (DevNo, CfgIndex, epnum, DIR_OUT);

	if(RxLen == EpSize)
	{
		return;
	}
	RxData = Data;
	RxLen = Length;
	
	Length = 0; // get ready for the next packet
	
	if(IsRndis()) // Need special care for rndis packets
	{

		RNDIS_MSG *pMsg = (RNDIS_MSG *) Data;

		if(pMsg->MessageType != REMOTE_NDIS_PACKET_MSG)
		{
			TCRIT ("EthRxHandler () : The message received is not RNDIS Packet Message\n");
			return;
		}
		RxData += 8 + pMsg->Packet.DataOffset;
		RxLen   = pMsg->Packet.DataLength;
		
		RndisInfo.RxPackets++;
	}

	if (0 != UsbCore.CoreUsbNet_OSPushPacket(&Dev, RxData, RxLen))
	{
		TCRIT ("EthRxHandler() : Error Pushing Packet onto network stack...\n");
	}
}


static int EthNetTxTimeoutHandler (void)
{
	NET_PACKET *Pkt;
	unsigned char unlocked_flag = 0;
	unsigned long flags;

	spin_lock_irqsave(&Lock, flags);
	if (Current)
	{
		Pkt = Current;
		Current = NULL;
		unlocked_flag = 1;
		spin_unlock_irqrestore(&Lock, flags);
		//utHandler
		UsbCore.CoreUsbNet_TxComplete(Pkt);
		if(IsRndis())
			RndisInfo.TxPackets++;
	}
	if(unlocked_flag != 1)
		spin_unlock_irqrestore(&Lock, flags);
	return 0;

}

void EthTxHandler(uint8 DevNo,uint8 ifnum, uint8 epnum)
{

	if(ifnum == EthControlIfNum)
	{
		if(atomic_dec_return(&RndisRequestPending) > 0)
			RndisNotifyHost();
		return;
	}
	
	if(ifnum != EthDataIfNum)
	{
		printk("EthTxHandler() : ifnum is %d\n", ifnum);
		return;
	}

	EthNetTxHandler(NULL);
}

static inline void SendUsbData(uint8 *Data, uint16 Length, uint8 ZeroLenPkt)
{
	uint8 epnum, unlocked_flag = 0;
	unsigned long flags;
	
	if(Data == NULL || Length == 0)
	{
		TCRIT("SendUsbData(): Data length is 0.\n");
		return;
	}
	
	epnum = UsbCore.CoreUsbGetDataInEp(DeviceNumber, EthDataIfNum);
	if(IsRndis())
		epnum = DataInEpNum [0];
	else
		epnum = DataInEpNum [1];

	if ( 0 != UsbCore.CoreUsbWriteDataNonBlock(DeviceNumber, epnum, Data, Length, ZeroLenPkt))
	{
		NET_PACKET *Pkt;
		TCRIT ("SendUsbData() : Error sending the packet for DevNo %d epnum %d\n", DeviceNumber, epnum);
		spin_lock_irqsave(&Lock, flags);
		if ((epnum != 0) && (Current))
		{
			Pkt = Current;
			Current = NULL;
			spin_unlock_irqrestore(&Lock, flags);
			unlocked_flag = 1;
			UsbCore.CoreUsbNet_TxComplete(Pkt);
		}
		if(unlocked_flag != 1)
			spin_unlock_irqrestore(&Lock, flags);
	}

}

static int EthNetTxHandler(NET_PACKET *Pkt) // Pkt will be null when called by EthTxHandler
{
	uint8 *Data;
	uint16 Count;
	static uint8 Buf[512];
	int EpSize;
	int EpNum;
	uint8 CfgIndex, unlocked_flag = 0;
	unsigned long flags;

	CfgIndex = UsbCore.CoreUsbDevGetCfgIndex (EthDevNo);
	if (CfgIndex == 0)
	{
		spin_lock_irqsave(&Lock, flags);
		if ((Current == NULL) && (Pkt != NULL))
		{
			spin_unlock_irqrestore(&Lock, flags);
			unlocked_flag = 1;
			UsbCore.CoreUsbNet_TxComplete(Pkt);
		}
		else if ((Current != NULL) && (Pkt == NULL))
		{
			Pkt = Current;
			Current = NULL;
			spin_unlock_irqrestore(&Lock, flags);
			unlocked_flag = 1;
			UsbCore.CoreUsbNet_TxComplete(Pkt);
		}
		if(unlocked_flag != 1)
			spin_unlock_irqrestore(&Lock, flags);
		return 0;		
	}
	EpNum = UsbCore.CoreUsbGetDataInEp (EthDevNo, EthDataIfNum);
	CfgIndex = UsbCore.CoreUsbDevGetCfgIndex(EthDevNo);
	EpSize = UsbCore.CoreUsbGetMaxPacketSize (EthDevNo, CfgIndex, EpNum, DIR_IN);

	if(Pkt) // new packet
	{
		spin_lock_irqsave(&Lock, flags);
		if(Current != NULL) // This is a bug
		{
			TCRIT("EthNetTxHandler() : packet pending %p %p\n", Current, Pkt);
			spin_unlock_irqrestore(&Lock, flags);
			return 0;
		}
		Current = Pkt;
		if(IsRndis()) // First RNDIS usb packet will contain header
		{
			RNDIS_MSG *pMsg = (RNDIS_MSG *) Buf;

			pMsg->MessageType = REMOTE_NDIS_PACKET_MSG;
			pMsg->MessageLength = 44 + Current->Size;
			pMsg->Packet.DataOffset = 44 - 8;
			pMsg->Packet.DataLength = Current->Size;
			pMsg->Packet.OOBDataOffset = 0;
			pMsg->Packet.OOBDataLength = 0;
			pMsg->Packet.NumOOBDataElements = 0;
			pMsg->Packet.PerPacketInfoOffset = 0;
			pMsg->Packet.PerPacketInfoLength = 0;
			pMsg->Packet.VcHandle = 0;
			pMsg->Packet.Reserved = 0;
			
			Count = EpSize - 44;
			if(Current->Size < Count)
				Count = Current->Size;
			
			memcpy(Buf + 44, Current->Data, Count);
			Current->Sent += Count;
			
			if(Current->Size == Count)
			{
				spin_unlock_irqrestore(&Lock, flags);
				unlocked_flag = 1;
				SendUsbData(Buf, Count + 44, ZERO_LEN_PKT);
			}	
			else
			{
				spin_unlock_irqrestore(&Lock, flags);
				unlocked_flag = 1;
				SendUsbData(Buf, Count + 44, NO_ZERO_LEN_PKT);
			}	
			if(unlocked_flag != 1)
				spin_unlock_irqrestore(&Lock, flags);
			return 0;
		}
		if(unlocked_flag != 1)
				spin_unlock_irqrestore(&Lock, flags);
	}
	spin_lock_irqsave(&Lock, flags);
	if(Current == NULL) // This will be a bug
	{
		TCRIT("EthNetTxHandler() : no packets\n");
		spin_unlock_irqrestore(&Lock, flags);
		return 0;
	}
	
	if(Current->Sent == Current->Size) // Tx completed
	{
		
		Pkt = Current;
		Current = NULL;
		spin_unlock_irqrestore(&Lock, flags);
		UsbCore.CoreUsbNet_TxComplete(Pkt);
		if(IsRndis())
			RndisInfo.TxPackets++;
		
		return 0;
	}
	if (Current->Sent > Current->Size)
	{
		TCRIT("EthNetTxHandler() : we can not send more bytes than actual bytes.\n");
		TCRIT("EthNetTxHandler() : Current->Sent = %d and Current->Size = %d\n", Current->Sent,Current->Size );	
		spin_unlock_irqrestore(&Lock, flags);
		return 0;
	}
	
	Data = Current->Data + Current->Sent;
	if((Count = Current->Size - Current->Sent) > EpSize)
		Count = EpSize;
	
	Current->Sent += Count;
	
	if(Current->Sent == Current->Size && Count == EpSize)
	{
		spin_unlock_irqrestore(&Lock, flags);
		unlocked_flag = 1;
		SendUsbData(Data, Count, ZERO_LEN_PKT);
	}
	else
	{
		spin_unlock_irqrestore(&Lock, flags);
		unlocked_flag = 1;
		SendUsbData(Data, Count, NO_ZERO_LEN_PKT);
	}
	if(unlocked_flag != 1)
		spin_unlock_irqrestore(&Lock, flags);
	return 0;
}

int EthReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *pRequest)
{

	if(IsRndis())
		return HandleRndisRequest(&Dev, pRequest);
	
	/* Handle CDC requests if any */
	switch(pRequest->bRequest)
	{
	case CDC_REQ_SET_LINE_CODING:
	case CDC_REQ_GET_LINE_CODING:
	case CDC_REQ_SET_CONTROL_LINE_STATE:
	case CDC_REQ_SEND_BREAK:
	case CDC_SET_ETHERNET_MULTICAST_FILTERS:
	case CDC_SET_ETHERNET_PM_PATTERN_FILTER:
	case CDC_GET_ETHERNET_PM_PATTERN_FILTER:
	case CDC_SET_ETHERNET_PACKET_FILTER:
	case CDC_GET_ETHERNET_STATISTIC:
		TWARN("EthReqHandler(): Unimplemented CDC request : %d\n", pRequest->bRequest);
		return 1;
		
	default:
		TCRIT("EthReqHandler() : Unknown request : %d\n", pRequest->bRequest);
		return 1;
		
	}
	
	return 0;
}

void EthRemHandler(uint8 DevNo,uint8 ifnum)
{
	DeviceNumber = DevNo;
}


int EthCreateDevice(void)
{
	int r;
	if((r = UsbCore.CoreUsbNet_OSRegisterDriver(&Dev)) == 0)
		EthInitDone = 1;
	
	return r;
}

void EthRemoveDevice(void)
{
	UsbCore.CoreUsbNet_OSUnregisterDriver(&Dev);
}
