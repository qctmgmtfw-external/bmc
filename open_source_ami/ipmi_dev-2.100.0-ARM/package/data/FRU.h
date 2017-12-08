/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 ******************************************************************
 *
 * fru.h
 * fru.c extern declarations
 *
 *  Author: Rama Bisa <govindk@ami.com>
 *
 ******************************************************************/

#ifndef FRU_H
#define FRU_H

#include "Types.h"

/**
 * @defgroup fdc FRU Device Command handlers
 * @ingroup storage
 * IPMI Platform Management FRU (Field Replaceable Unit) information storage
 * interface commands. Implemented as per Version 1.0 and Document Rev 1.1.
 * @{
**/
#define  FRU_DEVICE_NOT_FOUND  0xCB
#define  FRU_NOT_ACCESSIBLE        0x81
#define  FRU_ACCESSIBLE                  0x0
#define MAX_PDK_FRU_SUPPORTED 10

#define FRU_COMMON_HEADER_FORMAT_VERSION 0x01

#define  FRU_IC_ONE  0x01
#define  FRU_IC_TWO  0x02

#define  FRU_DEVICE_TYPE  10

extern int GetFRUAreaInfo (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int ReadFRUData (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int WriteFRUData (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

/**
 * @brief Validates and Initializes FRU.
 * @return 0 if success, -1 if error.
**/
extern int InitFRU (int BMCInst);

extern int ValidateFRUChecksum (INT8U FRUChksumType, INT8U *Ptr);

#endif /* FRU_H */
