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
#include "descriptors.h"
#include "hid.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"

#define DATA_QUEUE_SIZE		64
#define MAX_HID_IFACES		4

typedef struct
{
	uint8 Queue [DATA_QUEUE_SIZE][MAX_KEYBD_DATA_SIZE];
	int WaitForHostToReceiveKeybdData;
	int WrIndex;
	int RdIndex;
	int Size;
	spinlock_t	Lock;
	
} KEYBD_QUEUE_T;

static KEYBD_QUEUE_T KeybdQueue [MAX_HID_IFACES];
static int FirstTime [MAX_HID_IFACES];


/************************ Keyboard report Descriptor **************************/
static uint8 KeybdReport[] =
{
	USAGE_PAGE1(GENERIC_DESKTOP) >> 8,
	USAGE_PAGE1(GENERIC_DESKTOP) & 0xFF,
	USAGE1(KEYBOARD) >> 8,
	USAGE1(KEYBOARD) & 0xFF,
	COLLECTION1(APPLICATION) >> 8,
	COLLECTION1(APPLICATION) & 0xFF,
			USAGE_PAGE1(KEYBOARD_KEYPAD) >> 8,
			USAGE_PAGE1(KEYBOARD_KEYPAD) & 0xFF,
			USAGE_MINIMUM1(224) >> 8,
			USAGE_MINIMUM1(224) & 0xFF,
 			USAGE_MAXIMUM1(231) >> 8,
 			USAGE_MAXIMUM1(231) & 0xFF,
 			LOGICAL_MINIMUM1(0) >> 8,
 			LOGICAL_MINIMUM1(0) & 0xFF,
 			LOGICAL_MAXIMUM1(1) >> 8,
 			LOGICAL_MAXIMUM1(1) & 0xFF,
 			REPORT_SIZE1(1) >> 8,
 			REPORT_SIZE1(1) & 0xFF,
 			REPORT_COUNT1(8) >> 8,
 			REPORT_COUNT1(8) & 0xFF,
 			INPUT3(DATA, VARIABLE, ABSOLUTE) >> 8,
 			INPUT3(DATA, VARIABLE, ABSOLUTE) & 0xFF,
 			REPORT_COUNT1(1) >> 8,
 			REPORT_COUNT1(1) & 0xFF,
 			REPORT_SIZE1(8) >> 8,
 			REPORT_SIZE1(8) & 0xFF,
 			INPUT1(CONSTANT) >> 8,
 			INPUT1(CONSTANT) & 0xFF,
 			REPORT_COUNT1(5) >> 8,
 			REPORT_COUNT1(5) & 0xFF,
 			REPORT_SIZE1(1) >> 8,
 			REPORT_SIZE1(1) & 0xFF,
 			USAGE_PAGE1(LEDS) >> 8,
 			USAGE_PAGE1(LEDS) & 0xFF,
 			USAGE_MINIMUM1(1) >> 8,
			USAGE_MINIMUM1(1) & 0xFF,
 			USAGE_MAXIMUM1(5) >> 8,
 			USAGE_MAXIMUM1(5) & 0xFF,
 			OUTPUT3(DATA, VARIABLE, ABSOLUTE) >> 8,
 			OUTPUT3(DATA, VARIABLE, ABSOLUTE) & 0xFF,
			REPORT_COUNT1(1) >> 8,
 			REPORT_COUNT1(1) & 0xFF,
 			REPORT_SIZE1(3) >> 8,
 			REPORT_SIZE1(3) & 0xFF,
 			OUTPUT1(CONSTANT) >> 8,
			OUTPUT1(CONSTANT) & 0xFF,
			REPORT_COUNT1(6) >> 8,
 			REPORT_COUNT1(6) & 0xFF,	
 			REPORT_SIZE1(8) >> 8,
 			REPORT_SIZE1(8) & 0xFF,
 			LOGICAL_MINIMUM1(0) >> 8,
 			LOGICAL_MINIMUM1(0) & 0xFF,
		    LOGICAL_MAXIMUM2(255) >> 16,
		    (uint8)(LOGICAL_MAXIMUM2(255) >> 8),
		    LOGICAL_MAXIMUM2(255) & 0xFF,
			USAGE_PAGE1(KEYBOARD_KEYPAD) >> 8,
			USAGE_PAGE1(KEYBOARD_KEYPAD) & 0xFF,
			USAGE_MINIMUM1(0) >> 8,
			USAGE_MINIMUM1(0) & 0xFF,
			USAGE_MAXIMUM2(255) >> 16,
			(uint8)(USAGE_MAXIMUM2(255) >> 8),
			USAGE_MAXIMUM2(255) & 0xFF,
			INPUT2(DATA, ARRAY) >> 8,
 			INPUT2(DATA, ARRAY) & 0xFF,
	END_COLLECTION
};


uint16
GetKeybdReportSize(void)
{
	return sizeof(KeybdReport);
}


