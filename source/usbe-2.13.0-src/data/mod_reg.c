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

#include <linux/module.h>
#include "header.h"
#include "usb_hw.h"
#include "descriptors.h"
#include "usb_core.h"
#include "arch.h"
#include "bot.h"
#include "helper.h"
#include "dbgout.h"
#include "entry.h"


/* USB Device Initialization Table */
UsbHwInit DevInit[MAX_USB_HW] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
UsbHwExit DevExit[MAX_USB_HW] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
CREATE_DESC CreateDesc[MAX_USB_HW] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
video_callback     g_video_callback = { NULL };

extern IUSB_DEVICE_DETAILS_T IUSBDeviceDetails [];
static int iUSBDevCount = 0;

static int GetFreeUsbDev (usb_device_driver* UsbDevDriver);
static int UsbSetNumLUN (uint8 DevNo, uint8 NumLUN);

static USB_CORE mUsbCoreFunc = {
			.CoreUsbGetDeviceConnectState			= UsbGetDeviceConnectState,
			.CoreUsbBusDisconnect					= UsbBusDisconnect,
			.CoreUsbBusSuspend						= UsbBusSuspend,
			.CoreUsbBusResume						= UsbBusResume,
			.CoreUsbBusReset						= UsbBusReset,
			.CoreUsbRxHandler0						= UsbRxHandler0,
			.CoreUsbTxHandler						= UsbTxHandler,
			.CoreUsbRxHandler						= UsbRxHandler,
			.CoreUsbSetRxDataLen					= UsbSetRxDataLen,
			.CoreUsbGetRxData						= UsbGetRxData,
			.CoreUsb_OsRegisterIsr					= Usb_OsRegisterIsr,
			.CoreUsb_OsUnregisterIsr				= Usb_OsUnregisterIsr,
			.CoreUsbHaltHandler						= UsbHaltHandler,
			.CoreUsbUnHaltHandler					= UsbUnHaltHandler,
			.CoreUsb_OsDelay						= Usb_OsDelay,
			.CoreUsbConfigureHS						= UsbConfigureHS,
			.CoreUsbConfigureFS						= UsbConfigureFS,
			.CoreUsbConfigureLS						= UsbConfigureLS,
			.CoreUsbCreateDescriptor				= CreateDescriptor,
			.CoreUsbDestroyDescriptor				= DestroyDescriptor,
			.CoreUsbAddDeviceDesc					= AddDeviceDesc,
			.CoreUsbAddHidDesc						= AddHidDesc,
			.CoreUsbAddInterfaceDesc				= AddInterfaceDesc,
			.CoreUsbAddCfgDesc						= AddCfgDesc,
			.CoreUsbAddEndPointDesc					= AddEndPointDesc,
			.CoreUsbRemoteWakeupHost				= UsbRemoteWakeupHost,
			.CoreUsbDeviceDisconnect				= UsbDeviceDisconnect,
			.CoreUsbDeviceReconnect					= UsbDeviceReconnect,
			.CoreUsbGetInterfaceData				= GetInterfaceData,
			.CoreUsb_OsWakeupOnTimeout				= Usb_OsWakeupOnTimeout,
			.CoreUsbGetDataInEp						= GetDataInEp,
			.CoreUsbWriteData						= UsbWriteData,
			.CoreUsb_OsInitSleepStruct				= Usb_OsInitSleepStruct,
			.CoreUsb_OsSleepOnTimeout				= Usb_OsSleepOnTimeout,
			.CoreUsbGetDataOutEp					= GetDataOutEp,
			.CoreUsbGetInterfaceDesc				= GetInterfaceDesc,
			.CoreUsbGetFreeUsbDev					= GetFreeUsbDev,
			.CoreUsbBotRxHandler					= BotRxHandler,
			.CoreUsbBotReqHandler					= BotReqHandler,
			.CoreUsbBotProcessHandler				= BotProcessHandler,
			.CoreUsbBotHaltHandler					= BotHaltHandler,
			.CoreUsbBotUnHaltHandler				= BotUnHaltHandler,
			.CoreUsbBotRemHandler					= BotRemHandler,
			.CoreUsbBotSendData						= BotSendData,
			.CoreUsbBotSetScsiErrorCode				= BotSetScsiErrorCode,
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500)
			.CoreUsbBotSetScsiEventStatus			= BotSetScsiEventStatus,
