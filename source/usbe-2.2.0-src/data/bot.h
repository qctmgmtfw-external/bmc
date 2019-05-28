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

#ifndef BOT_H
#define BOT_H

#include "scsi.h"
#include "coreusb.h"
#include "bot_if.h"

/* Command/Data/Status Protocol for Bulk Only Devices*/
/* Refer USB Mass Storage Class - Bulk Only Transport Rev 1.0 Page 13 */
typedef struct
{
	uint32	dCBWSignature;
	uint32  dCBWTag;
	uint32	dCBWDataTransferLength;
	uint8	bmCBWFlags;
	uint8	bCBWLUN;
	uint8	bCBWCBLength;
	uint8	CBWCB[16];
} __attribute__((packed))  COMMAND_BLOCK_WRAPPER;

typedef struct
{
	uint32	dCSWSignature;
	uint32  dCSWTag;
	uint32	dCSWDataResidue;
	uint8	bCSWStatus;
} __attribute__((packed))  COMMAND_STATUS_WRAPPER;


/* Max Possible Length*/
#define MAX_CBW_LENGTH	31
#define MAX_CSW_LENGTH	13

/* Packet Signatures*/
#define CBW_SIGNATURE   0x43425355
#define CSW_SIGNATURE	0x53425355 

/* Status values for bmCSWStatus */
#define CSW_PASSED		0x00
#define CSW_FAILED		0x01
#define CSW_PHASE_ERROR	0x02


# define MAX_NOMEDIUM_RETRIES 10			
/* Note: Even when the medium is present, there are instances where we may 
		 not be able to read the medium for a brief period of time (due to 
         spin time, network busy, building on fire). If we return no medium 
         for this, the host will think the medium is removed and does its 
		 cleanup. To avoid this, we retry the command for MAX_NOMEDIUM_RETIRES 
        (with returning status medium getting ready)
*/		 


/********************* USB Mass Devices Related stuff *************************/
int  BotReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *DevRequest);
void BotRxHandler(uint8 DevNo,uint8 ifnum, uint8 epnum);
void BotProcessHandler(uint8 DevNo,uint8 ifnum);
void BotHaltHandler(uint8 DevNo,uint8 ifnum, uint8 epnum);
void BotUnHaltHandler(uint8 DevNo,uint8 ifnum, uint8 epnum);
void BotRemHandler(uint8 DevNo,uint8 ifnum);

/* These functions are used by iUSB module */
int  BotSendData(uint8 DevNo, uint8 epnum, uint8 *DataPkt, uint32 SendSize, uint32 *RemainLen);
int  BotSendCSW(uint8 DevNo, uint8 ifnum,uint32 Tag,uint8 status,uint32 remain);
void BotSetScsiErrorCode (uint8 DevNo, uint8 ifnum, uint8 Key, uint8 Code, uint8 CodeQ);
void BotSetScsiEventStatus (uint8 DevNo, uint8 ifnum, uint8 LunNo, uint8 EventCode, uint8 MediaStatus);


#endif
