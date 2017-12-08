/*****************************************************************
*	File		:	CDROMMediaReader.cpp						 *
*	Author		:	Rajasekhar J(rajasekharj@amiindia.co.in)	 *
*	Created		:	18-June-2003								 *
*	Description	:	Implements routines for redirecting a		 *
*						CDROM media.							 *
******************************************************************/

/**
* @file CDROMMediaReader.cpp
* @author Raja Sekhar J(rajasekharj@amiindia.co.in)
* @date 18-Jun-2003
*
* @brief Implements routines for redirecting a CDROM media.
*/
#include <string>
#include "CDROMMediaReader.h"
#include "dbgout.h"
/*****************************************************************
	Function	:	SetErrorStatus
	
	Parameters	:	nError		 - Error type
					pResponsePkt - response packet in which the
						error flags to be filled in.
				
	Description	:	sets the error flags in the response packet
					based on the error type.
******************************************************************/
void CCDROMMediaReader::SetErrorStatus(int nError, IUSB_SCSI_PACKET *pResponsePkt)
{
	switch(nError)
	{
		case SUCCESS:
			pResponsePkt->StatusPkt.OverallStatus	= 0;
			pResponsePkt->StatusPkt.SenseKey		= 0x00;
			pResponsePkt->StatusPkt.SenseCode		= 0x00;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case SECTOR_RANGE_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x21;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case WRONG_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 0;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x01;
			break;
		case MEDIUM_CHANGE:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x06;
			pResponsePkt->StatusPkt.SenseCode		= 0x28;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case MEDIA_ERROR:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x11;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case NO_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x02;
			pResponsePkt->StatusPkt.SenseCode		= 0x3A;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case MEDIUM_GETTING_READY:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x06;
			pResponsePkt->StatusPkt.SenseCode		= 0x28;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case INVALID_PARAMS:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x26;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
		case UNREADABLE_MEDIA:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x03;
			pResponsePkt->StatusPkt.SenseCode		= 0x30;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x02;
			break;
		case REMOVAL_PREVENTED:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x53;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x02;
			break;
		case UNSUPPORTED_COMMAND:
		default:
			pResponsePkt->StatusPkt.OverallStatus	= 1;
			pResponsePkt->StatusPkt.SenseKey		= 0x05;
			pResponsePkt->StatusPkt.SenseCode		= 0x20;
			pResponsePkt->StatusPkt.SenseCodeQ		= 0x00;
			break;
	}

	return;
}

std::string CCDROMMediaReader::GetVersion()
{
    std::string version( CDROM_VERSION );
    return( version );
}
