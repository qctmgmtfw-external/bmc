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
 * types_win32.h
 * Windows specific types.
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 * 
 *****************************************************************/
#ifndef TYPES_W32_H
#define TYPES_W32_H

#include <malloc.h>
#include <assert.h>
#include <windows.h>


/**
 * Basic data types.
**/
typedef unsigned char       u8;
typedef signed   char       i8;

typedef unsigned short      u16;
typedef signed   short      i16;

typedef unsigned long       u32;
typedef signed   long       i32;

typedef unsigned __int64    u64;
typedef signed   __int64    i64;

typedef u8 INT8U;
typedef u16 INT16U;
typedef u32 INT32U;


#define OS_TIME_DELAY_HMSM(H, M, S, MS)    Sleep (MS)

#endif  // TYPES_W32_H
