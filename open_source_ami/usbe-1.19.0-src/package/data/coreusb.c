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
#include "descriptors.h"
#include "coreusb.h"
#include "arch.h"
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"
#include "entry.h"


#define RX_BUFFER_SIZE		1024
#define KEYBOARD_INTERFACE	0x01

/* USB Device Information=Hardware Info, Endpoints Info, CfgIndex, Valid Bit */
USB_DEV_INFO DevInfo[MAX_USB_HW];
USB_DEV_EP_INFO DevEpInfo [MAX_USB_HW];

/* Local Static Functions */
static int SendPendingEP0Data(uint8 DevNo);
static void EnableEndPoint(uint8 DevNo, uint8 EpNum, uint8 EpDir);
void HandleTxData(uint8 DevNo, uint8 ep, ENDPOINT_INFO *EpInfo);
static void CallInterfaceTxHandler(uint8 DevNo, uint8 ep);
static void CallInterfaceRxHandler(uint8 DevNo, uint8 ep);



/******************************************************************************/
/************************** Initialization Function ***************************/
/******************************************************************************/
void
UsbCoreFini(UsbHwInit *DevInit)
{
	uint8 DevNo;
	uint8 EpIndex;
	ENDPOINT_INFO *EpInfo;

	for (DevNo = 0; DevNo < MAX_USB_HW; DevNo++) {
		uint8 NumEndPoints;

		if (DevInit[DevNo] == NULL)
			continue;

		NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;

		/* Remove outstanding timers and free memory */
		for (EpIndex = 0; EpIndex < NumEndPoints; EpIndex++) {
			EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);
			if (EpInfo->RxData) 
				kfree(EpInfo->RxData);
		}

		/* Free Memory for Endpoints structure */
		if (DevInfo[DevNo].EndPoint)
			kfree(DevInfo[DevNo].EndPoint);
	}

	return;
}




/*****************************************************************************/
/*** Core Functions called from all modules - Setup and Device Modules *******/
/*****************************************************************************/
int
UsbWriteData(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt)
{
	uint8 EpIndex;
	ENDPOINT_INFO *EpInfo;
	long timeout = 0;
	int Ret;

	TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData():Sending Data of Length %ld bytes to ep#%d\n",Len,ep);

	EpIndex = GetEndPointIndex (DevNo, ep, DIR_IN);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbWriteData(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return 1;	
	}
	/* Get Endpoint Information */
	EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);
	if (EpInfo == NULL)
	{
		TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData(): Unable to Get Endpoint Information for ep%d\n",ep);
		return 1;
	}

	if (ep != 0)
	{
		/* Check if Endpoint is Enabled */
		if (!EpInfo->Enabled)
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData(): Endpoint is not enabled dev %d and ep %d\n",DevNo, ep);
			return 1;
		}
		
		/* Can Write only a IN EndPoint */
		if (!EpInfo->Dir)
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData(): Endpoint direction is not IN dev %d and ep %d\n",DevNo, ep);
			return 1;
		}
		
        /* Can Write only when the device is configured */
		if (DevInfo[DevNo].CfgIndex == 0)
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData(): Device is not configured dev %d and ep %d\n",DevNo, ep);
			return 1;
		}

		/* Can not write if device suspended */
		if (DevInfo[DevNo].Suspend == 1)
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbWriteData(): Device is suspended dev %d and ep %d\n",DevNo, ep);
			return 1;
		}

	}

	/* Load Endpoint with Buffer Details */
	if ((EpInfo->TxData != NULL)|| (EpInfo->TxReady != 1))
	{
		printk("UsbWriteData():Previous Write Pending for Dev %d Ep %d\n",DevNo,ep);
		printk("UsbWriteData():TxData=0x%lx DataLen=0x%lx DataPos=0x%lx Zero=%d Ready=%d\n",
							(uint32)EpInfo->TxData,
							EpInfo->TxDataLen,EpInfo->TxDataPos,
							EpInfo->TxZeroLenPkt,EpInfo->TxReady);
	}


	EpInfo->TxData 		= Buffer;
	EpInfo->TxDataLen	= Len;
	EpInfo->TxDataPos	= 0;
	EpInfo->TxZeroLenPkt= ZeroLenPkt;
	EpInfo->TxReady		= 0;

	DevInfo [DevNo].BlockingSend = 1;
	/* Start Transfer Data */
	HandleTxData(DevNo,ep,EpInfo);

	/* For Endpoint 0, we cannot wait as UsbWriteData are called from ISR */
	if (ep == 0)
		return 0;
    /* We are not going to wait for ack from host in the case of HID mouse, we will go ahead and 
	   complete this transaction.
	   This was done to, not queue up mouse events when host(windows) configured the device but not
	   accepting the data from device.
	   Please note - here we are assuming HID mouse is at default endpoint 0x02 , if your project has
	   something different , change endpoint accordingly
	 */   

    if(IsDevHid(DevNo))
    {
        timeout = TRANSMIT_TIMEOUT_HID;
		return 0;		
    }
    else
    {
	timeout = TRANSMIT_TIMEOUT;
    }
	
	/* Wait for Completion, signal or timeout */
	Ret = Usb_OsSleepOnTimeout(&(EpInfo->TxReadySleep), &(EpInfo->TxReady), timeout);
	if (Ret < 0)
	{
		TWARN ("Error: Usb_OsSleepOnTimeout is interrupted\n");	
		return -EINTR;
	}
	if (Ret == 0)
	{
		TCRIT("ERROR: UsbWriteData(): Transmission timeout for Device %d EP %d for DataSize 0x%lX\n",DevNo,ep,EpInfo->TxDataLen);
		EpInfo->TxData  = NULL;
		EpInfo->TxReady = 1;

		if (DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions)
				 DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions (DevNo, ep, ACTION_USB_WRITE_DATA_TIMEOUT);

		return 1;
	}


	if (DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions)
			 DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions (DevNo, ep, ACTION_USB_WRITE_DATA_SUCCESS);

	return 0;									
}

static int 
UsbGetDevBusSpeed (uint8 DevNo)
{
	uint8 Mask;
	uint8 Speed = SUPPORT_FULL_SPEED;

	Mask = DevInfo[DevNo].UsbDevice.SupportedSpeeds & DevInfo[DevNo].DevReqSpeeds;

	if (DevInfo[DevNo].UsbDevType != CREATE_HID_DESCRIPTOR)
	{
		if (Mask & SUPPORT_HIGH_SPEED) 
			Speed = SUPPORT_HIGH_SPEED;
		else if (Mask & SUPPORT_FULL_SPEED) 
			Speed = SUPPORT_FULL_SPEED;
		else if (Mask & SUPPORT_LOW_SPEED) 
			Speed = SUPPORT_LOW_SPEED;
		else 
			TCRIT ("UsbGetDevBusSpeed(): Speed requirements mismatch between Hardware and Devices for Non HID Device %d\n", DevNo);
	}
	else
	{
		if (Mask & SUPPORT_LOW_SPEED) 
			Speed = SUPPORT_LOW_SPEED;
		else if (Mask & SUPPORT_FULL_SPEED) 
			Speed = SUPPORT_FULL_SPEED;
		else if (Mask & SUPPORT_HIGH_SPEED) 
			Speed = SUPPORT_HIGH_SPEED;
		else 
			TCRIT ("UsbGetDevBusSpeed(): Speed requirements mismatch between Hardware and Devices for HID Device %d\n", DevNo);
	}

	return Speed;
}

void 	
UsbDeviceEnable(uint8 DevNo)
{
	uint8 Speed;

	Speed = UsbGetDevBusSpeed(DevNo);
	DevInfo[DevNo].UsbDevice.UsbHwEnable(DevNo, Speed);
	return;
}

void
UsbDeviceDisable(uint8 DevNo)
{
	uint8 Speed;

	Speed = UsbGetDevBusSpeed(DevNo);
	DevInfo[DevNo].UsbDevice.UsbHwDisable(DevNo, Speed);
	return;
}

