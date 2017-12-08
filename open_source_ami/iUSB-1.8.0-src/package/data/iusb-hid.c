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
#include "hid_usb.h"
#include "iusb-scsi.h"

#define MAX_HID_IFACES	4	//includes both keyboards and mice interfaces together.

/* Static Varibales */
static uint8 iUsbKeybdActive[MAX_HID_IFACES] = { 0, 0, 0, 0 };
static uint8 iUsbKeybdExit[MAX_HID_IFACES]   = { 0, 0, 0, 0 };
static uint8 iUsbKeybdWakeup[MAX_HID_IFACES] = { 0, 0, 0, 0 };
static IUSB_HID_PACKET LedKernelModePkt[MAX_HID_IFACES];
static IUSB_HID_PACKET*	iUsbKeybdLedPkt[MAX_HID_IFACES] = { NULL, NULL, NULL, NULL};
static Usb_OsSleepStruct KeybdReqWait[MAX_HID_IFACES];
static uint8 PendingFlag[MAX_HID_IFACES] = { 0, 0, 0, 0 };
static uint8 LEDDataReceived[MAX_HID_IFACES] = { 0, 0, 0, 0 };
static uint8 PendingDevNo[MAX_HID_IFACES];
static uint8 PendingIfNum[MAX_HID_IFACES];
static uint8 PendingData[MAX_HID_IFACES];
static char KernelModeMouseData_abs[sizeof(IUSB_HID_PACKET)+ABS_REQUIRED_MOUSE_DATA_SIZE];
static char KernelModeMouseData_rel[sizeof(IUSB_HID_PACKET)+REL_REQUIRED_MOUSE_DATA_SIZE];
static char KernelModeKeybdData[sizeof(IUSB_HID_PACKET)+7];
static uint8 FirstTime[MAX_HID_IFACES] = {1, 1, 1, 1};


static int 
FillLedPacket(uint8 DevNo,uint8 IfNum,uint8 Data)
{
	uint32 DataLen;

	TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"FillLedPacket(): Waking up KeybdRequest\n");

	/* Fill the iUSB Keyboard Report Packet */
	iUsbKeybdLedPkt[IfNum]->DataLen		= 1;
	iUsbKeybdLedPkt[IfNum]->Data 		= Data;

	/* Form iUSB Header */
	DataLen = sizeof(IUSB_HID_PACKET)-sizeof(IUSB_HEADER);
	UsbCore.CoreUsbFormiUsbHeader (&(iUsbKeybdLedPkt[IfNum]->Header),DataLen,DevNo,IfNum,
			IUSB_DEVICE_KEYBD,IUSB_PROTO_KEYBD_STATUS);

	/* Wakeup CdromRequest to notify Request Packet is ready*/
	iUsbKeybdExit[IfNum] 	= 0;
	iUsbKeybdWakeup[IfNum] = 1;
	UsbCore.CoreUsb_OsWakeupOnTimeout(&KeybdReqWait[IfNum]);
	return 1;
}


int 
KeybdLedWaitStatus(IUSB_HID_PACKET *UserModePkt, uint8 IfNum, uint8 NoWait)
{
	IUSB_HID_PACKET *Pkt =&LedKernelModePkt[IfNum];
	unsigned long flags;
	uint32 DataLen;
	IUSB_HID_PACKET TempHidPkt;
	
	if (IfNum >= MAX_HID_IFACES)
	{
		TWARN ("KeybdLedWaitStatus(): received Invalid Interface Number %d.Supported IfNums are 0 to 3.\n", IfNum);
		return -EINVAL;
	}

	if ((0 == LEDDataReceived[IfNum]) && NoWait)
	{
		return -1;
	}
	
	if ((0 != LEDDataReceived[IfNum]) && NoWait)
	{
		TempHidPkt.DataLen = 1;
		TempHidPkt.Data = PendingData[IfNum];

		/* Form iUSB Header */
		DataLen = sizeof(IUSB_HID_PACKET)-sizeof(IUSB_HEADER);
		UsbCore.CoreUsbFormiUsbHeader (&(TempHidPkt.Header),DataLen,PendingDevNo[IfNum],IfNum,
				IUSB_DEVICE_KEYBD,IUSB_PROTO_KEYBD_STATUS);

		/* Copy from kernel data area to user data area*/
		if (__copy_to_user((void *)UserModePkt,(void *)&TempHidPkt,sizeof(IUSB_HID_PACKET)))
		{
			TWARN ("KeybdLedWaitStatus():__copy_to_user failed\n");
			return -EFAULT;
		}
		return 0;
	}
	/* Initialize Sleep Structure for the First Time */
	if (FirstTime[IfNum])
	{
		UsbCore.CoreUsb_OsInitSleepStruct(&KeybdReqWait[IfNum]);
		FirstTime[IfNum] = 0;
	}

	TDBG_FLAGGED(iusb,DEBUG_IUSB_HID,"KeybdLedStatus():Sleep Entering \n");

	/* Wait for a Status Request Packet */
	local_save_flags(flags);
	local_irq_disable();
	iUsbKeybdLedPkt[IfNum] 	= Pkt;
	iUsbKeybdActive[IfNum] 	= 1;
	iUsbKeybdWakeup[IfNum] 	= 0;

	if (!PendingFlag[IfNum])
		UsbCore.CoreUsb_OsSleepOnTimeout(&KeybdReqWait[IfNum],&iUsbKeybdWakeup[IfNum],0);
	else
	{
		TDBG_FLAGGED(iusb,DEBUG_IUSB_HID,"KeybdLedStatus(): Sending pending Led Data 0x%x\n",PendingData[IfNum]);
		FillLedPacket(PendingDevNo[IfNum],PendingIfNum[IfNum],PendingData[IfNum]);
		PendingFlag[IfNum] = 0;
	}
	
	/* Got a Status Packet. Invaildate internal structure  */
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"KeybdLedStatus(): Sleep Leaving\n");
	iUsbKeybdActive[IfNum] 	= 0;
	iUsbKeybdLedPkt[IfNum] 	= NULL;
	local_irq_restore(flags);
	
	/* Copy from kernel data area to user data area*/
	if (__copy_to_user((void *)UserModePkt,(void *)&LedKernelModePkt[IfNum],sizeof(IUSB_HID_PACKET)))
	{
		TWARN ("KeybdLedWaitStatus():__copy_to_user failed\n");
		return -EFAULT;
	}

	return iUsbKeybdExit[IfNum];
}

