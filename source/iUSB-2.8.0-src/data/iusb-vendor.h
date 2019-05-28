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
#ifndef _IUSB_VENDOR_H_
#define _IUSB_VENDOR_H_

#include <coreTypes.h>
#include "iusb.h"

extern int ProcessVendorCommands(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo);
extern int VendorWaitRequest(IUSB_SCSI_PACKET *UserModePkt);
extern int VendorExitWait(void);

#endif /* _IUSB_VENDOR_H_ */


