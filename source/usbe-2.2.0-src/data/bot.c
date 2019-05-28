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

#include "header.h"
#include "coreusb.h"
#include "descriptors.h"
#include "bot.h"
#include "scsi.h"
#include "iusb.h"
#include "usb_core.h"
#include "arch.h"
#include "helper.h"
#include "dbgout.h"
#include "usb_module.h"

extern video_callback     g_video_callback;

#define FLOPPY_BLOCK_SIZE (512)
#define FLOPPY_TOTAL_BLOCKS (2880)
typedef struct
{
	uint8	Reserved[3];
	uint8	ListLength;
	uint32	TotalBlocks;
	uint32	BlockSize;
} READ_FORMAT_CAPACITIES;

static int  BotInquiry 		   (uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen ,uint8 LunNo);
static int  BotSense		   (uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen);
static void	BotGetScsiErrorCode(uint8 DevNo, uint8 ifnum, uint8 *Key, uint8 *Code, uint8 *CodeQ);


static int IsCDROM(uint8 DevNo,uint8 ifnum,uint8 LunNo )
{
	BOT_IF_DATA *IfData;
	uint8 DeviceType;
	
	IfData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	
	if (IfData == NULL)
		return 0;

	DeviceType = (IfData->Inquiry[LunNo].RMB) | (IfData->Inquiry[LunNo].PeripheralType);	
	
	if ( (DeviceType & 0x7F) == IUSB_DEVICE_CDROM)
		return 1;
	
	/* default not a cdrom */
	return 0;
}

void 
BotRemHandler(uint8 DevNo,uint8 ifnum)
{
	BOT_IF_DATA *IntData;
	
	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotRemHandler(): Disconnect Handler Called\n");

	IntData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	if (IntData == NULL)
		return;	
	IntData->UnHalted 	= 1;
	IntData->SenseKey	= 0;
	IntData->SenseCode	= 0;
	IntData->SenseCodeQ = 0;
	IntData->NoMediumRetries = MAX_NOMEDIUM_RETRIES;		
	IntData->ScsiDataOutLen = 0;
	IntData->LastSeqNo	= 0;	/* Invalid Seq No */
	return;
}


int
BotReqHandler(uint8 DevNo, uint8 ifnum, DEVICE_REQUEST *pRequest)
{
	uint8 gMaxLun;

	gMaxLun = DevInfo[DevNo].MaxLUN;
	
	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotReqHandler(): Called!\n");

	/* Mass Get Max Lun */		
	if ((REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE) && 
	    (pRequest->bRequest == 0xFE))	
	{
		pRequest->Data[0] = gMaxLun;
		return 0;
	}
	/* Mass Reset */		
	if ((REQ_RECIP_MASK(pRequest->bmRequestType) == REQ_INTERFACE) && 
	    (pRequest->bRequest == 0xFF))	
	{
		TWARN("BotReqHandler() : BOT Reset Request Called\n");
		//TODO: Reset the controller and make it ready to accept the next CBW. 
		//DATA0 and DATA1 should be saved.
		return 0;
	}

	TWARN("BotReqHandler() : Unhandled Request %d\n",pRequest->bRequest);
	return 1;
}

void 
BotHaltHandler(uint8 DevNo,uint8 ifnum, uint8 epnum)
{
	BOT_IF_DATA *MassData;

	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotHaltHandler(): Enter\n");

	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	if (MassData == NULL)
		return;
	
	return;

}

void 
BotUnHaltHandler(uint8 DevNo,uint8 ifnum, uint8 epnum)
{
	BOT_IF_DATA *MassData;


	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotUnHaltHandler(): Enter\n");

	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	if (MassData == NULL)
		return;
	
	MassData->UnHalted = 1;	
	Usb_OsWakeupOnTimeout(&MassData->UnHaltedSleep);
	return;
}

void
BotSetScsiErrorCode(uint8 DevNo, uint8 ifnum, uint8 Key, uint8 Code, uint8 CodeQ)
{
	BOT_IF_DATA *MassData;
	
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);	
	if (MassData == NULL)
		return;
		
	MassData->SenseKey 		= Key;
	MassData->SenseCode 	= Code;
	MassData->SenseCodeQ 	= CodeQ;	
	
	return;
}


