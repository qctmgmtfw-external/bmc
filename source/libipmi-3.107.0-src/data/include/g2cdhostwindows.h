/*****************************************************************************
*	File	:	G2CDHostWindows.h
*	Purpose	:	Implements OS specific CDROM access function
*
*	Author	:	Venkatesan B
*	Date	"	Dec 4, 2002
*****************************************************************************/
#ifndef _G2CD_HOST_WINDOWS_H_
#define _G2CD_HOST_WINDOWS_H_


/*****A NOTE ON THIS*******/
/**PLATFORM SDK HEADER FILES DO DEFINE A ULONG_PTR
but normally include files that came with vc++ do not
So if this is going to be compiled in a environment
where you dont have the latest platform SDK headers
then you just allow the typedef
/***************************/
#if(WINVER > 0x0500) //this will hold true if platform SDK is around
//typedef ULONG* ULONG_PTR; /// What about this ???
#else
#ifndef ULONG_PTR
typedef ULONG* ULONG_PTR; /// What about this ???
#endif
#endif




#include <stdio.h.>
#include <winioctl.h>
// From the Windows NT DDK \Ddk\Src\Storage\Inc
#include "ntddscsi.h"
#include "libipmi_scsi.h"
#include "hostconf.h"

typedef struct tagSCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER
{
	SCSI_PASS_THROUGH_DIRECT sptd;
	ULONG	Filler;
	UCHAR	ucSenseBuf[32];
}
SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER;


typedef struct tagSCSI_PASSTHROUGH_WITH_SENSE_DATA_BUFFER
{
    SCSI_PASS_THROUGH spt;
    ULONG             Filler;      // realign buffers to double word boundary
    UCHAR             ucSenseBuf[32];
    UCHAR             ucDataBuf[512];
}
SCSI_PASS_THROUGH_WITH_BUFFERS;


#endif  /*_G2CD_HOST_WINDOWS_H_ */



