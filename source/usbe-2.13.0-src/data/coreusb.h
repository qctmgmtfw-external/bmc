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

#ifndef CORE_USB_H
#define CORE_USB_H

#include <coreTypes.h>
#include "usb_hw.h"

/*****************************************************************************/
/******************************Data Structures *******************************/
/*****************************************************************************/

/* Interface Handler Routines */
typedef void (*EP_TX_HANDLER)	(uint8 DevNo, uint8 ifnum, uint8 epnum);
typedef void (*EP_RX_HANDLER)	(uint8 DevNo, uint8 ifnum, uint8 epnum);
typedef int  (*REQ_HANDLER)  	(uint8 DevNo, uint8 ifnum, DEVICE_REQUEST *DevRequest);
typedef void (*PROC_HANDLER) 	(uint8 DevNo, uint8 ifnum);
typedef void (*HALT_HANDLER) 	(uint8 DevNo, uint8 ifnum, uint8 epnum);
typedef void (*UNHALT_HANDLER) 	(uint8 DevNo, uint8 ifnum, uint8 epnum);
typedef void (*REM_HANDLER) 	(uint8 DevNo, uint8 ifnum);

/* Interface Information Structure */
typedef struct
{
	EP_TX_HANDLER 	TxHandler;
	EP_RX_HANDLER 	RxHandler;
	REQ_HANDLER 	ClassReqHandler;
	PROC_HANDLER 	ProcHandler;	
	HALT_HANDLER	HaltHandler;
	UNHALT_HANDLER	UnHaltHandler;	
	REM_HANDLER		DisconnectHandler;
	void *	 		InterfaceData;
	uint8	 		WakeupFlag;
	uint8			DataInEp;
	uint8			DataOutEp;	
	uint8			NonDataEp;
} INTERFACE_INFO;


/* Endpoint Information Structure */
typedef struct
{
	uint8		EpNum;
	uint8 		Enabled;
	uint8	 	Interface;		/* The Interface where it belongs */	
	uint16		MaxPacketSize;	//BALA changed from uint8 to uint16
	uint8		Dir;
	uint16		FifoSize;	
	uint8		*RxData;		/* Mem will be Allocated during Configuration */
	uint32		RxDataLen;
	uint8		*TxData;		/* TxData will point to the  caller's buffer */
	uint32		TxDataLen;
	uint32		TxDataPos;
	uint8		TxZeroLenPkt;	/* Should we send a 0 Len Pkt to notify EOP */
	uint8 		TxReady;
	Usb_OsSleepStruct	TxReadySleep;
	uint8		EpUsed;
} ENDPOINT_INFO;

/* USB Device Information */
typedef struct 
{
	char			Name[9];
	USB_HW 			UsbDevice;		/* Hardware Device Information */
	ENDPOINT_INFO 	*EndPoint;		/* EndPoints Informatiom */	
	uint8			Valid;			/* Valid Device.(i.e)i Initialized */
	uint8			CfgIndex;		/* Configuration Index Used */
	uint8			Suspend;		/* Device Suspended or Not  */
	uint8			ConnectState;		/* Device Connected or Not */
	void			*DevConf;		/* Device Internal Data - Not used by coreusb */		
	USB_DEV			UsbCoreDev;
	uint8			UsbDevType;
	uint8			RemoteWakeup;
	uint8			DevReqSpeeds;
	uint8			MaxLUN;
	DEVICE_REQUEST	InRequest;
	DEVICE_REQUEST	OutRequest;
	int				BlockingSend;
} USB_DEV_INFO;

typedef struct
{
	uint32 Valid;
	uint32 Key;
	uint32 Used;
	IUSB_DEVICE_INFO	iUSBDevInfo;

}IUSB_DEVICE_DETAILS_T;

#define MAX_IN_AND_OUT_ENDPOINTS	30

typedef struct
{
	ep_config_t	ep_config [MAX_IN_AND_OUT_ENDPOINTS];
	uint8		num_eps;
    int			epnumbuf [MAX_IN_AND_OUT_ENDPOINTS];
	
} __attribute__ ((packed)) USB_DEV_EP_INFO;

extern USB_DEV_INFO DevInfo[];
extern USB_DEV_EP_INFO DevEpInfo [];