void
BotGetScsiErrorCode(uint8 DevNo, uint8 ifnum, uint8 *Key, uint8 *Code, uint8 *CodeQ)
{
	BOT_IF_DATA *MassData;
	
	*Key=*Code=*CodeQ=0;
	
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);	
	if (MassData == NULL)
		return;

	*Key 	= MassData->SenseKey;
	*Code 	= MassData->SenseCode;
	*CodeQ	= MassData->SenseCodeQ;
	return;
}

void
BotSetScsiEventStatus(uint8 DevNo, uint8 ifnum, uint8 LunNo, uint8 EventCode, uint8 MediaStatus)
{
	BOT_IF_DATA *MassData;
	
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);	
	if (MassData == NULL)
		return;

	(MassData->EventStatus[LunNo]).EventCode 	= EventCode;
	(MassData->EventStatus[LunNo]).MediaStatus 	= MediaStatus;
	
	return;
}

int
BotSendCSW(uint8 DevNo, uint8 ifnum,uint32 Tag,uint8 status,uint32 remain)
{
	COMMAND_STATUS_WRAPPER csw;
	BOT_IF_DATA *MassData;
	uint8 epnum;
	
	/* Get Endpoint */
	epnum = GetDataInEp(DevNo,ifnum);	
	
	/* Send Stall Command if we sent less data */		
	if (remain)		
	{

		TDBG_FLAGGED(usbe, DEBUG_BOT,"BotSendCSW: %d Residue Bytes Present. Stalling EP%d\n",
							remain,epnum);

		/* Get the Unhalt Flag from Interface Data */
		MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
		if (MassData == NULL)
			return 1;
		
		/* Set UnHalt flag to 0 and issue Stall Command */
		MassData->UnHalted = 0;
		UsbStallEp(DevNo,epnum,DIR_IN);			
		
		/* Wait till the UnHalt flag is set */
		if (Usb_OsSleepOnTimeout(&(MassData->UnHaltedSleep),
							&(MassData->UnHalted),STALL_TIMEOUT) <= 0)
		{
			TWARN("ERROR: BotSendCSW(): Waiting for Bot UnHalted Timed out or interrupted\n");
			MassData->UnHalted = 1;
		}
	}
	
	/* Form the CSW packet*/	
	csw.dCSWSignature 	= usb_long(CSW_SIGNATURE);
	csw.dCSWDataResidue	= usb_long(remain);	
	csw.dCSWTag 		= Tag;
	csw.bCSWStatus		= status;	
	
	
	UsbCoreDumpData("Bot CSW", epnum,(uint8 *)&csw,MAX_CSW_LENGTH); 

	/* Send the Packet */		
	if (UsbWriteData(DevNo,epnum,(uint8 *)&csw,MAX_CSW_LENGTH,ZERO_LEN_PKT) != 0)	
	{
		TCRIT("Error in Sending CSW for EP%d\n",epnum);
		return 1;
	}
	
	return 0;		
}

int
BotSendData(uint8 DevNo, uint8 ifnum, uint8 *DataPkt, uint32 SendSize, uint32 *RemainLen)
{ 
	uint8 epDataIn;
	
	/* Get the Endpoint to be used for Data In (Transfer to HOST) */	
	epDataIn = GetDataInEp(DevNo,ifnum);	

	/* Send the Packet */		
	if (UsbWriteData(DevNo,epDataIn,DataPkt,SendSize,NO_ZERO_LEN_PKT) != 0)
			return 1;
			
	/*Compute Remaining Length */
	if (*RemainLen > SendSize)
		*RemainLen -= SendSize;
	else
		*RemainLen = 0;	
		
	return 0;		
}