#endif
			.CoreUsbBotSendCSW						= BotSendCSW,
			.CoreUsbAddHubDesc						= AddHubDesc,
			.CoreUsbGetHubDesc						= GetHubDesc,
			.CoreUsbGetHubPortsStatus				= UsbGetHubPortsStatus,
			.CoreUsbSetHubPortsStatus				= UsbSetHubPortsStatus,
			.CoreUsbHubEnableDevice					= UsbHUBEnableDevice,
			.CoreUsbHubDisableDevice				= UsbHUBDisableDevice,
			.CoreUsbClearHubDeviceEnableFlag		= UsbClearHubDeviceEnableFlag,
			.CoreUsbSetHubDeviceEnableFlag			= UsbSetHubDeviceEnableFlag,
			.CoreUsbHubPortBitmap					= UsbHUBPortBitmap,
			.CoreUsbChangeHubRegs					= UsbHubChangeHubRegs,
			.CoreUsbGetHubSpeed						= UsbGetHubSpeed,
			.CoreUsbIsHubDeviceConnected			= UsbIsHubDeviceConnected,
			.CoreUsbIsHubDeviceEnabled				= UsbIsHubDeviceEnabled,
			.CoreUsbFormiUsbHeader					= FormiUsbHeader,
			.CoreUsbGlobalConnectSupport			= 0,
			.CoreUsbDevGetCfgIndex					= UsbGetCfgIndex,
			.CoreUsbGetDeviceDesc					= GetDeviceDesc,
			.CoreUsbAddStringDescriptor				= AddStringDescriptor,
			.CoreUsbGetConfigDesc					= GetConfigDesc,
			.CoreUsbAddDescriptor					= AddDescriptor,
			.CoreUsbNet_OSPushPacket				= Net_OSPushPacket,
			.CoreUsbNet_TxComplete				= Net_TxComplete,
			.CoreUsbNet_OSRegisterDriver			= Net_OSRegisterDriver,
			.CoreUsbNet_OSUnregisterDriver			= Net_OSUnregisterDriver,
			.CoreUsbGetRxDataLen					= UsbGetRxDataLen,
			.CoreUsbGetNonDataEp					= GetNonDataEp,
			.CoreUsbWriteDataNonBlock				= UsbWriteDataNonBlock,
			.CoreUsbGetMaxPacketSize				= GetMaxPacketSize,
			.CoreUsbSetNumLUN						= UsbSetNumLUN,
			.CoreUsbAddOsStringDesc					= AddOsStringDesc,
			.CoreUsbAddExtPropertiesDesc			= AddExtPropertiesDesc,
        };

static int 
UsbSetNumLUN (uint8 DevNo, uint8 NumLUN)
{
	DevInfo[DevNo].MaxLUN = NumLUN;
	return 0;
}
static int
GetFreeUsbDev (usb_device_driver* UsbDevDriver)
{
	int DevNo;
	int HighSpeedDevNo = -1;
	int FullSpeedDevNo = -1;
	int LowSpeedDevNo = -1;
	
	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++)
	{
		if (CreateDesc[DevNo]) continue;
		if (DevInfo[DevNo].Name[0] == 0) continue;
		if (0 == strncmp (DevInfo[DevNo].Name, UsbDevDriver->name, sizeof (DevInfo[DevNo].Name)-1)) 
		{
		   return DevNo;
		}
	}

	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++)
	{
		if (DevInfo[DevNo].Name[0] == 0) continue;
		if (CreateDesc[DevNo]) continue;
		if ((DevInfo[DevNo].UsbDevice.SupportedSpeeds & SUPPORT_LOW_SPEED) &&
			(UsbDevDriver->speeds & SUPPORT_LOW_SPEED))
		{
			if (LowSpeedDevNo == -1) LowSpeedDevNo = DevNo;
		}
		if ((DevInfo[DevNo].UsbDevice.SupportedSpeeds & SUPPORT_FULL_SPEED) &&
			(UsbDevDriver->speeds & SUPPORT_FULL_SPEED))
		{
			if (FullSpeedDevNo == -1) FullSpeedDevNo = DevNo;
		}
		if ((DevInfo[DevNo].UsbDevice.SupportedSpeeds & SUPPORT_HIGH_SPEED) &&
			(UsbDevDriver->speeds & SUPPORT_HIGH_SPEED))
		{
			if (HighSpeedDevNo == -1) HighSpeedDevNo = DevNo;
		}
	}
	if (UsbDevDriver->descriptor == CREATE_HID_DESCRIPTOR)
	{
		if (-1 != LowSpeedDevNo) return LowSpeedDevNo;
		if (-1 != FullSpeedDevNo) return FullSpeedDevNo;
		if (-1 != HighSpeedDevNo) return HighSpeedDevNo;
	}
	else
	{
		if (-1 != HighSpeedDevNo) return HighSpeedDevNo;
		if (-1 != FullSpeedDevNo) return FullSpeedDevNo;
		if (-1 != LowSpeedDevNo) return LowSpeedDevNo;
	}
	
	TWARN ("USB Device Module could not find suitable USB Hardware module name\n");
	return -1;

}


