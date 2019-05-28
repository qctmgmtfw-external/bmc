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

#ifndef USB_HW_H
#define USB_HW_H


#include <linux/semaphore.h>
#include "setup.h"
#include "bot_if.h"
#include "iusb.h"


#define MAX_USB_HW	7

/* 
   USB Hardware functions exposed to the core usb via this structure. This 
   structure will be returned after the Hw initilaizaion is called 
 */
typedef struct 
{
	void 	(*UsbHwEnable)		(uint8 DeviceNo, uint8 Speed);
	void 	(*UsbHwDisable)		(uint8 DeviceNo, uint8 Speed);
	int 	(*UsbHwIntr)		(uint8 DeviceNo,uint8 Ep);
	void	(*UsbHwSetAddr)		(uint8 DeviceNo,uint8 Addr, uint8 Enable);
	void	(*UsbHwGetAddr)		(uint8 DeviceNo,uint8 *Addr, uint8 *Enable);
	int 	(*UsbHwWrite)		(uint8 DeviceNo,uint8 Ep, uint8 *Buff, uint16 Len);
	void 	(*UsbTxComplete)	(uint8 DeviceNo,uint8 Ep);
	int		(*UsbHwRead)		(uint8 DeviceNo,uint8 Ep, uint8 *Buff,uint16 *Len, uint8 Setup);
	int 	(*UsbHwAllocBuffer)	(uint8 DeviceNo,uint8 Ep, uint16 Fifo, uint16 MaxPkt, uint8 Dir, uint8 Iso); 		
	int		(*UsbHwEnableEp)	(uint8 DeviceNo,uint8 Ep, uint8 EpDir, uint8 EpType);
	int		(*UsbHwDisableEp)	(uint8 DeviceNo,uint8 Ep, uint8 EpDir, uint8 EpType);
	int		(*UsbHwGetEpStatus) (uint8 DeviceNo,uint8 Ep, uint8 EpDir, uint8 *Enable, uint8 *Stall);
	int		(*UsbHwStallEp)		(uint8 DeviceNo,uint8 Ep, uint8 EpDir);
	int		(*UsbHwUnstallEp)	(uint8 DeviceNo,uint8 Ep, uint8 EpDir);
	void	(*UsbHwSetRemoteWakeup)(uint8 DeviceNo,uint8 Set);
	uint8	(*UsbHwGetRemoteWakeup)(uint8 DeviceNo);
	void 	(*UsbHwCompleteRequest)(uint8 DeviceNo,uint8 Status,DEVICE_REQUEST *Req);
	void	(*UsbHwDeviceDisconnect)(uint8 DevNo);
	void	(*UsbHwDeviceReconnect)(uint8 DevNo);
	void	(*UsbHwRemoteWakeup)(uint8 DevNo);
	int		(*UsbHwGetEps) (uint8 DevNo, uint8* pdata, uint8* num_eps);
	int		(*UsbHwGetHubPortsStatus) (uint8 DevNo, uint8 PortNum, uint32* Status);
	void    (*UsbHwSetHubPortsStatus) (uint8 DevNo, uint8 PortNum, uint32 Mask, int SetClear);
    void    (*UsbHwHubEnableDevice) (uint8 DevNo, uint8 IfNum);
	void	(*UsbHwHubDisableDevice) (uint8 DevNo, uint8 IfNum);
	void	(*UsbHwClearHubDeviceEnableFlag) (uint8 DevNo, uint8 IfNum);
	void	(*UsbHwSetHubDeviceEnableFlag) (uint8 DevNo, uint8 IfNum);
	void	(*UsbHwHubPortBitmap) (uint8 DevNo, uint8 IfNum);
	void	(*UsbHwChangeHubRegs) (uint8 DevNo, uint8 bRequest, uint8 wValue, uint8 wIndex);
	int		(*UsbHwGetHubSpeed) (uint8 DevNo);	
	int		(*UsbHwIsHubDeviceConnected) (uint8 DevNo, uint8 ifnum);
	int 	(*UsbHwIsHubDeviceEnabled) (uint8 DevNo, uint8 ifnum);
#ifdef CONFIG_SPX_FEATURE_POWER_CONSUMPTION_SUPPORT
	int 	(*UsbHwUpstreamDisable) (uint8 DevNo);
	int 	(*UsbHwUpstreamEnable) (uint8 DevNo);
#endif
	int 	(*UsbHwUpstreamMediaDisable) (uint8 DevNo);
	int 	(*UsbHwUpstreamMediaEnable) (uint8 DevNo);
    uint8	NumEndpoints;		/* Usable Endpoints, excluding Control 0 endpoints */
	uint8	BigEndian;	
    uint8   WriteFifoLock;
	uint16  EP0Size;
	uint8   SupportedSpeeds;
	uint8	EpMultipleDMASupport;
	uint16  EpMultipleDMASize;
	int		IsThisHUBDevice;
	int		NumHUBPorts;
} USB_HW;


