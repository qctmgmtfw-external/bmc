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

#ifndef __BOT_IF_H__
#define __BOT_IF_H__

#include <coreTypes.h>
#include "scsi.h"
#include "linux.h"

#define MAX_LUN 2

#define LUN_0 0x0
#define LUN_1 0x1
#define LUN_2 0x2
#define LUN_3 0x3

/* Maximum SCSI Data that can be communicated */
/*This should match CD_MAX_READ_SIZE and HD_MAX_READ_SIZE in apphead.h (cdserver/hdserver)*/
#define MAX_SCSI_DATA 	(64*2048)				

typedef struct
{
	uint8	volatile CBW[32];
	uint8 	ScsiData[MAX_SCSI_DATA];	
	uint32	ScsiDataLen;
	Usb_OsSleepStruct UnHaltedSleep;
	uint8 	UnHalted;
	uint8	volatile SenseKey;
	uint8	volatile SenseCode;
	uint8	volatile SenseCodeQ;
	uint8 	volatile NoMediumRetries;		/* Number of Retries before reporting NO MEDIUM */	
	SCSI_INQUIRY_PACKET* Inquiry;
	SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET* EventStatus;
	uint32	ScsiDataOutLen;
	uint32	volatile LastSeqNo;
} BOT_IF_DATA;

#endif /* __BOT_IF_H__ */