int get_usb_core_funcs (USB_CORE *UsbCoreFunc)
{
		memcpy (UsbCoreFunc, &mUsbCoreFunc, sizeof (USB_CORE));
		return 0;
}

int register_usb_chip_driver_module (usb_ctrl_driver *usbctrldrv)
{
	uint8 DevNo;
	uint8 NumEndPoints;

	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++)
	{
		if (!DevInit[DevNo]) break;
	}

	if (DevNo >= MAX_USB_HW)
	{
		TCRIT ("register_usb_chip_driver_module(): cannot accommodate this device module\n");
		return -1;
	}

	if (!usbctrldrv->usb_driver_init)
	{
		TCRIT ("register_usb_chip_driver_module(): The driver init function pointer is NULL\n");
		return -1;
	}
	else
	{
		DevInit[DevNo] = usbctrldrv->usb_driver_init;
	}

	if (!usbctrldrv->usb_driver_exit)
	{
		TCRIT ("register_usb_chip_driver_module(): The driver exit function pointer is NULL\n");
		return -1;
	}
	else
	{
		DevExit[DevNo] = usbctrldrv->usb_driver_exit;
	}

	usbctrldrv->devnum = DevNo;

	memset (DevInfo[DevNo].Name, 0, sizeof (DevInfo[DevNo].Name));
	strncpy (DevInfo[DevNo].Name, usbctrldrv->name, sizeof (DevInfo[DevNo].Name) - 1);
	
	/* Set device to InValid */
	DevInfo[DevNo].Valid = 0;

	/* Set device to Not Configured */
	DevInfo[DevNo].CfgIndex = 0;

	DevInfo[DevNo].RemoteWakeup = 0;

	/* Call Device Initializaion Routine */
	if ((*DevInit[DevNo])(DevNo,&(DevInfo[DevNo].UsbDevice),&(DevInfo[DevNo].DevConf)))
	{
		TCRIT("register_usb_device():Error in Initializing USB Hardware Device %d\n",DevNo);
		DevInit[DevNo] = NULL;
		return -1;
	}

	if(DevInfo[DevNo].UsbDevice.UsbHwGetEps (DevNo, (uint8*)&DevEpInfo[DevNo].ep_config[0], &DevEpInfo[DevNo].num_eps))
	{
		TCRIT ("register_usb_device(): Unable to read the end point info\n");
		return -1;
	}

	/* Get the Number of EndPoints */ 	/* +1 is for Endpoint 0 */
	NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 1;

	/* Allocate Memory for Endpoints structure*/
	DevInfo[DevNo].EndPoint =(ENDPOINT_INFO*) kmalloc(sizeof(ENDPOINT_INFO)*(NumEndPoints+2), GFP_ATOMIC);
	if (DevInfo[DevNo].EndPoint == NULL)
	{
		TCRIT ("register_usb_chip_device():Error in Memory Allocation for Endpoint of Device %d\n",DevNo);
		return -1;
	}
	memset (DevInfo[DevNo].EndPoint, 0, sizeof(ENDPOINT_INFO)*(NumEndPoints+2));
	/* Set the device is not suspended */
	DevInfo[DevNo].Suspend = 0;

	/* Set the device is not connected */
	if(DevInfo[DevNo].ConnectState != 0)
		UsbDeviceDisconnect(DevNo);

	return 0;
}

