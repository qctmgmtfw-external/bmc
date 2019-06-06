/*****************************************************************
 **                                                             **
 **     (C) Copyright 2006-2009, American Megatrends Inc.       **
 **                                                             **
 **             All Rights Reserved.                            **
 **                                                             **
 **         5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                             **
 **         Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                             **
 ****************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "coreTypes.h"
#include "dbgout.h"
#include "hidtypes.h"
#include "iusb.h"

#include "hid_errno.h"
#include "hid_cmdhandlr.h"

static PKT_TYPES_E get_packet_type(u8 *pPkt)
{
	if ( pPkt == NULL )
	{
		TCRIT("Adviserd: %s :Invalid buffer. Returning unknown Packet type",__FUNCTION__ );
		return PKTTYPE_INVALID;
	}

    if(0 == strncmp ((char *)pPkt, IUSB_SIG, 8))
    { /* USB packet */
        return PKTTYPE_IUSB;
    }
    else if (0 == strncmp((char *)pPkt, HIDCMD_SIG, 8))
    { /* Hid Cmd packet */
        return PKTTYPE_HID_CMD;
    }
    else
    {
        TCRIT("Adviserd: %s : Unknwon Packet type.", __FUNCTION__ );
        return PKTTYPE_INVALID;
    }

	return PKTTYPE_INVALID;
}


int
hid_process_packet (int sock, u8* pPkt)
{
    int  ecode = 0;
 	PKT_TYPES_E pkt_type;

        /* validate the packet */
    if((pPkt == NULL) )
    {
        TCRIT("Adviserd: %s : Unknwon Packet type.", __FUNCTION__ );
        return -1;
    }

	pkt_type = get_packet_type(pPkt);

    switch (pkt_type)
    {
        case PKTTYPE_IUSB:
			TDBG("packet type is IUSB");
			ecode = HandleUsbPacket (sock, pPkt);
            break;
        case PKTTYPE_HID_CMD:
			TDBG("packet type is CMD");
            ecode = HandleHidCmdPacket (sock, pPkt);
            break;

        case PKTTYPE_INVALID:
		default:
            TWARN("Adviserd: %s : packet type is invalid. ignoring.....", __FUNCTION__ );
            ecode = HID_EINVALID_CMD;
            break;
    }

    return ecode;
}

int
HandleHidCmdPacket (int sock, u8* pPkt)
{
    int ecode = 0;
    HID_CMD_PACKET* pHidCmdPkt = (HID_CMD_PACKET* )pPkt;
	if(0)
	{
		sock=sock; /* -Wextra, fix for unused parameters */
	}
    TINFO ("Cmd No = %d", mac2short(pHidCmdPkt->Hdr.wCmdNo));
    switch (mac2short(pHidCmdPkt->Hdr.wCmdNo))
    {
        default:
			TWARN ("Adviserd: %s : Unsupported command : %d. ignoring.....", __FUNCTION__, mac2short(pHidCmdPkt->Hdr.wCmdNo) );
//            ecode = HandleUnsupportedCmd (sock, thread_id, pHidCmdPkt);
	}

	return ecode;
}


