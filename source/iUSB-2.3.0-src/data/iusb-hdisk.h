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
#ifndef _IUSB_HDISK_H_
#define _IUSB_HDISK_H_

#include <coreTypes.h>
#include "iusb.h"

extern int GetHdiskInstanceIndex (void);
extern int ReleaseHdiskInstanceIndex (uint8 Instance);
extern int HdiskRemoteActivate(uint8 Instance);
extern int HdiskRemoteWaitRequest(IUSB_SCSI_PACKET *UserModePkt, uint8 Instance);
extern int HdiskRemoteDeactivate(uint8 Instance);
extern int HdiskCheckSupportedCmd(uint8 DevNo,uint8 ifnum,uint8 Command, BOT_IF_DATA *MassData, uint8 Instance);
extern int HdiskRemoteCall(IUSB_SCSI_PACKET *iUsbScsiPkt,uint32 PktLen, uint8 Instance);
extern IUSB_SCSI_PACKET* HdiskGetiUsbPacket(uint8 DevNo,uint8 ifnum, uint8 Instance);

#define MAX_FD_INSTANCES		4
#define MAX_HD_INSTANCES		4
#define MAX_SD_INSTANCES		1

#endif /* _IUSB_HDISK_H_ */

