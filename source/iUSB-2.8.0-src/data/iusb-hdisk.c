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
#include "iusb-hdisk.h" 

#define MAX_HDISK_INSTANCE		(MAX_FD_INSTANCES + MAX_HD_INSTANCES + MAX_SD_INSTANCES)

/* Static Variables */
static uint8 iUsbHdiskInstance [MAX_HDISK_INSTANCE] = { 0 };
static uint8 iUsbHdiskActive [MAX_HDISK_INSTANCE] = { 0 };
static uint8 iUsbHdiskWakeup [MAX_HDISK_INSTANCE] = { 0 };
static IUSB_SCSI_PACKET *iUsbHdiskReqPkt [MAX_HDISK_INSTANCE] = { NULL };
static wait_queue_head_t hd_req_wait[MAX_HDISK_INSTANCE];
static char HdKernelModePkt [MAX_HDISK_INSTANCE][sizeof(IUSB_SCSI_PACKET)+MAX_SCSI_DATA];
static uint8 FirstTime = 1;


int
GetHdiskInstanceIndex (void)
{
	int Instance;
	
	if (FirstTime)
	{
		FirstTime = 0;
		for (Instance = 0; Instance < MAX_HDISK_INSTANCE; Instance++)
			init_waitqueue_head(&hd_req_wait[Instance]);
	}
	for (Instance = 0; Instance < MAX_HDISK_INSTANCE; Instance++)
	{
		if (0 == iUsbHdiskInstance [Instance]) 
		{
			iUsbHdiskInstance [Instance] = 1;
			return Instance;
		}
		
	}
	TWARN ("GetHdiskInstanceIndex(): No available hard disk Instances (max=%d)\n", MAX_HDISK_INSTANCE);
	return -1;			
}

int
ReleaseHdiskInstanceIndex (uint8 Instance)
{
	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("ReleaseHdiskInstanceIndex(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}

	if (FirstTime == 0)
	{
		iUsbHdiskInstance [Instance] = 0;
		if (iUsbHdiskActive [Instance])
		{
			iUsbHdiskActive [Instance] = 0; 
			wake_up(&hd_req_wait[Instance]);
			iUsbHdiskReqPkt[Instance] = NULL;
		}
		return 0;
	}
	return -1;
}


int 
HdiskCheckSupportedCmd(uint8 DevNo,uint8 ifnum,uint8 Command, BOT_IF_DATA *MassData, uint8 Instance)
{

	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskCheckSupportedCmd(): Invalid Instance index %d received\n", Instance); 
		return 0;
	}
	switch (Command)
	{
		/* List of Commands supported from Remote HARD_DISK */
		case	SCSI_TEST_UNIT_READY:	
		case	SCSI_READ_CAPACITY:					
		case	SCSI_READ_FORMAT_CAPACITIES:		
		case	SCSI_READ_10:						
		case	SCSI_VERIFY:				
		case	SCSI_WRITE_10:						
		case 	SCSI_WRITE_AND_VERIFY:				
		case	SCSI_MEDIUM_REMOVAL:					
		case	SCSI_START_STOP_UNIT:				
		case	SCSI_MODE_SENSE:
		case	SCSI_MODE_SENSE_6:
		case	SCSI_READ_12:			
		case	SCSI_WRITE_12:			
				break;				

		/* List of known Commands not supported from Remote HARD_DISK */
		case	SCSI_MODE_SELECT:					
		case	SCSI_FORMAT_UNIT:
		case	SCSI_SEEK:
									
		/* All other unknown unsupported commands */
		default:	
				printk("CheckHdiskSupportedCmd(): SCSI Unsupported Command 0x%x for instance %d\n",Command, Instance);				
				UsbCore.CoreUsbBotSetScsiErrorCode (DevNo,ifnum,0x05,0x20,0x00);			
				return 0;
	}				

	if (!iUsbHdiskActive[Instance])
	{
		UsbCore.CoreUsbBotSetScsiErrorCode (DevNo,ifnum, 0x02,0x3A,0x00);
#if defined(SOC_PILOT_IV) || defined(SOC_AST2400) || defined(SOC_AST2500)
		if (((SCSI_GET_EVENT_STATUS_NOTIFICATION_PACKET *)&(MassData->EventStatus[Instance]))->MediaStatus == 0x02)  // Media Present
			UsbCore.CoreUsbBotSetScsiEventStatus (DevNo,ifnum,Instance, 0x03, 0x01);
#endif
		return 0;
	}
	return 1;
}

