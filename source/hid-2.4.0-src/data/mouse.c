/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#include <linux/spinlock.h>
#include <coreTypes.h>
#include "descriptors.h"
#include "hid.h"
#include "hid_usb.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"

#define DATA_QUEUE_SIZE		64
#define MAX_HID_IFACES		4

typedef struct
{
	uint8 Queue [DATA_QUEUE_SIZE][MAX_MOUSE_DATA_SIZE];
	int WaitForHostToReceiveMouseData;
	int WrIndex;
	int RdIndex;
	int Size;
	spinlock_t	Lock;
	
} MOUSE_QUEUE_T;

static MOUSE_QUEUE_T MouseQueue [MAX_HID_IFACES];
static int FirstTime [MAX_HID_IFACES];
static unsigned long LastMouseEventTimeStamp [MAX_HID_IFACES] = {0,0,0,0}; 
static unsigned long CurrentTimeStamp [MAX_HID_IFACES] = {0,0,0,0}; 

/************************* Mouse Report Descriptor **************************/

//Wheel Support for Mouse
static uint8 AbsoluteMouseReport[]  = {
    USAGE_PAGE1(GENERIC_DESKTOP) >> 8,
    USAGE_PAGE1(GENERIC_DESKTOP) & 0xFF,
    USAGE1(MOUSE) >> 8,
    USAGE1(MOUSE) & 0xFF,
    COLLECTION1(APPLICATION) >> 8,
    COLLECTION1(APPLICATION) & 0xFF,
    USAGE1(POINTER) >> 8,
    USAGE1(POINTER) & 0xFF,
    COLLECTION1(PHYSICAL) >> 8,
    COLLECTION1(PHYSICAL) & 0xFF,
    USAGE_PAGE1(BUTTON) >> 8,
    USAGE_PAGE1(BUTTON) & 0xFF,
    USAGE_MINIMUM1(1) >> 8,
    USAGE_MINIMUM1(1) & 0xFF,
    USAGE_MAXIMUM1(3) >> 8,
    USAGE_MAXIMUM1(3) & 0xFF,
    LOGICAL_MINIMUM1(0) >> 8,
    LOGICAL_MINIMUM1(0) & 0xFF,
    LOGICAL_MAXIMUM1(1) >> 8,
    LOGICAL_MAXIMUM1(1) & 0xFF,
    REPORT_COUNT1(3) >> 8,
    REPORT_COUNT1(3) & 0xFF,
    REPORT_SIZE1(1) >> 8,
    REPORT_SIZE1(1) & 0xFF,
    INPUT3(DATA, VARIABLE, ABSOLUTE) >> 8,
    INPUT3(DATA, VARIABLE, ABSOLUTE) & 0xFF,
    REPORT_COUNT1(1) >> 8,
    REPORT_COUNT1(1) & 0xFF,
    REPORT_SIZE1(5) >> 8,
    REPORT_SIZE1(5) & 0xFF,
    INPUT1(CONSTANT) >> 8,
    INPUT1(CONSTANT) & 0xFF,
    USAGE_PAGE1(GENERIC_DESKTOP) >> 8,
    USAGE_PAGE1(GENERIC_DESKTOP) & 0xFF,
    USAGE1(X) >> 8,
    USAGE1(X) & 0xFF,
    USAGE1(Y) >> 8,
    USAGE1(Y) & 0xFF,
    LOGICAL_MINIMUM1(0) >> 8,
    LOGICAL_MINIMUM1(0) & 0xFF,
    LOGICAL_MAXIMUM2(32767) >> 16,
    (uint8)(LOGICAL_MAXIMUM2(32767) >> 8),
    LOGICAL_MAXIMUM2(32767) & 0xFF,
    REPORT_SIZE1(16) >> 8,
    REPORT_SIZE1(16) & 0xFF,
    REPORT_COUNT1(2) >> 8,
    REPORT_COUNT1(2) & 0xFF,
    INPUT3(DATA, VARIABLE, ABSOLUTE) >> 8,
    INPUT3(DATA, VARIABLE, ABSOLUTE) & 0xFF,
	USAGE1(WHEEL) >> 8,
	USAGE1(WHEEL) & 0xFF,
    LOGICAL_MINIMUM1(-127) >> 8,
    LOGICAL_MINIMUM1(-127) & 0xFF,
    LOGICAL_MAXIMUM1(127) >> 8,
    LOGICAL_MAXIMUM1(127) & 0xFF,
    REPORT_SIZE1(8) >> 8,
    REPORT_SIZE1(8) & 0xFF,
    REPORT_COUNT1(1) >> 8,
    REPORT_COUNT1(1) & 0xFF,
	INPUT3(DATA, VARIABLE, RELATIVE) >> 8,
    INPUT3(DATA, VARIABLE, RELATIVE) & 0xFF,
    
    END_COLLECTION,
    END_COLLECTION
};

