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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <coreTypes.h>
#include "usb_hw.h"
#include "descriptors.h"
#include "hid.h"
#include "iusb.h"
#include "usb_ioctl.h"
#include "hid_usb.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"
#include "mod_reg.h"
#include "keybd.h"
#include "mouse.h"
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
#include "pipe.h"
#endif
#include "iusb-inc.h"


/* Local Definitions */
#define VENDOR_ID				1131	/* AMI = Decimal 1131 */
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
#define PRODUCT_ID_COMP_HID_PIPE	0xFF11	/*Composite Hid Device : Mouse & Keybd % Pipe */
#else
#define PRODUCT_ID_COMP_HID		0xFF10	/*Composite Hid Device : Mouse & Keybd*/
#endif
#define DEVICE_REV				0x0100

/* Global Variables */
TDBG_DECLARE_DBGVAR(hid);
uint8 MouseMode=ABSOLUTE_MOUSE_MODE; //Mouse Mode is first initialzed to absolute.
USB_CORE UsbCore;
USB_IUSB iUSB;

/* Function prototypes */
static int CreateHidDescriptor(uint8 DevNo);
static int HidIoctl (unsigned int cmd,unsigned long arg, int* RetVal);
static int HidMiscActions (uint8 DevNo, uint8 ep, uint8 Action);
static int FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo);
static int SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key);
static int ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum);

/* Static Variables */
static uint32 IfaceAuthKeys [2]= {0,0};
static uint8 KeybdIfNum;
static uint8 MouseIfNum;
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
static uint8 PipeIfNum;
#endif
static USB_DEV	UsbDev = {
			.DevUsbCreateDescriptor 		= CreateHidDescriptor,
			.DevUsbIOCTL 					= HidIoctl,
			.DevUsbSetKeybdSetReport 		= SetKeybdSetReport,
			.DevUsbMiscActions 				= HidMiscActions,
			.DevUsbIsEmulateFloppy 			= NULL,
			.DevUsbRemoteScsiCall 			= NULL,
			.DevUsbFillDevInfo 				= FillDevInfo,
			.DevUsbSetInterfaceAuthKey		= SetInterfaceAuthKey,
			.DevUsbClearInterfaceAuthKey	= ClearInterfaceAuthKey,
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT			
			.DevUsbRead			= PipeRead,
			.DevUsbWrite			= PipeWrite,
#endif			
					};
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
static DEFINE_SEMAPHORE (HeartBeatSem);
#else
static DECLARE_MUTEX (HeartBeatSem);	
#endif
static int 	hid_notify_sys(struct notifier_block *this, unsigned long code, void *unused);
static struct notifier_block hid_notifier =
{
       .notifier_call = hid_notify_sys,
};
static char *HWModuleName = NULL;
static usb_device_driver hid_device_driver =
{
	.module 		= THIS_MODULE,
	.descriptor    	= CREATE_HID_DESCRIPTOR,
	.devnum        	= 0xFF,
	.speeds         = (SUPPORT_LOW_SPEED | SUPPORT_FULL_SPEED),
};
static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table HIDctlTable [] =
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,11))
	{"DebugLevel",&(TDBG_FORM_VAR_NAME(hid)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else	
	{CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(hid)),sizeof(int),0644,NULL,NULL,&proc_dointvec}, 
#endif		
	{0} 
};
static uint8 TempCounter = 0;
static uint8 MouseTmOutCount;
static int Interval = 1;
module_param (HWModuleName, charp, 0000);
MODULE_PARM_DESC(HWModuleName, "which hardware module is correspond to HID device module");
module_param (Interval, int, S_IRUGO);
MODULE_PARM_DESC(Interval, "bInterval field of Endpoint Descriptor");


static int
changeMouseAbs2Rel (void)
{
    int retVal = 0;
    uint8 DevNo = hid_device_driver.devnum;

    TDBG_FLAGGED(hid, DEBUG_HID,"changeMouseAbs2Rel()...\n");
	UsbCore.CoreUsbDeviceDisconnect( DevNo );
    MouseMode = RELATIVE_MOUSE_MODE;
	GetHidDesc_Mouse (DevNo, 1, MouseIfNum);
    UsbCore.CoreUsbDeviceReconnect( DevNo );
    UsbCore.CoreUsb_OsDelay(1000);
    return retVal;
}