int register_usb_device (usb_device_driver* usb_device_driver, USB_DEV* UsbDev)
{
	uint8 DevNo;
	uint8 NumEndPoints;
	uint8 EpIndex;
	uint8 i,j;
	uint8 EpDir;
	int NumiUSBDevs;
	int MatchFound;
	
	IUSB_DEVICE_INFO iUSBDevBuf [MAX_IUSB_DEVICES];

	if (usb_device_driver->devnum >= MAX_USB_HW)
	{
		TCRIT ("Error:The Device Number %d cannot be more than %d\n", usb_device_driver->devnum, MAX_USB_HW);
	}
	DevNo = usb_device_driver->devnum;

	memcpy (&DevInfo[DevNo].UsbCoreDev, UsbDev, sizeof (USB_DEV));
	CreateDesc[DevNo] = UsbDev->DevUsbCreateDescriptor;
    DevInfo[DevNo].UsbDevType = usb_device_driver->descriptor;
	DevInfo[DevNo].DevReqSpeeds = usb_device_driver->speeds;
	DevInfo[DevNo].MaxLUN = usb_device_driver->maxlun;

	/* Set the device is valid and initialized */
	DevInfo[DevNo].Valid = 1;

	/* Get the Number of EndPoints */ 	/* +1 is for Endpoint 0 */
	NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;

	EpIndex = GetNextFreeEndpointIndex (DevNo);
	if (EpIndex == 0xFF) 
	{
		TCRIT ("register_usb_device(): Error in obtaining free EpIndex for DevNo %d\n", DevNo);
		return -1;
	}
	InitializeEndPoint(DevNo,0,DIR_IN, EpIndex);

	EpIndex = GetNextFreeEndpointIndex (DevNo);
	if (EpIndex == 0xFF) 
	{
		TCRIT ("register_usb_device(): Error in obtaining free EpIndex for DevNo %d\n", DevNo);
		return -1;
	}
	InitializeEndPoint(DevNo,0,DIR_OUT, EpIndex);
	/* Initialize Endpoint Structure */
	for (i = 0; NumEndPoints != 0; i++)
	{
		EpIndex = GetNextFreeEndpointIndex (DevNo);
		if (EpIndex == 0xFF) 
		{
			TCRIT ("register_usb_device(): Error in obtaining free EpIndex for DevNo %d\n", DevNo);
			return -1;
		}
		EpDir = DevEpInfo[DevNo].ep_config[i].ep_dir;
		if (EpDir == OUT) EpDir = DIR_OUT;
		if (EpDir == IN) EpDir = DIR_IN;

		InitializeEndPoint(DevNo,DevEpInfo[DevNo].ep_config[i].ep_num,
								EpDir, EpIndex);
		NumEndPoints -= 1;
	}

	DevInfo[DevNo].InRequest.Data = kmalloc(1024, GFP_ATOMIC);
	if (DevInfo[DevNo].InRequest.Data == NULL)
	{
		TCRIT ("kmallc failed for DevInfo[%d].InRequest.Data\n", DevNo);
		return -1;
	}
	DevInfo[DevNo].OutRequest.Data = kmalloc(1024, GFP_ATOMIC);
	if (DevInfo[DevNo].OutRequest.Data == NULL)
	{
		TCRIT ("kmallc failed for DevInfo[%d].OutRequest.Data\n", DevNo);
		return -1;
	}

	UsbDeviceEnable(DevNo);

	if (1 != usb_device_driver->DisconnectOnRegister)
        UsbDeviceReconnect(DevNo);
	mdelay (1000);

	if (iUSBDevCount == 0)
	{
		for (i = 0; i < MAX_IUSB_DEVICES; i++)
		{
			IUSBDeviceDetails[i].Valid = 0;
			IUSBDeviceDetails[i].iUSBDevInfo.DeviceType = 0xFF;
			IUSBDeviceDetails[i].iUSBDevInfo.DevNo = 0xFF;
			IUSBDeviceDetails[i].Used = 0;
		}
	}
	if (DevInfo[DevNo].UsbCoreDev.DevUsbFillDevInfo)
	{
		NumiUSBDevs = DevInfo[DevNo].UsbCoreDev.DevUsbFillDevInfo((IUSB_DEVICE_INFO*)iUSBDevBuf);
		if (0 == NumiUSBDevs)
		{
			TWARN ("register_usb_device(): Device No %d did not report any iUSB devices\n", DevNo);
			return -1;
		}
		MatchFound = 0;
		for (i = 0; i < NumiUSBDevs; i++)
		{
			for (j = 0; j < MAX_IUSB_DEVICES; j++)
			{
				if ((IUSBDeviceDetails[j].iUSBDevInfo.DevNo == iUSBDevBuf[i].DevNo) &&
					(IUSBDeviceDetails[j].iUSBDevInfo.DeviceType == iUSBDevBuf[i].DeviceType) &&
					(IUSBDeviceDetails[j].Valid == 0))
				{
					IUSBDeviceDetails[j].Valid = 0x55AA55AA;
					MatchFound++;
					break;
				}	
			}
		}
		if (MatchFound == NumiUSBDevs)
		{
			printk ("register_usb_device(): Matching Device Found\n");
			return 0;
		}
		for (i = 0; i < NumiUSBDevs; i++)
		{
			memcpy ((void*)&IUSBDeviceDetails[i+iUSBDevCount].iUSBDevInfo, 
				(void*)&iUSBDevBuf[i], sizeof (IUSB_DEVICE_INFO));
			IUSBDeviceDetails[i+iUSBDevCount].Valid = 0x55AA55AA;
			IUSBDeviceDetails[i+iUSBDevCount].Key	= (unsigned long)&IUSBDeviceDetails[i+iUSBDevCount].Valid;
		}
		iUSBDevCount += NumiUSBDevs;
	}
	return 0;
}