//Wheel Support for Mouse
static uint8 RelativeMouseReport[] = 
{
    USAGE_PAGE1(GENERIC_DESKTOP) >> 8,
    USAGE_PAGE1(GENERIC_DESKTOP) & 0xFF,
    USAGE1(MOUSE) >> 8,
    USAGE1(MOUSE) & 0xFF,
    COLLECTION1(APPLICATION) >> 8,
    COLLECTION1(APPLICATION) & 0xFF,
    USAGE1(POINTER) >> 8,
    USAGE1(POINTER) & 0xFF,
    COLLECTION1(PHYSICAL) >> 8,
    COLLECTION1(PHYSICAL) & 0xFF,
    USAGE_PAGE1(BUTTON) >> 8,
    USAGE_PAGE1(BUTTON) & 0xFF,
    USAGE_MINIMUM1(1) >> 8,
    USAGE_MINIMUM1(1) & 0xFF,
    USAGE_MAXIMUM1(3) >> 8,
    USAGE_MAXIMUM1(3) & 0xFF,
    LOGICAL_MINIMUM1(0) >> 8,
    LOGICAL_MINIMUM1(0) & 0xFF,
    LOGICAL_MAXIMUM1(1) >> 8,
    LOGICAL_MAXIMUM1(1) & 0xFF,
    REPORT_COUNT1(3) >> 8,
    REPORT_COUNT1(3) & 0xFF,
    REPORT_SIZE1(1) >> 8,
    REPORT_SIZE1(1) & 0xFF,
    INPUT3(DATA, VARIABLE, ABSOLUTE) >> 8,
    INPUT3(DATA, VARIABLE, ABSOLUTE) & 0xFF,
    REPORT_COUNT1(1) >> 8,
    REPORT_COUNT1(1) & 0xFF,
    REPORT_SIZE1(5) >> 8,
    REPORT_SIZE1(5) & 0xFF,
    INPUT1(CONSTANT) >> 8,
    INPUT1(CONSTANT) & 0xFF,
    USAGE_PAGE1(GENERIC_DESKTOP) >> 8,
    USAGE_PAGE1(GENERIC_DESKTOP) & 0xFF,
    USAGE1(X) >> 8,
    USAGE1(X) & 0xFF,
    USAGE1(Y) >> 8,
    USAGE1(Y) & 0xFF,

    LOGICAL_MINIMUM1(-127) >> 8,
    LOGICAL_MINIMUM1(-127) & 0xFF,
    LOGICAL_MAXIMUM1(127) >> 8,
    LOGICAL_MAXIMUM1(127) & 0xFF,
    REPORT_SIZE1(8) >> 8,
    REPORT_SIZE1(8) & 0xFF,
    REPORT_COUNT1(2) >> 8,
    REPORT_COUNT1(2) & 0xFF,
    INPUT3(DATA, VARIABLE, RELATIVE) >> 8,
    INPUT3(DATA, VARIABLE, RELATIVE) & 0xFF,

	USAGE1(WHEEL) >> 8,
	USAGE1(WHEEL) & 0xFF,
    LOGICAL_MINIMUM1(-127) >> 8,
    LOGICAL_MINIMUM1(-127) & 0xFF,
    LOGICAL_MAXIMUM1(127) >> 8,
    LOGICAL_MAXIMUM1(127) & 0xFF,
    REPORT_SIZE1(8) >> 8,
    REPORT_SIZE1(8) & 0xFF,
    REPORT_COUNT1(1) >> 8,
    REPORT_COUNT1(1) & 0xFF,
	INPUT3(DATA, VARIABLE, RELATIVE) >> 8,
    INPUT3(DATA, VARIABLE, RELATIVE) & 0xFF,

    END_COLLECTION,
    END_COLLECTION

    
};

