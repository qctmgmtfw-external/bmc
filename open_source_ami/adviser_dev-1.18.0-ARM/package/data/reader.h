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
 * Reader.h
 * Reader thread data structures and associated definitions
**/

#ifndef _COMMON_READER_H
#define _COMMON_READER_H

#include "coreTypes.h"
#include "session_data.h"

int recv_bytes (int sock, u8* buf, u32 len, session_info_t* p_si);

#endif /* _LIBVIDEO_READER_H */