#define TRANSMIT_TIMEOUT	45000	/* Transmission Timeout in msecs */	
#define TRANSMIT_TIMEOUT_HID	1500	/* Transmission Timeout in msecs */	
#define STALL_TIMEOUT		10000	/* Stall to Unstall Timeout in msecs */


/*****************************************************************************/
/*************************** Function Prototypes *****************************/
/*****************************************************************************/

/* Core Routines called by the USB hardware module*/
void UsbConfigureHS		(uint8 DevNo);
void UsbConfigureFS		(uint8 DevNo);
void UsbConfigureLS		(uint8 DevNo);
void UsbBusSuspend		(uint8 DevNo);	
void UsbBusResume		(uint8 DevNo);	
void UsbBusReset		(uint8 DevNo);
void UsbBusDisconnect	(uint8 DevNo);
void UsbHaltHandler		(uint8 DevNo,uint8 ep, uint8 EpDir);
void UsbUnHaltHandler	(uint8 DevNo,uint8 ep, uint8 EpDir);
int  UsbTxHandler		(uint8 DevNo,uint8 ep);
void UsbRxHandler		(uint8 DevNo,uint8 ep);
void UsbRxHandler0		(uint8 DevNo,uint8 *RxData, uint32 DataLen, uint8 SetupPkt); /* Endpoint 0 Rx*/


/* Core Functions called by setup and device modules and some by USB hardware module*/
void  UsbDeviceEnable	(uint8 DevNo);
void  UsbDeviceDisable	(uint8 DevNo);
irqreturn_t  UsbDeviceIntr		(uint8 DevNo,uint8 ep);
int   UsbWriteData  	(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, 
									uint8 ZeroLenPkt);
/*
 * int   UsbReadData   	(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 *Len);
 */
int   UsbUnCfgDevice	(uint8 DevNo);
int   UsbCfgDevice  	(uint8 DevNo, uint16 CfgIndex);
void  UsbSetAddr		(uint8 DevNo, uint8 Addr, uint8 Enable);
int	  UsbGetEpStatus	(uint8 DevNo, uint8 Ep, uint8 EpDir, uint8 *Enable, uint8 *Stall);
int   UsbStallEp		(uint8 DevNo, uint8 Ep, uint8 EpDir);
int   UsbUnStallEp		(uint8 DevNo, uint8 Ep, uint8 EpDir);
void  UsbSetRemoteWakeup(uint8 DevNo, uint8 Set);
uint8 UsbGetRemoteWakeup(uint8 DevNo);
void  UsbRemoteWakeup(uint8 DevNo);
uint8 UsbGetBusSuspendState(uint8 DevNo);
uint8 UsbGetCfgIndex	(uint8 DevNo);
uint8* UsbGetRxData		(uint8 DevNo,uint8 ep);
uint32 UsbGetRxDataLen	(uint8 DevNo,uint8 ep);
void  UsbSetRxDataLen   (uint8 DevNo,uint8 ep,uint32 Len);
void  UsbCompleteRequest(uint8 DevNo, DEVICE_REQUEST *Req);




/* Interface related functions */
void   *GetInterfaceData(uint8 DevNo, uint8 IfNum);
void  	SetInterfaceFlag(uint8 DevNo, uint8 IfNum);
void  	ClearInterfaceFlag(uint8 DevNo, uint8 IfNum);
uint8  	GetInterfaceFlag(uint8 DevNo, uint8 IfNum);
uint8   GetDataInEp(uint8 DevNo,uint8 ifnum);
uint8   GetDataOutEp(uint8 DevNo,uint8 ifnum);
uint8   GetNonDataEp(uint8 DevNo,uint8 ifnum);
REQ_HANDLER   GetInterfaceReqHandler(uint8 DevNo, uint8 IfNum);
EP_TX_HANDLER GetInterfaceTxHandler(uint8 DevNo, uint8 IfNum);
EP_RX_HANDLER GetInterfaceRxHandler(uint8 DevNo, uint8 IfNum);
PROC_HANDLER  GetInterfaceProcHandler(uint8 DevNo, uint8 IfNum);	
HALT_HANDLER  GetInterfaceHaltHandler(uint8 DevNo, uint8 IfNum);
UNHALT_HANDLER GetInterfaceUnHaltHandler(uint8 DevNo, uint8 IfNum);
REM_HANDLER   GetInterfaceDisconnectHandler(uint8 DevNo, uint8 IfNum);
void 	CallInterfaceHandlers(void);


