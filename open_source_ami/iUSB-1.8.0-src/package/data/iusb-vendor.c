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

#include <coreTypes.h>
#include "bot.h"
#include "scsi.h"
#include "iusb-scsi.h"

/* Local Definitions */
#define VENDOR_MAX_READ_SIZE    32768

/* Static Variables */
static uint8 iUsbVendorActive = 0;
static uint8 iUsbVendorExit   = 0;
static uint8 iUsbVendorWakeup = 0;
static IUSB_SCSI_PACKET *iUsbVendorReqPkt = NULL;
static DECLARE_WAIT_QUEUE_HEAD(VendorReqWait);
static char VendorKernelModePkt[sizeof(IUSB_SCSI_PACKET)+VENDOR_MAX_READ_SIZE];

IUSB_SCSI_PACKET *
VendorGetiUsbPacket(uint8 DevNo,uint8 ifnum)
{
    return iUsbVendorReqPkt;
}

int
VendorRemoteCall(IUSB_SCSI_PACKET *iUsbScsiPkt,uint32 PktLen)
{
    /* Wakeup VendorRequest to notify Request Packet is ready*/
	TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR,"VendorCallVendorRemoteCall(): Waking up VendorRequest\n");

	iUsbVendorExit  = 0;
    iUsbVendorWakeup = 1;
    wake_up(&VendorReqWait);
    return 0;
}

int
VendorWaitRequest(IUSB_SCSI_PACKET *UserModePkt)
{
	int ret_val;
    IUSB_SCSI_PACKET *Pkt = (IUSB_SCSI_PACKET *)&VendorKernelModePkt[0];


    TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR,"VendorRequest:Sleep Entering, VendorReqWait 0x%lx \n",
                    (uint32)&VendorReqWait);


    /* Wait for a Request Packet */
    iUsbVendorReqPkt        = Pkt;
    iUsbVendorActive    = 1;

	ret_val = wait_event_interruptible (VendorReqWait, (iUsbVendorWakeup == 1));
	
    if(ret_val)
    {
//        TWARN("wait for vendor request was disturbed by an interrupt please restart system call\n");
        return  -EINTR;
    }
    iUsbVendorWakeup    = 0;

    /* Got a Request Packet. Invaildate internal structure  */
    TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR,"VendorRequest:Sleep Leaving\n");

	iUsbVendorActive    = 0;
    iUsbVendorReqPkt        = NULL;

    TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR,"VendorRequest:Before copying to userModePkt\n");

    /* Copy from kernel data area to user data area*/
    if(__copy_to_user((void *)UserModePkt,(void *)(&VendorKernelModePkt[0]),sizeof(IUSB_SCSI_PACKET)+VENDOR_MAX_READ_SIZE))
	
	{
		TWARN ("VendorWaitRequest():__copy_to_user failed\n");
		return -EFAULT;
	}
	
    iUsbVendorReqPkt        = Pkt;

    TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR,"VendorRequest:After copying to userModePkt\n");

    return iUsbVendorExit;
}

int
VendorExitWait(void)
{
	TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR, "Vendor Exit Wait...\n");
    iUsbVendorExit  = 1;
    iUsbVendorWakeup = 1;
    wake_up(&VendorReqWait);
    return 0;
}

int
ProcessVendorCommands(uint8 DevNo, uint8 ifnum, BOT_IF_DATA *MassData, uint8 LunNo)
{
    uint32 PktLen;
    IUSB_SCSI_PACKET        *iUsbScsi;

	TDBG_FLAGGED(iusb, DEBUG_IUSB_VENDOR, "Process Vendor Commands\n");

    iUsbScsi = VendorGetiUsbPacket(DevNo,ifnum);

    if (iUsbScsi == NULL)
        return 1;

    /* Form the packet on the given packet memory */
    PktLen=FormiUsbScsiPacket(iUsbScsi,DevNo,ifnum,MassData,LunNo );

    /* Issue Remote Cdrom Call */
    return VendorRemoteCall(iUsbScsi,PktLen);
}
