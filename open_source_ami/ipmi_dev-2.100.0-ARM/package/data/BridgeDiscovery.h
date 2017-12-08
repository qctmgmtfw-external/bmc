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
 **                                                            **
 ****************************************************************
 ****************************************************************/
/****************************************************************
 * $Header:
 *
 * $Revision:
 *
 * $Date:
 *
 *****************************************************************/
/*****************************************************************
 *
 * BridgDiscovery.h
 * Bridge Discovery functions
 *
 * Author: Balasaravanan.P <balasaravananp@amiindia.co.in>
 *
 *****************************************************************/
#ifndef BRIDGE_DISCOVERY_H
#define BRIDGE_DISCOVERY_H

#include "Types.h"

/**
 * @var g_IsDiscovered
 * @brief Bridge device discovery status.
**/
extern _FAR_ BOOL g_IsDiscovered;

/**
 * @defgroup bdf Bridge Discovery Command handlers
 * @ingroup bridge
 * IPMI Intelligent Chassis Management Bus Bridge Discovery command interface.
 * Implemented as per ICMB ver 1.0, Revision 1.3
 * @{
**/
extern int PrepareForDiscovery  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetAddresses         (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetDiscovered        (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetChassisDeviceID   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetChassisDeviceID   (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

#endif /*BRIDGE_DISCOVERY_H */