irqreturn_t
UsbDeviceIntr(uint8 DevNo, uint8 ep)
{
	return DevInfo[DevNo].UsbDevice.UsbHwIntr(DevNo,ep);
}


void
UsbSetAddr(uint8 DevNo,uint8 Addr, uint8 Enable)
{
	if (Enable && Addr)
		printk("Usb SetAddress() Called for device %d with Addr %d \n",DevNo,Addr);
	DevInfo[DevNo].UsbDevice.UsbHwSetAddr(DevNo,Addr,Enable);
	return;
}

int		
UsbGetEpStatus(uint8 DevNo,uint8 Ep, uint8 EpDir, uint8 *Enable, uint8 *Stall)
{
	return DevInfo[DevNo].UsbDevice.UsbHwGetEpStatus(DevNo,Ep, EpDir, Enable,Stall);
}

int
UsbStallEp(uint8 DevNo,uint8 Ep, uint8 EpDir)
{	
	int RetVal = 0;
	
	RetVal = DevInfo[DevNo].UsbDevice.UsbHwStallEp(DevNo,Ep, EpDir);
/*	UsbHaltHandler(DevNo,Ep);	*/ /* Called by the Hardware */		
	return RetVal;
}


int
UsbUnStallEp(uint8 DevNo,uint8 Ep, uint8 EpDir)
{	
	int RetVal = 0;
	
	RetVal = DevInfo[DevNo].UsbDevice.UsbHwUnstallEp(DevNo,Ep, EpDir);
/*	UsbUnHaltHandler(DevNo,Ep);	*/ /* Called by the Hardware */	
	return RetVal;
}

void	
UsbSetRemoteWakeup(uint8 DevNo,uint8 Set)
{
	if (DevInfo[DevNo].UsbDevice.UsbHwSetRemoteWakeup)
			DevInfo[DevNo].UsbDevice.UsbHwSetRemoteWakeup(DevNo,Set);
	else
			DevInfo[DevNo].RemoteWakeup = Set;
	return;
}

uint8	
UsbGetRemoteWakeup(uint8 DevNo)
{
	if (DevInfo[DevNo].UsbDevice.UsbHwGetRemoteWakeup)
			return DevInfo[DevNo].UsbDevice.UsbHwGetRemoteWakeup(DevNo);
	else
			return DevInfo[DevNo].RemoteWakeup;
}

void
UsbRemoteWakeup(uint8 DevNo)
{
	if ((DevInfo[DevNo].RemoteWakeup) || (DevInfo[DevNo].UsbDevice.UsbHwSetRemoteWakeup))	
	{
		if (DevInfo[DevNo].UsbDevice.UsbHwRemoteWakeup)
			DevInfo[DevNo].UsbDevice.UsbHwRemoteWakeup(DevNo);
	}
	return;
}

uint8
UsbGetBusSuspendState(uint8 DevNo)
{
    return DevInfo[DevNo].Suspend;
}

int
UsbDeviceDisconnect(uint8 DevNo)
{
	DevInfo[DevNo].ConnectState = 0;
	if (!(DevInfo[DevNo].UsbDevice.UsbHwDeviceDisconnect))
		return 1;
	DevInfo[DevNo].UsbDevice.UsbHwDeviceDisconnect(DevNo);
	Usb_OsDelay(1000);
	return 0;
}


int
UsbDeviceReconnect(uint8 DevNo)
{
	DevInfo[DevNo].ConnectState = 1;
	if (!(DevInfo[DevNo].UsbDevice.UsbHwDeviceReconnect))
		return 1;
	DevInfo[DevNo].UsbDevice.UsbHwDeviceReconnect(DevNo);
	return 0;
}

int
UsbGetDeviceConnectState(uint8 DevNo)
{
	return DevInfo[DevNo].ConnectState;
}

uint8 
UsbGetCfgIndex(uint8 DevNo)
{
	return DevInfo[DevNo].CfgIndex;
}	


uint8*
UsbGetRxData(uint8 DevNo,uint8 ep)
{
	uint8 EpIndex;
	
	EpIndex = GetEndPointIndex (DevNo, ep, DIR_OUT);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbGetRxData(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return NULL;	
	}
	return DevInfo[DevNo].EndPoint[EpIndex].RxData;
}	

uint32
UsbGetRxDataLen(uint8 DevNo,uint8 ep)
{
	
	uint8 EpIndex;
	
	EpIndex = GetEndPointIndex (DevNo, ep, DIR_OUT);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbGetRxDataLen(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return 0;	
	}
	return DevInfo[DevNo].EndPoint[EpIndex].RxDataLen;
}	

void
UsbSetRxDataLen(uint8 DevNo,uint8 ep,uint32 Len)
{
	
	uint8 EpIndex;
	
	EpIndex = GetEndPointIndex (DevNo, ep, DIR_OUT);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbSetRxDataLen(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return;	
	}
	DevInfo[DevNo].EndPoint[EpIndex].RxDataLen = Len;
	return;
}	




int
UsbCfgDevice(uint8 DevNo,uint16 CfgIndex)
{
	uint8 EpNum, DevNumEndPoints;
	uint8 bNumEndPoints, bNumInterfaces;
	uint8 IfaceNum;
	uint8 EpType,EpDir;
	USB_ENDPOINT_DESC*	EndDesc;
	uint8 TotalEndPoints;
	uint8 EpIndex;
	uint8 Direction;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbCfgDevice():For Device 0x%x for CfgIndex 0x%x\n",DevNo,CfgIndex);

	/* Get the Number of EndPoints */ 	/* +1 is for Endpoint 0 */
	DevNumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;	

	/* Set the Configuraiton Index */
	DevInfo[DevNo].CfgIndex = CfgIndex;	

	/* Configure the Endpoints for this Device */
	UsbEnableAllEndPoints(DevNo);

	/* Enable the Endpoints for this CfgIndex */
	TotalEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;
	bNumInterfaces = GetNumInterfaces(DevNo,CfgIndex);
	for (IfaceNum=0;IfaceNum<bNumInterfaces;IfaceNum++)
	{
		bNumEndPoints = GetNumEndpoints(DevNo, CfgIndex,IfaceNum);
		for (EpNum = 1; ((EpNum <= TotalEndPoints) && bNumEndPoints); EpNum++)
		{
			for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
			{
				EndDesc = GetEndpointDesc(DevNo, CfgIndex,IfaceNum, EpNum, Direction);
				if (EndDesc != NULL)
				{
					bNumEndPoints -= 1;
					EpDir = (EndDesc->bEndpointAddress & IN)?DIR_IN:DIR_OUT;
					EpType = (EndDesc->bmAttributes) & 0x03;
					if (DevInfo[DevNo].UsbDevice.UsbHwEnableEp)
						DevInfo[DevNo].UsbDevice.UsbHwEnableEp(DevNo,EpNum,EpDir,EpType);
					EpIndex = GetEndPointIndex (DevNo, EpNum, EpDir);
					if (EpIndex == 0xFF)
					{
						TCRIT ("UsbCfgDevice(): EndPoint Index could not be obtained for dev %d ep %d EpDir %d\n", DevNo, EpNum, EpDir);
						continue; 
					}
					DevInfo[DevNo].EndPoint[EpIndex].Enabled = 1;		
				}			
			}
		}
	}
	return 0;
}