static int
changeMouseRel2Abs (void)
{
    int retVal = 0 ;
	uint8 DevNo = hid_device_driver.devnum;

	TDBG_FLAGGED(hid, DEBUG_HID,"changeMouseRel2Abs()...\n");
    UsbCore.CoreUsbDeviceDisconnect( DevNo);
    MouseMode = ABSOLUTE_MOUSE_MODE;
	GetHidDesc_Mouse (DevNo, 1, MouseIfNum);
    UsbCore.CoreUsbDeviceReconnect( DevNo);
    UsbCore.CoreUsb_OsDelay(1000);

	return retVal;
}

static int
getCurrentMouseMode (uint8 *MouseModeStatus)
{
    int retVal = 0;

    if (MouseMode == ABSOLUTE_MOUSE_MODE)
	{
		TDBG_FLAGGED(hid, DEBUG_HID,"Mousue Mode is Absolute...\n");
	}
    else
	{
		TDBG_FLAGGED(hid, DEBUG_HID,"Mousue Mode is Relative...\n");
	}

	*MouseModeStatus = MouseMode;

    return retVal;
}
static int
AuthenticateIOCTLCaller (unsigned long arg, int* RetVal, int argHasIUSBHeader)
{
	uint32 AuthKey;
	int Temp;
	uint8 IfNum;
	IUSB_HEADER iUsbHeader;
	IUSB_IOCTL_DATA IoctlData;

	if (argHasIUSBHeader)
	{
		Temp = __copy_from_user((void *)(&iUsbHeader),(void*)arg, sizeof(IUSB_HEADER));
		AuthKey = iUsbHeader.Key;
		IfNum = iUsbHeader.InterfaceNo;
	}
	else
	{
		Temp = __copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA));
		AuthKey = IoctlData.Key;
		IfNum = IoctlData.DevInfo.IfNum;
	}
	if (Temp)
	{
		TWARN ("AuthenticateIOCTLCaller(): __copy_from_user failed\n");
		*RetVal = -EFAULT;
		return -1;
	}
	if (IfNum > MouseIfNum)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Interface Number %d\n", IfNum);
		*RetVal = -EINVAL;
		return -1;
	}
	if (0 == IfaceAuthKeys[IfNum])
	{
		TWARN ("AuthenticateIOCTLCaller(): Auth Key has not been set for this interface\n");
		*RetVal = -EPERM;
		return -1;
	}
	if (AuthKey != IfaceAuthKeys[IfNum])
	{
		TWARN ("AuthenticateIOCTLCaller(): Auth Key mismatch, expected %x received %x\n",
														IfaceAuthKeys[IfNum], AuthKey);
		*RetVal = -EPERM;
		return -1;
	}
	return 0;
}

