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

#ifndef __USB__CORE__H
#define __USB__CORE__H

#include <coreTypes.h>
#include "descriptors.h"


/* 
   USB core functions exposed to the other usb modules via this structure. This 
   structure will be returned after the USB hw/dev registration is done successfully.
 */
typedef struct 
{
	int 	(*CoreUsbGetDeviceConnectState)	(uint8 DevNo);
	void 	(*CoreUsbBusDisconnect)	(uint8 DevNo);
	void    (*CoreUsbBusSuspend) (uint8 DevNo);
	void    (*CoreUsbBusResume) (uint8 DevNo);
	void    (*CoreUsbBusReset) (uint8 DevNo);
	void    (*CoreUsbRxHandler0) (uint8 DevNo, uint8 *RxData0, uint32 DataLen, uint8 SetupPkt);
	int	(*CoreUsbTxHandler) (uint8 DevNo,uint8 ep);
	void	(*CoreUsbRxHandler) (uint8 DevNo,uint8 ep);
	void	(*CoreUsbSetRxDataLen) (uint8 DevNo,uint8 ep,uint32 Len);
	uint8*  (*CoreUsbGetRxData) (uint8 DevNo,uint8 ep);
	int	(*CoreUsb_OsRegisterIsr) (char *Name,int Vector,uint8 DevNo,uint8 epnum, uint32 Flags);
	void    (*CoreUsb_OsUnregisterIsr) (char *Name,int Vector,uint8 DevNo,uint8 epnum, uint32 Flags);
	void	(*CoreUsbHaltHandler) (uint8 DevNo, uint8 EpNum, uint8 EpDir);
	void	(*CoreUsbUnHaltHandler) (uint8 DevNo, uint8 EpNum, uint8 EpDir);
	void	(*CoreUsb_OsDelay) (long msecs);
	void    (*CoreUsbConfigureHS) (uint8 DevNo);
	void    (*CoreUsbConfigureFS) (uint8 DevNo);
	void    (*CoreUsbConfigureLS) (uint8 DevNo);
	uint8*  (*CoreUsbCreateDescriptor) (uint8 DevNo,uint16 DescSize); 
	void	(*CoreUsbDestroyDescriptor) (uint8 DevNo);

	int     (*CoreUsbAddDeviceDesc) (uint8 DevNo, uint16 VendorId, uint16 ProductId, uint16 DeviceRev,
									char *ManufStr, char*ProdStr, int IsThisLunDev);
	int		(*CoreUsbAddHidDesc) (uint8 DevNo, uint16 ReportSize);
	int		(*CoreUsbAddInterfaceDesc) (uint8 DevNo, uint8 bNumEndPoints, uint8 bClass, 
							uint8 bSubClass, uint8 bProtocol, char *IntStr,INTERFACE_INFO *InterfaceInfo);
	int		(*CoreUsbAddCfgDesc) (uint8 DevNo, uint8 NumInterfaces, uint8 hid);
	int		(*CoreUsbAddEndPointDesc) (uint8 DevNo, uint8 Dir, uint8 Type, uint16 MinPktSize, uint16 MaxPktSize,uint8 Interval, uint8 DataEp);


	int		(*CoreUsbRemoteWakeupHost) (void);
	int		(*CoreUsbDeviceDisconnect) (uint8 DevNo);
	int		(*CoreUsbDeviceReconnect) (uint8 DevNo);

	void* 	(*CoreUsbGetInterfaceData) (uint8 DevNo, uint8 IfNum);

	void 	(*CoreUsb_OsWakeupOnTimeout) (Usb_OsSleepStruct *Sleep);

	uint8	(*CoreUsbGetDataInEp) (uint8 DevNo,uint8 ifnum);
	int		(*CoreUsbWriteData) (uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt);

	void 	(*CoreUsb_OsInitSleepStruct) (Usb_OsSleepStruct *Sleep);
	long 	(*CoreUsb_OsSleepOnTimeout) (Usb_OsSleepStruct *Sleep,uint8 *Var,long msecs);
	uint8	(*CoreUsbGetDataOutEp) (uint8 DevNo,uint8 ifnum);

	USB_INTERFACE_DESC* (*CoreUsbGetInterfaceDesc) (uint8 DevNo, uint8 ConfigIndex, uint8 IfaceIndex);
	int		(*CoreUsbGetFreeUsbDev) (usb_device_driver* UsbDevDriver);
	void	(*CoreUsbBotRxHandler) (uint8 DevNo, uint8 ifnum,uint8 epnum);
	int		(*CoreUsbBotReqHandler) (uint8 DevNo, uint8 ifnum, DEVICE_REQUEST *pRequest);
	void	(*CoreUsbBotProcessHandler) (uint8 DevNo, uint8 ifnum);
	void 	(*CoreUsbBotHaltHandler) (uint8 DevNo,uint8 ifnum, uint8 epnum);
	void 	(*CoreUsbBotUnHaltHandler) (uint8 DevNo,uint8 ifnum, uint8 epnum);
	void 	(*CoreUsbBotRemHandler) (uint8 DevNo,uint8 ifnum);
	int     (*CoreUsbBotSendData) (uint8 DevNo, uint8 ifnum, uint8 *DataPkt, uint32 SendSize, uint32 *RemainLen);
	void	(*CoreUsbBotSetScsiErrorCode) (uint8 DevNo, uint8 ifnum, uint8 Key, uint8 Code, uint8 CodeQ);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500)
	void	(*CoreUsbBotSetScsiEventStatus) (uint8 DevNo, uint8 ifnum, uint8 LunNo, uint8 EventCode, uint8 MediaStatus);