int
UsbUnCfgDevice(uint8 DevNo)
{
	uint8 bNumEndPoints, bNumInterfaces;
	uint8 IfaceNum,EpNum;
	uint16 CfgIndex;
	uint8 EpType,EpDir;
	uint8 TotalEndPoints;
	USB_ENDPOINT_DESC* EndDesc;
	uint8 EpIndex;
	uint8 Direction;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbUnCfgDevice(): For Device 0x%x\n",DevNo);


	/* Get the Configuraiton Index and clear it*/
	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return 0;
	
	DevInfo[DevNo].CfgIndex = 0;			

	/* Disable the Endpoints for this CfgIndex */
	TotalEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;
	bNumInterfaces = GetNumInterfaces(DevNo,CfgIndex);
	for (IfaceNum=0;IfaceNum<bNumInterfaces;IfaceNum++)
	{
		bNumEndPoints = GetNumEndpoints(DevNo, CfgIndex,IfaceNum);
		for (EpNum = 1; ((EpNum <= TotalEndPoints) && bNumEndPoints); EpNum++)
		{
			for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
			{
				EndDesc = GetEndpointDesc(DevNo, CfgIndex,IfaceNum, EpNum, Direction);
				if (EndDesc != NULL)
				{
					bNumEndPoints -= 1;
					EpDir = (EndDesc->bEndpointAddress & IN)?DIR_IN:DIR_OUT;
					EpType = (EndDesc->bmAttributes) & 0x03;
					TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbUnCfgDevice(): Disabling EP%d\n",EpNum);
					if (DevInfo[DevNo].UsbDevice.UsbHwDisableEp)
						DevInfo[DevNo].UsbDevice.UsbHwDisableEp(DevNo,EpNum,EpDir,EpType);
					EpIndex = GetEndPointIndex (DevNo, EpNum, EpDir);
					if (EpIndex == 0xFF)
					{
						TCRIT ("UsbUnCfgDevice(): EndPoint Index could not be obtained for dev %d ep %d EpDir %d\n", DevNo, EpNum, EpDir);
						continue; 
					}
					DevInfo[DevNo].EndPoint[EpIndex].Enabled 	= 0;		
				}
			}
		}			
	}	
	
	return 0;
}


/******************************************************************************/
/**************** Functions Called from USB Hardware modules ******************/
/******************************************************************************/
static
void
UsbRestoreInitState(uint8 DevNo)
{

	uint8 EpIndex, DevNumEndPoints; 
	uint8 IfNum, DevInterfaces;	
	REM_HANDLER	DisconnectHandler;
	uint8 bNumConfigurations, CfgIndex;
	

	/* UnConfigure Device and Set Address to 0 */	
	UsbUnCfgDevice(DevNo);	
	UsbSetAddr(DevNo,0,1); 	
	DevInfo[DevNo].InRequest.DataLen = 0;
	DevInfo[DevNo].InRequest.wLength = 0;

	DevInfo[DevNo].OutRequest.DataLen = 0;
	DevInfo[DevNo].OutRequest.wLength = 0;
		
	/* Get the Number of EndPoints */ 	/* +1 is for Endpoint 0 */
	DevNumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;	

	/* Set TxReady to 1 for all endpoints */		
	for (EpIndex=0;EpIndex<DevNumEndPoints;EpIndex++)
	{
		DevInfo[DevNo].EndPoint[EpIndex].TxReady = 1;	
		DevInfo[DevNo].EndPoint[EpIndex].TxData = NULL;	
	}

	bNumConfigurations = GetNumConfigurations (DevNo);

	for (CfgIndex = 1; CfgIndex <= bNumConfigurations; CfgIndex++)
	{
		/* Get Number of Interfaces */
		DevInterfaces = GetNumInterfaces(DevNo, CfgIndex);		
			
		/* Restore State of the Interfaces to Init State */	
		for (IfNum=0;IfNum <DevInterfaces;IfNum++)
		{
			DisconnectHandler=GetInterfaceDisconnectHandler(DevNo,IfNum);
			if (DisconnectHandler)
				(*DisconnectHandler)(DevNo,IfNum);
		}			
	}	
    UsbConfigureFS(DevNo);

	return;
}


void
UsbBusReset(uint8 DevNo)
{

	uint8 bNumConfigurations, CfgIndex;
	
	printk("UsbBusReset(): ******** USB Bus RESET Detected for Device 0x%x ********\n",DevNo);

	if (DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions)
			 DevInfo[DevNo].UsbCoreDev.DevUsbMiscActions (DevNo, 0, ACTION_USB_BUS_RESET_OCCURRED);

	DevInfo[DevNo].Suspend = 0;
	UsbRestoreInitState(DevNo);

	{
		/* Note: Our process threads sleep and later wake up when we have ack from host.
		  a bus reset can come before the Ack , and the sleeping thread is unaware of bus reset.
		  This can result in failure of  new request from host
		  So code here make sure to wake up this thread during busreset*/
		
		uint8 NumIfaces;
		uint8 IfaceIndex;
        uint8 EpNum;
		uint8 TotalEndPoints;
		uint8 NumEndpoints;
		USB_ENDPOINT_DESC* UsbEndPointDesc;
        ENDPOINT_INFO* EpInfo;
		uint8 EpIndex;
		uint8 Direction;

        DevInfo[DevNo].CfgIndex = 0; 	//This will disable write to the TxFifo. 

		TotalEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;
		bNumConfigurations = GetNumConfigurations (DevNo);
		for (CfgIndex = 1; CfgIndex <= bNumConfigurations; CfgIndex++)
		{
			NumIfaces = GetNumInterfaces(DevNo, CfgIndex);
			for (IfaceIndex = 0; IfaceIndex < NumIfaces; IfaceIndex++)
			{
				NumEndpoints = GetNumEndpoints(DevNo, CfgIndex,IfaceIndex);
				for (EpNum = 1; ((EpNum <= TotalEndPoints) && NumEndpoints); EpNum++)
				{
					for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
					{
						UsbEndPointDesc = GetEndpointDesc (DevNo, CfgIndex, IfaceIndex, EpNum, Direction);
						if (UsbEndPointDesc != NULL)
						{
							NumEndpoints -= 1;
							/* Get Endpoint Info */
							EpIndex = GetEndPointIndex (DevNo, EpNum, Direction);
							if (EpIndex == 0xFF)
							{
								TCRIT ("UsbBusReset(): EndPoint Index could not be obtained for dev %d ep %d EpDir %d\n", DevNo, EpNum, Direction);
								continue; 
							}
							EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);	
							//Wake up sleeping process	
							Usb_OsWakeupOnTimeout(&(EpInfo->TxReadySleep));
						}
					}
				}
			}
		}

    }
	return;	
}


/* Should be called when the device is disconnected from the host  */ 
void
UsbBusDisconnect(uint8 DevNo)
{
	printk("UsbBusDisconnect(): Called for Device 0x%x \n",DevNo); 
	UsbRestoreInitState(DevNo);	
	return;	
}




void
UsbBusSuspend(uint8 DevNo)
{
	uint8 NumIfaces;
	uint8 IfaceIndex;
	uint8 EpNum;
	uint8 NumEndpoints;
	uint8 TotalEndPoints;
	USB_ENDPOINT_DESC* UsbEndPointDesc;
	ENDPOINT_INFO *EpInfo;
	uint8 bNumConfigurations, CfgIndex;
	uint8 EpIndex;
	uint8 Direction;
	
	printk("UsbBusSuspend(): Called for Device 0x%x\n",DevNo);
	/* Removed CfgIndex = 0 on actual Suspend/Resume CfgIndex should be preserved */
//	DevInfo[DevNo].CfgIndex = 0; 	//This will disable write to the TxFifo. 
					//This fix the entering into BIOS problem when del key pressed continually
	DevInfo[DevNo].Suspend = 1;

//The following code is added to fix the re-insert usb cable problem while the
//USB Device is in use
//Wake up any sleeping process after the cable disconnected.
	TotalEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints;
	bNumConfigurations = GetNumConfigurations (DevNo);
	for (CfgIndex = 1; CfgIndex <= bNumConfigurations; CfgIndex++)
	{
		NumIfaces = GetNumInterfaces(DevNo, CfgIndex);
		for (IfaceIndex = 0; IfaceIndex < NumIfaces; IfaceIndex++)
		{
			NumEndpoints = GetNumEndpoints(DevNo, CfgIndex, IfaceIndex);
			for (EpNum = 1; ((EpNum <= TotalEndPoints) && NumEndpoints); EpNum++)
			{
				for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
				{
					UsbEndPointDesc = GetEndpointDesc (DevNo, CfgIndex, IfaceIndex, EpNum, Direction);
					if (UsbEndPointDesc != NULL)
					{
						NumEndpoints -= 1;
						/* Get Endpoint Info */
						EpIndex = GetEndPointIndex (DevNo, EpNum, Direction);
						if (EpIndex == 0xFF)
						{
							TCRIT ("UsbBusSuspend(): EndPoint Index could not be obtained for dev %d ep %d EpDir %d\n", DevNo, EpNum, Direction);
							continue; 
						}
						EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);	
						//Wake up sleeping process	
						Usb_OsWakeupOnTimeout(&(EpInfo->TxReadySleep));
					}
				}
				
			}
		}
	}	
}