static int
HidIoctl (unsigned int cmd,unsigned long arg, int* RetVal)
{
	uint8 MouseData [MAX_MOUSE_DATA_SIZE];
	uint8 KeybdData [MAX_KEYBD_DATA_SIZE];
	uint8 DevNo, IfNum;
	uint8 MMode;
	
	DevNo = hid_device_driver.devnum;
	switch (cmd)
	{
	case USB_MOUSE_DATA:
			TDBG_FLAGGED(hid, DEBUG_HID,"USB_MOUSE_DATA IOCTL...\n");
			if (AuthenticateIOCTLCaller (arg, RetVal, 1))
			{
				TWARN ("HidIoctl(): Invalid Auth Key in USB_MOUSE_DATA IOCTL\n");
				return 0;
			}
			if (__copy_from_user((void *)(&IfNum),&((IUSB_HEADER*)arg)->InterfaceNo, sizeof(uint8)))
			{
				TWARN ("HidIoctl(): __copy_from_user failed in USB_MOUSE_DATA IOCTL\n");
				*RetVal = -1;
				return 0;
			}
			if(UsbCore.CoreUsbRemoteWakeupHost())
			{
				*RetVal = 0;
				return 0;		
			}
			if ((MouseTmOutCount > 4)) 
			{ 
				/* Start local counter */
				TempCounter++;
				if (TempCounter > 80)
				{
					TempCounter = 0;	
		
					*RetVal = iUSB.iUSBMousePrepareData((IUSB_HID_PACKET *)arg, MouseMode, MouseData);
					if (*RetVal == 0)
					{
						*RetVal = SendMouseData (DevNo,IfNum,MouseData);
					}
					return 0;
				}
				else
				{
					*RetVal = 0;			
					return 0;
				}
			}
			*RetVal = iUSB.iUSBMousePrepareData((IUSB_HID_PACKET *)arg, MouseMode, MouseData);
			if (*RetVal == 0)
			{
				IfNum = MouseIfNum;
				*RetVal = SendMouseData (DevNo,IfNum,MouseData);
			}
		
			return 0;
		
	case USB_KEYBD_DATA:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_KEYBD_DATA IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in USB_KEYBD_DATA IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&IfNum),&((IUSB_HEADER*)arg)->InterfaceNo, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_KEYBD_DATA IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if(UsbCore.CoreUsbRemoteWakeupHost()) 
		{
			*RetVal = 0;	
			return 0;		
		}
		if (down_interruptible(&HeartBeatSem))
		{
			*RetVal= -EINTR;
			return 0;
		}
		*RetVal = iUSB.iUSBKeybdPrepareData((IUSB_HID_PACKET *)arg, KeybdData); 
		if (*RetVal == 0)
		{
			*RetVal = SendKeybdData (DevNo, IfNum, KeybdData);
		}
		up(&HeartBeatSem);
		return 0;

	case USB_KEYBD_LED:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_KEYBD_LED IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("HidIoctl():Invalid Auth Key in USB_KEYBD_LED IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&IfNum),&((IUSB_HEADER*)arg)->InterfaceNo, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_KEYBD_LED IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		/* This call will sleep and forms a complete IUSB Packet on Return */
		*RetVal = iUSB.iUSBKeybdLedWaitStatus((IUSB_HID_PACKET *)arg,IfNum,0);
		return 0;

	case USB_KEYBD_LED_NO_WAIT:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_KEYBD_LED_NO_WAIT IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("HidIoctl():Invalid Auth Key in USB_KEYBD_LED_NO_WAIT IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&IfNum),&((IUSB_HEADER*)arg)->InterfaceNo, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_KEYBD_LED_NO_WAIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		/* This call will not sleep and forms a complete IUSB Packet on Return */
		*RetVal = iUSB.iUSBKeybdLedWaitStatus((IUSB_HID_PACKET *)arg,IfNum,1);
		return 0;

	case USB_KEYBD_LED_RESET:
			TDBG_FLAGGED(hid, DEBUG_HID,"USB_KEYBD_LED_RESET IOCTL...\n");
			if (AuthenticateIOCTLCaller (arg, RetVal, 1))
			{
				TWARN ("HidIoctl():Invalid Auth Key in USB_KEYBD_LED_RESET IOCTL\n");
				return 0;
			}
			
			*RetVal = iUSB.iUSBKeybdLedRemoteCall(((IUSB_IOCTL_DATA*)arg)->DevInfo.DevNo, ((IUSB_HEADER*)arg)->InterfaceNo, 0);
			return 0;

	case USB_KEYBD_EXIT:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_KEYBD_EXIT IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in USB_KEYBD_EXIT IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&IfNum),&((IUSB_IOCTL_DATA*)arg)->DevInfo.IfNum, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_KEYBD_EXIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		*RetVal = iUSB.iUSBKeybdExitWait(IfNum);
		return 0;

	/* abs/relative start */
	case MOUSE_ABS_TO_REL:
		TDBG_FLAGGED(hid, DEBUG_HID,"MOUSE_ABS_TO_REL IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in MOUSE_ABS_TO_REL IOCTL\n");
			return 0;
		}
		*RetVal = changeMouseAbs2Rel();
		return 0;

	case MOUSE_REL_TO_ABS:
		TDBG_FLAGGED(hid, DEBUG_HID,"MOUSE_REL_TO_ABS IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key  in MOUSE_REL_TO_ABS IOCTL\n");
			return 0;
		}
		*RetVal = changeMouseRel2Abs();
		return 0;

	case MOUSE_GET_CURRENT_MODE :
		TDBG_FLAGGED(hid, DEBUG_HID,"MOUSE_GET_CURRENT_MODE IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in MOUSE_GET_CURRENT_MODE IOCTL\n");
			return 0;
		}
		*RetVal = getCurrentMouseMode (&MMode);
		/* Copy from kernel data area to user data area*/
		if(__copy_to_user((void *)&((IUSB_IOCTL_DATA*)arg)->Data,(void *)&MMode,sizeof(uint8)))
		{
			TWARN ("HidIoctl():__copy_to_user failed\n");
			return -EFAULT;
		}
		return 0;

	case USB_DEVICE_DISCONNECT:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_DEVICE_DISCONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&DevNo),&((IUSB_IOCTL_DATA*)arg)->DevInfo.DevNo, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_DEVICE_DISCONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hid_device_driver.devnum != DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in USB_DEVICE_DISCONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceDisconnect (hid_device_driver.devnum);
		return 0;
		
	case USB_DEVICE_RECONNECT:
		TDBG_FLAGGED(hid, DEBUG_HID,"USB_DEVICE_RECONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&DevNo),&((IUSB_IOCTL_DATA*)arg)->DevInfo.DevNo, sizeof(uint8)))
		{
			TWARN ("HidIoctl(): __copy_from_user failed in USB_DEVICE_RECONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (hid_device_driver.devnum != DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("HidIoctl(): Invalid Auth Key in USB_DEVICE_RECONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceReconnect (hid_device_driver.devnum);
		return 0;			

	default:
		TDBG_FLAGGED(hid, DEBUG_HID,"This IOCTL is not supported by HID device module\n");
		return -1;
	}

}

static int
CreateHidDescriptor(uint8 DevNo)
{
	uint8 *MyDescriptor;
	uint16 TotalSize;
	uint16 ReportSize;
	INTERFACE_INFO KeybdIfInfo =
	{
		KeybdTxHandler,NULL,KeybdReqHandler,NULL,NULL,NULL,KeybdRemHandler,
		NULL,0,0,0,0
	};
	INTERFACE_INFO MouseIfInfo =
	{
		MouseTxHandler,NULL,MouseReqHandler,NULL,NULL,NULL,MouseRemHandler,
		NULL,0,0,0,0
	};

#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
	INTERFACE_INFO PipeIfInfo =
	{
		PipeTxHandler,PipeRxHandler,PipeReqHandler,NULL,NULL,NULL,PipeRemHandler,
		NULL,0,0,0,0
	};
#endif

	KeybdIfInfo.InterfaceData =(void *)vmalloc(sizeof(HID_IF_DATA));
	if (KeybdIfInfo.InterfaceData == NULL)
	{
		TCRIT("CreateHidDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}
	((HID_IF_DATA *)(KeybdIfInfo.InterfaceData))->Idle 		= 0;
	((HID_IF_DATA *)(KeybdIfInfo.InterfaceData))->Protocol 	= 1;
	((HID_IF_DATA *)(KeybdIfInfo.InterfaceData))->SetReportCalled   = 0;
	
	((HID_IF_DATA *)(KeybdIfInfo.InterfaceData))->ReportReq 		= 0;

#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT
	PipeIfInfo.InterfaceData =(void *)vmalloc(sizeof(HID_IF_DATA));
	if (PipeIfInfo.InterfaceData == NULL)
	{
		TCRIT("CreateHidDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}
	((HID_IF_DATA *)(PipeIfInfo.InterfaceData))->Idle 		= 0;
	((HID_IF_DATA *)(PipeIfInfo.InterfaceData))->Protocol 	= 1;
	((HID_IF_DATA *)(PipeIfInfo.InterfaceData))->SetReportCalled   = 0;
	((HID_IF_DATA *)(PipeIfInfo.InterfaceData))->ReportReq 		= 0;
#endif
	
	MouseIfInfo.InterfaceData =(void *)vmalloc(sizeof(HID_IF_DATA));
	if (MouseIfInfo.InterfaceData == NULL)
	{
		TCRIT("CreateHidDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}
	((HID_IF_DATA *)(MouseIfInfo.InterfaceData))->Idle 		= 0;
	((HID_IF_DATA *)(MouseIfInfo.InterfaceData))->Protocol 	= 1;
	((HID_IF_DATA *)(MouseIfInfo.InterfaceData))->SetReportCalled   = 0;
	((HID_IF_DATA *)(MouseIfInfo.InterfaceData))->ReportReq 		= 0;

	TotalSize =  sizeof(USB_DEVICE_DESC);
	TotalSize += sizeof(USB_CONFIG_DESC);

#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT	
	TotalSize += (3 * sizeof(USB_INTERFACE_DESC));	// +1 for pipe
	TotalSize += (3 * sizeof(USB_HID_DESC));	// +1 for pipe 
	TotalSize += (4 * sizeof(USB_ENDPOINT_DESC));	// +2 for pipe
#else
	TotalSize += (2 * sizeof(USB_INTERFACE_DESC));
	TotalSize += (2 * sizeof(USB_HID_DESC));
	TotalSize += (2 * sizeof(USB_ENDPOINT_DESC));
#endif


	MyDescriptor = UsbCore.CoreUsbCreateDescriptor(DevNo,TotalSize);
	if (MyDescriptor == NULL)
	{
		TCRIT("CreateHidDescriptor(): Memory Allocation Failure\n");
		return -1;
	}
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT		
	if (UsbCore.CoreUsbAddDeviceDesc(DevNo,VENDOR_ID,PRODUCT_ID_COMP_HID_PIPE,DEVICE_REV,
				"American Megatrends Inc.","Virtual Keyboard and Mouse + Host Pipe ", 0) != 0)
#else
	if (UsbCore.CoreUsbAddDeviceDesc(DevNo,VENDOR_ID,PRODUCT_ID_COMP_HID,DEVICE_REV,
				"American Megatrends Inc.","Virtual Keyboard and Mouse", 0) != 0)
#endif
	{
		TCRIT("CreateHidDescriptor(): Error in creating Device Desc\n");
		return -1;
	}
#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT		
	if (UsbCore.CoreUsbAddCfgDesc(DevNo,3, 1) != 0)
#else
	if (UsbCore.CoreUsbAddCfgDesc(DevNo,2, 1) != 0)
#endif	
	{
		TCRIT("CreateHidDescriptor(): Error in creating Cfg Desc\n");
		return -1;
	}
	/* Keyboard Section Begin */
	KeybdIfNum = 0;
	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,1,HID_INTERFACE,BOOT_INTERFACE,
				KEYBOARD_INTERFACE,"Keyboard Interface",&KeybdIfInfo) !=0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}
	ReportSize = GetKeybdReportSize();
	if (UsbCore.CoreUsbAddHidDesc(DevNo,ReportSize) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Hid Desc\n");
		return -1;
	}
	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,INTERRUPT,0x08,0x40,Interval,DATA_EP) != 0) 
	{
		TCRIT("CreateHidDescriptor(): Error in creating IN EndPointDesc\n");
		return -1;
	}
	/* Keyboard Section End */

	/* Mouse Section Begin */
	MouseIfNum = 1;
	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,1,HID_INTERFACE,BOOT_INTERFACE,
						MOUSE_INTERFACE,"Mouse Interface",&MouseIfInfo) !=0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}
	ReportSize = GetMouseReportSize();
	if (UsbCore.CoreUsbAddHidDesc(DevNo,ReportSize) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Hid Desc\n");
		return -1;
	}
    if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,INTERRUPT,0x08,0x40,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating IN EndPointDesc\n");
		return -1;
	}
	/* Mouse Section End */
    /*    Add microsoft string here... */
            
    if (UsbCore.CoreUsbAddOsStringDesc(DevNo, 0xA5, "MSFT100") != 0)
    {
    	TCRIT("CreateAddOsStringDesc: Error in creating OS String Desc\n");
        return -1;
    }

    /* Add Extended properties OS Feature descriptor */

    if (UsbCore.CoreUsbAddExtPropertiesDesc("SelectiveSuspendEnabled") != 0)
    {
     	TCRIT("CreateAddExtPropertiesDesc: Error in creating Extended Property  Desc\n");
        return -1;
    }


#ifdef CONFIG_SPX_FEATURE_PIPE_SUPPORT			
	PipeIfNum = 2;
	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,2,HID_INTERFACE,NO_SUBCLASS,
						NONE,"Pipe Interface",&PipeIfInfo) !=0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Interface Desc\n");
		return -1;
	}
	ReportSize = GetPipeReportSize();
	if (UsbCore.CoreUsbAddHidDesc(DevNo,ReportSize) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating Hid Desc\n");
		return -1;
	}
    	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,INTERRUPT,0x08,0x40,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating IN EndPointDesc\n");
		return -1;
	}
    	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,OUT,INTERRUPT,0x08,0x40,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateHidDescriptor(): Error in creating OUT EndPointDesc\n");
		return -1;
	}
