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

#ifndef CDC_H
#define CDC_H

#define CDC_VERSION_MAJOR  0x01
#define CDC_VERSION_MINOR  0x10

#define USB_CDC_MTU       1514

#define USB_ECM_SUBCLASS  0x06
#define USB_ECM_PROTOCOL  0x00

#define CS_INTERFACE  0x24
#define CS_ENDPOINT   0x25

#define CDC_SUBTYPE_CALLMGMT  0x01
#define CDC_SUBTYPE_ACM       0x02

typedef struct
{
        uint8 bLength;
        uint8 bDescriptorType;
	uint8 bDescriptorSubtype;
	uint8 bcdCDCL;
	uint8 bcdCDCH;
} __attribute__((packed)) CDC_HEADER_DESC;

typedef struct
{
        uint8 bLength;
        uint8 bDescriptorType;
	uint8 bDescriptorSubtype;
	uint8 bmCapabilities;
} __attribute__((packed)) CDC_ACM_DESC;

typedef struct
{
        uint8 bLength;
        uint8 bDescriptorType;
	uint8 bDescriptorSubtype;
	uint8 bMaster;
	uint8 bSlave;
} __attribute__((packed)) CDC_UNION_DESC;

typedef struct
{
        uint8 bLength;
        uint8 bDescriptorType;
	uint8 bDescriptorSubtype;
	uint8 bmCapabilities;
	uint8 bDataInterface;
} __attribute__((packed)) CDC_CALLMGMT_DESC;

typedef struct
{
        uint8 bLength;
        uint8 bDescriptorType;
        uint8 bDescriptorSubtype;
        uint8 bMAC;
        uint8 bStat[4];
        uint8 bSegSizeL;
        uint8 bSegSizeH;
	uint8 bNumMCFiltersL;
	uint8 bNumMCFiltersH;
	uint8 bNumPwrFilters;
} __attribute__((packed)) ECM_ETH_DESC;

#endif