void
UsbBusResume(uint8 DevNo)
{
	printk("USBBusResume(): Called for Device 0x%x\n",DevNo);
	DevInfo[DevNo].Suspend = 0;
}



void
UsbHaltHandler(uint8 DevNo, uint8 EpNum, uint8 EpDir)
{
	uint8 IfNum;
	HALT_HANDLER Haltfn;
	uint16 CfgIndex;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"USBHaltHandler(): Called for Device %d EP %d\n",DevNo,EpNum);

	
	/* If Endpoint 0 , return */	
	if (EpNum == 0)
		return;

	
	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return;
	
	/* Get the corresponding Interface for the Epnum */
	IfNum = GetInterface(DevNo, CfgIndex, EpNum, EpDir);
	if (IfNum == 0xFF)		/* No associated Interface */
	{
		TCRIT("WARNING: UsbHaltHandler(): No Interface assocaited with ep#%d\n",EpNum);
		return;
	}

	/* Get the Halt Handler for this interface and call it */	
	Haltfn = GetInterfaceHaltHandler(DevNo,IfNum);
	if (Haltfn)
		(*Haltfn)(DevNo,IfNum,EpNum);
		
	return;	
}


void
UsbUnHaltHandler(uint8 DevNo, uint8 EpNum, uint8 EpDir)
{
	uint8 IfNum;
	UNHALT_HANDLER UnHaltfn;
	uint16 CfgIndex;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"USBUnHaltHandler(): Called for Device %d EP %d\n",DevNo,EpNum);

	
	/* If Endpoint 0 , return */	
	if (EpNum == 0)
		return;
	
	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return;

	/* Get the corresponding Interface for the Epnum */
	IfNum = GetInterface(DevNo,CfgIndex,EpNum, EpDir);
	if (IfNum == 0xFF)		/* No associated Interface */
	{
		TCRIT("WARNING: UsbUnHaltHandler(): No Interface assocaited with ep#%d\n",EpNum);
		return;
	}

	/* Get the UnHalt Handler for this interface and call it */	
	UnHaltfn = GetInterfaceUnHaltHandler(DevNo,IfNum);
	if (UnHaltfn)
		(*UnHaltfn)(DevNo,IfNum,EpNum);
		
	return;	

}


void
UsbRxHandler(uint8 DevNo,uint8 ep)
{

	TDBG_FLAGGED(usbe, DEBUG_CORE,"USBRxHandler(): Called for EP%d\n",ep); 

	/* Ep 0 is handled seperately */
	CallInterfaceRxHandler(DevNo,ep);			
	return;
}

void
UsbRxHandler0(uint8 DevNo, uint8 *RxData0, uint32 DataLen, uint8 SetupPkt)
{
	int count;
	
	DEVICE_REQUEST *Request;
	

	/* If SETUP Packet is Received */
	if (SetupPkt && (((DEVICE_REQUEST *)RxData0)->bmRequestType & REQ_DEVICE2HOST))
	{
		memcpy(&DevInfo[DevNo].InRequest, RxData0, 8);
		DevInfo[DevNo].InRequest.wLengthOrig = DevInfo[DevNo].InRequest.wLength;
		DevInfo[DevNo].InRequest.Command = "Unknown IN Request";
		DevInfo[DevNo].InRequest.Status = 1;
		ProcessSetup(DevNo,&DevInfo[DevNo].InRequest);

		if(DevInfo[DevNo].InRequest.Status != 0)
		{
			UsbCompleteRequest(DevNo, &DevInfo[DevNo].InRequest);
			return;
		}
		DevInfo[DevNo].InRequest.DataLen = 0;
		
		SendPendingEP0Data(DevNo);
		return;
	}
	
	// Its either Out-Setup or Out-Data
	Request = &DevInfo[DevNo].OutRequest;
	if(SetupPkt)
	{
		memcpy(Request, RxData0, 8);
		Request->Command = "Unknown OUT Request";
		Request->Status = 1;
		
		TDBG_FLAGGED(usbe, DEBUG_CORE,"Setup Packet Received \n");
		TDBG_FLAGGED(usbe, DEBUG_CORE,"       bmRequestType = 0x%x\n",Request->bmRequestType);
		TDBG_FLAGGED(usbe, DEBUG_CORE,"            bRequest = 0x%x\n",Request->bRequest);
		TDBG_FLAGGED(usbe, DEBUG_CORE,"            wValue   = 0x%x\n",Request->wValue);
		TDBG_FLAGGED(usbe, DEBUG_CORE,"            wIndex   = 0x%x\n",Request->wIndex);
		TDBG_FLAGGED(usbe, DEBUG_CORE,"            wLength  = 0x%x\n",Request->wLength);
		
		if(Request->wLength == 0) // process request immediately if there is no data
		{
			ProcessSetup(DevNo,Request);
			UsbCompleteRequest(DevNo, Request);
			return;
		}
		
		Request->DataLen = 0;
		return;
	}

	/* Non SETUP Packet (i.e) OUT Packet for EP0*/

	TDBG_FLAGGED(usbe, DEBUG_CORE,"UsbRxHandler0():EP0 Received OUT Packet of length %ld \n",DataLen);

	/* These Out Packets may be the data part of the Setup Requests*/
	/* So call the routine for devices which may expect such data  */
	/* Routines should check if expecting a data and if so, process*/
	if (DataLen == 0 || Request->Data == NULL || Request->wLength == 0)
		return;
	
	count = Request->wLength - Request->DataLen;
	if(DataLen <= count)
		count = DataLen;
	
	memcpy(Request->Data + Request->DataLen, RxData0, count);
	Request->DataLen += count;
	
	if(Request->DataLen >= Request->wLength) // data stage is complete
	{
		ProcessSetup(DevNo, Request);
		UsbCompleteRequest(DevNo, Request);
	}

	return;
}


int
UsbTxHandler(uint8 DevNo,uint8 ep)
{
	ENDPOINT_INFO *EpInfo;
	uint8 EpIndex;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"USBTxHandler(): Called for EP%d\n",ep); 

	EpIndex = GetEndPointIndex (DevNo, ep, DIR_IN);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbTxHandler(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return 1;
	}
	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);	


	
	/* Nothing to Send, (i.e) End of Transmission  */
	if (EpInfo->TxData == NULL)
	{
		TDBG_FLAGGED(usbe, DEBUG_CORE,"Tranmission complete for EP%d\n",ep);

		/* Set Transmission Complete */
		EpInfo->TxReady		= 1;

		/* Added for any hardware specific completion to be done */
		/* For devices (like US9090) which generate xmit interrupt 
		   when the buffer is empty, xmit intr is disabled by default.
		   Whenever a hardware write is called, the device should enable
		   the xmit intr, which calls Tx Complete till all the data is 
		   written. The xmit intr should be disabled once all data is
		   written. Since device does not know the xmit is completed,
 		   the core call this function to tell it that Xmit is completed
		 */
		if (DevInfo[DevNo].UsbDevice.UsbTxComplete != NULL)
			DevInfo[DevNo].UsbDevice.UsbTxComplete(DevNo,ep);


		/* Wakeup Sleeping Process. For Ep=0, nothing will sleep */
		if ((ep != 0) && (DevInfo [DevNo].BlockingSend))
			Usb_OsWakeupOnTimeout(&(EpInfo->TxReadySleep));

		/* Call Interface Specific Tx Handler */
		CallInterfaceTxHandler(DevNo,ep);	

		return 0;
	}		

	HandleTxData(DevNo,ep,EpInfo);
	return 1;
}



/******************************************************************************/
/*************************** Internal Local Functions  ************************/
/******************************************************************************/