static USB_HID_DESC *	   
GetHidDesc(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex)
{
	USB_INTERFACE_DESC *IfaceDesc;
	uint8 *Buffer;

	IfaceDesc = UsbCore.CoreUsbGetInterfaceDesc (DevNo, CfgIndex, IntIndex);
	if (IfaceDesc == NULL)
		return NULL;
	
	Buffer = (uint8*) IfaceDesc;
	Buffer += sizeof (USB_INTERFACE_DESC);
	if (Buffer [1] == HID_DESC)
			return ((USB_HID_DESC *)Buffer);
	else
			return NULL;

}

int
SetKeybdSetReport(uint8 DevNo,uint8 IfNum,uint8 *Data,uint16 DataLen)
{

	HID_IF_DATA *KeybdData;

	TDBG_FLAGGED(hid, DEBUG_KEYBD,"SetKeybdSetReport() is called for Interface %d\n", IfNum);
	KeybdData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,IfNum);
	if (KeybdData == NULL)
		return 0;

	if (!(KeybdData->SetReportCalled))
		return 0;

	if (DataLen != 1)
		return 0;

	KeybdData->SetReportCalled =0;
	iUSB.iUSBKeybdLedRemoteCall(DevNo,IfNum,*Data);			
	return 0;
}

int
KeybdReqHandler(uint8 DevNo, uint8 ifnum,DEVICE_REQUEST *pRequest)
{
	HID_IF_DATA *KeybdData;
	USB_HID_DESC *KeybdHidDesc;	
	uint8 CfgIndex;
	int RetVal = 0;	

	TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): Called! for ifnum =%d\n", ifnum);

	pRequest->Command = "KEYBOARD REQUEST";
	if (REQ_RECIP_MASK(pRequest->bmRequestType) != REQ_INTERFACE)
		return 1;
		
	KeybdData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (KeybdData == NULL)
		return 1;

	switch(pRequest->bRequest) 
	{
		case GET_REPORT:
			if(pRequest->wLength > sizeof(KeybdData->ReportReq))
				pRequest->wLength = sizeof(KeybdData->ReportReq);
			memcpy(pRequest->Data, &(KeybdData->ReportReq), pRequest->wLength);
			break;
			
		case GET_IDLE: 
			TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): GET_IDLE request\n");
			if(pRequest->wLength > sizeof(KeybdData->Idle))
					pRequest->wLength = sizeof(KeybdData->Idle);
				memcpy(pRequest->Data, &(KeybdData->Idle), pRequest->wLength);
			break;
			
		case GET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): GET_PROTOCOL request\n");
			if(pRequest->wLength > sizeof(KeybdData->Protocol))
				pRequest->wLength = sizeof(KeybdData->Protocol);
			memcpy(pRequest->Data, &(KeybdData->Protocol), pRequest->wLength);
			break;
		case SET_REPORT:
			TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): SET_REPORT request \n");
			/* Set flag to read the report later */
			KeybdData->SetReportCalled = 1;
			SetKeybdSetReport(DevNo, ifnum, pRequest->Data, pRequest->wLength);
			break;
			
		case SET_IDLE:
			TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): SET_IDLE request with Value 0x%x\n",(pRequest->wValue >> 8));			
			KeybdData->Idle = (uint8)(pRequest->wValue >> 8);
			break;
			
		case SET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): SET_PROTOCOL request\n");
			KeybdData->Protocol = (uint8) pRequest->wValue;
			break;
			
		case GET_DESCRIPTOR:
			switch (pRequest->wValue >> 8)
			{
				case HID_DESC:
					TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): Received GET_DESCRIPTOR(HID_DESC)\n");
					CfgIndex = UsbCore.CoreUsbDevGetCfgIndex (DevNo);
					KeybdHidDesc = GetHidDesc(DevNo,CfgIndex,ifnum);					
					if(pRequest->wLength > sizeof(USB_HID_DESC))
						pRequest->wLength = sizeof(USB_HID_DESC);
					memcpy(pRequest->Data, (uint8 *)KeybdHidDesc, pRequest->wLength);
					break;
					
				case REPORT_DESC:
					TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdReqHandler(): Received GET_DESCRIPTOR(REPORT_DESC)\n");
					if (pRequest->wLength > sizeof(KeybdReport))
						pRequest->wLength = sizeof(KeybdReport);
					memcpy(pRequest->Data, &KeybdReport[0], pRequest->wLength);
					break;
					
				case PHYSICAL_DESC:
					TWARN("KeybdReqHandler(): Received GET_DESCRIPTOR(PHYSICAL_DESC). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
					
				default:
					TWARN("KeybdReqHandler(): Received GET_DESCRIPTOR(unknown). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
			}
			break;
		default:
			TWARN("KeybdReqHandler(): Unknown CLASS request. Not Supported!!!\n");
			/* not supported */
			RetVal =1;
			break;
	}
	
	return RetVal;
}

static void
ClearKeybdDataQueue (uint8 DevNo, uint8 IfNum)
{
	unsigned long Flags;

	spin_lock_irqsave (&KeybdQueue[IfNum].Lock, Flags);
	KeybdQueue[IfNum].RdIndex = 0;
	KeybdQueue[IfNum].WrIndex = 0;
	KeybdQueue[IfNum].Size = 0;
	KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 0;
	spin_unlock_irqrestore (&KeybdQueue[IfNum].Lock, Flags);
}


