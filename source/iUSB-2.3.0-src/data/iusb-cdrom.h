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
#ifndef _IUSB_CDROM_H_
#define _IUSB_CDROM_H_

#include <coreTypes.h>
#include "iusb.h"

extern int GetCdromInstanceIndex (void);
extern int ReleaseCdromInstanceIndex (uint8 Instance);
extern int CdromRemoteActivate(uint8 Instance);
extern int CdromRemoteWaitRequest(IUSB_SCSI_PACKET *UserModePkt, uint8 Instance);
extern int CdromRemoteDeactivate(uint8 Instance);
extern int CdromRemoteCall(IUSB_SCSI_PACKET *iUsbScsiPkt,uint32 PktLen, uint8 Instance);
extern int CdromCheckSupportedCmd(uint8 DevNo,uint8 ifnum,uint8 Command, BOT_IF_DATA *MassData, uint8 Instance);
extern IUSB_SCSI_PACKET* CdromGetiUsbPacket(uint8 DevNo,uint8 ifnum, uint8 Instance);

#define MAX_CD_INSTANCES		4

#endif /* _IUSB_CDROM_H_ */