static int SendPendingEP0Data(uint8 DevNo)
{
	int count;
	uint8 *data;
	int EP0MaxPacketSize  	= DevInfo[DevNo].EndPoint[0].MaxPacketSize;

	if(DevInfo[DevNo].InRequest.wLength == 0)
	{
		TDBG_FLAGGED(usbe, DEBUG_CORE,"SendPendingEP0Data(): DevInfo[%d].InRequest.wLength is 0\n", DevNo);
		return 1;
	}
	
	if(DevInfo[DevNo].InRequest.DataLen >= DevInfo[DevNo].InRequest.wLength)
	{
		UsbCompleteRequest(DevNo,&DevInfo[DevNo].InRequest);
		return 1;
	}
	
	count = DevInfo[DevNo].InRequest.wLength - DevInfo[DevNo].InRequest.DataLen;
	if(count > EP0MaxPacketSize)
		count = EP0MaxPacketSize;
	
	data = DevInfo[DevNo].InRequest.Data + DevInfo[DevNo].InRequest.DataLen;
	DevInfo[DevNo].InRequest.DataLen += count;
	
	if(DevInfo[DevNo].InRequest.DataLen >= DevInfo[DevNo].InRequest.wLength && 
	   count == EP0MaxPacketSize && DevInfo[DevNo].InRequest.wLengthOrig > DevInfo[DevNo].InRequest.wLength)
	{
		UsbWriteDataNonBlock(DevNo, 0, data, count, ZERO_LEN_PKT);
	}
	else
	{
		UsbWriteDataNonBlock(DevNo, 0, data, count, NO_ZERO_LEN_PKT);
	}
	return 0;
}


static
void
CallInterfaceTxHandler(uint8 DevNo, uint8 ep)
{
	uint8 IfNum;
	EP_TX_HANDLER IntTxHandler;	
	uint16 CfgIndex;
	
	TDBG_FLAGGED(usbe, DEBUG_CORE,"USBTxHandler(): End of Transmission Received for EP%d\n",ep);

	/* If Endpoint 0 , Check if Defer Command Completion to be done  */	
	if (ep == 0)
	{
		SendPendingEP0Data(DevNo);
		return;
	}

	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return;

	/* Get the associated Interface */
	IfNum = GetInterface(DevNo,CfgIndex,ep, DIR_IN);
	if (IfNum == 0xFF)		/* No associated Interface */
	{
		TWARN("WARNING: CallInterfaceTxHandler(): No Interface assocaited with ep#%d\n",ep);
		return;
	}
	
	/* Call Interface Tx Completion routine */
	IntTxHandler = GetInterfaceTxHandler(DevNo,IfNum);
	if (IntTxHandler) 
			(*IntTxHandler)(DevNo,IfNum,ep);
			
	return;


}

static
void
CallInterfaceRxHandler(uint8 DevNo, uint8 ep)
{
	uint8 IfNum;
	EP_RX_HANDLER IntRxHandler;
	uint16 CfgIndex;
	
	/* If Endpoint 0 , return */	
	if (ep == 0)
		return;

	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return;

	/* Get the associated Interface */
	IfNum = GetInterface(DevNo,CfgIndex,ep,DIR_OUT);
	if (IfNum == 0xFF)		/* No associated Interface */
	{
		TWARN("WARNING: CallInterfaceRxHandler(): No Interface assocaited with ep#%d\n",ep);
		return;
	}
	
	/* Call Interface Rx Completion routine */
	IntRxHandler = GetInterfaceRxHandler(DevNo,IfNum);
	if (IntRxHandler) 
			(*IntRxHandler)(DevNo,IfNum,ep);
			
	return;
}



void
HandleTxData(uint8 DevNo, uint8 ep, ENDPOINT_INFO *EpInfo)
{
	
	uint32 Len;
	uint8 *Buffer;
	uint32 MaxSize;

	MaxSize = EpInfo->MaxPacketSize;
	if ((DevInfo[DevNo].UsbDevice.EpMultipleDMASupport == 1) && ep)
	{
		MaxSize = DevInfo[DevNo].UsbDevice.EpMultipleDMASize;
	}

	/*************************MAX PKT SIZE PACKET ***********************/
	/* If Data Remaining >= Maxpacketsize , send MaxPktSize chunk */
	if ((EpInfo->TxDataLen - EpInfo->TxDataPos) >= MaxSize)
	{
		
		/* Buffer and Len to be transmitted */
		Buffer = &EpInfo->TxData[EpInfo->TxDataPos];
		Len 	= MaxSize;
		/* Increment Buffer Pointer */
		EpInfo->TxDataPos +=Len;

		/* Transferred All Bytes. Check if a Zero length packet to be Sent*/
		if (EpInfo->TxDataLen == EpInfo->TxDataPos)
		{
			/* If no zero len packet to be sent, EOT is reached */
			if (!EpInfo->TxZeroLenPkt)	
				EpInfo->TxData = NULL;		/* Set End of Transmission */		
		}


		if (EpInfo->TxData == NULL)
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"HandleTxData():EOT:Full Pkt Len = (%ld) for EP%d\n",Len,ep);
		}
		else
		{
			TDBG_FLAGGED(usbe, DEBUG_CORE,"HandleTxData(): Full Pkt Len = (%ld) for EP%d\n",Len,ep);
		}

		DevInfo[DevNo].UsbDevice.UsbHwWrite(DevNo,ep,Buffer,Len);

		return;
	}

	/*************************PARTIAL SIZE PACKET ***********************/
	/* If Remaing Length is < than MaxPacket Size , Send the remaining Bytes */
	if (EpInfo->TxDataLen != EpInfo->TxDataPos)
	{
		/* Buffer and Len to be transmitted */
		Buffer = &EpInfo->TxData[EpInfo->TxDataPos];
		Len 	= EpInfo->TxDataLen - EpInfo->TxDataPos;

		/* Increment Buffer Pointer */
		EpInfo->TxDataPos +=Len;

		/* EOT is reached because of partial length packet */
		EpInfo->TxData = NULL;		/* Set End of Transmission */		

		TDBG_FLAGGED(usbe, DEBUG_CORE,"HandleTxData():EOT:Partial Pkt Len = (%ld) for EP%d\n",Len,ep);

		DevInfo[DevNo].UsbDevice.UsbHwWrite(DevNo,ep,Buffer,Len);
		return;																
	}
	
	/************************** ZERO LEN PACKET ***********************/
	/* If RemainingLength is zero, and Zero Len packet can be sent */
	if ((EpInfo->TxDataLen == EpInfo->TxDataPos) && (EpInfo->TxZeroLenPkt))
	{				
		TDBG_FLAGGED(usbe, DEBUG_CORE,"HandleTxData():EOT:Sending Zero Len Packet for EP%d\n",ep);
		EpInfo->TxData = NULL;		/* Set End of Transmission */		
		DevInfo[DevNo].UsbDevice.UsbHwWrite(DevNo,ep,NULL,0);
		return;
	}

	TCRIT("FATAL: HandleTxData(): Control Should not be here\n");
	return;
}

