/*
 ---------------------------------------------------------------------------
 Copyright (c) 2002, Dr Brian Gladman <brg@gladman.me.uk>, Worcester, UK.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products
      built using this software without specific written permission.

 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 Issue Date: 24/01/2003

 This file contains the definitions needed for SHA1
*/

#ifndef _SHA1_H
#define _SHA1_H
#include "Types.h"
#include "coreTypes.h"


#define SHA1_BLOCK_SIZE  64
#define SHA1_DIGEST_SIZE 20

/* type to hold the SHA256 context  */

#ifdef __GNUC__
#define PACK __attribute__ ((packed))
#else
#define PACK
#pragma pack( 1 )
#endif

typedef struct
{
	uint32 count [2];
    uint32 hash [5];
    uint32 wbuf [16];

} PACK Sha1_Ctx_T;

void sha1_compile ( Sha1_Ctx_T* pctx);
void sha1_begin ( Sha1_Ctx_T* pctx);
void sha1_hash ( INT8U* pdata, INT16U len, Sha1_Ctx_T* pctx);
void sha1_end ( INT8U* phval, Sha1_Ctx_T* pctx);
void sha1 ( INT8U* phval, INT8U* pdata, INT16U len);

#undef PACK
#ifndef __GNUC__
#pragma pack()
#endif

#endif
