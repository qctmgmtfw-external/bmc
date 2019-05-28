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
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <coreTypes.h>

#include "bot.h"
#include "scsi.h"
#include "iusb.h"
#include "usb_core.h"
#include "usb_hw.h"
#include "iusb-scsi.h"
#include "iusb-cdrom.h"
#include "iusb-hdisk.h"
#include "iusb-vendor.h"
#include "iusb-hid.h"
#include "mod_reg.h"


extern video_callback	g_video_callback;

enum ERRORCASE {
	NO_ERRORCASE = 0,
	ERRORCASE_1,
	ERRORCASE_2,
	ERRORCASE_3,
	ERRORCASE_4,
	ERRORCASE_5,
	ERRORCASE_6,
	ERRORCASE_7,
	ERRORCASE_8,
	ERRORCASE_9,
	ERRORCASE_10,
	ERRORCASE_11,
	ERRORCASE_12,
	ERRORCASE_13,
};
static enum ERRORCASE TestCase = NO_ERRORCASE;
static uint8 CBWErrorOccur = 0;
static uint32 DataOutLen = 0;

/* Global Variables */
USB_CORE UsbCore;
TDBG_DECLARE_DBGVAR(iusb);

/* Function Prototypes */
static int 	iusb_notify_sys(struct notifier_block *this, unsigned long code, void *unused);
static int RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo, uint8 Instance);
static int ScsiSendResponse(IUSB_SCSI_PACKET *UserModePkt);

/* Static Variables */
static struct notifier_block iusb_notifier =
{
       .notifier_call = iusb_notify_sys,
};

static struct ctl_table_header *my_sys 	= NULL;
static struct ctl_table iUSBctlTable [] =
{
#if (LINUX_VERSION_CODE >=  KERNEL_VERSION(3,4,11))
	{"DebugLevel",&(TDBG_FORM_VAR_NAME(iusb)),sizeof(int),0644,NULL,&proc_dointvec,NULL},
#else
	{CTL_UNNUMBERED,"DebugLevel",&(TDBG_FORM_VAR_NAME(iusb)),sizeof(int),0644,NULL,NULL,&proc_dointvec}, 
#endif
	{0} 
};

static char gKernelModePktCD[sizeof(IUSB_SCSI_PACKET)+MAX_SCSI_DATA];	
static char gKernelModePktHD[sizeof(IUSB_SCSI_PACKET)+MAX_SCSI_DATA];	
static USB_IUSB iUSB = {
				.iUSBScsiSendResponse 		= ScsiSendResponse,
				.iUSBCdromRemoteActivate	= CdromRemoteActivate,
				.iUSBCdromRemoteWaitRequest = CdromRemoteWaitRequest,
				.iUSBCdromRemoteDeactivate	= CdromRemoteDeactivate,
				.iUSBHdiskRemoteActivate	= HdiskRemoteActivate,
				.iUSBHdiskRemoteWaitRequest	= HdiskRemoteWaitRequest,
				.iUSBHdiskRemoteDeactivate	= HdiskRemoteDeactivate,
				.iUSBVendorWaitRequest		= VendorWaitRequest,
				.iUSBVendorExitWait			= VendorExitWait,
				.iUSBRemoteScsiCall			= RemoteScsiCall,
				.iUSBKeybdLedWaitStatus		= KeybdLedWaitStatus,
				.iUSBKeybdExitWait			= KeybdExitWait,
				.iUSBKeybdLedRemoteCall		= KeybdLedRemoteCall,
				.iUSBKeybdPrepareData		= KeybdPrepareData,
				.iUSBMousePrepareData		= MousePrepareData,
				.iUSBGetHdiskInstanceIndex  = GetHdiskInstanceIndex,
				.iUSBGetCdromInstanceIndex  = GetCdromInstanceIndex,
				.iUSBReleaseHdiskInstanceIndex = ReleaseHdiskInstanceIndex,
				.iUSBReleaseCdromInstanceIndex = ReleaseCdromInstanceIndex,
				
				};



int 
get_iusb_funcs (USB_IUSB *iUSBFuncs)
{
    memcpy (iUSBFuncs, &iUSB, sizeof (USB_IUSB));
	return 0;
}

