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
/****************************************************************
 *
 * SOLCmds.c
 * Serial Over Lan commands.
 *
 *  Author: Govindarajan <govindarajann@amiindia.co.in>
 *          Vinoth Kumar <vinothkumars@amiindia.co.in>
 ****************************************************************/
#ifndef SOL_H
#define SOL_H

#include "Types.h"

/**
 * @defgroup sol SOL Command handlers
 * @ingroup devcfg
 * IPMI Serial Over LAN interface command handlers.
 * Commands are used for activating/deactivating SOL payload and 
 * retrieving/setting various SOL parameters.
 * @{
**/
extern int SOLActivating (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSOLConfig (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSOLConfig (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

#endif  /* SOL_H */