uint16
GetMouseReportSize(void)
{

    uint16 mouseReportSize = 0;

    /* Mouse should change to relative or absolute dynamically */
    if ( MouseMode == RELATIVE_MOUSE_MODE ) 
	{
        mouseReportSize = sizeof(RelativeMouseReport);
        TDBG_FLAGGED(hid, DEBUG_MOUSE,"GetMouseReportSize:MouseMode is %d  ReportSize = %x \n",MouseMode,mouseReportSize);
    }
    else if ( MouseMode == ABSOLUTE_MOUSE_MODE )
    {
        mouseReportSize = sizeof(AbsoluteMouseReport);
        TDBG_FLAGGED(hid, DEBUG_MOUSE,"GetMouseReportSize:MouseMode is %d ReportSize = %x \n",MouseMode,mouseReportSize);
    }
    else
	{
	    TCRIT("ERROR: MouseMode is Not defined. Sending Relative Mouse Mode\n");
        mouseReportSize = sizeof(RelativeMouseReport);
	}

    return mouseReportSize;
}

USB_HID_DESC *	   
GetHidDesc_Mouse (uint8 DevNo,uint8 CfgIndex, uint8 IntIndex)
{
	USB_INTERFACE_DESC *IfaceDesc;
	uint8 *Buffer;

	IfaceDesc = UsbCore.CoreUsbGetInterfaceDesc (DevNo, CfgIndex, IntIndex);
	if (IfaceDesc == NULL)
		return NULL;
	
	Buffer = (uint8*) IfaceDesc;
	Buffer += sizeof (USB_INTERFACE_DESC);
	if (Buffer [1] == HID_DESC)
	{
		uint16 ReportSize;
		ReportSize = GetMouseReportSize();
		Buffer[7] = (uint8)ReportSize;
		Buffer[8] = (uint8)((uint16)ReportSize >> 8);
		return ((USB_HID_DESC *)Buffer);
	}
	else
		return NULL;

}

int 
MouseReqHandler(uint8 DevNo, uint8 ifnum, DEVICE_REQUEST *pRequest)
{
	USB_HID_DESC *MouseHidDesc;
	HID_IF_DATA *MouseData;
	uint8 CfgIndex;
	int RetVal = 0;
	
    uint8 *pReportBuf = NULL;
    uint16 sizeReportBuf = 0 ;
	
	TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): Called!\n");

	pRequest->Command = "MOUSE REQUEST";

	if (REQ_RECIP_MASK(pRequest->bmRequestType) != REQ_INTERFACE)
		return 1;
		
	MouseData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (MouseData == NULL)
		return 1;
		

	switch(pRequest->bRequest) 
	{
		case GET_REPORT:
			TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): GET_REPORT requested!!\n");
			if(pRequest->wLength > sizeof(MouseData->ReportReq))
				pRequest->wLength = sizeof(MouseData->ReportReq);
			memcpy(pRequest->Data, &(MouseData->ReportReq), pRequest->wLength);
			break;
			
		case GET_IDLE: 
			TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): GET_IDLE request\n");
			if(pRequest->wLength > sizeof(MouseData->Idle))
				pRequest->wLength = sizeof(MouseData->Idle);
			memcpy(pRequest->Data, &(MouseData->Idle), pRequest->wLength);
			break;
			
		case GET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): GET_PROTOCOL request\n");
			if(pRequest->wLength > sizeof(MouseData->Protocol))
				pRequest->wLength = sizeof(MouseData->Protocol);
			memcpy(pRequest->Data, &(MouseData->Protocol), pRequest->wLength);
			break;
			
		case SET_REPORT:
			TWARN("MouseReqHMouseIfNumandler(): SET_REPORT request.Not Supported!!\n");
			/* not supported */
			RetVal = 1;
			break;
			
		case SET_IDLE:
			TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): SET_IDLE request with Value 0x%x\n",(pRequest->wValue >> 8));
			MouseData->Idle = (uint8)(pRequest->wValue >> 8);
			break;
			
		case SET_PROTOCOL:
			TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): SET_PROTOCOL request\n");
			MouseData->Protocol = (uint8) pRequest->wValue;
			break;
			
		case GET_DESCRIPTOR:
			switch ((pRequest->wValue) >> 8)
			{
				case HID_DESC:
					TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): Received GET_DESCRIPTOR(HID_DESC)\n");
					CfgIndex = UsbCore.CoreUsbDevGetCfgIndex (DevNo);
					MouseHidDesc = GetHidDesc_Mouse(DevNo,CfgIndex,ifnum);					
					if(pRequest->wLength > sizeof(USB_HID_DESC))
						pRequest->wLength = sizeof(USB_HID_DESC);
					memcpy(pRequest->Data, (uint8 *)MouseHidDesc, pRequest->wLength);
					break;
					
				case REPORT_DESC:
					TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseReqHandler(): Received GET_DESCRIPTOR(REPORT_DESC)\n");
                    if ( MouseMode == RELATIVE_MOUSE_MODE ) {
                        pReportBuf =  &RelativeMouseReport[0];
                        sizeReportBuf = sizeof(RelativeMouseReport);
                    }
                    else if ( MouseMode == ABSOLUTE_MOUSE_MODE ){
                        pReportBuf =  &AbsoluteMouseReport[0];
                        sizeReportBuf = sizeof(AbsoluteMouseReport);
                    } 
                    else
					{
	    				TCRIT("ERROR: MouseMode is Not defined. Sending Relative Mouse Mode\n");
                        pReportBuf =  &RelativeMouseReport[0];
                        sizeReportBuf = sizeof(RelativeMouseReport);
					}

					if(pRequest->wLength > sizeReportBuf)
						pRequest->wLength = sizeReportBuf;
					memcpy(pRequest->Data, pReportBuf, pRequest->wLength);
					break;
					
				case PHYSICAL_DESC:
					TWARN("MouseReqHandler(): Received GET_DESCRIPTOR(PHYSICAL_DESC). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
					
				default:
					TWARN("MouseReqHandler(): Received GET_DESCRIPTOR(unknown). Not Supported!!!\n");
					/* not supported */
					RetVal =1;
					break;
			}
			break;
		default:
			TWARN("MouseReqHandler(): Unknown CLASS request. Not Supported!!!\n");
			/* not supported */
			RetVal =1;
			break;
	}
	
	return RetVal;
}