#endif
    	
	return 0;
}


static int
HidMiscActions (uint8 DevNo, uint8 ep, uint8 Action)
{
	switch (Action)
	{
	case ACTION_USB_WRITE_DATA_TIMEOUT:
		TDBG_FLAGGED(hid, DEBUG_HID,"HidMiscActions(): ACTION_USB_WRITE_DATA_TIMEOUT...\n");
		if (ep == UsbCore.CoreUsbGetDataInEp (DevNo, MouseIfNum))
		{
			printk("***C%d*** ",MouseTmOutCount);    
			MouseTmOutCount++;
		}
		return 0;
	case ACTION_USB_WRITE_DATA_SUCCESS:
		TDBG_FLAGGED(hid, DEBUG_HID,"HidMiscActions(): ACTION_USB_WRITE_DATA_SUCCESS...\n");
		if (ep == UsbCore.CoreUsbGetDataInEp (DevNo, MouseIfNum))
		{
				MouseTmOutCount = 0;
		}
		return 0;
	case ACTION_USB_BUS_RESET_OCCURRED:
		TDBG_FLAGGED(hid, DEBUG_HID,"HidMiscActions(): ACTION_USB_BUS_RESET_OCCURRED...\n");
		MouseTmOutCount = 0;
		return 0;
	}
	return -1;
}