uint32
FormiUsbScsiPacket(IUSB_SCSI_PACKET *iUsbScsiPkt,uint8 DevNo, uint8 IfNum,BOT_IF_DATA *MassData,uint8 LunNo)
{
	COMMAND_BLOCK_WRAPPER 	*cbw;
	uint32 DataPktLen;
	uint8 DeviceType;

	CBWErrorOccur = 0;

	DeviceType = (MassData->Inquiry[LunNo].RMB) | (MassData->Inquiry[LunNo].PeripheralType);

	/* Get Command Block Wrapper of BOT */
	cbw	=(COMMAND_BLOCK_WRAPPER *)(&(MassData->CBW[0]));	

	/* Tag Number is same as that passed in CBW by Host */
	iUsbScsiPkt->TagNo			= cbw->dCBWTag;		

    /* Copy the Scsi Command Packet in CBW (Max 16 Bytes) */
        memcpy(&(iUsbScsiPkt->CommandPkt),&(cbw->CBWCB),16);
	/* Set Data According to the Direction */	
	if ((cbw->bmCBWFlags & 0x80) || (!(cbw->dCBWDataTransferLength)))
	{	/* Read Data From Remote Device */
		TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"Direction: Data Read from Device\n");
		if (iUsbScsiPkt->CommandPkt.OpCode == SCSI_WRITE_10) 
		{ 
			/*this case isn't allowed*/
			CBWErrorOccur =1;
		} 
		else 
		{
			iUsbScsiPkt->DataDir		= READ_DEVICE;
			iUsbScsiPkt->ReadLen		= cbw->dCBWDataTransferLength;	
			iUsbScsiPkt->DataLen		= usb_long(0);
			
			if (iUsbScsiPkt->CommandPkt.OpCode == SCSI_READ_10) 
			{
				if ((DeviceType & 0x7F)== IUSB_DEVICE_HARDDISK)
					DataOutLen = ((cbw->CBWCB[7] << 8) | cbw->CBWCB[8]) * 0x200;
				else if ((DeviceType & 0x7F)== IUSB_DEVICE_CDROM)
					DataOutLen = ((cbw->CBWCB[7] << 8) | cbw->CBWCB[8]) * 0x800;

				if (DataOutLen !=  iUsbScsiPkt->ReadLen)
					CBWErrorOccur =1;
						
			}
		}
	}		
	else
	{	
		if (iUsbScsiPkt->CommandPkt.OpCode == SCSI_READ_10)
		{ /* this case isn't allowed*/
			CBWErrorOccur =1;
		}
		else
		{
			/* Write Data to Remote Device */
			TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,
						 "Direction: Data Write To Device of Size 0x%x\n",MassData->ScsiDataLen);
			iUsbScsiPkt->DataDir		= WRITE_DEVICE;		
			iUsbScsiPkt->ReadLen		= 0;
			iUsbScsiPkt->DataLen		= usb_long(MassData->ScsiDataLen);
			memcpy(&(iUsbScsiPkt->Data),&(MassData->ScsiData),MassData->ScsiDataLen);	
			
			if (iUsbScsiPkt->CommandPkt.OpCode == SCSI_WRITE_10)
			{
				if ((DeviceType & 0x7F)== IUSB_DEVICE_HARDDISK)
					DataOutLen = ((cbw->CBWCB[7] << 8) | cbw->CBWCB[8]) * 0x200;
				else if ((DeviceType & 0x7F)== IUSB_DEVICE_CDROM)
					DataOutLen = ((cbw->CBWCB[7] << 8) | cbw->CBWCB[8]) * 0x800;
			
				if (DataOutLen != iUsbScsiPkt->DataLen)
					CBWErrorOccur = 1;
			}
		}
	}		

	
	if (CBWErrorOccur) 
	{
		if (!(cbw->dCBWDataTransferLength)) 
		{ 
			// case 2 3
			if (iUsbScsiPkt->DataDir == WRITE_DEVICE)
				TestCase = ERRORCASE_3;
			else
				TestCase = ERRORCASE_2;
			CBWErrorOccur = 2;
		} 
		else if (cbw->bmCBWFlags & 0x80) 
		{  
			// case 4 5 7 8
			if (!DataOutLen) 
			{
				TestCase = ERRORCASE_4;
				DataOutLen = cbw->dCBWDataTransferLength;
			} 
			else if (cbw->dCBWDataTransferLength > DataOutLen)
			{
				DataOutLen = ERRORCASE_5;
				DataOutLen = cbw->dCBWDataTransferLength - DataOutLen;
			}
			else if (cbw->dCBWDataTransferLength < DataOutLen)
			{
				TestCase = ERRORCASE_7;
				CBWErrorOccur = 2;
			} 
			else
			{
				TestCase = ERRORCASE_8;
				CBWErrorOccur = 2;
			}
		} 
		else 
		{ 
			// case 9 10 11 13
			if (!DataOutLen) 
			{
				TestCase = ERRORCASE_9;
				DataOutLen = cbw->dCBWDataTransferLength;
			} 
			else if (cbw->dCBWDataTransferLength > DataOutLen)
			{
				TestCase = ERRORCASE_11;
				DataOutLen = cbw->dCBWDataTransferLength - DataOutLen;
			}
			else if (cbw->dCBWDataTransferLength < DataOutLen)
			{
				TestCase = ERRORCASE_13;
				CBWErrorOccur = 2;
			}
			else
			{
				TestCase = ERRORCASE_10;
				CBWErrorOccur = 2;
			}
		}
	}
	/* Fill the iUSB Header */
	DataPktLen = sizeof(IUSB_SCSI_PACKET)-1+usb_long(iUsbScsiPkt->DataLen);
	UsbCore.CoreUsbFormiUsbHeader(&(iUsbScsiPkt->Header),DataPktLen-sizeof(IUSB_HEADER),
				DevNo,IfNum,
				(MassData->Inquiry[LunNo].RMB) | (MassData->Inquiry[LunNo].PeripheralType),
				IUSB_PROTO_SCSI);

	MassData->LastSeqNo = iUsbScsiPkt->Header.SeqNo;		
	return DataPktLen;
}

