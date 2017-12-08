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
 *****************************************************************
 *****************************************************************
 *
 * IPMI_BrEvt.c
 * Bridge Event Commands
 *
 * Author: Gowtham.M.S <gowthamms@amiindia.co.in>
 *
 *****************************************************************/
#ifndef IPMI_BR_EVT_H
#define IPMI_BR_EVT_H

#include "Types.h"

#pragma pack( 1 )

/* GetEvtCountRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   EvtCount;

} PACKED  GetEvtCountRes_T;


/* SetEvtDestReq_T */
typedef struct 
{
    INT16U  DestAddr;

} PACKED  SetEvtDestReq_T;


/* SetEvtRecpStateReq_T */
typedef struct 
{
    INT8U   State;
    INT8U   SlaveAddr;
    INT8U   LUN;

} PACKED  SetEvtRecpStateReq_T;


/* SendICMBEvtMsgReq_T */
typedef struct 
{
    INT8U   EventCode;

} PACKED  SendICMBEvtMsgReq_T;


/* GetEvtDestRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT16U  DestAddr;

} PACKED  GetEvtDestRes_T;


/* GetEvtRecpStateRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   State;

} PACKED  GetEvtRecpStateRes_T;

#pragma pack( )

#endif  /*IPMI_BR_EVT_H*/