IUSB_SCSI_PACKET *
HdiskGetiUsbPacket(uint8 DevNo,uint8 ifnum, uint8 Instance)
{
	
	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskGetiUsbPacket(): Invalid Instance index %d received\n", Instance); 
		return NULL;
	}
	return iUsbHdiskReqPkt[Instance];
}

int 
HdiskRemoteCall(IUSB_SCSI_PACKET *iUsbScsiPkt,uint32 PktLen, uint8 Instance)
{
	
	/* Wakeup HdiskRequest to notify Request Packet is ready*/
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
		"RemoteHdiskCall(): Waking up HdiskRequest for Instance %d\n", Instance);

	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskRemoteCall(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}
	iUsbHdiskWakeup[Instance] = 1;
	wake_up(&hd_req_wait[Instance]);
	return 0;
}

int
HdiskRemoteActivate(uint8 Instance)
{
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
		"HdiskRemoteActivate(): Harkdisk remote activate for Instance %d\n",Instance);
	
	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskRemoteActivate(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}
	
	iUsbHdiskActive[Instance] = 1;
	iUsbHdiskWakeup[Instance] = 0; 
	iUsbHdiskReqPkt[Instance] = (IUSB_SCSI_PACKET *)HdKernelModePkt[Instance];
	return 0;
}

int
HdiskRemoteDeactivate(uint8 Instance)
{
	
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
		"HdiskRemoteDeactivate(): Harkdisk remote Deactivate for Instance %d\n", Instance);

	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskRemoteDeactivate(): Invalid Instance index %d received\n", Instance); 
		return -1;
	}

	iUsbHdiskActive [Instance] = 0; 
	wake_up(&hd_req_wait[Instance]);
	iUsbHdiskReqPkt[Instance] = NULL;
	return 1;
}
                              
int
HdiskRemoteWaitRequest(IUSB_SCSI_PACKET *UserModePkt, uint8 Instance)
{
	int retval;
	
	
	if (Instance >= MAX_HDISK_INSTANCE)
	{
		TWARN ("HdiskRemoteWaitRequest(): Invalid Instance index %d received\n", Instance); 
		return -EINVAL;
	}
	
	if(iUsbHdiskActive[Instance] == 0)
	{
		TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
			"Hdisk wait request called without activating first for instance %d!!\n", Instance);
	}
		
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
		"HdiskRequest:Sleep Entering for Instance %d\n", Instance);

    retval = wait_event_interruptible(hd_req_wait[Instance],
					(iUsbHdiskWakeup[Instance] || !iUsbHdiskActive[Instance]));
    if(retval)
    {
        TWARN("wait for Hdisk request was disturbed by an interrupt please restart system call for Instance %d\n", Instance);
        return  -EINTR;
    }

	iUsbHdiskWakeup[Instance] = 0;
							 
    if(!iUsbHdiskActive[Instance])
    {
        TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
			"returning because iUSB HDISK is not active for Instance %d\n", Instance);
        return -EPIPE;
    }

	/* Got a Request Packet. Invaildate internal structure  */
	TDBG_FLAGGED(iusb, DEBUG_IUSB_HARD_DISK,
						"HdiskRequest:Sleep Leaving for Instance %d\n", Instance);
	
	/* Copy from kernel data area to user data area*/
	if (__copy_to_user((void *)UserModePkt,(void *)(HdKernelModePkt[Instance]),sizeof(IUSB_SCSI_PACKET)))
	{
		TWARN ("HdiskRemoteWaitRequest():__copy_to_user failed for Instance %d\n", Instance);
		return -EFAULT;
	}
	
	iUsbHdiskReqPkt[Instance] =  (IUSB_SCSI_PACKET *)HdKernelModePkt[Instance];       
	
	if(iUsbHdiskReqPkt[Instance]->DataLen)   /*if Data needs to be Xfered */
	{
		if(__copy_to_user((void *)(&(UserModePkt->Data)),(void *)(&(iUsbHdiskReqPkt[Instance]->Data)),
						iUsbHdiskReqPkt[Instance]->DataLen))
		{
			TWARN ("HdiskRemoteWaitRequest():__copy_to_user failed for Instance %d\n", Instance);
			return -EFAULT;
		}
	}
	return 0;
}

/************************************************************************************/


	
