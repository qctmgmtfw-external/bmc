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
 ****************************************************************
 ****************************************************************
 *
 * IPMI_Bridging.h
 * Bridging Commands
 *
 * Author: Gowtham.M.S <gowthamms@amiindia.co.in>
 *
 *****************************************************************/
#ifndef IPMI_BRIDGING_H
#define IPMI_BRIDGING_H

#include "Types.h"

#pragma pack( 1 )

/* BridgeReqReq_T */
typedef struct  
{
    INT16U  RmtBrAddr;
    INT8U   RmtBrCmd;
    INT8U   *RmtBrReqData;

} PACKED  BridgeReqReq_T;


/* BridgeReqRes_T */
typedef struct  
{
    INT8U   CompletionCode;
    INT8U   *RmtBrResData;

} PACKED  BridgeReqRes_T;

#pragma pack( )

#endif  /*IPMI_BRIDGING*/
