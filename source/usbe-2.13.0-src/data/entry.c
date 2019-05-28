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

#include "header.h"
#include "usb_hw.h"
#include "coreusb.h"
#include "descriptors.h"
#include "arch.h"
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"
#include "mod_reg.h"
#include "entry.h"

IUSB_DEVICE_DETAILS_T IUSBDeviceDetails [MAX_IUSB_DEVICES];
#define VALID_IUSB_DEV	0x55AA55AA

/*Desc Table access functions*/
int IsDevCdrom(uint8 DevNo)
{
	if(DevNo > MAX_USB_HW)
	{
		TCRIT("Invalid check CDROM device number");
		return 0;
	}

	if ((DevInfo[DevNo].UsbDevType == CREATE_CDROM_DESCRIPTOR) ||
		(DevInfo[DevNo].UsbDevType ==  CREATE_SUPERCOMBO_DESCRIPTOR) ||
		(DevInfo[DevNo].UsbDevType ==  CREATE_LUN_COMBO_DESCRIPTOR) ||
		(DevInfo[DevNo].UsbDevType ==  CREATE_COMBO_DESCRIPTOR))
			return 1;
		else
			return 0;
}

int IsDevFloppy(uint8 DevNo)
{

	if(DevNo > MAX_USB_HW)
	{
		TCRIT("Invalid check floppy device number");
		return 0;
	}
	if((DevInfo[DevNo].UsbDevType == CREATE_FLOPPY_DESCRIPTOR) ||
       (DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR) ||
 	   (DevInfo[DevNo].UsbDevType == CREATE_LUN_COMBO_DESCRIPTOR) ||
       (DevInfo[DevNo].UsbDevType == CREATE_COMBO_DESCRIPTOR))
		return 1;
	else
		return 0;
}

int IsDevHid(uint8 DevNo)
{

	if(DevNo > MAX_USB_HW)
	{
		TCRIT("Invalid check Hid device number");
		return 0;
	}

	if((DevInfo[DevNo].UsbDevType == CREATE_HID_DESCRIPTOR)  ||
        (DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR)) 
		return 1;
	else
		return 0;

	return 1;
}

int IsDevBotCombo(uint8 DevNo)
{

	if(DevNo > MAX_USB_HW)
	{
		TCRIT("Invalid check CDROM device number");
		return 0;
	}

 	   if((DevInfo[DevNo].UsbDevType == CREATE_LUN_COMBO_DESCRIPTOR) || 
		  (DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR) || 
		  (DevInfo[DevNo].UsbDevType == CREATE_COMBO_DESCRIPTOR) ||
		  (DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR))
		   return 1;
	   else
		   return 0;
}

int IsDevSuperCombo(uint8 DevNo) /*All in one decice */
{

	if(DevNo > MAX_USB_HW)
	{
		TCRIT("Invalid check Hid device number");
		return 0;
	}

	if(DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR) 
		return 1;
	else
		return 0;
}


/* Interrupt Service Routine */
irqreturn_t
Entry_UsbIsr(uint8 DevNo,uint8 epnum)
{
	if ((DevNo < MAX_USB_HW) && (DevInit[DevNo]))
		return UsbDeviceIntr(DevNo,epnum);
	else
		TWARN("usb_isr(): Spurious Interrupt\n");
	return IRQ_NONE;
}

/* Deffered Interrupt Processing Routine */
int
UsbDeferProcess(void *arg)
{
	/* Call the Interface Handlers if needed */
	CallInterfaceHandlers();
	return 0;
}


/* This is called during initialization from */
int
Entry_UsbInit(void)
{
	/* Initialize Deffered Interrupt Processing */
	Usb_OsInitDPC(UsbDeferProcess);

	return 0;
}

void
Entry_UsbExit(void)
{
	uint8 DevNo;

	/*
	 * For devices which have Descriptors, disconnect to host.
	 */
	for (DevNo=0;DevNo<MAX_USB_HW;DevNo++)
	{
		if (!DevInit[DevNo])
		 continue;
		if (!CreateDesc[DevNo])
		 continue;
		UsbDeviceDisconnect(DevNo);
	}

	/* Disable Hardware to Stop Interrupts */
	for (DevNo=0;DevNo<MAX_USB_HW;DevNo++)
	{
		if (!DevInit[DevNo])
			continue;
		UsbDeviceDisable(DevNo);
		UsbUnCfgDevice(DevNo);
		if (DevExit[DevNo])
			(*DevExit[DevNo])(DevNo);

	}

	/* Stop Deffered Interrupt Processing */
	Usb_OsKillDPC();


	/*
	 * For devices which have Descriptors, remove the descriptors.
	 */
	for (DevNo = 0; (DevNo < MAX_USB_HW);DevNo++) 
	{
		
		if (!DevInit[DevNo])
		 continue;
		if (!CreateDesc[DevNo])
		 continue;
		UsbRemoveDevice(DevNo);
	}

	/* Free the structures */
	UsbCoreFini(DevInit);
	return;
}