int unregister_usb_device (usb_device_driver *usb_device_driver)
{
	uint8 DevNo;
	uint8 EpNum;
	ENDPOINT_INFO *EpInfo;
	uint8 NumEndPoints;
	int i;
	DevNo = usb_device_driver->devnum;

	if (CreateDesc[DevNo])
	{
		NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;
		/* Remove outstanding timers and free memory */
		for (EpNum = 0; EpNum < NumEndPoints; EpNum++)
		{
		    EpInfo = &(DevInfo[DevNo].EndPoint[EpNum]);

            if (EpInfo->RxData != NULL)
            {
                kfree(EpInfo->RxData);
                EpInfo->RxData = NULL;
            }
		}

	}

	if ((DevInit[DevNo]) && (CreateDesc[DevNo]))
	{
		UsbDeviceDisable(DevNo);
		UsbDeviceDisconnect(DevNo);
		UsbUnCfgDevice(DevNo);

		NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;
		for (EpNum = 0; EpNum < NumEndPoints; EpNum++)
		{
			EpInfo = &(DevInfo[DevNo].EndPoint[EpNum]);
			EpInfo->EpUsed = 0;
		}

		UsbRemoveDevice(DevNo);
	}


	/* Set all devices to InValid */
	DevInfo[DevNo].Valid = 0;
	DevInfo[DevNo].DevReqSpeeds = 0;

	for (i = 0; i < MAX_IN_AND_OUT_ENDPOINTS; i++)
	{
	   DevEpInfo[DevNo].ep_config[i].is_ep_used = 0;
	   DevEpInfo[DevNo].epnumbuf [i] = 0;
	}
	DevInfo[DevNo].UsbCoreDev.DevUsbIOCTL = NULL;

	if (DevInfo[DevNo].InRequest.Data == NULL)
	{
		TWARN ("DevInfo[%d].InRequest.Data cannot be NULL\n", DevNo);
	}
	else
	{
		kfree (DevInfo[DevNo].InRequest.Data);
		DevInfo[DevNo].InRequest.Data = NULL;
	}

	if (DevInfo[DevNo].OutRequest.Data == NULL)
	{
		TWARN ("DevInfo[%d].OutRequest.Data cannot be NULL\n", DevNo);
	}
	else
	{
		kfree (DevInfo[DevNo].OutRequest.Data);
		DevInfo[DevNo].OutRequest.Data = NULL;
	}

	for (i = 0; i < MAX_IUSB_DEVICES; i++)
	{
		if (IUSBDeviceDetails[i].iUSBDevInfo.DevNo == DevNo)
		{
			IUSBDeviceDetails[i].Valid = 0;
		}
	}
	
	return 0;
}

int unregister_usb_chip_driver_module (usb_ctrl_driver *usbctrldrv)
{
	uint8 DevNo;
	usb_device_driver usb_device;

	DevNo = usbctrldrv->devnum;
	usb_device.devnum = usbctrldrv->devnum;

	unregister_usb_device (&usb_device);

    if (DevInfo[DevNo].EndPoint != NULL)
    {
	    /* Free Memory for Endpoints structure */
	    kfree(DevInfo[DevNo].EndPoint);
        DevInfo[DevNo].EndPoint = NULL;
    }

	if (DevInit[DevNo])
	{
		if (DevExit[DevNo]) (*DevExit[DevNo])(DevNo);
	}

	DevInit[DevNo] = NULL;
	DevExit[DevNo] = NULL;
	memset (DevInfo[DevNo].Name, 0, sizeof (DevInfo[DevNo].Name));

	return 0;
}

int
install_video_handler(void (*pt2Func)(int))
{
	if (g_video_callback.CallBackHandler == NULL)
	{
		g_video_callback.CallBackHandler = pt2Func;
		return 0;
	}
	return 1;
}

int
uninstall_video_handler(void (*pt2Func)(int))
{
	if(pt2Func == g_video_callback.CallBackHandler)
	{
		g_video_callback.CallBackHandler = NULL;
		return 0;
	}
	return 1;
}


EXPORT_SYMBOL (register_usb_device);
EXPORT_SYMBOL (unregister_usb_device);
EXPORT_SYMBOL (register_usb_chip_driver_module);
EXPORT_SYMBOL (unregister_usb_chip_driver_module);
EXPORT_SYMBOL (get_usb_core_funcs);
EXPORT_SYMBOL (g_video_callback);
EXPORT_SYMBOL (install_video_handler);
EXPORT_SYMBOL (uninstall_video_handler);

