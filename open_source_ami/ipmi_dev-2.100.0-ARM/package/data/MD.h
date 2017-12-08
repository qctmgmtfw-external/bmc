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
 ****************************************************************/
/*****************************************************************
 *
 * MD.h
 * Message Digest Functions
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 * 		 : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef MD_H
#define MD_H
#include "Types.h"

#pragma pack( 1 )

/* Authentication Type */
#define		AUTH_TYPE_NONE							0
#define		AUTH_TYPE_MD2							1
#define		AUTH_TYPE_MD5							2
#define		AUTH_TYPE_PASSWORD						4
#define		AUTH_TYPE_OEM							5


/* MD2Context_T */
typedef struct 
{
    INT8U		State[16];      /* state */
    INT8U		Checksum[16];   /* checksum */
    INT8U		Count;			/* number of bytes, modulo 16 */
    INT8U		Buffer[16];     /* input buffer */
} PACKED  MD2Context_T;


/* MD5Context_T */
typedef struct 
{
    INT32U	state[4];		/* state (ABCD) */
    INT32U	count[2];		/* number of bits, modulo 2^64 (lsb first) */
    INT8U		buffer[64];		/* input buffer */
} PACKED  MD5Context_T;

#pragma pack( )

/*--------------------------
 * Extern Declarations
 *--------------------------*/
extern void AuthCodeCalMD2 (_NEAR_ INT8U* String, _NEAR_ INT8U*	MD2Result, INT8U StrLen);
extern void AuthCodeCalMD5 (_NEAR_ INT8U* Str, _NEAR_ INT8U* MD5Result, INT16U StrLen);

#endif	/* MD_H */