/* Core Routine called from Startup to Initialize and cleanup */
int UsbCoreInit(UsbHwInit *DevInit);
void UsbCoreFini(UsbHwInit *DevInit);

/* Miscellaneous Routines related to device connect/reconnect/remove */
int UsbGlobalDisconnect(void);
int UsbGlobalReconnect(void);
int UsbDeviceDisconnect(uint8 DeviceNo);
int UsbDeviceReconnect(uint8 DeviceNo);
int UsbGetDeviceConnectState(uint8 DeviceNo);
int UsbRemoveDevice(uint8 DeviceNo);
int UsbAddDevice(uint8 DeviceNo,uint8 DeviceType);
int UsbRemoteWakeupHost(void);
void UsbEnableAllEndPoints(uint8 DeviceNo);
void HandleTxData(uint8 DevNo, uint8 ep, ENDPOINT_INFO *EpInfo);
int UsbWriteDataNonBlock(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt);
int getCurrentAddress( uint8* Buf ); 
int UsbWriteDataNonBlock(uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt);


uint16 GetEndPointBufSize (uint8 DevNo, uint8 EpNum, uint8 EpDir);
void InitializeEndPoint(uint8 DevNo, uint8 EpNum, uint8 EpDir, uint8 EpIndex);
int GetNextFreeEndpointIndex (uint8 DevNo);
int GetEndPointIndex (uint8 DevNo, uint8 EpNum, uint8 EpDir);
extern void UsbCoreDumpData(char *Title, uint8 ep, uint8 *Data, uint32 Len);
int UsbGetHubPortsStatus (uint8 DevNo, uint8 PortNum, uint32* Status);
void UsbSetHubPortsStatus (uint8 DevNo, uint8 PortNum, uint32 Mask, int SetClear);
void UsbHUBEnableDevice(uint8 DevNo, uint8 IfNum);
void UsbHUBDisableDevice(uint8 DevNo, uint8 IfNum);
void UsbHUBPortBitmap(uint8 DevNo, uint8 IfNum);
void UsbHubChangeHubRegs (uint8 DevNo, uint8 bRequest, uint8 wValue, uint8 wIndex);
int UsbGetHubSpeed (uint8 DevNo);
int UsbIsHubDeviceConnected (uint8 DevNo, uint8 ifnum);
int UsbIsHubDeviceEnabled (uint8 DevNo, uint8 ifnum);
void UsbClearHubDeviceEnableFlag (uint8 DevNo, uint8 ifnum);
void UsbSetHubDeviceEnableFlag (uint8 DevNo, uint8 ifnum);
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
int UsbUpstreamDisable_Device_all (void);
int UsbUpstreamEnable_Device_all (void);
int UsbUpstreamGet_Device_all_status(IUSB_IOCTL_DEVICE_STATUS *);
#endif
//USB 2.0 enabling/disabling support
int UsbUpstreamDisable_MediaDevice (void);
int UsbUpstreamEnable_MediaDevice (void);
void FormiUsbHeader(IUSB_HEADER *Header, uint32 DataLen, uint8 DevNo, uint8 ifnum,
				uint8 DeviceType,uint8 Protocol);
uint8 UsbGetCfgIndex(uint8 DevNo);


/**********************************************************************/
/******************* Miscellaneous Defines ****************************/
/**********************************************************************/

#define LO_BYTE(x)	(uint8)((x) & 0xFF) 
#define HI_BYTE(x)	(uint8)(((x)>>8) & 0xFF) 

/* Values for ZeroLenPkt */
#define ZERO_LEN_PKT	1
#define NO_ZERO_LEN_PKT	0

/* Maxiumum interfaces per devices */
#define MAX_INTERFACES		8	

#define REL_REQUIRED_MOUSE_DATA_SIZE 3  
#define REL_EFFECTIVE_MOUSE_DATA_SIZE 4

#define ABS_REQUIRED_MOUSE_DATA_SIZE 5
#define ABS_EFFECTIVE_MOUSE_DATA_SIZE 6

#endif /* CORE_USB_H */