int
BotInquiry(uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen ,uint8 LunNo )
{
	uint32 SendSize;
	BOT_IF_DATA *MassData;

	/* Get Interface Data */	
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);	
	if (MassData == NULL)	
	{
		TCRIT("Error in Getting Inquiry Packet from Interface Data\n");
		return 1;
	}		
	
	/* Calculate the Size to be sent */	
	SendSize = (*RemainLen > sizeof(SCSI_INQUIRY_PACKET))?
								sizeof(SCSI_INQUIRY_PACKET):*RemainLen;
	
	/* Adjust the Length in the Inquiry Packet */								
 	//(MassData->Inquiry[LunNo]).AdditionalLength=	SendSize-5;								
	/* Actual extra valid data is only 31 bytes */ /* Vista is happy now */
	(MassData->Inquiry[LunNo]).AdditionalLength= 31; /* This field should be hard-coded to 31 */								

	

	UsbCoreDumpData("Bot Inquiry",epnum,(uint8 *)&(MassData->Inquiry[LunNo]),SendSize);	


	/* Send the Packet */		
	if (UsbWriteData(DevNo,epnum,(uint8 *)&(MassData->Inquiry[LunNo]),SendSize,NO_ZERO_LEN_PKT) != 0)			
	{
			TCRIT("Error in Sending SCSI_INQUIRY\n");
			return 1;
	}
	
	/*Compute Remaining Length */
	if (*RemainLen > SendSize)
		*RemainLen -= SendSize;
	else
		*RemainLen = 0;	

	BotSetScsiErrorCode (DevNo,ifnum,0,0,0);
	return 0;			
}


int
BotFormatCaps(uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen)
{
	uint32 SendSize;
	READ_FORMAT_CAPACITIES rfc;
	
	memset(&rfc,0,sizeof(READ_FORMAT_CAPACITIES));
	rfc.ListLength = 8;
	rfc.TotalBlocks = __cpu_to_be32(FLOPPY_TOTAL_BLOCKS);
	rfc.BlockSize = __cpu_to_be32(FLOPPY_BLOCK_SIZE);

			
	
	/* Calculate the Size to be sent */	
	SendSize = (*RemainLen > sizeof(READ_FORMAT_CAPACITIES))?
								sizeof(READ_FORMAT_CAPACITIES):*RemainLen;
								
						
	

	/* Send the Packet */		
	if (UsbWriteData(DevNo,epnum,(uint8 *)&(rfc),SendSize,NO_ZERO_LEN_PKT) != 0)			
	{
			TCRIT("Error in Sending SCSI_FORMAT_CAPS\n");
			return 1;
	}
	
	/*Compute Remaining Length */
	if (*RemainLen > SendSize)                                      
		*RemainLen -= SendSize;
	else
		*RemainLen = 0;	

	BotSetScsiErrorCode (DevNo,ifnum,0,0,0);
	return 0;			
}

int
BotSense(uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen)
{
	uint32 SendSize;
	SCSI_REQUEST_SENSE_PACKET Sense;
	
	/* Calculate the Size to be sent */	
	SendSize = (*RemainLen > sizeof(SCSI_REQUEST_SENSE_PACKET))?
							sizeof(SCSI_REQUEST_SENSE_PACKET):*RemainLen;
	
	/* Form the Packet */	
	memset((void *)&Sense,0,sizeof(SCSI_REQUEST_SENSE_PACKET));
	Sense.ErrorCode				=   0x70;
	BotGetScsiErrorCode(DevNo,ifnum,&Sense.SenseKey,&Sense.SenseCode,&Sense.SenseCodeQ);
	Sense.AdditionalLength		=	SendSize-8;


	UsbCoreDumpData("Bot Request Sense",epnum, (uint8 *)&Sense,SendSize);	


	/* Send the Packet */		
	if (UsbWriteData(DevNo,epnum,(uint8 *)&Sense,SendSize,NO_ZERO_LEN_PKT) != 0)				
	{
			TCRIT("Error in Sending SCSI_REQUEST_SENSE\n");
			return 1;
	}

	/*Compute Remaining Length */
	if (*RemainLen > SendSize)
		*RemainLen -= SendSize;
	else
		*RemainLen = 0;	
		
	BotSetScsiErrorCode (DevNo,ifnum,0,0,0);		 

	return 0;			
}

