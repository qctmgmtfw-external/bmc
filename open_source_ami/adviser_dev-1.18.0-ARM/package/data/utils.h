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
 * utils.h
**/
#ifndef _COMMON_UTILS_H
#define _COMMON_UTILS_H
#include "coreTypes.h"

/**
 * @brief htonsEx
**/
u16 htonsEx (u16 InValue);

/**
 * @brief htonlEx
**/
u32 htonlEx (u32 InValue);

/**
 * @brief dump_buffer
**/
void dump_buffer (char *Buffer, int size);

#endif	/* UTILS_H */