#if 0
static void
MaintainMediaInserted(IUSB_SCSI_PACKET *Pkt)
{
	if( ((Pkt->Header.DeviceType & 0x7F) == IUSB_DEVICE_CDROM)   && (Pkt->CommandPkt.OpCode == SCSI_TEST_UNIT_READY)   )
	{

			/*if current is media*/
			if( (Pkt->StatusPkt.SenseKey == 0x00) && (Pkt->StatusPkt.SenseCode == 0x00) && (Pkt->StatusPkt.SenseCodeQ == 0x00) )
			{
				/*if previous was  no media*/
				if(  (gPrev_SenseKey != 0x00) || (gPrev_SenseCode != 0x00) || (gPrev_SenseCodeQ != 0x00)		)
				{
					printk("No Media to media change\n");
					gMediaInserted = 1;
				}
			}
					
		gPrev_SenseKey = Pkt->StatusPkt.SenseKey;
		gPrev_SenseCode = Pkt->StatusPkt.SenseCode;
	        gPrev_SenseCodeQ = Pkt->StatusPkt.SenseCodeQ;

	}
}
#endif


static void
DumpData(char *Title, uint8 ep, uint8 *Data, uint32 Len)
{
	int i;

	if (TDBG_IS_DBGFLAG_SET (iusb, DEBUG_IUSB_DUMP_DATA))
	{
		printk("------- %s --------%d Bytes for ep %d\n",Title,Len,ep);
		for (i=0;i<Len;i++)
			printk("0x%02X ",Data[i]);
		printk("\n");
	}

}

/* 
 * IMPORTANT:
 * WARNING:  Don't define more than one BOT devices at a time. The following buffer
 * buffer is global and if two BOT devices issue this ioctl at the same time, it 
 * will be corrupted.
 * TODO: Protect this buffer by locking/sleep/wakeup. But this will delay transmission
 * which may cause USB errors. Another way is keep seperate buffers for each BOT 
 * device or a pool of buffers for all BOT devices
 */