int
BotGetEventStatusNotification(uint8 DevNo, uint8 ifnum, uint8 epnum, uint32 *RemainLen, uint8 LunNo)
{
	uint32 SendSize;
	BOT_IF_DATA *MassData;

	/* Get Interface Data */	
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);	
	if (MassData == NULL)	
	{
		TCRIT("Error in Getting EventStatus Packet from Interface Data\n");
		return 1;
	}	

	/* Calculate the Size to be sent */	
	SendSize = (*RemainLen > sizeof(SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET))?
							sizeof(SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET):*RemainLen;

	(MassData->EventStatus[LunNo]).EventLength		= SendSize-2;
	(MassData->EventStatus[LunNo]).NotificationClass= 0x04;	// 100b as Media
	(MassData->EventStatus[LunNo]).SupportEventClass= 0xDE; // Refer MMC-6 Table 263-Notification Class Request file definition
															// Bit 0: Reserved
															// Bit 1: Operation Change
															// Bit 2: Power Management
															// Bit 3: External Request
															// Bit 4: Media
															// Bit 5: Muti-Host
															// Bit 6: device Busy
															// bit 7: Reserved
	(MassData->EventStatus[LunNo]).StartSlot		= 0x00;
	(MassData->EventStatus[LunNo]).EndSlot			= 0x00;

	UsbCoreDumpData("Bot Get Event Status Notification",epnum,(uint8 *)&(MassData->EventStatus[LunNo]),SendSize);	

	/* Send the Packet */		
	if (UsbWriteData(DevNo,epnum,(uint8 *)&(MassData->EventStatus[LunNo]),SendSize,NO_ZERO_LEN_PKT) != 0)				
	{
		TCRIT("Error in Sending SCSI_GET_EVENT_STATUS_NOTIFICATION\n");
		return 1;
	}
	/*Compute Remaining Length */
	if (*RemainLen > SendSize)
		*RemainLen -= SendSize;
	else
		*RemainLen = 0;	

	// Media Status Definition (MMC-6 Table 280)
	// 00b Door or Tray open
	// 01b Media Present

	// Media Event Format (MMC-6 Table 279)
	// 0h NoChg
	// 1h EjectRequest
	// 2h NewMedia
	// 3h MediaRemoval
	// 4h MediaChanged
	// 5h BgformatCompleted
	// 6h BgformatRestarted
	(MassData->EventStatus[LunNo]).EventCode = 0x00;
 
 	return 0;			
 }
 
static
int
BotReadCBW(uint8 DevNo,uint8 epnum,BOT_IF_DATA *MassData)
{
	COMMAND_BLOCK_WRAPPER *cbw;
	uint32 Len;	
	uint8 *RxData;
	/* Get the Received Data and Length */
	RxData  = UsbGetRxData(DevNo,epnum);
	Len 	= UsbGetRxDataLen(DevNo,epnum);	
		
	/* Validate the length */
	if (Len > MAX_CBW_LENGTH)
	{
		TWARN("WARNING:BotRxHandler(): Received more than Max CBW Size Len = %d ,ep = %d \n",Len,epnum);
		UsbCoreDumpData("Bot Read CBW",epnum,RxData,Len);	
		return 1;
	}	
	
	if ((RxData == NULL) || (Len <= 0))
	{
		TWARN("WARNING:BotReadCBW(): Receive Buffer is invalid or Len is Invalid. Len = %d, ep = %d \n", Len, epnum);
		return 1;
	}

	/* Copy the buffer to Interface memory */
	cbw	 =(COMMAND_BLOCK_WRAPPER *)(&(MassData->CBW[0]));
	memcpy(cbw,RxData,Len);

	/* Check the validitity of CBW */				
	if (usb_long(cbw->dCBWSignature) != CBW_SIGNATURE)
	{
		TWARN("WARNING:BotRxHandler(): Invalid CBW Signature. Len = %d\n",Len);
		return 1;
	}		

	/* Validate the length */	
	if (Len < 15 + cbw->bCBWCBLength)
	{
		TWARN("WARNING:BotRxHandler(): Received less data than the CBWCBLength specified in Command Block Wrapper\n");
		return 1;
	}		

	return 0;
}




static
void
BotQueueDPC(uint8 DevNo,uint8 ifnum)
{
	/* Raise the Flag */
	SetInterfaceFlag(DevNo,ifnum);

	/* Set Deffered Interrupt Processing */
	Usb_OsWakeupDPC();

	return;						
}

