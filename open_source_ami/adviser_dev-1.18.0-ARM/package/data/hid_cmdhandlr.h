/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#ifndef __HID_CMD_HANDLER_H__
#define __HID_CMD_HANDLER_H__

#include "coreTypes.h"

int hid_process_packet (int sock, u8* pPkt);
int HandleHidCmdPacket (int sock, u8* pPkt);

int HandleUsbPacket (int sock, u8* pPkt);
u8  get_usb_mouse_mode(void);
int set_usb_mouse_mode(u8 mouse_mode);
int init_usb_mouse_mode(void);
int init_usb_resources (void);
void release_usb_resources (void);
int Refresh_HID_Device(void);
/***************************Big to little conversions based on platform****************/
//PLEASE REMEBER TO INCLUDE ICC_WHAT.H TO HAVE THIS AVAILABLE
#ifdef ICC_PLATFORM_BIG_ENDIAN
#define  mac2short(x)   ((u16) (((u16)(x) >> 8) | ((u16)(x) << 8)))
#define  mac2long(x)    (((x) >> 24) | ((x) << 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8))
#else
#define  mac2long(x)    (x)
#define  mac2short(x)   (x)
#endif
/****************************************************************************/

#endif







