/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 *****************************************************************
 *
 * aes.c
 * Advanced Standard Encryptiion
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 * 		 : Vinoth Kumar <vinothkumars@amiindia.co.in>
 *
 *****************************************************************/
#ifndef AES_H
#define AES_H

#include "Types.h"

extern void aesEncrypt (_FAR_ INT8U* pPayloadMsg, INT16U Payloadlen, _FAR_ INT8U* pIV, _FAR_ INT8U* pKey, _FAR_ INT8U* pCipher);
extern void aesDecrypt (_FAR_ INT8U	*pPayloadMsg, INT16U Payloadlen, _FAR_ INT8U* pIV, _FAR_ INT8U* pKey, _FAR_ INT8U* pResult);


#define	WPOLY	0x1b
// Finite Field multiplication -- used in mix column transformation
#define f2(x)   ((x<<1) ^ (((x>>7) & 1) * WPOLY))
#define f4(x)   ((x<<2) ^ (((x>>6) & 1) * WPOLY) ^ (((x>>6) & 2) * WPOLY))
#define f8(x)   ((x<<3) ^ (((x>>5) & 1) * WPOLY) ^ (((x>>5) & 2) * WPOLY) \
                        ^ (((x>>5) & 4) * WPOLY))
#define f3(x)   (f2(x) ^ x)
#define f9(x)   (f8(x) ^ x)
#define fb(x)   (f8(x) ^ f2(x) ^ x)
#define fd(x)   (f8(x) ^ f4(x) ^ x)
#define fe(x)   (f8(x) ^ f4(x) ^ f2(x))


#endif /* #ifdef AES_H */