int
SendMouseEvent(uint8 DevNo,uint8 IfNum,uint8 *Buff)
{
	uint8 epnum;
    uint8 copySize = 0;
	int   Ret;

	/* Get Endpoint of Mouse */	
	epnum = UsbCore.CoreUsbGetDataInEp(DevNo,IfNum);		

	/* Calculate packet Size */
    if ( MouseMode == RELATIVE_MOUSE_MODE )
		copySize = REL_EFFECTIVE_MOUSE_DATA_SIZE;
    else if ( MouseMode == ABSOLUTE_MOUSE_MODE )
		copySize = ABS_EFFECTIVE_MOUSE_DATA_SIZE;
    else 
	{
		TCRIT("ERROR: MouseMode is Not defined. Defaulting to Relative Mouse\n");
		copySize = REL_EFFECTIVE_MOUSE_DATA_SIZE;
	}

	/* Send Mouse Data */
	TDBG_FLAGGED(hid, DEBUG_MOUSE,"Sending %d Bytes of Mouse Data to Device %d IF %d EP %d\n",
													copySize,DevNo,IfNum,epnum);

	Ret = UsbCore.CoreUsbWriteData(DevNo,epnum,Buff, copySize, NO_ZERO_LEN_PKT);
	if (Ret != 0)
    {	
		TDBG_FLAGGED(hid, DEBUG_MOUSE, "ERROR: SendMouseEvent(): Failed to Transmit Data (timed out or interrupted)\n");
    }		

    return Ret;
}

