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
 * types_common.h
 * Types common to all platforms.
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 * 
 *****************************************************************/
#ifndef TYPES_CMN_H
#define TYPES_CMN_H

/**
 * @brief Defines a bitmask.
**/
#define BMASK_U8(start_bit, num_bits) \
    (((u8)1 << (start_bit + num_bits)) - ((u8)1 << start_bit))

#define BMASK_U16(start_bit, num_bits) \
    (((u16)1 << (start_bit + num_bits)) - ((u16)1 << start_bit))

#define BMASK_U32(start_bit, num_bits) \
    (((u32)1 << (start_bit + num_bits)) - ((u32)1 << start_bit))


#ifndef WIN32

/**
 * @brief Qualifiers for parameters to functions.
**/
#define IN 
#define OUT
#define INOUT
#define CONST

#endif // WIN32

#endif  // TYPES_CMN_H