int 
GetFreeInterfaces (uint8 DeviceType, uint8 LockType, IUSB_DEVICE_LIST *DevList)
{
	uint8 iUSBDev;
	uint32 DataLen;
	uint8 iUsbDevListBuf [sizeof (IUSB_HEADER) + sizeof (uint8) + 
							(MAX_IUSB_DEVICES * sizeof (IUSB_DEVICE_INFO))];
	IUSB_DEVICE_LIST* iUSBDevList = (IUSB_DEVICE_LIST*)iUsbDevListBuf;
	IUSB_DEVICE_INFO* iUSBDevInfo = (IUSB_DEVICE_INFO*)&iUSBDevList->DevInfo;

	TDBG_FLAGGED(usbe, DEBUG_CORE,"GetFreeInterfaces() is called");
	
	iUSBDevList->DevCount = 0;
	for (iUSBDev = 0; iUSBDev < MAX_IUSB_DEVICES; iUSBDev++)
	{
		if (0x55AA55AA != IUSBDeviceDetails[iUSBDev].Valid) continue;
		if ((LockType == LOCK_TYPE_EXCLUSIVE) && (0 != IUSBDeviceDetails[iUSBDev].Used)) continue;
		if (DeviceType != IUSBDeviceDetails[iUSBDev].iUSBDevInfo.DeviceType) continue;
		if (LockType != IUSBDeviceDetails[iUSBDev].iUSBDevInfo.LockType) continue;
		memcpy ((void*)&iUSBDevInfo[iUSBDevList->DevCount],
					(void*)&IUSBDeviceDetails[iUSBDev].iUSBDevInfo, sizeof (IUSB_DEVICE_INFO));
		iUSBDevList->DevCount++;
	}
	if (0 == iUSBDevList->DevCount)
	{	
//		TWARN ("Device Type 0x%02x interfaces are not available to use\n", DeviceType);
		return -1;
	}
	/* Form iUsb Header */
	DataLen = (iUSBDevList->DevCount * sizeof(IUSB_DEVICE_INFO)) +1;
	FormiUsbHeader(&(iUSBDevList->Header),DataLen,0xFF,0xFF,
								IUSB_DEVICE_RESERVED,IUSB_PROTO_RESERVED);	
	if (__copy_to_user((void *)DevList,(void *)iUSBDevList,(DataLen + sizeof iUSBDevList->Header)))
	{
		TWARN ("GetFreeInterfaces():__copy_to_user failed\n");
		return -EFAULT;
	}
	return 0;
			
}

int
RequestInterface (IUSB_REQ_REL_DEVICE_INFO* iUSBInfo)
{
	uint8 iUSBDev;
	uint8 DevNo;
	uint8 IfNum;
	uint32 Key;
	IUSB_DEVICE_INFO iUSBDevInfo;


	TDBG_FLAGGED(usbe, DEBUG_CORE,"RequestInterface() is called");

	if (__copy_from_user((void *)&iUSBDevInfo,(void *)iUSBInfo,sizeof (IUSB_DEVICE_INFO)))
	{
		TWARN ("RequestInterface():__copy_from_user failed\n");
		return -EFAULT;
	}
	
	for (iUSBDev = 0; iUSBDev < MAX_IUSB_DEVICES; iUSBDev++)
	{
		if (0x55AA55AA != IUSBDeviceDetails[iUSBDev].Valid) continue;
		if (iUSBDevInfo.DeviceType != 
			IUSBDeviceDetails[iUSBDev].iUSBDevInfo.DeviceType) continue;
		if (iUSBDevInfo.DevNo != 
			IUSBDeviceDetails[iUSBDev].iUSBDevInfo.DevNo) continue;
		if (iUSBDevInfo.IfNum != 
			IUSBDeviceDetails[iUSBDev].iUSBDevInfo.IfNum) continue;
		if (iUSBDevInfo.LockType != 
			IUSBDeviceDetails[iUSBDev].iUSBDevInfo.LockType) continue;
		if ((IUSBDeviceDetails[iUSBDev].Used) && (iUSBDevInfo.LockType == LOCK_TYPE_EXCLUSIVE))
		{
			TWARN ("RequestInterface(): Requested Interface is already in use\n");
			return -1; 	
		}
		else 
		{
			IUSBDeviceDetails[iUSBDev].Used = 1;
			DevNo = iUSBDevInfo.DevNo;
			IfNum = iUSBDevInfo.IfNum;
			Key = IUSBDeviceDetails[iUSBDev].Key;
			if (DevInfo[DevNo].UsbCoreDev.DevUsbSetInterfaceAuthKey(DevNo,IfNum,Key))
			{
				TWARN ("RequestInterface(): Error while setting Interface Auth key\n");
				return -1;
			}
			if (__copy_to_user((void *)&iUSBInfo->Key,(void *)&Key,sizeof (Key)))
			{
				TWARN ("RequestInterface():__copy_to_user failed\n");
				return -EFAULT;
			}
			return 0;
		}
	}
	return -1;
}