void
KeybdRemHandler(uint8 DevNo,uint8 ifnum)
{
	HID_IF_DATA *IntData;
	
	TDBG_FLAGGED(hid, DEBUG_KEYBD,"KeybdRemHandler(): Disconnect Handler Called\n");

	IntData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (IntData == NULL)
		return;	

	IntData->Idle 	  = 0;
	IntData->Protocol = 1;
	IntData->ReportReq = 0;

	/* On a Reset, clear the Led Status to 0 */
	iUSB.iUSBKeybdLedRemoteCall(DevNo,ifnum,0);
	ClearKeybdDataQueue (DevNo, ifnum);
	return;
}

int
SendKeybdEvent(uint8 DevNo,uint8 IfNum, uint8 *Buff)
{
	uint8 epnum; 
	int Ret;

	/* Get Endpoint of Keybd */	
	epnum = UsbCore.CoreUsbGetDataInEp(DevNo,IfNum);		
	
	/* Send KeyBoard Data */
	TDBG_FLAGGED(hid, DEBUG_KEYBD,"Sending 8 Bytes of Keyboard Data to Device %d IF %d EP %d\n",DevNo,IfNum,epnum);

	Ret = UsbCore.CoreUsbWriteData(DevNo,epnum,Buff,8,NO_ZERO_LEN_PKT);
	if (Ret != 0)
	{
		TDBG_FLAGGED(hid, DEBUG_KEYBD,"ERROR: SendKeybdEvent(): Failed to Transmit Data (timed out or interrupted)\n");
	}

	return Ret;
}



int
SendKeybdData (uint8 DevNo,uint8 IfNum, uint8 *Buff)
{
	unsigned long Flags;	
	int ret = 0;
	int WriteIndex;
	
	if (FirstTime[IfNum] == 0)
	{
		FirstTime[IfNum] = 1;
		KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 0;
		KeybdQueue[IfNum].WrIndex = 0;
		KeybdQueue[IfNum].RdIndex = 0;
		KeybdQueue[IfNum].Size = 0;
		spin_lock_init (&KeybdQueue[IfNum].Lock);
	}

	if (0 == UsbCore.CoreUsbDevGetCfgIndex (DevNo))
	{
		return 0;
	}
	spin_lock_irqsave (&KeybdQueue[IfNum].Lock, Flags);
	WriteIndex = KeybdQueue[IfNum].WrIndex;
	memcpy ((void*)KeybdQueue[IfNum].Queue[WriteIndex], Buff, MAX_KEYBD_DATA_SIZE);
	KeybdQueue[IfNum].WrIndex = (KeybdQueue[IfNum].WrIndex + 1) % DATA_QUEUE_SIZE;
	KeybdQueue[IfNum].Size++;
	if (KeybdQueue[IfNum].Size >= DATA_QUEUE_SIZE)
	{
		TWARN ("Keybd Data Queue is full for Iface number %d\n", IfNum);
		KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 0;
	}

	if (KeybdQueue[IfNum].WaitForHostToReceiveKeybdData == 0)
	{
		KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 1;
		spin_unlock_irqrestore (&KeybdQueue[IfNum].Lock, Flags);
		ret = SendKeybdEvent (DevNo, IfNum, KeybdQueue[IfNum].Queue[WriteIndex]);
		spin_lock_irqsave (&KeybdQueue[IfNum].Lock, Flags);
		if (ret)
		{
			KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 0;
		}
		spin_unlock_irqrestore (&KeybdQueue[IfNum].Lock, Flags);
	}
	else
	{
		spin_unlock_irqrestore (&KeybdQueue[IfNum].Lock, Flags);
	}

	return ret;
	
}

void
KeybdTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum)
{
	int ReadIndex;
	
	if (FirstTime[IfNum] == 0)
	{
		TWARN ("Keybd queue not yet initialized\n"); 
		return;
	}
	
	spin_lock (&KeybdQueue[IfNum].Lock);
	if (KeybdQueue[IfNum].WaitForHostToReceiveKeybdData == 0)
	{
		spin_unlock (&KeybdQueue[IfNum].Lock);
		return;
	}
	KeybdQueue[IfNum].RdIndex = (KeybdQueue[IfNum].RdIndex + 1) % DATA_QUEUE_SIZE;
	KeybdQueue[IfNum].Size --;

	if (0 != KeybdQueue[IfNum].Size)
	{
		ReadIndex = KeybdQueue[IfNum].RdIndex;
		spin_unlock (&KeybdQueue[IfNum].Lock);
		SendKeybdEvent (DevNo, IfNum, KeybdQueue[IfNum].Queue[ReadIndex]);
	}
	else
	{
		KeybdQueue[IfNum].WaitForHostToReceiveKeybdData = 0;
		spin_unlock (&KeybdQueue[IfNum].Lock);
	}

}