int
ScsiSendResponse(IUSB_SCSI_PACKET *UserModePkt)
{
	uint8 OverallStatus; 
	uint8 *DataPkt;
	uint32 SendSize;
	uint32 RemainLen;
	int RetVal;
	int err;
	int SendVal=0;
	BOT_IF_DATA *MassData;
	
	IUSB_SCSI_PACKET *Pkt;
	
	TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"ScsiSendResponse: Entering\n");
	
	/* Copy from user data area to kernel data area*/
	if((UserModePkt->Header.DeviceType & 0x7F) ==   IUSB_DEVICE_CDROM)
	{
		
		Pkt = (IUSB_SCSI_PACKET*) &gKernelModePktCD[0];
	}
	else
	{
		
		Pkt = (IUSB_SCSI_PACKET*)&gKernelModePktHD[0];
	}
	
	
//	__copy_from_user((void *)(Pkt),(void *)UserModePkt,sizeof(IUSB_SCSI_PACKET)+MAX_SCSI_DATA);

	err=__copy_from_user((void *)(Pkt),(void *)UserModePkt,sizeof(IUSB_SCSI_PACKET));
	SendSize		= usb_long(Pkt->DataLen);
	err=__copy_from_user((void *)(&(Pkt->Data)),(void *)&(UserModePkt->Data),SendSize);
	err=err; // To avoid unused error
	

/*	MaintainMediaInserted(Pkt);*/

	MassData =(BOT_IF_DATA *)UsbCore.CoreUsbGetInterfaceData(Pkt->Header.DeviceNo,
												Pkt->Header.InterfaceNo);
	if (MassData == NULL)
		return 1;

	/* Check if the seqno matches */	
	if (MassData->LastSeqNo != Pkt->Header.SeqNo)
	{
		TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"SeqNo Mismatch: Due to delayed packet and device reset\n");
		return 0; 
	}		

	/* Clear the Stored last seq no, to indicate we have received pkt */
	MassData->LastSeqNo = 0;

	/* Display Message for Remote Unsupported Commands */
	if ((Pkt->StatusPkt.SenseKey==0x05) && (Pkt->StatusPkt.SenseCode==0x20) 
										&& (Pkt->StatusPkt.SenseCodeQ==0x00))
	{
		switch (Pkt->Header.DeviceType & 0x7F)  
		{
			case IUSB_DEVICE_CDROM:
				TWARN("Unsupported Remote Cdrom Command 0x%x\n",
											Pkt->CommandPkt.OpCode);
				break;
			case IUSB_DEVICE_HARDDISK:
				TWARN("Unsupported Remote Harddisk Command 0x%x\n",
											Pkt->CommandPkt.OpCode);
				break;
			default:
				TWARN("Unsupported Remote Command 0x%x\n",
											Pkt->CommandPkt.OpCode);
				break;
		}
	}
		

	/* Get the Status Values */
	OverallStatus 	= Pkt->StatusPkt.OverallStatus;
	UsbCore.CoreUsbBotSetScsiErrorCode (Pkt->Header.DeviceNo,Pkt->Header.InterfaceNo,
					 		Pkt->StatusPkt.SenseKey,Pkt->StatusPkt.SenseCode,
							Pkt->StatusPkt.SenseCodeQ);
	
	if (CBWErrorOccur == 1)
	{
		RemainLen = usb_long(DataOutLen); 
		CBWErrorOccur = 0;
		TestCase = NO_ERRORCASE;
		return UsbCore.CoreUsbBotSendCSW(Pkt->Header.DeviceNo,Pkt->Header.InterfaceNo,
					Pkt->TagNo,CSW_FAILED,RemainLen);
	}
    /*patch for microsoft bug*/
	/*if cd handle is open when no media and then cd is inserted explorer dislikes*/
    /*to make explorer happy close handle and open again*/
	/*so handle opener needs to know if media has been inserted*/

	/* Data Sent to Remote. We just has to send CSW to Host */
	if (Pkt->DataDir == WRITE_DEVICE)
	{
//		if (OverallStatus == CSW_FAILED)
		return UsbCore.CoreUsbBotSendCSW(Pkt->Header.DeviceNo,Pkt->Header.InterfaceNo,
						Pkt->TagNo,(OverallStatus)?CSW_FAILED:CSW_PASSED,0);
	}

	/* At this point, we know some Data might have been Received from Remote */
	
	/* Get Data Size and Buffer to be Sent to Host*/
	SendSize		= usb_long(Pkt->DataLen);
	DataPkt			= &(Pkt->Data); 
	RemainLen		= usb_long(Pkt->ReadLen);  

	/* Send the SCSI response packet, if any */
	if ((SendSize > 0) && (OverallStatus == 0))
	{
		TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"Sending Data Packet to host for 0x%x of Size %d\n",Pkt->CommandPkt.OpCode,SendSize);	
		if (Pkt->CommandPkt.OpCode != SCSI_READ_10)
			DumpData("Data",0xFF,DataPkt,SendSize);	
		if ((Pkt->CommandPkt.OpCode == SCSI_READ_10) || (Pkt->CommandPkt.OpCode == SCSI_READ_12)) 
		{
			if (g_video_callback.CallBackHandler != NULL)
				g_video_callback.CallBackHandler (1);
		}
		/* Send the Packet */		
		if (UsbCore.CoreUsbBotSendData(Pkt->Header.DeviceNo,Pkt->Header.InterfaceNo,
								(uint8 *)&Pkt->Data,SendSize,&RemainLen) == 1)
		{
			TCRIT("Error in Sending Data for 0x%x\n",Pkt->CommandPkt.OpCode);
			SendVal = 1;  /* return 1; */
		}
			
	}