int
ReleaseInterface (IUSB_REQ_REL_DEVICE_INFO* iUSBInfo)
{

	uint8 iUSBDev;
	uint8 DevNo;
	uint8 IfNum;
	IUSB_REQ_REL_DEVICE_INFO iUSBRelDevInfo;

	TDBG_FLAGGED(usbe, DEBUG_CORE,"ReleaseInterface() is called");

	if (__copy_from_user((void *)&iUSBRelDevInfo,(void *)iUSBInfo,
										sizeof (IUSB_REQ_REL_DEVICE_INFO)))
	{
		TWARN ("ReleaseInterface():__copy_from_user failed\n");
		return -EFAULT;
	}

	for (iUSBDev = 0; iUSBDev < MAX_IUSB_DEVICES; iUSBDev++)
	{
		if (VALID_IUSB_DEV != IUSBDeviceDetails[iUSBDev].Valid) continue;
		if (iUSBRelDevInfo.DevInfo.DeviceType != 
				IUSBDeviceDetails[iUSBDev].iUSBDevInfo.DeviceType) continue;
		if (iUSBRelDevInfo.DevInfo.DevNo != 
				IUSBDeviceDetails[iUSBDev].iUSBDevInfo.DevNo) continue;
		if (iUSBRelDevInfo.DevInfo.IfNum != 
				IUSBDeviceDetails[iUSBDev].iUSBDevInfo.IfNum) continue;
		if (0 == IUSBDeviceDetails[iUSBDev].Used)
		{
			TWARN ("ReleaseInterface():Trying to release interface that is not in use\n");
			return -1; 	
		}
		if (iUSBRelDevInfo.Key != IUSBDeviceDetails[iUSBDev].Key)
		{
			TWARN ("ReleaseInterface():Error: AuthKey Mismatch\n");
			return -1;
		}
		else if (IUSBDeviceDetails[iUSBDev].iUSBDevInfo.LockType == LOCK_TYPE_EXCLUSIVE)
		{
			IUSBDeviceDetails[iUSBDev].Used = 0;
			DevNo = iUSBRelDevInfo.DevInfo.DevNo;
			IfNum = iUSBRelDevInfo.DevInfo.IfNum;
				
			if (DevInfo[DevNo].UsbCoreDev.DevUsbClearInterfaceAuthKey(DevNo,IfNum))
			{
				TWARN ("RequestInterface(): Error while clearing Interface Auth key\n");
				return -1;
			}
		}
		return 0;
	}
	TWARN ("ReleaseInterface(): Error in releasing the interface requested\n"); 
	return -1;
		
}

int
GetiUsbDeviceList(IUSB_DEVICE_LIST *DevList)
{
	uint8 DevNo;
	uint8 iUsbDevListBuf [sizeof (IUSB_HEADER) + sizeof (uint8) + 
							(MAX_IUSB_DEVICES * sizeof (IUSB_DEVICE_INFO))];
	IUSB_DEVICE_LIST* iUSBDevList = (IUSB_DEVICE_LIST*)iUsbDevListBuf;
	uint32 DataLen;

	iUSBDevList->DevCount = 0;
	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++)
	{
		if (CreateDesc[DevNo] == NULL) continue;
		if (DevInfo[DevNo].UsbCoreDev.DevUsbFillDevInfo)
		{
			iUSBDevList->DevCount += DevInfo[DevNo].UsbCoreDev.DevUsbFillDevInfo
				(&iUSBDevList->DevInfo + iUSBDevList->DevCount);
		}
	}
	
	/* Form iUsb Header */
	DataLen = (iUSBDevList->DevCount * sizeof(IUSB_DEVICE_INFO)) +1;
	FormiUsbHeader(&(iUSBDevList->Header),DataLen,0xFF,0xFF,
							IUSB_DEVICE_RESERVED,IUSB_PROTO_RESERVED);	

	if (__copy_to_user((void *)DevList,(void *)iUSBDevList,(DataLen + sizeof iUSBDevList->Header)))
	{
		TWARN ("GetiUsbDeviceList():__copy_to_user failed\n");
		return -EFAULT;
	}
	return 0;

}

int
UsbRemoveDevice(uint8 DevNo)
{
	if (CreateDesc[DevNo])
	{
		CreateDesc[DevNo] = NULL;
		DevInfo[DevNo].UsbDevType = 0;
		DestroyDescriptor(DevNo);
		return 0;
	}
	return 0;
}

int
UsbRemoteWakeupHost(void)
{
	uint8 DevNo;
	int retval = 0;

	for (DevNo=0;DevNo<MAX_USB_HW;DevNo++)
	{	
		if (!DevInit[DevNo])
			continue;
		if (!UsbGetBusSuspendState(DevNo))
			continue;
		UsbRemoteWakeup(DevNo);
		//After remote wakeup if we return 1 for HID which will result first key drop and it will not wakeup
		//if we press two/twice key it will wake up, if a single key press it will not wakeup
		//if(DevInfo[DevNo].UsbDevType == CREATE_HID_DESCRIPTOR) retval = 1;
		if(DevInfo[DevNo].UsbDevType == CREATE_SUPERCOMBO_DESCRIPTOR) retval = 1;
	}
	return retval;
}



