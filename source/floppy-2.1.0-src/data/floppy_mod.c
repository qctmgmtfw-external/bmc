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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <coreTypes.h>
#include "usb_hw.h"
#include "descriptors.h"
#include "bot.h"
#include "iusb.h"
#include "usb_ioctl.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"
#include "mod_reg.h"
#include "iusb-inc.h"

/* Local Definitions */
#define DEBUG_FLOPPY		0x01
#define VENDOR_ID			1131	/* AMI = Decimal 1131 */
#define PRODUCT_ID_HUB		0xFF01
#define PRODUCT_ID_COMP_HID	0xFF10	/*Composite Hid Device : Mouse & Keybd*/
#define PRODUCT_ID_MOUSE	0xFF11
#define PRODUCT_ID_KEYBD	0xFF12
#define PRODUCT_ID_CDROM	0xFF20
#define PRODUCT_ID_FIX_HD	0xFF30	/* Fixed Harddisk */
#define PRODUCT_ID_REM_HD	0xFF31	/* Removable Harddisk */
#define PRODUCT_ID_FLOPPY	0xFF40
#define PRODUCT_ID_SUP_COMP	0xFF90	/* All in One - Super Combo */
#define PRODUCT_ID_COMP	    0xFF91	/* CD & Floppy Combo */
#define PRODUCT_ID_LUN_COMP 0xFF92	/* CD & Floppy Lun Based Combo */
#define DEVICE_REV			0x0100
#define AMI_FLOPPY_SERIAL_NUM_STRING	"AAAABBBBCCCC2"
#define MAX_LUN_SUPPORT		0x06


/* Global Variables */
TDBG_DECLARE_DBGVAR(floppy);

/* Function Prototypes */
static int CreateFloppyDescriptor(uint8 DevNo);
static int FloppyIoctl (unsigned int cmd,unsigned long arg, int* RetVal);
static int IsEmulateFloppyEnabled (uint8 LunNo);
static int FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo);
static int RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo);
static int SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key);
static int ClearInterfaceAuthKey (uint8 DevNo, uint8 IfNum);

/* Static Variables */
static uint8	IsEmulateFloppy = 1;
static uint32 IfaceAuthKeys[1] = {0};
static USB_DEV	UsbDev = {
	.DevUsbCreateDescriptor 	= CreateFloppyDescriptor,
	.DevUsbIOCTL 				= FloppyIoctl,
	.DevUsbSetKeybdSetReport	= NULL,
	.DevUsbMiscActions 			= NULL,
	.DevUsbIsEmulateFloppy 		= IsEmulateFloppyEnabled,
	.DevUsbRemoteScsiCall 		= RemoteScsiCall,
	.DevUsbFillDevInfo 			= FillDevInfo,
	.DevUsbSetInterfaceAuthKey	= SetInterfaceAuthKey,
	.DevUsbClearInterfaceAuthKey	= ClearInterfaceAuthKey,
				};
static USB_CORE UsbCore;
static USB_IUSB iUSB;

static int 	floppy_notify_sys(struct notifier_block *this, unsigned long code, void *unused);
static struct notifier_block floppy_notifier =
{
       .notifier_call = floppy_notify_sys,
};

static char *HWModuleName = NULL;
static usb_device_driver floppy_device_driver =
{
	.module 		= THIS_MODULE,
	.descriptor     = CREATE_FLOPPY_DESCRIPTOR,
	.devnum         = 0xFF,
	.speeds         = (SUPPORT_FULL_SPEED | SUPPORT_HIGH_SPEED),
	.maxlun			= 0, // it is 0 based field, 1 means this device uses LUN0 and LUN1
	.DisconnectOnRegister = 1,
};