static 
void 
EnableEndPoint(uint8 DevNo, uint8 EpNum, uint8 EpDir)
{
	ENDPOINT_INFO *EpInfo;
	uint8 EpType;
    USB_DEVICE_DESC  *DevDesc;
	uint16 CfgIndex = 1;	 
	USB_ENDPOINT_DESC *EndDesc; 
	uint8 EpIndex;

	EpIndex = GetEndPointIndex (DevNo, EpNum, EpDir);
	if (EpIndex == 0xFF)
	{
		TCRIT ("EnableEndPoint(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, EpNum);
		return;
	}
	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);	

	/* Set it to Enabled */
/*	EpInfo->Enabled 	  	= 1; */

	if (EpNum == 0)
	{
        DevDesc = GetDeviceDesc(DevNo);
        EpInfo->MaxPacketSize = DevDesc->bMaxPacketSize0;
		EpInfo->FifoSize		= (EpInfo->MaxPacketSize * 2);		
		EpInfo->Interface 	  	= 0xFF;		/* No Interface */			     
		EpType = CONTROL;
	}
	else
	{
        CfgIndex = DevInfo[DevNo].CfgIndex;         
		EpInfo->MaxPacketSize 	= GetMaxPacketSize(DevNo,CfgIndex,EpNum, EpDir);
		EpInfo->FifoSize		= (EpInfo->MaxPacketSize * 2);	
		EpInfo->Interface 	  	= GetInterface(DevNo,CfgIndex,EpNum,EpDir);
		EndDesc = GetEndpointDesc(DevNo,CfgIndex,EpInfo->Interface,EpNum, EpDir);
		if (EndDesc == NULL) return;
		EpInfo->Dir				= (EndDesc->bEndpointAddress & 0x80)?DIR_IN: DIR_OUT;
	}

	TDBG_FLAGGED(usbe, DEBUG_CORE,"AllocBuffer for Ep%d: MaxPkt %d Interface %d  Dir %d Type %d\n",
		EpNum,EpInfo->MaxPacketSize,EpInfo->Interface,EpInfo->Dir, EpType);

	EpType = GetEndPointType(DevNo,CfgIndex,EpNum,EpDir);
	DevInfo[DevNo].UsbDevice.UsbHwAllocBuffer(DevNo,EpNum,
			DevInfo[DevNo].EndPoint[EpIndex].FifoSize,
			DevInfo[DevNo].EndPoint[EpIndex].MaxPacketSize,
			DevInfo[DevNo].EndPoint[EpIndex].Dir,EpType);
	return;													
}



 
void 
InitializeEndPoint(uint8 DevNo, uint8 EpNum, uint8 EpDir, uint8 EpIndex)
{
	ENDPOINT_INFO *EpInfo;

	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);	

	EpInfo->EpNum   	= EpNum;	
	EpInfo->TxData 		= NULL;	
	EpInfo->TxDataLen 	= 0;
	EpInfo->TxDataPos 	= 0;
	EpInfo->TxZeroLenPkt= 0;
	EpInfo->TxReady	    = 1;	
	EpInfo->RxDataLen	= 0; 			
	EpInfo->Dir			= EpDir;		
	EpInfo->RxData		= (uint8 *) kmalloc(RX_BUFFER_SIZE, GFP_ATOMIC);


	EpInfo->Enabled		= 0;		
	Usb_OsInitSleepStruct(&(EpInfo->TxReadySleep));

	/* Enable Endpoint 0. Other endpoints will be enabled during config */
	if (EpNum == 0)
		EnableEndPoint(DevNo,EpNum,EpDir);

	return;
}

int
GetNextFreeEndpointIndex (uint8 DevNo)
{
	uint8 EpIndex, NumEndPoints;
	ENDPOINT_INFO* EpInfo;
	
	/* Get the Number of EndPoints */	/* +1 is for Endpoint 0 */
	NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;
	for (EpIndex = 0; EpIndex < NumEndPoints; EpIndex++)
	{
		EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);
		if (EpInfo->EpUsed == 0)
		{
			EpInfo->EpUsed = 1;
			return EpIndex;
		}
	}
	return 0xFF;
}

int
GetEndPointIndex (uint8 DevNo, uint8 EpNum, uint8 EpDir)
{
	uint8 EpIndex, NumEndPoints;
	ENDPOINT_INFO* EpInfo;
	
	/* Get the Number of EndPoints */	/* +1 is for Endpoint 0 */
	NumEndPoints = DevInfo[DevNo].UsbDevice.NumEndpoints + 2;
	for (EpIndex = 0; EpIndex < NumEndPoints; EpIndex++)
	{
		EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);
		if ((EpInfo->EpUsed) && (EpInfo->Dir == EpDir) && (EpInfo->EpNum == EpNum))
			return EpIndex;
		if ((EpInfo->EpUsed) && (EpInfo->Dir == CONFIGURABLE) && (EpInfo->EpNum == EpNum))
			return EpIndex;
	}
	printk ("GetEndPointIndex returns 0xFF. This should not happen\n");
	return 0xFF;
}

static
void
ProcessInterfaceHandler(uint8 DevNo)
{
	uint8 DevInterfaces,ifnum;
	PROC_HANDLER ProcHandler;
	uint8 CfgIndex;

	CfgIndex = DevInfo[DevNo].CfgIndex;
	if (CfgIndex == 0)
		return;

	/* Get Number of Interfaces */
	DevInterfaces = GetNumInterfaces(DevNo, CfgIndex);		
	
	/* For each Interface, if Flag is raised, call the handler */		
	for (ifnum=0;ifnum<DevInterfaces;ifnum++)
	{
		if (GetInterfaceFlag(DevNo,ifnum))
		{
			ClearInterfaceFlag(DevNo,ifnum);
			ProcHandler = GetInterfaceProcHandler(DevNo,ifnum);
			if (ProcHandler)
				(*ProcHandler)(DevNo,ifnum);
		}				
	}		
	
	return;
}

void
CallInterfaceHandlers(void)
{
	uint8 DevNo;
	
	for (DevNo=0;DevNo<MAX_USB_HW;DevNo++)
	{
		if (DevInfo[DevNo].Valid)
			ProcessInterfaceHandler(DevNo);
	}
	return;
}	

void
UsbCompleteRequest(uint8 DevNo,DEVICE_REQUEST *Req)
{

	if (Req->Status == 1)
		TDBG_FLAGGED(usbe, DEBUG_CORE,"ReturnRequestError(): For %s Device 0x%x\n",Req->Command, DevNo); 


	/* Call Hardware specific Request Completion */
	if (DevInfo[DevNo].UsbDevice.UsbHwCompleteRequest)
	{
		DevInfo[DevNo].UsbDevice.UsbHwCompleteRequest(DevNo,Req->Status,Req);
		return;
	}

	/* Proceess Standard Completion Request */

	/* If Error , Send Stall */
	if (Req->Status == 1)
	{
		UsbStallEp(DevNo,0, DIR_IN);
		UsbStallEp(DevNo,0, DIR_OUT);
		return;
	}

	/* No Error. Handle Status Stage of Control Transfer*/
	/* For Control Writes and No-Data Control (Page 165 of USB 1.1)
           the host will issue a IN and we will send a Zero len Packet */
	if ((Req->wLength == 0) || (!(Req->bmRequestType & REQ_DEVICE2HOST)))
		UsbWriteData(DevNo,0,0,0,ZERO_LEN_PKT); 

	return;		
}

void
UsbEnableAllEndPoints(uint8 DevNo)
{
	uint8 EpIndex;

	for (EpIndex=0;EpIndex<DevInfo[DevNo].UsbDevice.NumEndpoints;EpIndex++)
	{
		EnableEndPoint(DevNo,DevEpInfo[DevNo].ep_config[EpIndex].ep_num,
								DevEpInfo[DevNo].ep_config[EpIndex].ep_dir >> 7);
	}
}


void
UsbCoreDumpData(char *Title, uint8 ep, uint8 *Data, uint32 Len)
{
	int i;

	if (TDBG_IS_DBGFLAG_SET(usbe, DEBUG_DUMP_DATA))
	{
		printk("------- %s --------%ld Bytes for ep %d\n",Title,Len,ep);
		for (i=0;i<Len;i++)
			printk("0x%02X ",Data[i]);
		printk("\n");
	}

}

uint16
GetEndPointBufSize (uint8 DevNo, uint8 EpNum, uint8 EpDir)
{
	int i;

	for (i = 0; i < DevEpInfo[DevNo].num_eps; i++)
	{
		if (EpNum != DevEpInfo[DevNo].ep_config[i].ep_num) continue;
		if ((EpDir != DevEpInfo[DevNo].ep_config[i].ep_dir) &&
			 (DevEpInfo[DevNo].ep_config[i].ep_dir != CONFIGURABLE)) continue;
		return DevEpInfo[DevNo].ep_config[i].ep_size;
	}

	return 0;
}


