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
 * MiscApi.h
 * Miscellaneous API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 *
 ******************************************************************/
#ifndef MISCAPI_H
#define MISCAPI_H
#include "Types.h"
#include "SharedMem.h"

/*----------------------------------------------------------------
 * @fn API_GenerateBeep
 * @brief This function is called to generate beep.
 * @param  Pattern 	- The Beep Pattern
 * @param  NumSec 	- Number of seconds to beep (for a single bit pattern)
 *---------------------------------------------------------------*/
extern INT8U API_GenerateBeep (INT16U Pattern, INT8U NumSec, int BMCInst);

/*----------------------------------------------------------------
 * @fn API_GlowLED
 * @brief This function is called to glow the LED
 * @param		LEDNum  - LED number.
 * @param		Pattern - LED Glow pattern.
 * @param		NumSec  - Number of seconds to glow(for a single bit pattern).
 *---------------------------------------------------------------*/
extern INT8U API_GlowLED (INT8U LEDNum, INT16U Pattern, INT16U NumSec, int BMCInst);

/*----------------------------------------------------------------
 * @fn API_GetLEDEnabled
 * @brief This function is called to find LED enabled state
 * @param		LEDNum  - LED number.
 * @return TRUE or FALSE
 *---------------------------------------------------------------*/
extern BOOL API_GetLEDEnabled (INT8U LEDNum, int BMCInst);

/*----------------------------------------------------------------
 * @fn API_GetLEDPattern
 * @brief This function is called to find LED pattern
 * @param		LEDNum  - LED number.
 * @return LED Pattern
 *---------------------------------------------------------------*/
extern int API_GetLEDPattern (INT8U LEDNum, int BMCInst);

/*-----------------------------------------------------------------
 * @fn API_OSTimeDelayHMSM
 * @brief This function provides a time delay as specified in the
 * input.
 *
 * @param   MdlNum      - Module Number of this module.
 * @param   Hours       - Number of hours.
 * @param   Minutes     - Number of Minutes.
 * @param   Secods      - Number of Seconds.
 * @param   MilliSecs   - Number of Milli Seconds.
 *-----------------------------------------------------------------*/
extern INT8U API_OSTimeDelayHMSM (INT8U Hours,  INT8U   Minutes,
                               INT8U Seconds,INT16U MilliSecs);

/*----------------------------------------------------------------
 * @fn API_GetBMCSharedMem
 * @brief This function is called to get the shared memory config
 *
 *---------------------------------------------------------------*/
extern BMCSharedMem_T* API_GetBMCSharedMem (int BMCInst);

#if 0
/*----------------------------------------------------------------
 * @fn API_FlushPMConfig
 * @brief This Function is used to write PM Configuration contents
 * of NVRAM . This Function returns 0 if successful and -1 otherwise.
 *
 * @param pNVRPtr   - Offset of local copy of PMConfig
 *        size      - Number of bytes to be written.
 *---------------------------------------------------------------*/
extern int API_FlushPMConfig (INT8U* pNVRPtr, INT16U Size, int BMCInst);


/*----------------------------------------------------------------
 * @fn API_GetPMConfig
 * @brief This Function is used to read the PM Configuration. This
 * Function returns pointer to PMConfig_T if successful and NULL pointer
 * otherwise.
 *
 *---------------------------------------------------------------*/
extern PMConfig_T* API_GetPMConfig (int BMCInst);
#endif


/*----------------------------------------------------------------
 * API_ChassisIdentify
*---------------------------------------------------------------*/
extern int API_ChassisIdentify (INT8U Timeout, int BMCInst);


#endif	/* MISCAPI_H */