typedef struct 
{
	int 	(*DevUsbCreateDescriptor) (uint8 DevNo);
	int		(*DevUsbIOCTL) ( unsigned int cmd,unsigned long arg, int* RetVal);
	int 	(*DevUsbSetKeybdSetReport) (uint8 DevNo, uint8 IfNum, uint8 *Data ,uint16 DataLen);
	int		(*DevUsbMiscActions) (uint8 DevNo, uint8 ep, uint8 Action);
	int		(*DevUsbIsEmulateFloppy) (uint8 LunNo);
	int  	(*DevUsbRemoteScsiCall)(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo);
	int		(*DevUsbFillDevInfo) (IUSB_DEVICE_INFO* iUSBDevInfo);
	int 	(*DevUsbSetInterfaceAuthKey) (uint8 DevNo, uint8 IfNum, uint32 Key);
	int 	(*DevUsbClearInterfaceAuthKey) (uint8 DevNo, uint8 IfNum);	
	ssize_t (*DevUsbRead)  (struct file *, char *, size_t, loff_t *);
	ssize_t (*DevUsbWrite) (struct file * file , const char __user *buf, size_t count, loff_t *ppos);

} USB_DEV;


typedef struct
{
	int (*iUSBScsiSendResponse) (IUSB_SCSI_PACKET *UserModePkt);
	int (*iUSBCdromRemoteActivate) (uint8 Instance);
	int	(*iUSBCdromRemoteWaitRequest) (IUSB_SCSI_PACKET *UserModePkt, uint8 Instance);
    int (*iUSBCdromRemoteDeactivate) (uint8 Instance);
	int (*iUSBHdiskRemoteActivate) (uint8 Instance);
	int (*iUSBHdiskRemoteWaitRequest) (IUSB_SCSI_PACKET *UserModePkt, uint8 Instance);
	int (*iUSBHdiskRemoteDeactivate) (uint8 Instance);
	int (*iUSBVendorWaitRequest) (IUSB_SCSI_PACKET *UserModePkt);
	int (*iUSBVendorExitWait) (void);
	int (*iUSBRemoteScsiCall) (uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo, uint8 Instance);
	int (*iUSBKeybdLedWaitStatus) (IUSB_HID_PACKET *UserModePkt, uint8 IfNum, uint8 NoWait);
	int (*iUSBKeybdExitWait) (uint8 IfNum);
	int (*iUSBKeybdLedRemoteCall)(uint8 DevNo,uint8 IfNum,uint8 Data);
	int (*iUSBKeybdPrepareData) (IUSB_HID_PACKET *UserModePkt, uint8 *KeybdData);
	int (*iUSBMousePrepareData) (IUSB_HID_PACKET *UserModePkt, uint8 MouseMode, uint8 *MouseData);
	int (*iUSBGetHdiskInstanceIndex) (void);
	int (*iUSBGetCdromInstanceIndex) (void);
	int (*iUSBReleaseHdiskInstanceIndex) (uint8 Instance);
	int (*iUSBReleaseCdromInstanceIndex) (uint8 Instance);
	
} USB_IUSB;