static void
ClearMouseDataQueue (uint8 DevNo, uint8 IfNum)
{
	unsigned long Flags;

	spin_lock_irqsave (&MouseQueue[IfNum].Lock, Flags);
	MouseQueue[IfNum].RdIndex = 0;
	MouseQueue[IfNum].WrIndex = 0;
	MouseQueue[IfNum].Size = 0;
	MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0;
	spin_unlock_irqrestore (&MouseQueue[IfNum].Lock, Flags);
}
	
	
void 
MouseRemHandler(uint8 DevNo,uint8 ifnum)
{
	HID_IF_DATA *IntData;

	TDBG_FLAGGED(hid, DEBUG_MOUSE,"MouseRemHandler(): Disconnect Handler Called\n");

	IntData =(HID_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(DevNo,ifnum);
	if (IntData == NULL)
		return;	
	IntData->Idle 	  = 0;
	IntData->Protocol = 1;
	ClearMouseDataQueue (DevNo, ifnum);
	return;
}
int
SendMouseData (uint8 DevNo,uint8 IfNum, uint8 *Buff)
{
	unsigned long Flags;	
	int ret = 0;
	int WriteIndex;
	
	if (FirstTime[IfNum] == 0)
	{
		FirstTime[IfNum] = 1;
		MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0;
		MouseQueue[IfNum].WrIndex = 0;
		MouseQueue[IfNum].RdIndex = 0;
		MouseQueue[IfNum].Size = 0;
		spin_lock_init (&MouseQueue[IfNum].Lock);
	}
	if (0 == UsbCore.CoreUsbDevGetCfgIndex (DevNo))
	{
		return 0;
	}
	spin_lock_irqsave (&MouseQueue[IfNum].Lock, Flags);
	WriteIndex = MouseQueue[IfNum].WrIndex;
	memcpy ((void*)MouseQueue[IfNum].Queue[WriteIndex], Buff, MAX_MOUSE_DATA_SIZE);
	MouseQueue[IfNum].WrIndex = (MouseQueue[IfNum].WrIndex + 1) % DATA_QUEUE_SIZE;
	MouseQueue[IfNum].Size++;

	if (LastMouseEventTimeStamp[IfNum]) 
	{ 
		CurrentTimeStamp[IfNum] = jiffies; 
		if (CurrentTimeStamp[IfNum] > LastMouseEventTimeStamp [IfNum]) 
		{ 
			if (((CurrentTimeStamp [IfNum] - LastMouseEventTimeStamp [IfNum]) * 1000 / HZ) > 1000) 
			{ 
				printk ("Mouse event timed out..\n"); 
				MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0; 
			} 
		} 
	} 
	
	if (0 == MouseQueue[IfNum].Size % DATA_QUEUE_SIZE)
	{
		TWARN ("Mouse Data Queue is full for Iface number %d...\n", IfNum);
		MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0;
	}

	if (MouseQueue[IfNum].WaitForHostToReceiveMouseData == 0)
	{
		MouseQueue[IfNum].WaitForHostToReceiveMouseData = 1;
		spin_unlock_irqrestore (&MouseQueue[IfNum].Lock, Flags);
		ret = SendMouseEvent (DevNo, IfNum, MouseQueue[IfNum].Queue[WriteIndex]);
		spin_lock_irqsave (&MouseQueue[IfNum].Lock, Flags);
		if (ret)
		{
			MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0;
		}
		else 
		{ 
			LastMouseEventTimeStamp [IfNum] = jiffies; 
		} 
		spin_unlock_irqrestore (&MouseQueue[IfNum].Lock, Flags);
	}
	else
	{
		spin_unlock_irqrestore (&MouseQueue[IfNum].Lock, Flags);
	}
	return ret;
}

void
MouseTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum)
{
	int ReadIndex;

	if (FirstTime[IfNum] == 0)
	{
		TWARN ("Mouse queue not yet initialized\n"); 
		return;
	}
	
	spin_lock (&MouseQueue[IfNum].Lock);
	LastMouseEventTimeStamp [IfNum] = 0;
	if (MouseQueue[IfNum].WaitForHostToReceiveMouseData == 0)
	{
		spin_unlock (&MouseQueue[IfNum].Lock);
		return;
	}
	MouseQueue[IfNum].RdIndex = (MouseQueue[IfNum].RdIndex + 1) % DATA_QUEUE_SIZE;
	MouseQueue[IfNum].Size --;

	if (0 != MouseQueue[IfNum].Size)
	{
		ReadIndex = MouseQueue[IfNum].RdIndex;
		spin_unlock (&MouseQueue[IfNum].Lock);
		SendMouseEvent (DevNo, IfNum, MouseQueue[IfNum].Queue[ReadIndex]);
		LastMouseEventTimeStamp [IfNum] = jiffies;
	}
	else
	{
		MouseQueue[IfNum].WaitForHostToReceiveMouseData = 0;
		spin_unlock (&MouseQueue[IfNum].Lock);
	}
}