static int
FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo)
{
	uint8 NumDevices = 0;

	iUSBDevInfo->DeviceType = IUSB_DEVICE_KEYBD;
	iUSBDevInfo->DevNo      = hid_device_driver.devnum;
    iUSBDevInfo->IfNum      = KeybdIfNum;
	iUSBDevInfo->LockType	= LOCK_TYPE_EXCLUSIVE;
	iUSBDevInfo++;
	NumDevices++;

	iUSBDevInfo->DeviceType = IUSB_DEVICE_MOUSE;
	iUSBDevInfo->DevNo      = hid_device_driver.devnum;
    iUSBDevInfo->IfNum      = MouseIfNum; 
	iUSBDevInfo->LockType	= LOCK_TYPE_EXCLUSIVE;
	iUSBDevInfo++;
	NumDevices++;

	return NumDevices;
}

static int 
SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key)
{

	TDBG_FLAGGED(hid, DEBUG_HID,"SetInterfaceAuthKey() is called\n");
	if (DevNo != hid_device_driver.devnum)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if ((IfNum != KeybdIfNum) && (IfNum != MouseIfNum))
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = Key;
	return 0;
}

static int 
ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum)
{

	TDBG_FLAGGED(hid, DEBUG_HID,"ClearInterfaceAuthKey() is called\n");
	if (DevNo != hid_device_driver.devnum)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if ((IfNum != KeybdIfNum) && (IfNum != MouseIfNum))
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = 0;
	return 0;
}

