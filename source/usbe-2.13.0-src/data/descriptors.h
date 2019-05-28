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

#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

#include "coreusb.h"
/********************************************************************/

/* Standard USB descriptor types */
#define DEVICE					0x01
#define CONFIGURATION			0x02
#define STRING					0x03
#define INTERFACE				0x04
#define ENDPOINT				0x05
#define DEVICE_QUALIFIER		0x06

/* Class Descriptor Types */
#define HID_DESC				0x21
#define REPORT_DESC				0x22
#define PHYSICAL_DESC			0x23
#define HUB_DESC				0x29

/* USB Standard Request Types - bRequestType */
#define IN						0x80
#define OUT						0x00
#define STANDARD				0x00
#define CLASS					0x20
#define VENDOR					0x40
#define RQ_DEVICE				0x00
#define RQ_INTERFACE			0x01
#define RQ_ENDPOINT				0x02

/* USB Standard Request Codes - bRequest */
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7
#define GET_CONFIGURATION	8
#define SET_CONFIGURATION	9
#define GET_INTERFACE		10
#define SET_INTERFACE		11
#define SYNCH_FRAME			12

/* Configuration bmAttributes fields */
#define BUS_POWERED			0x80
#define SELF_POWERED		0xC0
#define REMOTE_WAKEUP		0xA0

/* Endpoint bmAttributes fields */
#define CONTROL				0x00
#define ISOCHRONOUS			0x01
#define BULK				0x02
#define INTERRUPT			0x03
#define DISABLED			0xFF
#define CONFIGURABLE		0x04

/* Standard Feature Selectors */
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOINT_HALT			0

/* Structure for USB OS String Descriptor */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
    uint8 qwSignature[14];
    uint8 bMS_VendorCode;
    uint8 bPad;
} __attribute__((packed))  USB_OS_STRING_DESC;

/* Structure for USB OS Extended Property Descriptor */
typedef struct {
	//Header section
    uint32 dwLength;
	uint16 bcdVersion;
    uint16 wIndex;
    uint16 wCount;
    // Custom property
    uint32 dwSize;
    uint32 dwPropertyDataType;
    uint16 wPropertyNameLength;
    uint8  bPropertyName[48];
    uint32 dwPropertyDataLength;
    uint32 bPropertyData;
} __attribute__((packed))  USB_EXTENDED_PROPERTY_DESC;
                                          


/* Structure for USB Device Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bcdUSBL;
	uint8 bcdUSBH;
	uint8 bDeviceClass;
	uint8 bDeviceSubClass;
	uint8 bDeviceProtocol;
	uint8 bMaxPacketSize0;
	uint8 idVendorL;
	uint8 idVendorH;
	uint8 idProductL;
	uint8 idProductH;
	uint8 bcdDeviceL;
	uint8 bcdDeviceH;
	uint8 iManufacturer;
	uint8 iProduct;
	uint8 iSerialNumber;
	uint8 bNumConfigurations;
} __attribute__((packed))  USB_DEVICE_DESC;

typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bcdUSBL;
	uint8 bcdUSBH;
	uint8 bDeviceClass;
	uint8 bDeviceSubClass;
	uint8 bDeviceProtocol;
	uint8 bMaxPacketSize0;
	uint8 bNumConfigurations;
	uint8 bReserved;
} __attribute__((packed))  USB_DEVICE_QUALIFIER_DESC;

/* Structure for USB Configuration Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 wTotalLengthL;
	uint8 wTotalLengthH;
	uint8 bNumInterfaces;
	uint8 bConfigurationValue;
	uint8 iConfiguration;
	uint8 bmAttributes;
	uint8 maxPower;
} __attribute__((packed))  USB_CONFIG_DESC;

/* Structure for USB Interface Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bInterfaceNumber;
	uint8 bAlternateSetting;
	uint8 bNumEndpoints;
	uint8 bInterfaceClass;
	uint8 bInterfaceSubClass;
	uint8 bInterfaceProtocol;
	uint8 iInterface;
} __attribute__((packed))  USB_INTERFACE_DESC;

/* Structure for USB Endpoint Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 bEndpointAddress;
	uint8 bmAttributes;
	uint8 wMaxPacketSizeL;
	uint8 wMaxPacketSizeH;
	uint8 bInterval;
} __attribute__((packed))  USB_ENDPOINT_DESC;

/* Structure for USB String Descriptors */
typedef struct {
	uint8 bLength;
	uint8 bDescriptorType;
	uint8 * bString;
} __attribute__((packed))  USB_STRING_DESC;


/* Structure for HID Class Descriptor */
typedef struct {
	uint8 bLength;
	uint8 bHidDescriptorType;
	uint8 bcdHIDL;
	uint8 bcdHIDH;
	uint8 bCountryCode;
	uint8 bNumDescriptors;
	uint8 bClassDescriptorType;
	uint8 wDescriptorLengthL;
	uint8 wDescriptorLengthH;
} __attribute__((packed))  USB_HID_DESC;