void
UsbConfigureHS(uint8 DeviceNo)
{
	USB_DEVICE_DESC  *DevDesc;
	USB_ENDPOINT_DESC *EndDesc;	
	ENDPOINT_INFO *EpInfo;
	uint8 bNumEndpoints,bNumInterfaces;
	uint8 IfaceNum;
	uint8 EpType;
	uint8 EpDir;
	uint8 EpNum;
	uint8 TotalEndPoints;
	uint16 EpSize;
	uint8 bNumConfigs;
	uint8 CfgIndex;
	uint8 Direction;
	 
	bNumConfigs = GetNumConfigurations (DeviceNo);

	printk("UsbConfigureHS(): USB Device %d is running in High Speed\n",DeviceNo);

	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DeviceNo].EndPoint[0]);	
	EpInfo->MaxPacketSize  	= DevInfo[DeviceNo].UsbDevice.EP0Size;
	
	DevDesc = GetDeviceDesc(DeviceNo);
	if (DevDesc == NULL) 
		return;
	DevDesc->bcdUSBL 			= 0x00;
	DevDesc->bcdUSBH 			= 0x02;	/* USB 2.0 */
	DevDesc->bMaxPacketSize0 	= DevInfo[DeviceNo].UsbDevice.EP0Size;

	TotalEndPoints = DevInfo[DeviceNo].UsbDevice.NumEndpoints;
	for (CfgIndex = 1; CfgIndex <= bNumConfigs; CfgIndex++)
	{
		bNumInterfaces = GetNumInterfaces(DeviceNo,CfgIndex);
		for (IfaceNum=0;IfaceNum<bNumInterfaces;IfaceNum++)
		{
			bNumEndpoints = GetNumEndpoints(DeviceNo, CfgIndex, IfaceNum);
			for (EpNum = 1; ((EpNum <= TotalEndPoints) && bNumEndpoints) ; EpNum++)
			{
				for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
				{
					EndDesc = GetEndpointDesc (DeviceNo, CfgIndex, IfaceNum, EpNum, Direction);
					if (EndDesc == NULL) continue;
					bNumEndpoints -= 1;
					EpType = (EndDesc->bmAttributes) & 0x03;
					EpDir  = (EndDesc->bEndpointAddress & IN)?DIR_IN:DIR_OUT;
					EpSize = GetEndPointBufSize (DeviceNo, EpNum, EpDir << 7);
					switch (EpType)
					{
					case 0x03: //EPTYPE_INTERRUPT
						if (EpSize > 1024)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureHS(): Interrupt EndPoint Size can not be more than 1024. So making it 1024\n");
							EpSize = 1024;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x02: //EPTYPE_BULK
						if (EpSize > 512)
						{
		                    TDBG_FLAGGED(usbe, DEBUG_CORE,
									  "UsbConfigureHS(): Bulk Endpoint Size can not be more than 512. So making it 512\n");
							EpSize = 512;
						}
						/* Endpoint sizes other than 64 and 512 will create issues */
						if ((EpSize > 64) && (EpSize < 512))
						{
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x01: //EPTYPE_ISO
						if (EpSize > 1024)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureHS(): Isochronous EndPoint Size can not be more than 1024. So making it 1024\n");
							EpSize = 1024;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x00: //CONTROL
						if (EpSize > 64)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureHS(): Control EndPoint Size can not be more than 64. So making it 64\n");
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					default:
					       break;	
					}

				}
			}

			
		}
	}
	return;
}

void
UsbConfigureFS(uint8 DeviceNo)
{
	USB_DEVICE_DESC  *DevDesc;
	USB_ENDPOINT_DESC *EndDesc;
	ENDPOINT_INFO *EpInfo;
	uint8 bNumEndpoints,bNumInterfaces;
	uint8 IfaceNum;
	uint8 EpType;
	uint8 EpDir;
	uint16 EpSize;
	uint8 EpNum;
	uint8 TotalEndPoints;
	uint8 bNumConfigs;
	uint8 CfgIndex;
	uint8 Direction;
	
	bNumConfigs = GetNumConfigurations (DeviceNo);

	printk("UsbConfigureFS(): USB Device %d is running in Full Speed\n",DeviceNo);

	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DeviceNo].EndPoint[0]);
    EpInfo->MaxPacketSize  	= DevInfo[DeviceNo].UsbDevice.EP0Size;

	DevDesc = GetDeviceDesc(DeviceNo);
	if (DevDesc == NULL)
		return;
	DevDesc->bcdUSBL 			= 0x10;
	DevDesc->bcdUSBH 			= 0x01;	/* USB 1.1 */
    DevDesc->bMaxPacketSize0 = DevInfo[DeviceNo].UsbDevice.EP0Size;

	TotalEndPoints = DevInfo[DeviceNo].UsbDevice.NumEndpoints;
	for (CfgIndex = 1; CfgIndex <= bNumConfigs; CfgIndex++)
	{
		bNumInterfaces = GetNumInterfaces(DeviceNo,CfgIndex);
		for (IfaceNum=0;IfaceNum<bNumInterfaces;IfaceNum++)
		{
			bNumEndpoints = GetNumEndpoints(DeviceNo, CfgIndex, IfaceNum);
			for (EpNum = 1; ((EpNum <= TotalEndPoints) && bNumEndpoints); EpNum++)
			{
				for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
				{
					EndDesc = GetEndpointDesc(DeviceNo, CfgIndex,IfaceNum, EpNum, Direction);
					if (EndDesc == NULL)
					{
						continue;
					}
					bNumEndpoints -= 1;
					EpType = (EndDesc->bmAttributes) & 0x03;
					EpDir  = (EndDesc->bEndpointAddress & IN)?DIR_IN:DIR_OUT;
					EpSize = GetEndPointBufSize (DeviceNo, EpNum, EpDir << 7);
					switch (EpType)
					{
					case 0x03: //EPTYPE_INTERRUPT
						if (EpSize > 64)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureFS(): Interrupt EndPoint Size can not be more than 64. So making it 64\n");
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x02: //EPTYPE_BULK
						if (EpSize > 64)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureFS(): Bulk EndPoint Size can not be more than 64. So making it 64\n");
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x01: //EPTYPE_ISO
						if (EpSize > 1023)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureFS(): Bulk EndPoint Size can not be more than 1023. So making it 1023\n");
							EpSize = 1023;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x00: //CONTROL
						if (EpSize > 64)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureFS(): Control EndPoint Size can not be more than 64. So making it 64\n");
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					default:
					       break;
					}
				}
			}
		}
	}
	return;
}

void
UsbConfigureLS(uint8 DeviceNo)
{
	USB_DEVICE_DESC  *DevDesc;
	USB_ENDPOINT_DESC *EndDesc;
	ENDPOINT_INFO *EpInfo;
	uint8 bNumEndpoints,bNumInterfaces;
	uint8 IfaceNum;
	uint8 EpType;
	uint8 EpDir;
	uint16 EpSize;
	uint8 TotalEndPoints;
	uint8 EpNum;
	uint8 bNumConfigs;
	uint8 CfgIndex;
	uint8 Direction;
	
	bNumConfigs = GetNumConfigurations (DeviceNo);

	printk("UsbConfigureLS(): USB Device %d is running in Low Speed\n",DeviceNo);

	/* Get Endpoint Info */
	EpInfo = &(DevInfo[DeviceNo].EndPoint[0]);
    EpInfo->MaxPacketSize  	= 8;

	DevDesc = GetDeviceDesc(DeviceNo);
	if (DevDesc == NULL)
		return;
	DevDesc->bcdUSBL 			= 0x10;
	DevDesc->bcdUSBH 			= 0x01;	/* USB 1.1 */
    DevDesc->bMaxPacketSize0 = 8;

	TotalEndPoints = DevInfo[DeviceNo].UsbDevice.NumEndpoints;
	for (CfgIndex = 1; CfgIndex <= bNumConfigs; CfgIndex++)
	{
		bNumInterfaces = GetNumInterfaces(DeviceNo,CfgIndex);
		for (IfaceNum=0;IfaceNum<bNumInterfaces;IfaceNum++)
		{
			bNumEndpoints = GetNumEndpoints(DeviceNo, CfgIndex, IfaceNum);
			for (EpNum = 1; ((EpNum <= TotalEndPoints) && bNumEndpoints); EpNum++)
			{
				for (Direction = DIR_OUT; Direction <= DIR_IN; Direction++)
				{
					EndDesc = GetEndpointDesc(DeviceNo, CfgIndex,IfaceNum, EpNum, Direction);
					if (EndDesc == NULL) continue;
					bNumEndpoints -= 1;
					EpType = (EndDesc->bmAttributes) & 0x03;
					EpDir  = (EndDesc->bEndpointAddress & IN)?DIR_IN:DIR_OUT;
					EpSize = GetEndPointBufSize (DeviceNo, EpNum, EpDir << 7);
					switch (EpType)
					{
					case 0x03: //EPTYPE_INTERRUPT
						if (EpSize > 8)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureLS(): Interrupt EndPoint Size can not be more than 8. So making it 8\n");
							EpSize = 8;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x02: //EPTYPE_BULK
						if (EpSize > 64)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureLS(): Bulk EndPoint Size can not be more than 64. So making it 64\n");
							EpSize = 64;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x01: //EPTYPE_ISO
						if (EpSize > 1023)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureLS(): Isochronous EndPoint Size can not be more than 1023. So making it 1023\n");
								EpSize = 1023;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;
					case 0x00: //CONTROL
						if (EpSize > 8)
						{
							TDBG_FLAGGED(usbe, DEBUG_CORE, 
									 "UsbConfigureLS(): Control EndPoint Size can not be more than 8. So making it 8\n");
							EpSize = 8;
						}
						EndDesc->wMaxPacketSizeL 	=   LO_BYTE(EpSize);
						EndDesc->wMaxPacketSizeH	=   HI_BYTE(EpSize);
						break;

					default:
					       break;
					}
				}
			}
		}
	}
	return;
}


