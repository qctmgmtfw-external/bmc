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

#ifndef SETUP_H
#define SETUP_H

/* Standard Device Requests */
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

/* Descriptor Types */
#define	DESC_DEVICE			1
#define DESC_CONFIGURATION	2
#define DESC_STRING			3
#define DESC_INTERFACE		4
#define DESC_ENDPOINT		5
#define DESC_DEVICE_QUALIFIER	6

/* Standard Feature Selectors */
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOINT_HALT			0

/* Device Request Format */
typedef struct 
{
	uint8	bmRequestType;
	uint8	bRequest;
	uint16	wValue;
	uint16	wIndex;
	uint16	wLength;
	char    *Command;
	uint8   Status;
	uint8   *Data; // Beware : this is non standard
	uint16  DataLen; // Used internally by coreusb.h
	uint16	wLengthOrig;
} __attribute__((packed))  DEVICE_REQUEST;


/* Defines for bmRequestType */
#define  REQ_DEVICE2HOST	0x80

#define  REQ_TYPE_MASK(x)	((x) & 0x60) >> 5 )
#define	 REQ_STANDARD		0x00
#define  REQ_CLASS			0x01
#define  REQ_VENDOR			0x02

#define  REQ_RECIP_MASK(x)	((x) & 0x1F)
#define	 REQ_DEVICE			0x00
#define  REQ_INTERFACE		0x01
#define  REQ_ENDPOINT		0x02


/* Setup Processing Routine - Called from Core USB*/
void ProcessSetup(uint8 DevNo,DEVICE_REQUEST *pRequest);



#endif /* SETUP_H */

