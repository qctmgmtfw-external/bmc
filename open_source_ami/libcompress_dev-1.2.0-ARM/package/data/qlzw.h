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

#ifndef _LIBCOMPRESS_QLZW_
#define _LIBCOMPRESS_QLZW_

#include "compress_types.h"

void qlzw_init (void);
int qlzw (u16* buf, int len, u8* out_buf);
int qlzwu (u8* buf, int len, u8* out_buf);

#endif
