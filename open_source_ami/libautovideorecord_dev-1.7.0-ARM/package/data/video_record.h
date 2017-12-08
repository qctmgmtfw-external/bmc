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

/**
 * Video Recording Engine Definitions
**/

#include <unistd.h>

#ifdef ICC_OS_WINDOWS
#pragma pack(1)
#endif

#ifdef ICC_OS_LINUX
#define PACK __attribute__((packed))
#else
#define PACK
#endif

#define TIMESTAMP			0
#define VDO_DATA			1
#define VIDEO_SCREEN			0x55
#define NO_CHANGE_SCREEN		0xAA
#define BLANK_SCREEN			0x66
#define VDO_DUMP_START_FILE		"/var/StartRecording"
#define REMOVE_FILE			unlink(VDO_DUMP_START_FILE);
#define VIDEO_FRAME_HDR_SIZE		32

typedef struct
{
	time_t timestamp;
	unsigned char code;
} PACK record_header_t;

#ifdef ICC_OS_WINDOWS
#pragma pack()
#endif