int
KeybdExitWait(uint8 IfNum)
{
	if (IfNum >= MAX_HID_IFACES)
	{
		TWARN ("KeybdExitWait(): received Invalid Interface Number %d.Supported IfNums are 0 to 3.\n", IfNum);
		return -EINVAL;
	}
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"KeybdExitWait()...\n");
	PendingFlag[IfNum] = 0;
	iUsbKeybdExit[IfNum] = 1;
	iUsbKeybdWakeup[IfNum] = 1;
	if (!FirstTime[IfNum])			// Some Rogue app called ExitWait first 
		UsbCore.CoreUsb_OsWakeupOnTimeout(&KeybdReqWait[IfNum]);
	return 0;
}

int 
KeybdLedRemoteCall(uint8 DevNo,uint8 IfNum,uint8 Data)
{

	if (IfNum >= MAX_HID_IFACES)
	{
		TWARN ("KeybdLedRemoteCall(): received IfNum %d.Supported IfNums are 0 to 3.\n", IfNum);
		return -EINVAL;
	}
	LEDDataReceived [IfNum] = 1;
	PendingData[IfNum] = Data;	
	PendingDevNo[IfNum] = DevNo;
	PendingIfNum[IfNum] = IfNum;

	/* Check if the remote keyboard is active and waiting for a report */
	if (!((iUsbKeybdActive[IfNum]) && (iUsbKeybdLedPkt[IfNum])))
	{	
		PendingFlag[IfNum] = 1;
		TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"KeybdLedRemoteCall(): Pending Data 0x%x\n",Data);
		return 0;
	}

	return FillLedPacket(DevNo,IfNum,Data);
}


int
KeybdPrepareData(IUSB_HID_PACKET *UserModePkt, uint8 *KeybdData)
{
	IUSB_HID_PACKET * HidPkt= (IUSB_HID_PACKET *)(&KernelModeKeybdData[0]);

	TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"KeybdPrepareData(): Called\n");

	/* Copy from user data area to kernel data area*/
	if (__copy_from_user((void *)(&KernelModeKeybdData[0]),(void *)UserModePkt,
			sizeof(IUSB_HID_PACKET)+7))
	{
		TWARN ("KeybdPrepareData():__copy_from_user failed\n");
		return -EFAULT;
	}
	memcpy (KeybdData, &(HidPkt->Data),  8);
	return 0;
}

int
MousePrepareData(IUSB_HID_PACKET *UserModePkt, uint8 MouseMode, uint8 *MouseData)
{
    IUSB_HID_PACKET * HidPkt;
    uint8 copySize = 0;
	
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HID,"MousePrepareData(): Called\n");
 
    /* Copy from user data area to kernel data area*/
	if (MouseMode == RELATIVE_MOUSE_MODE)
	{
        HidPkt= (IUSB_HID_PACKET *)(&KernelModeMouseData_rel[0]);
        copySize = sizeof(IUSB_HID_PACKET) + REL_REQUIRED_MOUSE_DATA_SIZE;
        if (__copy_from_user((void *)(&KernelModeMouseData_rel[0]),(void *)UserModePkt,copySize))
		{
			TWARN ("MousePrepareData():__copy_from_user failed\n");
			return -EFAULT;
		}
		memcpy (MouseData, &(HidPkt->Data), REL_EFFECTIVE_MOUSE_DATA_SIZE);
		return 0;
	}
	else if (MouseMode == ABSOLUTE_MOUSE_MODE)
	{
        HidPkt= (IUSB_HID_PACKET *)(&KernelModeMouseData_abs[0]);
        copySize = sizeof(IUSB_HID_PACKET) + ABS_REQUIRED_MOUSE_DATA_SIZE;
        if (__copy_from_user((void *)(&KernelModeMouseData_abs[0]),(void *)UserModePkt,copySize ))
		{
			TWARN ("MousePrepareData():__copy_from_user failed\n");
			return -EFAULT;
		}
		memcpy (MouseData, &(HidPkt->Data), ABS_EFFECTIVE_MOUSE_DATA_SIZE);
		return 0;
    }
	else 
	    TWARN("MouseMode is Not defined.\n");
   
	return -1;
}