#if 0		
/* 
   This else is Meaningless and error , because the control comes here if 
   SendSize is zero or Overallstatus is non zero. In both cases we don't 
   send any data and the RemainLen should remain the same. So removing the 
   else will solve the problem.
*/
	else
	{
		//Compute Remaining Length 
		if (RemainLen > SendSize)
			RemainLen -= SendSize;
		else
			RemainLen = 0;	
	}			
#endif

	TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"Sending CSW for 0x%x\n",Pkt->CommandPkt.OpCode);

	/* Send CSW Depending upon Overall Status */
	RetVal =UsbCore.CoreUsbBotSendCSW(Pkt->Header.DeviceNo,Pkt->Header.InterfaceNo,
				Pkt->TagNo,(OverallStatus)?CSW_FAILED:CSW_PASSED,RemainLen);	

	if ((Pkt->CommandPkt.OpCode == SCSI_READ_10) || (Pkt->CommandPkt.OpCode == SCSI_READ_12)) 
	{
		if (g_video_callback.CallBackHandler != NULL)
			g_video_callback.CallBackHandler (0);
	}
	TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"ScsiSendResponse: Leaving \n");

	return (RetVal|SendVal);	/* Return Combined Error Status */
}

static int  		
RemoteScsiCall(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo, uint8 Instance)
{

	uint8 DeviceType;
	uint32 PktLen;
	COMMAND_BLOCK_WRAPPER 	*cbw;
	SCSI_COMMAND_PACKET 	*ScsiPkt;
	IUSB_SCSI_PACKET		*iUsbScsi;
	uint32 RemainLen;

	TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"RemoteScsiCall(): Entering\n");
	/* Find what device it is */
	DeviceType = (MassData->Inquiry[LunNo].RMB) | (MassData->Inquiry[LunNo].PeripheralType);

	/* Get Command Block Wrapper of BOT */
	cbw	=(COMMAND_BLOCK_WRAPPER *)(&(MassData->CBW[0]));	
	
	/* Get the Scsi Command Packet */
	ScsiPkt = (SCSI_COMMAND_PACKET *)(&(cbw->CBWCB));
	