static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table floppyctlTable [] =
{
#if (LINUX_VERSION_CODE > KERNEL_VERSION(3,4,11))
        {"DebugLevel",&(TDBG_FORM_VAR_NAME(floppy)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else 
    #if (LINUX_VERSION_CODE >  KERNEL_VERSION(2,6,15)) 
        {CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(floppy)),sizeof(int),0644,NULL,NULL,&proc_dointvec},
    #else
        {1,"DebugLevel",&(TDBG_FORM_VAR_NAME(floppy)),sizeof(int),0644,NULL,&proc_dointvec},
#endif
#endif
        {0}
};
static uint8 NumLunComboFD = 0;
static uint8 LunNumAssignment [MAX_LUN_SUPPORT] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static uint8 FDInstannceAssignment [MAX_LUN_SUPPORT] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


static int Interval = 0;
module_param (HWModuleName, charp, 0000);
MODULE_PARM_DESC(HWModuleName, "which hardware module is correspond to Floppy device module");
module_param (Interval, int, S_IRUGO);
MODULE_PARM_DESC(Interval, "bInterval field of Endpoint Descriptor");

static int
AuthenticateIOCTLCaller (unsigned long arg, int* RetVal, int argHasIUSBHeader)
{
	uint32 AuthKey;
	int Temp;
	uint8 Instance;
	uint8 IfNum;
	IUSB_HEADER iUsbHeader;
	IUSB_IOCTL_DATA IoctlData;

	if (argHasIUSBHeader)
	{
		Temp = __copy_from_user((void *)(&iUsbHeader),(void*)arg, sizeof(IUSB_HEADER));
		AuthKey = iUsbHeader.Key;
		IfNum = iUsbHeader.InterfaceNo;
		Instance = iUsbHeader.Instance;
	}
	else
	{
		Temp = __copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA));
		AuthKey = IoctlData.Key;
		IfNum = IoctlData.DevInfo.IfNum;
		Instance = IoctlData.DevInfo.Instance;
	}
	if (Instance >= MAX_LUN_SUPPORT)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Instance %d\n", Instance);
		*RetVal = -EINVAL;
		return -1;
	}
	if (FDInstannceAssignment[Instance] == 0xFF)
	{
		TWARN ("AuthenticateIOCTLCaller(): Invalid Instance %d\n", Instance);
		*RetVal = -EINVAL;
		return -1;
	}
	if (Temp)
	{
		TWARN ("AuthenticateIOCTLCaller(): __copy_from_user failed\n");
		*RetVal = -EFAULT;
		return -1;
	}
	if (IfNum > 0)
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
FloppyIoctl (unsigned int cmd,unsigned long arg, int* RetVal)
{
	uint8 IfNum;
	uint8 DevNo;
	BOT_IF_DATA *BotIfData;
	SCSI_INQUIRY_PACKET *Inquiry;
	SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET *EventStatus;
	int i;
	uint8 Instance;
	IUSB_IOCTL_DATA IoctlData;
	
	switch (cmd)
	{

	case USB_FLOPPY_REQ:	
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_REQ IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_REQ IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_HEADER*)arg)->Instance, sizeof(uint8)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_FLOPPY_REQ IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		/* This call will sleep and forms a complete IUSB Packet on Return */
		*RetVal = iUSB.iUSBHdiskRemoteWaitRequest((IUSB_SCSI_PACKET *)arg, 
						FDInstannceAssignment[Instance]);
		return 0;

	case USB_FLOPPY_RES:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_RES IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 1))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_RES IOCTL\n");
			return 0;
		}
		*RetVal = iUSB.iUSBScsiSendResponse((IUSB_SCSI_PACKET *)arg);
		return 0;

	case USB_FLOPPY_ACTIVATE:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_ACTIVATE IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_ACTIVATE IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_IOCTL_DATA*)arg)->DevInfo.Instance, sizeof(uint8)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_FLOPPY_ACTIVATE IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("CdromIoctl(): __copy_from_user failed in USB_FLOPPY_ACTIVATE IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		EventStatus = &(BotIfData->EventStatus[Instance]);
		EventStatus->EventCode 	 = 0x02; // NewMedia
		EventStatus->MediaStatus = 0x02; // Media Present
		
		*RetVal = iUSB.iUSBHdiskRemoteActivate (FDInstannceAssignment[Instance]);
		return 0;

	case USB_FLOPPY_EXIT:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_EXIT IOCTL...\n");
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_EXIT IOCTL\n");
			return 0;
		}
		if (__copy_from_user((void *)(&Instance),&((IUSB_IOCTL_DATA*)arg)->DevInfo.Instance, sizeof(uint8)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_FLOPPY_EXIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("CdromIoctl(): __copy_from_user failed in USB_FLOPPY_EXIT IOCTL\n");
			*RetVal = -1;
			return 0;
		}

		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);
		EventStatus = &(BotIfData->EventStatus[Instance]);
		EventStatus->EventCode   = 0x01; // EjectRequest
		EventStatus->MediaStatus = 0x02; // Media Present
		
		*RetVal = iUSB.iUSBHdiskRemoteDeactivate (FDInstannceAssignment[Instance]);
		return 0;

	case USB_DEVICE_DISCONNECT:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_DEVICE_DISCONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_DEVICE_DISCONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (floppy_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_DEVICE_DISCONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceDisconnect (floppy_device_driver.devnum);
		return 0;
		
	case USB_DEVICE_RECONNECT:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_DEVICE_RECONNECT IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_DEVICE_RECONNECT IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (floppy_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_DEVICE_RECONNECT IOCTL\n");
			return 0;
		}
		*RetVal = UsbCore.CoreUsbDeviceReconnect (floppy_device_driver.devnum);
		return 0;
		
	case USB_FLOPPY_RST_IF:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_RST_IF IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_FLOPPY_RST_IF IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (floppy_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_RST_IF IOCTL\n");
			return 0;
		}
		floppy_device_driver.maxlun = 0;
		NumLunComboFD = 0;
		for (i = 0; i < MAX_LUN_SUPPORT; i++)
		{
			if (iUSB.iUSBReleaseHdiskInstanceIndex (i))
			{
				TWARN ("FloppyIoctl(): Error Releasing Floppy Instance %d\n", i);
			}
		}
		memset (LunNumAssignment, 0xFF, sizeof (LunNumAssignment));
		UsbCore.CoreUsbSetNumLUN (DevNo, floppy_device_driver.maxlun);
	
		*RetVal = 0;
		return 0;
		
	case USB_FLOPPY_ADD_FD:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"USB_FLOPPY_ADD_FD IOCTL...\n");
		if (__copy_from_user((void *)(&IoctlData), (void*)arg, sizeof(IUSB_IOCTL_DATA)))
		{
			TWARN ("FloppyIoctl(): __copy_from_user failed in USB_FLOPPY_ADD_FD IOCTL\n");
			*RetVal = -1;
			return 0;
		}
		if (floppy_device_driver.devnum != IoctlData.DevInfo.DevNo)
			return -1;
		IfNum = IoctlData.DevInfo.IfNum;
		DevNo = IoctlData.DevInfo.DevNo;
		if (AuthenticateIOCTLCaller (arg, RetVal, 0))
		{
			TWARN ("FloppyIoctl(): Invalid Auth Key in USB_FLOPPY_ADD_FD IOCTL\n");
			return 0;
		}
		BotIfData = (BOT_IF_DATA*) UsbCore.CoreUsbGetInterfaceData (DevNo, IfNum);

		EventStatus = &(BotIfData->EventStatus[floppy_device_driver.maxlun]);
		EventStatus->EventCode   = 0x0;  // 0h NoChg
		EventStatus->MediaStatus = 0x1; // 00b Door or Tray opena
		
		Inquiry = &(BotIfData->Inquiry[floppy_device_driver.maxlun]);
		
		Inquiry->ISO_ECMA_ANSI		=	0;
		Inquiry->ResponseDataFormat =	1;
		Inquiry->AdditionalLength	=	sizeof(SCSI_INQUIRY_PACKET)-5;

		Inquiry->PeripheralType 	=	PERIPHERAL_DIRECT;
		Inquiry->RMB				=	MEDIUM_REMOVABLE;