/* Structure for HUB Class Descriptor */
typedef struct {
        uint8 bDescLength;
        uint8 bDescriptorType;
        uint8 bNbrPorts;
        uint8 wHubCharacteristicsL;
        uint8 wHubCharacteristicsH;
        uint8 bPwrOn2PwrGood;
        uint8 bHubContrCurrent;
        uint8  DeiveRemovable;
        uint8  PortPwrCtrlMask;
} __attribute__((packed))  USB_HUB_DESC;



/* Interface Classes */
#define AUDIO_INTERFACE			0x01
#define CDC_CONTROL_INTERFACE	0x02
#define CDC_DATA_INTERFACE		0x0A
#define PHYSICAL_INTERFACE		0x05
#define PRINTER_INTERFACE		0x07
#define HID_INTERFACE			0x03
#define HUB_INTERFACE			0x09
#define MASS_INTERFACE			0x08

/* Mass Storage Sub Class Code */
#define MASS_RBC_INTERFACE		0x01
#define MASS_8020_INTERFACE		0x02
#define MASS_MMC2_INTERFACE		0x02
#define MASS_QIC157_INTERFACE	0x03
#define MASS_UFI_FDD_INTERFACE	0x04
#define MASS_8070_INTERFACE		0x05
#define MASS_SCSI_INTERFACE		0x06

/* Mass Storage Transport Protocol */
# define MASS_CBI_INTERRUPT_PROTOCOL	0x00
# define MASS_CBI_NOINTERRUPT_PROTOCOL	0x01
# define MASS_BULK_ONLY_PROTOCOL		0x50


/* Descriptor Parsing Routines */
uint8  GetStartingEpNum	(uint8 DevNo, uint8 CfgIndex, uint8 IntIndex);
uint8  GetNumEndpoints	(uint8 DevNo, uint8 CfgIndex, uint8 IntIndex);
uint8  GetNumInterfaces	(uint8 DevNo, uint8 CfgIndex);
uint16 GetConfigDescSize(uint8 DevNo, uint8 CfgIndex);
uint16 GetMaxPacketSize	(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir);
uint8  GetInterface		(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir);
uint8  GetEndPointDir	(uint8 DevNo, uint8 CfgIndex, uint8 EpNum);
uint8  GetEndPointType	(uint8 DevNo, uint8 CfgIndex, uint8 EpNum, uint8 EpDir);
uint8 *GetDescriptorofDevice(uint8 DevNo,uint16 *Len);	
uint8 *GetStringDescriptor	(uint8 DevNo,uint8 StrIndex, uint16 *Len); 
USB_DEVICE_DESC    *GetDeviceDesc	(uint8 DevNo);
USB_DEVICE_QUALIFIER_DESC    *GetDeviceQualifierDesc	(uint8 DevNo);
USB_CONFIG_DESC    *GetConfigDesc	(uint8 DevNo,uint8 CfgIndex);
USB_INTERFACE_DESC *GetInterfaceDesc(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex);
USB_ENDPOINT_DESC  *GetEndpointDesc	(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex, uint8 EndPointNum, uint8 EpDir);
int AddStringDescriptor(uint8 DevNo, char *Str);
uint8 GetNumConfigurations (uint8 DevNo);


/* Descriptor Creating Functions */
uint8* CreateDescriptor(uint8 DevNo,uint16 DescSize);
void   DestroyDescriptor(uint8 DevNo);
int AddDescriptor(uint8 DevNo, uint8* Desc , uint16 DescSize);
int AddDeviceDesc(uint8 DevNo, uint16 VendorId, uint16 ProductId, uint16 DeviceRev,
					char *ManufStr, char*ProdStr, int IsThisLunDev);
int AddCfgDesc(uint8 DevNo, uint8 NumInterfaces,uint8 Hid);
int AddInterfaceDesc(uint8 DevNo, uint8 bNumEndPoints, uint8 bClass, 
					uint8 bSubClass, uint8 bProtocol, char *IntStr,INTERFACE_INFO *InterfaceInfo);
int AddEndPointDesc(uint8 DevNo, uint8 Dir, uint8 Type, uint16 MinPktSize, uint16 MaxPktSize, uint8 Interval,uint8 DataEp);
int AddHidDesc(uint8 DevNo, uint16 ReportSize);
int AddHubDesc(uint8 DevNo, uint16 ReportSize, uint8 NumPorts);
USB_HUB_DESC* GetHubDesc(uint8 DevNo,uint8 CfgIndex, uint8 IntIndex);
int AddExtPropertiesDesc(char *CustomPropertyStr);
int AddOsStringDesc(uint8 DevNo,uint8 VendorCode,char *VendSigStr);
/* Values for DataEp in AddEndPointDesc */
#define DATA_EP			1
#define NON_DATA_EP		0

typedef int (*CREATE_DESC)(uint8 DevNo);

#endif /* DESCRIPTOR_H */