void
BotRxHandler(uint8 DevNo, uint8 ifnum,uint8 epnum)
{
	COMMAND_BLOCK_WRAPPER *cbw;
	BOT_IF_DATA *MassData;
	uint32 Len;	
	uint8 *RxData;
	SCSI_COMMAND_PACKET *ScsiPkt;

	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotRxHandler(): Enter\n");


	/* Get Interface Data */
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	if (MassData == NULL)
	{
		TWARN("WARNING:BotRxHandler(): GetInterfaceData returned NULL \n");
		return;
	}
	
	/* Check if we expect a CBW or some Data */
	if (!(MassData->ScsiDataOutLen))
	{
		/* Read CBW. If Error (1) return */
		if (BotReadCBW(DevNo,epnum,MassData) == 1)
			return;	
		
		/* Check if we will expect Data From Host */
		cbw	 =(COMMAND_BLOCK_WRAPPER *)(&(MassData->CBW[0]));
		if (!(cbw->bmCBWFlags & 0x80))		
		{
			MassData->ScsiDataOutLen =usb_long(cbw->dCBWDataTransferLength);
			if (MassData->ScsiDataOutLen)
			{
				/* Get the Scsi Command Packet */
				ScsiPkt = (SCSI_COMMAND_PACKET *)(&(cbw->CBWCB));
				
				if ((ScsiPkt->OpCode == SCSI_WRITE_10) || (ScsiPkt->OpCode == SCSI_WRITE_12))
				{
					if (g_video_callback.CallBackHandler != NULL)
					{
						g_video_callback.CallBackHandler(1);
					}
				}
				TDBG_FLAGGED(usbe, DEBUG_BOT,"We will recv Data from host for size 0x%x\n",
												MassData->ScsiDataOutLen);

				MassData->ScsiDataLen =0;
				return;
			}
		}
		
		/* @ Here we dont expect any Data , so start processing*/
		BotQueueDPC(DevNo,ifnum);
		return;			
	}
	
	/* Get the Received Data and Length */
	RxData  = UsbGetRxData(DevNo,epnum);
	Len 	= UsbGetRxDataLen(DevNo,epnum);	
	
	if ((RxData == NULL) || (Len <= 0))
	{
		TWARN("WARNING:BotReadCBW(): Receive Buffer is invalid. Len = %d, ep = %d \n", Len, epnum);
		return;
	}
	
	/* Copy the Data */
	memcpy(&MassData->ScsiData[MassData->ScsiDataLen],RxData,Len);		

	/* Adjust Lengths of ScsiDataLen and ScsiDataOutLen */	
	MassData->ScsiDataLen +=Len;
	if (Len <= MassData->ScsiDataOutLen)
		MassData->ScsiDataOutLen -=Len;
	else
		MassData->ScsiDataOutLen = 0;

	/* All Data Received. Start Processing */
	if (MassData->ScsiDataOutLen == 0)
	{
		if (g_video_callback.CallBackHandler != NULL)
		{
			g_video_callback.CallBackHandler(0);
		}
		BotQueueDPC(DevNo,ifnum);
	}
	return;						
}