#ifdef CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO

        memset((void *)&Inquiry->VendorInfo[0], ' ', 8);
        memset((void *)&Inquiry->ProductRev[0], ' ', 4);
        memset((void *)&Inquiry->ProductInfo[0], ' ', 16);

       if ( ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO) <= 8 ) && 
            ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION) <= 4) && 
            ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO) <= 14 ) )
       {
           strncpy((char *)&Inquiry->VendorInfo[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO));
           strncpy((char *)&Inquiry->ProductRev[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION));
           strncpy((char *)&Inquiry->ProductInfo[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO));
       }
       else
       {
           TCRIT (" Length of any Floppy Media Configuration is out of range");
           return -1;
       }
#else
       strncpy((char *)&Inquiry->VendorInfo[0],"AMI     ",8);
       strncpy((char *)&Inquiry->ProductRev[0],"1.00",4);
       strncpy((char *)&Inquiry->ProductInfo[0],"Virtual Floppy  ",16);
#endif

		Inquiry->ProductInfo[14] = 0x30 + NumLunComboFD;
	
		LunNumAssignment[floppy_device_driver.maxlun] = iUSB.iUSBGetHdiskInstanceIndex ();
		if (-1 == LunNumAssignment[floppy_device_driver.maxlun])
		{
			TCRIT ("FloppyIoctl: Unable to get Free HDISK instance from iUSB module");
			return -1;
		}
		FDInstannceAssignment[NumLunComboFD] = LunNumAssignment[floppy_device_driver.maxlun];
		NumLunComboFD++;
		UsbCore.CoreUsbSetNumLUN (DevNo, floppy_device_driver.maxlun);
		floppy_device_driver.maxlun++;
		*RetVal = 0;
		return 0;
		

	default:
		TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"Unsupported Floppy IOCTL received...\n");
			return -1;
	}

}