int
UsbGetHubPortsStatus (uint8 DevNo, uint8 PortNum, uint32* Status)
{
	return DevInfo[DevNo].UsbDevice.UsbHwGetHubPortsStatus (DevNo, PortNum, Status);
}

void
UsbSetHubPortsStatus (uint8 DevNo, uint8 PortNum, uint32 Mask, int SetClear)
{
	return DevInfo[DevNo].UsbDevice.UsbHwSetHubPortsStatus (DevNo, PortNum, Mask, SetClear);
}

void
UsbHUBEnableDevice(uint8 DevNo, uint8 IfNum)
{
        DevInfo[DevNo].UsbDevice.UsbHwHubEnableDevice(IfNum, 0);
        return;
}

void
UsbHUBDisableDevice(uint8 DevNo, uint8 IfNum)
{
        DevInfo[DevNo].UsbDevice.UsbHwHubDisableDevice(IfNum,0);
        return;
}

void
UsbHUBPortBitmap(uint8 DevNo, uint8 IfNum)
{
        DevInfo[DevNo].UsbDevice.UsbHwHubPortBitmap(DevNo, IfNum);
        return;
}

void UsbHubChangeHubRegs (uint8 DevNo, uint8 bRequest, uint8 wValue, uint8 wIndex)
{
	DevInfo[DevNo].UsbDevice.UsbHwChangeHubRegs (DevNo, bRequest, wValue, wIndex);	
	return;
}

int
UsbGetHubSpeed (uint8 DevNo)
{
	return DevInfo[DevNo].UsbDevice.UsbHwGetHubSpeed (DevNo);
}

int
UsbIsHubDeviceConnected (uint8 DevNo, uint8 ifnum)
{
	return DevInfo[DevNo].UsbDevice.UsbHwIsHubDeviceConnected (DevNo,ifnum);
}

int
UsbIsHubDeviceEnabled (uint8 DevNo, uint8 ifnum)
{
	return DevInfo[DevNo].UsbDevice.UsbHwIsHubDeviceEnabled (DevNo,ifnum);
}

void
UsbClearHubDeviceEnableFlag (uint8 DevNo, uint8 ifnum)
{
	DevInfo[DevNo].UsbDevice.UsbHwClearHubDeviceEnableFlag (DevNo,ifnum);
}

void
UsbSetHubDeviceEnableFlag (uint8 DevNo, uint8 ifnum)
{
	DevInfo[DevNo].UsbDevice.UsbHwSetHubDeviceEnableFlag (DevNo, ifnum);
}


static 
uint8
CalculateModulo100(uint8 *Buff, int BuffLen)
{
	uint8 ChkSum = 0;
	int i;

	for (i=0;i<BuffLen;i++)
		ChkSum += Buff[i];
	return (-ChkSum);
}

void
FormiUsbHeader(IUSB_HEADER *Header, uint32 DataLen, uint8 DevNo, uint8 ifnum,
				uint8 DeviceType,uint8 Protocol)
{
	static uint32 SeqNo=1;

	memset(Header,0,sizeof(IUSB_HEADER));
	strncpy(Header->Signature,IUSB_SIG,8);
	Header->Major 		= 1;
	Header->Minor 		= 0;
	Header->HeaderLen 	= sizeof(IUSB_HEADER);
	Header->DataPktLen 	= usb_long(DataLen);
	Header->DeviceType 	= DeviceType;
	Header->Protocol   	= Protocol;
	Header->Direction	= TO_REMOTE;	
	Header->DeviceNo	= DevNo;
	Header->InterfaceNo	= ifnum;
	Header->SeqNo	   	= usb_long(SeqNo);
	SeqNo++;	
	if (SeqNo == 0xffffffff)
		SeqNo=1;
	Header->HeaderCheckSum = CalculateModulo100((uint8 *)Header,sizeof(IUSB_HEADER));
	return;
}


int
UsbWriteDataNonBlock(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt)
{
	ENDPOINT_INFO *EpInfo;
	uint8 EpIndex;

	EpIndex = GetEndPointIndex (DevNo, ep, DIR_IN);
	if (EpIndex == 0xFF)
	{
		TCRIT ("UsbWriteDataNonBlock(): EndPoint Index could not be obtained for dev %d ep %d\n", DevNo, ep);
		return 1;
	}
	/* Get Endpoint Information */
	EpInfo = &(DevInfo[DevNo].EndPoint[EpIndex]);
	if (EpInfo == NULL)
	{
		printk ("UsbWriteDataNonBlock(): EpInfo is NULL\n");
		return 1;
	}
	
	if (ep != 0)
	{
		/* Check if Endpoint is Enabled */
		if (!EpInfo->Enabled)
		{
			printk ("UsbWriteDataNonBlock(): Ep disabled\n");	
			return 1;
		}
		/* Can Write only a IN EndPoint */
		if (!EpInfo->Dir)
		{
			printk ("UsbWriteDataNonBlock(): Ep not IN endpoint\n");
			return 1;
		}
        /* Can Write only when the device is configured */
		if (DevInfo[DevNo].CfgIndex == 0)
		{
			printk ("UsbWriteDataNonBlock(): device not configured\n");	
			return 1;
		}
		/* Can not write if device suspended */
		if (DevInfo[DevNo].Suspend == 1)
		{
			printk ("UsbWriteDataNonBlock(): device suspended\n");	
			return 1;
		}
	}
	
	/* Load Endpoint with Buffer Details */
	if ((EpInfo->TxData != NULL)|| (EpInfo->TxReady != 1))
	{
		printk("UsbWriteDataNonBlock():Previous Write Pending for Dev %d Ep %d\n",DevNo,ep);
		printk("UsbWriteDataNonBlock():TxData=0x%lx DataLen=0x%lx DataPos=0x%lx Zero=%d Ready=%d\n",
		       (uint32)EpInfo->TxData,
		       EpInfo->TxDataLen,EpInfo->TxDataPos,
		       EpInfo->TxZeroLenPkt,EpInfo->TxReady);
	}
	
	EpInfo->TxData 		= Buffer;
	EpInfo->TxDataLen	= Len;
	EpInfo->TxDataPos	= 0;
	EpInfo->TxZeroLenPkt= ZeroLenPkt;
	EpInfo->TxReady		= 0;
	DevInfo [DevNo].BlockingSend = 0;
	/* Start Transfer Data */
	HandleTxData(DevNo,ep,EpInfo);
	
	return 0;
}


