/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#ifndef _IUSB_MOD_H_
#define _IUSB_MOD_H_

#include <coreTypes.h>
#include "iusb.h"
#include "usb_core.h"
#include "helper.h"
#include "dbgout.h"

#define DEBUG_IUSB_SCSI 		0x01
#define DEBUG_IUSB_HARD_DISK	0x02
#define DEBUG_IUSB_CDROM		0x04
#define DEBUG_IUSB_VENDOR		0x08
#define DEBUG_IUSB_DUMP_DATA	0x10
#define DEBUG_IUSB_HID			0x20

/* external variables */
extern USB_CORE UsbCore;
TDBG_DECLARE_DBGVAR_EXTERN(iusb);

/* external function prototype */
extern uint32 FormiUsbScsiPacket(IUSB_SCSI_PACKET *iUsbScsiPkt,uint8 DevNo, 
								 uint8 IfNum,BOT_IF_DATA *MassData,uint8 LunNo);


#endif /* _IUSB_MOD_H_ */