static int
CreateFloppyDescriptor(uint8 DevNo)
{
	uint8 *MyDescriptor;
	uint16 TotalSize;
	SCSI_INQUIRY_PACKET *Inquiry;
	BOT_IF_DATA *IfData;
	USB_DEVICE_DESC	*DevDesc;
	
	/* HD is common term for floppy disk , memory key etc*/
	INTERFACE_INFO HDIfInfo =
	{
		NULL,UsbCore.CoreUsbBotRxHandler,UsbCore.CoreUsbBotReqHandler,UsbCore.CoreUsbBotProcessHandler,
		UsbCore.CoreUsbBotHaltHandler,UsbCore.CoreUsbBotUnHaltHandler,UsbCore.CoreUsbBotRemHandler,
		NULL,0,0,0,0

	};  

	/***********************CREATE FDisk INTERFACE DATA*********************/
	HDIfInfo.InterfaceData =(void *) vmalloc(sizeof(BOT_IF_DATA));
	if (HDIfInfo.InterfaceData == NULL)
	{
		TCRIT("CreateFloppyDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}

	/* Initialize Interface Data */
	IfData = (BOT_IF_DATA*)(HDIfInfo.InterfaceData);

	UsbCore.CoreUsb_OsInitSleepStruct(&(IfData->UnHaltedSleep));
	IfData->UnHalted 		= 1;
	IfData->SenseKey		= 0;
	IfData->SenseCode		= 0;
	IfData->SenseCodeQ		= 0;
	IfData->NoMediumRetries = MAX_NOMEDIUM_RETRIES;
	IfData->ScsiDataOutLen		= 0;
	IfData->LastSeqNo		= 0;	/* Invalid Seq No */

	
	IfData->Inquiry = (SCSI_INQUIRY_PACKET*) vmalloc (sizeof(SCSI_INQUIRY_PACKET)* 
								(MAX_LUN_SUPPORT + 1));

	if (IfData->Inquiry == NULL)
	{
		TCRIT("CreateFloppyDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}	
	IfData->EventStatus = (SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET*) vmalloc (sizeof(SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET)* 
							(MAX_LUN_SUPPORT + 1));
	if (IfData->EventStatus == NULL)
	{
		vfree (IfData->Inquiry);
		TCRIT("CreateFloppyDescriptor():Memory Alloc Failure for Interface\n");
		return -1;
	}
	IfData->EventStatus->EventCode   = 0;
	IfData->EventStatus->MediaStatus = 1;

	/* Form the Inquiry Packet Packet */
	Inquiry = &(IfData->Inquiry[LUN_0]);
	memset((void *)Inquiry,0,sizeof(SCSI_INQUIRY_PACKET));

	Inquiry->ISO_ECMA_ANSI		=	0;
	Inquiry->ResponseDataFormat	=	1;
	Inquiry->AdditionalLength	=	sizeof(SCSI_INQUIRY_PACKET)-5;

	/* HardDisk Specific Values in Inquiry Packet */
	Inquiry->PeripheralType		= 	PERIPHERAL_DIRECT;
	Inquiry->RMB				= 	MEDIUM_REMOVABLE;

#ifdef CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO

    memset((void *)&Inquiry->VendorInfo[0], ' ', 8);
    memset((void *)&Inquiry->ProductRev[0], ' ', 4);
    memset((void *)&Inquiry->ProductInfo[0], ' ', 16);
        
    if ( ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO) <= 8 ) && 
         ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION) <= 4) && 
         ( strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO) <= 14 ) )
    {
        strncpy((char *)&Inquiry->VendorInfo[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_VENDOR_INFO));
        strncpy((char *)&Inquiry->ProductRev[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_REVISION));
        strncpy((char *)&Inquiry->ProductInfo[0], CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO, strlen(CONFIG_SPX_FEATURE_FLOPPY_MEDIA_PRODUCT_INFO));
    }
    else
    {
        TCRIT (" Length of any Floppy Media Configuration is out of range");
        return -1;
    }