void
BotProcessHandler(uint8 DevNo, uint8 ifnum)
{
	COMMAND_BLOCK_WRAPPER 	*cbw;
	BOT_IF_DATA 			*MassData;
	SCSI_COMMAND_PACKET		*ScsiPkt;
	int EmulateFloppy = 0;
	uint32 DataTransferLen;
	uint8  epDataIn; 
	//uint8  epDataOut;
		

	TDBG_FLAGGED(usbe, DEBUG_BOT,"BotProcessHandler(): Enter\n");	


	/* Get the Endpoint to be used for Data In (Transfer to HOST) */	
	epDataIn = GetDataInEp(DevNo,ifnum);	
	
	/* Get the Endpoint to be used for Data Out (Transfer to DEVICE) */	
	//epDataOut = GetDataOutEp(DevNo,ifnum);	
		
	/* Get the Interface Specific Data */						
	MassData =(BOT_IF_DATA *)GetInterfaceData(DevNo,ifnum);
	if (MassData == NULL)
		return;
	cbw	=(COMMAND_BLOCK_WRAPPER *)(&(MassData->CBW[0]));	
	
	/* Get the Scsi Command Packet */
	ScsiPkt = (SCSI_COMMAND_PACKET *)(&(cbw->CBWCB));
	
	/* Dump the CBW Data */
	UsbCoreDumpData("Bot CBW",epDataIn,(uint8 *)cbw,MAX_CBW_LENGTH); 
	
	/* Get the length to be transfered */
	DataTransferLen=usb_long(cbw->dCBWDataTransferLength);
	
	/* Switch according to SCSI Opcode */
	switch (ScsiPkt->OpCode)
	{
		case	SCSI_INQUIRY:
			
			TDBG_FLAGGED(usbe, DEBUG_BOT,"BotProcessHandler(): SCSI_INQUIRY\n");
			if (cbw->bmCBWFlags == 0x00) 
			{
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,0);
				break;
			}
			if (BotInquiry(DevNo,ifnum,epDataIn,&DataTransferLen,cbw->bCBWLUN) == 1) 
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
			else				
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,DataTransferLen);
			break;
			
		case	SCSI_REQUEST_SENSE:									

			TDBG_FLAGGED(usbe, DEBUG_BOT,"BotProcessHandler(): SCSI_REQUEST_SENSE\n");

			if (cbw->bmCBWFlags == 0x00) 
			{
			
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,0);
				break;
			}

			if (BotSense(DevNo,ifnum,epDataIn,&DataTransferLen) == 1) 
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
			else				
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,DataTransferLen);
			break;

		case SCSI_GET_EVENT_STATUS_NOTIFICATION:

			TDBG_FLAGGED(usbe, DEBUG_BOT,"BotProcessHandler(): SCSI_GET_EVENT_STATUS_NOTIFICATION\n");
			if (cbw->bmCBWFlags == 0x00) 
			{
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,0);
				break;
			}

			if (BotGetEventStatusNotification(DevNo,ifnum,epDataIn,&DataTransferLen,cbw->bCBWLUN) == 1) 
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
			else				
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,DataTransferLen);
			break;

		case SCSI_MEDIUM_REMOVAL:
		case SCSI_SEEK:
		case SCSI_SYNC_CACHE:
		case SCSI_SET_CD_SPEED:
				BotSendCSW(DevNo, ifnum, cbw->dCBWTag, CSW_PASSED, DataTransferLen);
			break;

		case    SCSI_READ_FORMAT_CAPACITIES:

			TDBG_FLAGGED(usbe, DEBUG_BOT,"read format capacities recd for ifnum %d which is a ",ifnum);

			if (DevInfo[DevNo].UsbCoreDev.DevUsbIsEmulateFloppy)
				EmulateFloppy = DevInfo[DevNo].UsbCoreDev.DevUsbIsEmulateFloppy(cbw->bCBWLUN);
			
			if( !IsCDROM(DevNo, ifnum , cbw->bCBWLUN ))
			{
				TDBG_FLAGGED(usbe, DEBUG_BOT,"HDISK\n");
				if (EmulateFloppy)
				{	
					//sending 1.44 MB always
					if (BotFormatCaps(DevNo,ifnum,epDataIn,&DataTransferLen) == 1) 
						BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
					else				
						BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,DataTransferLen);
					break;
				}
			}
			else
			{
				TDBG_FLAGGED(usbe, DEBUG_BOT,"CDROM\n");
			}
			
		default:
			if ((ScsiPkt->OpCode == SCSI_TEST_UNIT_READY) && (DataTransferLen))
			{
				BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
				break;
			}
			if (DevInfo[DevNo].UsbCoreDev.DevUsbRemoteScsiCall(DevNo,ifnum,MassData, cbw->bCBWLUN))
			{
			
				if (ScsiPkt->OpCode == SCSI_MEDIUM_REMOVAL) 
				{ 
					// Sending success for SCSI_MEDIUM_REMOVAL cmd.
					// This fix is added to make linux happy and not 
					// continuously log msg "Device not ready, Make sure 
					// there is a disc in the drive"
					BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_PASSED,DataTransferLen);
				}
				else 
					BotSendCSW(DevNo,ifnum,cbw->dCBWTag,CSW_FAILED,DataTransferLen);
			}
			/* If the above returns 0, that means remote IO is active and 
			   CSW will be sent later*/
			break;

	}
	
	return;
	
}	



					