/*********************** Device = CDROM **************************************/
	if ((DeviceType & 0x7F)== IUSB_DEVICE_CDROM)
	{
		/* Check if the command is supported from Remote And if the Remote is enabled*/			
		if (!CdromCheckSupportedCmd(DevNo,ifnum,ScsiPkt->OpCode,MassData, Instance))
			return 1;

		/***Check for AMI commands***/
		if(ISAMICMD(ScsiPkt->OpCode))
		{
			return ProcessVendorCommands(DevNo,ifnum,MassData,ScsiPkt->Lun);
		}

		/**Non AMI Commands**/
		/* Get the iUsbScsi Packet Memory */
		iUsbScsi = CdromGetiUsbPacket(DevNo,ifnum, Instance);
		if (iUsbScsi == NULL)
		{
			TCRIT("CDROM Unable to get iUsb packet\n");
			return 1;
		}
		
        /* Form the packet on the given packet memory */
        PktLen=FormiUsbScsiPacket(iUsbScsi,DevNo,ifnum,MassData,LunNo);
        if(PktLen == 1)
            return 1;
		/* Issue Remote Cdrom Call */
		return CdromRemoteCall(iUsbScsi,PktLen, Instance);
	}		
/****************************************************************************/
	
/*********************** Device = HARDDISK **********************************/
	if ((DeviceType & 0x7F)== IUSB_DEVICE_HARDDISK)
	{
		/* Check if the command is supported from Remote And if the Remote is enabled*/			
		if (!HdiskCheckSupportedCmd(DevNo,ifnum,ScsiPkt->OpCode,MassData, Instance))
			return 1;

		/* Get the iUsbScsi Packet Memory */
		iUsbScsi = HdiskGetiUsbPacket(DevNo,ifnum, Instance);
		if (iUsbScsi == NULL)
		{
			TCRIT("HDISK Unable to get iUsb packet\n");
			return 1;
		}
		
		/* Form the packet on the given packet memory */
        PktLen=FormiUsbScsiPacket(iUsbScsi,DevNo,ifnum,MassData,LunNo);
        if(PktLen == 1)
            return 1;
		
		if (CBWErrorOccur == 2)
		{
			 CBWErrorOccur =0;
			 TestCase = NO_ERRORCASE;
			if (iUsbScsi->DataDir == WRITE_DEVICE)
				RemainLen		= usb_long(iUsbScsi->DataLen);
			else
				RemainLen		= usb_long(iUsbScsi->ReadLen);
			UsbCore.CoreUsbBotSendCSW(iUsbScsi->Header.DeviceNo,iUsbScsi->Header.InterfaceNo,
					iUsbScsi->TagNo,CSW_PHASE_ERROR,RemainLen);
			return 0;
		}
		
		/* Issue Remote Hdisk Call */
		return HdiskRemoteCall(iUsbScsi,PktLen, Instance);
	}		
/****************************************************************************/
	
/* Write Code for all supported devices here */	


/************************* Device = UNKNOWN *********************************/
	/* Unknown Device - Send Invalid Command Code */				
	TEMERG("NO WAY IAM HERE !!!!!!\n");
	UsbCore.CoreUsbBotSetScsiErrorCode (DevNo,ifnum,0x05,0x20,0x00);	
	return 1;
/****************************************************************************/		
}


static int
init_iusb_module (void)
{
	
	printk ("Loading iUSB Module...\n");
	if (0 != get_usb_core_funcs (&UsbCore))
	{
		TCRIT ("Error: Unable to get Core Module Functions...\n");
		return -1;
	}
	register_reboot_notifier (&iusb_notifier);
	my_sys = AddSysctlTable("iusb",&iUSBctlTable[0]);
	return 0;
}

static void
exit_iusb_module (void)
{
	printk ("Unloading iUSB module...\n");
	unregister_reboot_notifier (&iusb_notifier);
	if (my_sys) RemoveSysctlTable(my_sys);
	return;
}

static int
iusb_notify_sys(struct notifier_block *this, unsigned long code, void *unused)
{
	TDBG_FLAGGED(iusb, DEBUG_IUSB_SCSI,"iUSB module Reboot notifier...\n");
	if (code == SYS_DOWN || code == SYS_HALT)
	{
	   
	}
	return NOTIFY_DONE;
}


module_init(init_iusb_module);
module_exit(exit_iusb_module);

MODULE_AUTHOR("Rama Rao Bisa <RamaB@ami.com>");
MODULE_DESCRIPTION("iUSB module");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL (get_iusb_funcs);


