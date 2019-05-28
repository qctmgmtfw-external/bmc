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
#include <coreTypes.h>
#include "bot.h"
#include "scsi.h"
#include "iusb-scsi.h"

#define MAX_CDROM_INSTANCE	4

/* Static Variables */
static uint8 iUsbCdromInstance [MAX_CDROM_INSTANCE] = { 0, 0, 0, 0 };
static uint8 iUsbCdromActive [MAX_CDROM_INSTANCE] = { 0, 0, 0, 0 };
static uint8 iUsbCdromWakeup [MAX_CDROM_INSTANCE] = { 0, 0, 0, 0 };
static IUSB_SCSI_PACKET *iUsbCdromReqPkt [MAX_CDROM_INSTANCE] = { NULL, NULL, NULL, NULL };
static wait_queue_head_t cd_req_wait [MAX_CDROM_INSTANCE]; 
static char CdKernelModePkt [MAX_CDROM_INSTANCE][sizeof(IUSB_SCSI_PACKET)+MAX_SCSI_DATA];
static int FirstTime = 1;

int
GetCdromInstanceIndex (void)
{
	int Instance;
	
	if (FirstTime)
	{
		FirstTime = 0;
		for (Instance = 0; Instance < MAX_CDROM_INSTANCE; Instance++)
			init_waitqueue_head(&cd_req_wait[Instance]);
	}
	for (Instance = 0; Instance < MAX_CDROM_INSTANCE; Instance++)
	{
		if (0 == iUsbCdromInstance [Instance])
		{
			iUsbCdromInstance [Instance] = 1;
			return Instance;
		}
	}
	TWARN ("GetCdromInstanceIndex(): No available hard disk Instances\n");
	return -1;			
}

int
ReleaseCdromInstanceIndex (uint8 Instance)
{

	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("ReleaseCdromInstanceIndex(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}
	if (FirstTime == 0)
	{
		iUsbCdromInstance [Instance] = 0;
		if (iUsbCdromActive[Instance])
		{
			iUsbCdromActive[Instance] = 0; 
	    	wake_up(&cd_req_wait[Instance]);
			iUsbCdromReqPkt[Instance] = NULL;
		}
		return 0;
	}
	return -1;
}

int 
CdromCheckSupportedCmd(uint8 DevNo,uint8 ifnum,uint8 Command, BOT_IF_DATA *MassData, uint8 Instance)
{

	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromCheckSupportedCmd(): Invalid Instance index %d received\n", Instance); 
		return 0;
	}

	switch (Command)
	{
		/* List of Commands supported from Remote CDROM */
		case	SCSI_TEST_UNIT_READY:
		case	SCSI_READ_CAPACITY:
		case	SCSI_READ_10:
		case 	SCSI_READ_TOC:
		case	SCSI_START_STOP_UNIT:
		case	SCSI_MEDIUM_REMOVAL:
		case	SCSI_READ_12:
				break;


		/* List of known Commands not supported from Remote CDROM */
		case	SCSI_MODE_SENSE:					
		case	SCSI_MODE_SELECT:					
		case	SCSI_READ_FORMAT_CAPACITIES:		
		case	SCSI_SEEK:						
							
									
		/* No CD-RW as of now. So the following commands are not needed */
		case	SCSI_VERIFY:				
		case  	SCSI_FORMAT_UNIT:
		case	SCSI_WRITE_10:						
		case 	SCSI_WRITE_12:						
		case 	SCSI_WRITE_AND_VERIFY:				
			
		/* All other unknown unsupported commands */
		default:	
			if(ISAMICMD(Command))
			{
				/*Send 1 saying supported command*/
				return 1;
			}	
			//printk("CheckCdromSupportedCmd(): SCSI Unsupported Command 0x%x for Instance %d\n",Command, Instance);				
			UsbCore.CoreUsbBotSetScsiErrorCode (DevNo,ifnum,0x05,0x20,0x00);
			return 0;
	}

	if (!iUsbCdromActive[Instance])
	{
		UsbCore.CoreUsbBotSetScsiErrorCode (DevNo,ifnum, 0x02,0x3A,0x00);

		if (((SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET *)&(MassData->EventStatus[Instance]))->MediaStatus == 0x02)  // Media Present
			UsbCore.CoreUsbBotSetScsiEventStatus (DevNo,ifnum,Instance, 0x03, 0x01);
		return 0;
	}

	return 1;

}