/* USB Device Intialization Routine */
typedef int (*UsbHwInit) (uint8 DeviceNo, USB_HW *UsbDev, void**DevConf);
typedef void (*UsbHwExit) (uint8 DeviceNo);

typedef struct
{
	struct module *module;
	struct semaphore lock;
	char name[20];
	struct list_head list;
	int  descriptor;   
	int  devnum;
	uint8	speeds; /* bit field */
	uint8 maxlun; 	/* This field need to be filled for LUN combo devices */
	uint8 DisconnectOnRegister;
} usb_device_driver;

typedef struct
{
	struct module *module;
	struct semaphore lock;
	char name[20];
	struct list_head list;
	int  (*usb_driver_init)(uint8 DeviceNo, USB_HW *UsbDev, void **DevConf);
	void (*usb_driver_exit)(uint8 DeviceNo);
	int  devnum;
} usb_ctrl_driver;

typedef struct
{
	uint8	ep_num;
	uint8	ep_dir;
	uint16	ep_size;
	uint8	ep_type;
	uint8   is_ep_used;

} __attribute__ ((packed)) ep_config_t;

#define  usb_short(x)	(x)
#define  usb_long(x)	(x)

#define SUPPORT_LOW_SPEED		0x01
#define SUPPORT_FULL_SPEED		0x02
#define SUPPORT_HIGH_SPEED		0x04


/* Values for Dir */
#define DIR_OUT	0
#define DIR_IN	1

#define CREATE_HID_DESCRIPTOR           1
#define CREATE_LUN_COMBO_DESCRIPTOR     2
#define CREATE_CDROM_DESCRIPTOR         3
#define CREATE_HARDDISK_DESCRIPTOR      4
#define CREATE_FLOPPY_DESCRIPTOR        5
#define CREATE_COMBO_DESCRIPTOR         6
#define CREATE_SUPERCOMBO_DESCRIPTOR    7
#define CREATE_HUB_DESCRIPTOR			8
#define CREATE_ETH_DESCRIPTOR			9
#define ACTION_USB_WRITE_DATA_TIMEOUT	1
#define ACTION_USB_WRITE_DATA_SUCCESS	2
#define ACTION_USB_BUS_RESET_OCCURRED	3

/* USB HUB Class Request Types */
#define HUB_GET_STATUS			0x00
#define HUB_CLEAR_FEATURE		0x01
#define HUB_SET_FEATURE			0x03
#define HUB_GET_DESCRIPTOR		0x06
#define HUB_SET_DESCRIPTOR		0x07

/* USB Class Feature Selectors  (PORT Recipient)*/
#define PORT_CONNECTION		0x00
#define PORT_ENABLE		0x01
#define PORT_SUSPEND		0x02
#define PORT_OVER_CURRENT		0x03
#define PORT_RESET		0x04
#define PORT_POWER		0x08
#define PORT_LOW_SPEED		0x09
#define C_PORT_CONNECTION		0x10
#define C_PORT_ENABLE		0x11
#define C_PORT_SUSPEND		0x12
#define C_PORT_OVER_CURRENT		0x13
#define C_PORT_RESET		0x14
#define C_PORT_TEST		0x15
#define C_PORT_INDICATOR		0x16


/* Bit 31 ~ 16 for Port Status Change Bits. Bit 15 ~ 0 for Port Status Bits */

#define PORT_CONNECTION_CHANGE_BIT 					0x00010000
#define PORT_ENABLE_CHANGE_BIT 						0x00020000
#define PORT_SUSPEND_CHANGE_BIT						0x00040000
#define PORT_RESET_CHANGE_BIT 						0x00100000


#define PORT_CONNECTION_STATUS_BIT						0x00000001
#define PORT_ENABLE_STATUS_BIT							0x00000002
#define PORT_SUSPEND_STATUS_BIT							0x00000004
#define PORT_POWER_STATUS_BIT							0x00000100
#define PORT_HIGH_SPEED_STATUS_BIT						0x00000400


#endif /* USB_HW_H */
