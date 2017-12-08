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
 * WDT.h
 * 
 * 
 * Author: Govind Kothandapani <govindk@ami.com> 
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 * 
 *****************************************************************/
#ifndef WDTMGR_H
#define WDTMGR_H

#include "Types.h"
#include "PMConfig.h"
#include <features.h>

#pragma pack( 1 )

/*** Type definitions ***/
// Resume ARP Thread 
#define RESUME_ARPS     0x00 
#define JIFFY_VALUE "/proc/sys/ractrends/Helper/Jiffies"
#define WDT_SLEEP_TIME 100000
#define WDT_COUNT_MS 100
#define SEC_TO_MS 1000

/**
 * @struct  WDTTmrMgr_T
 * @brief   Watchdog Timer table
**/
typedef struct
{
    INT8U       TmrPresent;         /**< Flag to indicate timer is present or not */
    INT16U      TmrInterval;        /**< Timer interval */
    INT16U      PreTimeOutInterval; /**< Pre Timeout interval */
    WDTConfig_T WDTTmr;             /**< Watchdog timer configurations */

} PACKED  WDTTmrMgr_T;

#pragma pack( )

/**
 * @var _FAR_ WDTTmrMgr_T g_WDTTmrMgr
 * @brief   Watchdog Timer table
 * @warning Should not be used from task other than Message Handler
**/
extern _FAR_ WDTTmrMgr_T g_WDTTmrMgr;

/**
 * @brief Watchdog timer task invoked every second from timer task
 **/
extern void* WDTTimerTask (void*Arg);

#ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
 /*-----------------------------------------------------------------
 * @fn StopWDTTimer
 *
 * @brief This is provided to stop Watchdog Timer.
 *
 * @return None.
 *-----------------------------------------------------------------*/
extern void StopWDTTimer (int BMCInst);
#endif

#endif /* WDTMGR_H */