static int
init_hid_device_module (void)
{
	int ret_val;
	int devnum;

	printk ("Loading HID Device Module...\n");
	if (0 != get_usb_core_funcs (&UsbCore))
	{
		TCRIT ("init_hid_device_module(): Unable to get Core Module Functions...\n");
		return -1;
	}
	if (0 != get_iusb_funcs (&iUSB))
	{
		TCRIT ("init_hid_device_module(): Unable to get iUSB Module Functions...\n");
		return -1;
	}
	if ((HWModuleName != NULL) && (sizeof(hid_device_driver.name) > strlen(HWModuleName)))
	{
		strcpy (hid_device_driver.name, HWModuleName);
	}
	devnum = UsbCore.CoreUsbGetFreeUsbDev (&hid_device_driver);
	if (devnum == -1)
	{
		TCRIT ("init_hid_device_module(): Unable to get Free USB Hardware Module for HID Device");
		return -1;
	}
	hid_device_driver.devnum=devnum;
	if (0 != CreateHidDescriptor (devnum))
	{
		TCRIT ("init_hid_device_module(): Unable to create HID device descriptor\n");
		UsbCore.CoreUsbDestroyDescriptor (devnum);
		return -1;
	}
	ret_val = register_usb_device (&hid_device_driver, &UsbDev);
	if (!ret_val)
	{
        register_reboot_notifier(&hid_notifier);
 		my_sys = AddSysctlTable("hid",&HIDctlTable[0]);
	}
	else
	{
		TCRIT ("init_hid_device_module(): HID Device failed to register with USB core\n");
	}

	return ret_val;
}

static void
exit_hid_device_module (void)
{
	printk ("Unloading HID Device Module...\n");
	unregister_usb_device (&hid_device_driver);
	unregister_reboot_notifier(&hid_notifier);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
hid_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{

	TDBG_FLAGGED(hid, DEBUG_HID,"HID Device Reboot notifier...\n");
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	   unregister_usb_device (&hid_device_driver);	
	}
	return NOTIFY_DONE;
}


module_init(init_hid_device_module);
module_exit(exit_hid_device_module);

MODULE_AUTHOR("Rama Rao Bisa <RamaB@ami.com>");
MODULE_DESCRIPTION("USB HID Device module");
MODULE_LICENSE("GPL");



