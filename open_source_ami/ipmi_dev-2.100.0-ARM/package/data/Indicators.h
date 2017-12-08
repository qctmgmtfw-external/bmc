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
 *****************************************************************
 ******************************************************************
 * 
 * Indicators.h
 * Beep & LED functionalities.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef INDICATORS_H
#define INDICATORS_H
#include "Types.h"

#define MAX_LED_NUM			32

#define LED_TASK_INTERVAL_MS		125

#define LED_PATTERN_OFF			0x0000 /* steady off */
#define LED_PATTERN_ON			0xffff /* steady on */
#define LED_PATTERN_STANDBY_BLINK	0x8000 /* 100ms on 2900ms off */
#define LED_PATTERN_SLOW_BLINK		0xf0f0 /* 1HZ */
#define LED_PATTERN_FAST_BLINK		0x5555 /* 4HZ */

#pragma pack(1)
typedef struct
{
	INT8U	Enable;
	INT8U	LEDNum;
	INT16U	Pattern;
	INT16U	NumSec;
	INT8U	CurBit;
	INT32U	Count;
} PACKED  IndicatorInfo_T;
#pragma pack()

/**
 * @brief Start a particular LED to glow by a certain Pattern for N sec.
 * @param LEDNum   - LED number to glow.
 * @param Pattern  - Pattern for glowing.
 * @param NumSec   - No of seconds to glow.
**/
extern INT8U GlowLED (INT8U LEDNum, INT16U Pattern, INT16U NumSec, int BMCInst);

/**
 * @brief Return Enable flag for particular LED
 * @param LEDNum   - LED number to lookup.
**/
extern BOOL GetLEDEnabled (INT8U LEDNum, int BMCInst);

/**
 * @brief Return blink pattern for particular LED
 * @param LEDNum   - LED number to lookup.
**/
extern INT16U GetLEDPattern (INT8U LEDNum, int BMCInst);

/**
 * @brief Generates Beeps in a specified pattern for N sec.
 * @param Pattern  - Pattern for Beep.
 * @param NumSec   - No of seconds to Beep.
**/
extern INT8U GenerateBeep (INT16U Pattern, INT16U NumSec, int BMCInst);

/**
 * @brief Monitors LED & Beep in a specified pattern for N sec.
**/
extern void	MoniterIndicators (int BMCInst);



#endif	/* INDICATORS_H */
