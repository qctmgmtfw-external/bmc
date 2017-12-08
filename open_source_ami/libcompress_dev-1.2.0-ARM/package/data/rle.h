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

#ifndef _LIBCOMPRESS_RLE_H_
#define _LIBCOMPRESS_RLE_H_

#include "compress_types.h"

int fwd_rle_8(u8 *in_buf, int in_len, u8 *out_buf);
int fwd_rle_16(u16 *in_buf, int in_len, u16 *out_buf);
int fwd_rle_32(u32 *in_buf, int in_len, u32 *out_buf);
int rle32(LPBYTE buf, int buf_len, LPBYTE rle_buf);
int drle32 (LPBYTE rle_buf, int rle_len, int width, int height, LPBYTE screen_buf);


#endif // __RLE_H__