#else
    strncpy((char *)&Inquiry->VendorInfo[0],"AMI     ",8);
    strncpy((char *)&Inquiry->ProductRev[0],"1.00",4);
    strncpy((char *)&Inquiry->ProductInfo[0],"Virtual Floppy  ",16);
#endif

	/****************** Initialize FDisk  Interface Data End ****************/


	TotalSize = sizeof(USB_DEVICE_DESC);
	TotalSize += sizeof(USB_CONFIG_DESC);
	TotalSize += sizeof(USB_INTERFACE_DESC);
	TotalSize += (2*sizeof(USB_ENDPOINT_DESC));

	MyDescriptor = UsbCore.CoreUsbCreateDescriptor(DevNo,TotalSize);
	if (MyDescriptor == NULL)
	{
		TCRIT("CreateFloppyDescriptor(): Memory Allocation Failure\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}

	if (UsbCore.CoreUsbAddDeviceDesc(DevNo,VENDOR_ID,PRODUCT_ID_FLOPPY,DEVICE_REV,
				"American Megatrends Inc.","Virtual Floppy Device", 0) != 0)
	{
		TCRIT("CreateFloppyDescriptor: Error in creating Device Desc\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}
	
	DevDesc = UsbCore.CoreUsbGetDeviceDesc (DevNo);
	DevDesc->iSerialNumber = UsbCore.CoreUsbAddStringDescriptor(DevNo, AMI_FLOPPY_SERIAL_NUM_STRING);
	
	if (UsbCore.CoreUsbAddCfgDesc(DevNo,1,0) != 0)
	{
		TCRIT("CreateFloppyDescriptor(): Error iCreateFloppyDescriptorn creating Cfg Desc\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}
	
	if (UsbCore.CoreUsbAddInterfaceDesc(DevNo,2,MASS_INTERFACE,MASS_SCSI_INTERFACE,
						MASS_BULK_ONLY_PROTOCOL,"Virtual Floppy",&HDIfInfo) !=0)

	{
		TCRIT("CreateFloppyDescriptor(): Error in creating Interface Desc\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,IN,BULK,0x40,0x200,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateFloppyDescriptor(): Error in creating OUT EndPointDesc\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}

	if (UsbCore.CoreUsbAddEndPointDesc(DevNo,OUT,BULK,0x40,0x200,Interval,DATA_EP) != 0)
	{
		TCRIT("CreateFloppyDescriptor(): Error in creating OUT EndPointDesc\n");
		vfree (IfData->Inquiry);
		vfree (IfData->EventStatus);
		return -1;
	}
	return 0;				
}                                                               

static int
IsEmulateFloppyEnabled (uint8 LunNo)
{
	return IsEmulateFloppy;
}

static int
RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo)
{
	return iUSB.iUSBRemoteScsiCall (DevNo,ifnum,MassData,LunNo, LunNumAssignment[LunNo]);
}

static int
FillDevInfo (IUSB_DEVICE_INFO* iUSBDevInfo)
{
	uint8 NumDevices = 0;

	iUSBDevInfo->DeviceType = IUSB_DEVICE_FLOPPY;
	iUSBDevInfo->DevNo      = floppy_device_driver.devnum;
    iUSBDevInfo->IfNum      = 0; 
	iUSBDevInfo->LockType	= LOCK_TYPE_SHARED;
	iUSBDevInfo++;
	NumDevices++;

	return NumDevices;
}

static int 
SetInterfaceAuthKey (uint8 DevNo, uint8 IfNum, uint32 Key)
{
	TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"SetInterfaceAuthKey() is called\n");
	if (DevNo != floppy_device_driver.devnum)
	{
		TWARN ("SetInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
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
	TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"ClearInterfaceAuthKey() is called\n");
	if (DevNo != floppy_device_driver.devnum)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Device Number %d\n", DevNo);
		return -1;
	}
	if (IfNum != 0)
	{
		TWARN ("ClearInterfaceAuthKey(): Invalid Interface Number %d\n", IfNum);
		return -1;
	}
	IfaceAuthKeys[IfNum] = 0;
	return 0;
}

static int
init_floppy_device_module (void)
{
	int ret_val;
	int devnum;

	printk ("Loading Floppy Device Module...\n");
	get_usb_core_funcs (&UsbCore);
	
	get_iusb_funcs (&iUSB);
	
	if ((HWModuleName != NULL) && (sizeof(floppy_device_driver.name) > strlen(HWModuleName)))
	{
		strncpy (floppy_device_driver.name, HWModuleName,sizeof(floppy_device_driver.name));
		floppy_device_driver.name[sizeof(floppy_device_driver.name)-1]='\0';
	}

	devnum = UsbCore.CoreUsbGetFreeUsbDev (&floppy_device_driver);
	if (devnum == -1)
	{
		TCRIT ("init_floppy_device_module: Unable to get Free USB Hardware Module for Floppy Device");
		return -1;
	}

	ret_val = iUSB.iUSBGetHdiskInstanceIndex ();
	if (-1 == ret_val)
	{
		TCRIT ("init_floppy_device_module: Unable to get Free Hard disk instance from iUSB module");
		return -1;
	}

	LunNumAssignment[LUN_0] = (uint8)ret_val;
	FDInstannceAssignment[0] = LunNumAssignment[LUN_0]; 

	floppy_device_driver.devnum=devnum;
	if (0 != CreateFloppyDescriptor (devnum))
	{
		TCRIT ("init_floppy_device_module: Unable to create Floppy device descriptor\n");
		UsbCore.CoreUsbDestroyDescriptor (devnum);
		return -1;
	}

	ret_val = register_usb_device (&floppy_device_driver, &UsbDev);
	if (!ret_val) 
	{
		register_reboot_notifier (&floppy_notifier);
		my_sys = AddSysctlTable("floppy",&floppyctlTable[0]);
	}
	else
	{
		TCRIT ("init_floppy_device_module: floppy Device failed to register with USB core\n");
	}

	return ret_val;
}

static void
exit_floppy_device_module (void)
{
	int i;
	
	printk ("Unloading Floppy Device Module...\n");
	for (i = 0; i < MAX_LUN_SUPPORT; i++)
	{
		if (LunNumAssignment[i] == 0xFF) continue;
		if (iUSB.iUSBReleaseHdiskInstanceIndex(i))
		{
			TWARN ("exit_floppy_device_module(): Error Releasing Floopy Disk Instance\n");
		}
	}
	unregister_usb_device (&floppy_device_driver);
	unregister_reboot_notifier (&floppy_notifier);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
floppy_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{
	TDBG_FLAGGED(floppy, DEBUG_FLOPPY,"Floppy Device Reboot notifier...\n");
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	   unregister_usb_device (&floppy_device_driver);
	}
	return NOTIFY_DONE;
}


module_init(init_floppy_device_module);
module_exit(exit_floppy_device_module);

MODULE_AUTHOR("Rama Rao Bisa <RamaB@ami.com>");
MODULE_DESCRIPTION("USB Floppy Device module");
MODULE_LICENSE("GPL");
