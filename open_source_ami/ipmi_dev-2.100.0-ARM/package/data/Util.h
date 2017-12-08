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
 * util.h
 * Utility functions.
 *
 * Author: Govind Kothandapani <govindk@ami.com> 
 * 
 *****************************************************************/
#ifndef UTIL_H
#define UTIL_H
#include "Types.h"
#include "Platform.h"

/**
 * Returns the maximum of two values
**/
#define UTIL_MAX(val1, val2) \
    ((val1 > val2) ? val1 : val2)

/**
 * Returns the minimum of two values
**/
#define UTIL_MIN(val1, val2) \
    ((val1 < val2) ? val1 : val2)

/**
 * @brief Extracts the contents of the bits corresponding to the mask.
**/
extern INT8U GetBits (INT8U Val, INT8U Mask);

/**
 * @brief Sets the bits corresponding to the mask according to the value.
**/
extern INT8U SetBits (INT8U Mask, INT8U Val);

/**
 * @brief Find the checksum
**/
extern INT8U CalculateCheckSum (INT8U* Data, INT16U Len);

/**
*@ brief Retrieves the value from sysctl
*/
int GetJiffySysCtlvalue (const char *TagName, long long *SysVal);


#endif	/* UTIL_H */