IUSB_SCSI_PACKET *
CdromGetiUsbPacket(uint8 DevNo,uint8 ifnum, uint8 Instance)
{
	
	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromGetiUsbPacket(): Invalid Instance index %d received\n", Instance); 
		return NULL;
	}
	return iUsbCdromReqPkt[Instance];
}

int 
CdromRemoteCall(IUSB_SCSI_PACKET *iUsbScsiPkt,uint32 PktLen, uint8 Instance)
{
	/* Wakeup CdromRequest to notify Request Packet is ready*/
	TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
		"RemoteCdromCall(): Waking up CdromRequest for Instance %d\n", Instance);

	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromRemoteCall(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}

	iUsbCdromWakeup[Instance] = 1;
    wake_up(&cd_req_wait[Instance]);
	return 0;
}


int CdromRemoteActivate(uint8 Instance)
{
	TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
		"CdromRemoteActivate(): Cdrom Remote Activate for Instance %d\n", Instance);
	
	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromRemoteActivate(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}

	iUsbCdromActive[Instance] = 1;
	iUsbCdromWakeup[Instance] = 0; 
	iUsbCdromReqPkt[Instance] = (IUSB_SCSI_PACKET *)CdKernelModePkt[Instance];
	return 0;
}

int CdromRemoteDeactivate(uint8 Instance)
{

	TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
		"CdromRemoteDeactivate(): Cdrom Remote Deactivate for Instance %d\n", Instance);
	
	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromRemoteDeactivate(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}
	iUsbCdromActive[Instance] = 0; 
    wake_up(&cd_req_wait[Instance]);
	iUsbCdromReqPkt[Instance] = NULL;
	return 1;
}
                              
int
CdromRemoteWaitRequest(IUSB_SCSI_PACKET *UserModePkt, uint8 Instance)
{
    int retval;

	
	if (Instance >= MAX_CDROM_INSTANCE)
	{
		TWARN ("CdromRemoteWaitRequest(): Invalid Instance index %d received\n", Instance); 
		return -EINVAL;
	}
	
	if(iUsbCdromActive[Instance] == 0)
	{
		TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
			"Cdrom wait request called without activating first for Instance %d!!\n", Instance);
	}                                             
		
	TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
		"CdromRequest:Sleep Entering for Instance %d\n", Instance);

    retval = wait_event_interruptible(cd_req_wait[Instance],(iUsbCdromWakeup[Instance] || !iUsbCdromActive[Instance]));
    if(retval)
    {
        TWARN("wait for cdrom request was disturbed by an interrupt please restart system call for Instance %d\n", Instance);
        return  -EINTR;
    }

	iUsbCdromWakeup[Instance] = 0; //reset wakeup to 0 so that someone can wake us up again when we wait.
							 //no contention problems here because we are under cli
    if(!iUsbCdromActive[Instance])
    {
        TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM, 
			"returning because iUSB CDROM is not active for Instance %d\n", Instance);
        return -EPIPE;
    }

	/* Got a Request Packet. Invaildate internal structure  */
	TDBG_FLAGGED(iusb, DEBUG_IUSB_CDROM,
					"CdromRequest:Sleep Leaving for Instance %d\n", Instance);
	
	/* Copy from kernel data area to user data area*/
	if( __copy_to_user((void *)UserModePkt,(void *)(CdKernelModePkt[Instance]),sizeof(IUSB_SCSI_PACKET)))
	{
		TWARN ("CdromRemoteWaitRequest():__copy_to_user failed for Instance %d\n", Instance);
		return -EFAULT;
	}
	
	iUsbCdromReqPkt[Instance] =  (IUSB_SCSI_PACKET *)CdKernelModePkt[Instance];       

	if(iUsbCdromReqPkt[Instance]->DataLen > 0)   /*if Data needs to be Xfered */
	{
		if(__copy_to_user((void *)(&(UserModePkt->Data)),
			(void *)(&(iUsbCdromReqPkt[Instance]->Data)),iUsbCdromReqPkt[Instance]->DataLen))
		{
			TWARN ("CdromRemoteWaitRequest():__copy_to_user failed for Instance %d\n", Instance);
			return -EFAULT;
		}
	}
	return 0;
}