#endif
	int		(*CoreUsbBotSendCSW) (uint8 DevNo, uint8 ifnum,uint32 Tag,uint8 status,uint32 remain);

    int		(*CoreUsbAddHubDesc) (uint8 DevNo, uint16 ReportSize, uint8 NumPorts);
	USB_HUB_DESC* (*CoreUsbGetHubDesc) (uint8 DevNo,uint8 CfgIndex, uint8 IntIndex);
	int		(*CoreUsbGetHubPortsStatus) (uint8 DevNo, uint8 ifnum, uint32* Status);
    void	(*CoreUsbSetHubPortsStatus) (uint8 DevNo, uint8 ifnum, uint32 BitMask, int SetClear); 
	void    (*CoreUsbHubEnableDevice) (uint8 DevNo, uint8 IfNum);
	void	(*CoreUsbHubDisableDevice) (uint8 DevNo, uint8 IfNum);
	void 	(*CoreUsbClearHubDeviceEnableFlag) (uint8 DevNo, uint8 IfNum);
	void 	(*CoreUsbSetHubDeviceEnableFlag) (uint8 DevNo, uint8 IfNum);
	void	(*CoreUsbHubPortBitmap) (uint8 DevNo, uint8 IfNum);
	void    (*CoreUsbChangeHubRegs) (uint8 DevNo, uint8 bRequest, uint8 wValue, uint8 wIndex);
	int		(*CoreUsbGetHubSpeed) (uint8 DevNo);	
	int		(*CoreUsbIsHubDeviceConnected) (uint8 DevNo, uint8 ifnum);
	int 	(*CoreUsbIsHubDeviceEnabled) (uint8 DevNo, uint8 ifnum);
	void 	(*CoreUsbFormiUsbHeader) (IUSB_HEADER *Header, uint32 DataLen, uint8 DevNo, uint8 ifnum,
				uint8 DeviceType,uint8 Protocol);
	int 	CoreUsbGlobalConnectSupport;
	uint8	(*CoreUsbDevGetCfgIndex) (uint8 DevNo);
	USB_DEVICE_DESC* (*CoreUsbGetDeviceDesc) (uint8 DevNo);
	int 	(*CoreUsbAddStringDescriptor) (uint8 DevNo, char *Str);
	USB_CONFIG_DESC* (*CoreUsbGetConfigDesc) (uint8 DevNo, uint8 ConfigIndex);
	int		(*CoreUsbAddDescriptor) (uint8 DevNo, uint8* Desc , uint16 DescSize);
	int 	(*CoreUsbNet_OSPushPacket) (NET_DEVICE *Dev, uint8 *Data, int Len);
	void 	(*CoreUsbNet_TxComplete) (NET_PACKET *Pkt);
	int 	(*CoreUsbNet_OSRegisterDriver) (NET_DEVICE *Dev);
	void 	(*CoreUsbNet_OSUnregisterDriver) (NET_DEVICE *Dev);	
	uint32 	(*CoreUsbGetRxDataLen) (uint8 DevNo,uint8 ep);
	uint8   (*CoreUsbGetNonDataEp) (uint8 DevNo,uint8 ifnum);
	int		(*CoreUsbWriteDataNonBlock) (uint8 DevNo, uint8 ep, uint8 *Buffer, uint32 Len, uint8 ZeroLenPkt);
	uint16	(*CoreUsbGetMaxPacketSize) (uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir);
	int		(*CoreUsbSetNumLUN) (uint8 DevNo, uint8 NumLUN);	
	int (*CoreUsbAddExtPropertiesDesc)(char *CustomPropertyStr);
	int 	(*CoreUsbAddOsStringDesc)(uint8 DevNo,uint8 VendorCode,char *VendSigStr);
	
} USB_CORE;


typedef struct video_callback_t 
{
	void  (*CallBackHandler)(int EnableDelay);

} video_callback;


#endif /* __USB__CORE__H */
