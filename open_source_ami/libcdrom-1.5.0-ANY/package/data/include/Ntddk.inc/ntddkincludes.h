#ifndef __NT_DDK_INCLUDES_H__
#define __NT_DDK_INCLUDES_H__

#include <windows.h>
#include "devioctl.h"

#if (_MSC_VER <= 1200)
#include "ntdddisk.h"
#else
#endif
#include "ntddcdrm.h"
#include "ntddscsi.h"

typedef struct tagSCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER
{
	SCSI_PASS_THROUGH_DIRECT sptd;
	ULONG	Filler;
	UCHAR	ucSenseBuf[32];
}
SCSI_PASSTHROUGH_DIRECT_WITH_SENSEBUFFER;

#endif