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

#pragma pack( 1 )

#define SHA1_BLOCK_SIZE  64
#define SHA1_DIGEST_SIZE 20

/* type to hold the SHA256 context  */

typedef struct
{
    INT32U count [2];
    INT32U hash [5];
    INT32U wbuf [16];

} PACKED  Sha1_Ctx_T;

#pragma pack( )

void sha1_compile (_NEAR_ Sha1_Ctx_T* pctx);
void sha1_begin (_NEAR_ Sha1_Ctx_T* pctx);
void sha1_hash (_FAR_ INT8U* pdata, INT16U len, _NEAR_ Sha1_Ctx_T* pctx);
void sha1_end (_FAR_  INT8U* phval, _NEAR_ Sha1_Ctx_T* pctx);
void sha1 (_FAR_  INT8U* phval, _FAR_ INT8U* pdata, INT16U len);
void dispscont (_NEAR_ Sha1_Ctx_T*    pcx);

#endif
