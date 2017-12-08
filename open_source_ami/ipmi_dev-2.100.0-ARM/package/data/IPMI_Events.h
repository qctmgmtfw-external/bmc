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
 * ipmi_events.h
 * Event processing command handler
 *
 * Author: Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_EVENTS_H
#define IPMI_EVENTS_H

#include "Types.h"

#pragma pack( 1 )

/**
 * @struct IPMBGenID_T
 * @brief IPMB generator id
**/
typedef struct 
{
    INT8U   RqSA_SWID;
    INT8U   RqLUN;

} PACKED  IPMBGenID_T;


/* SetEvtRcvReq_T */
typedef struct 
{
    INT8U   RcvSlaveAddr;
    INT8U   RcvLUN;

} PACKED  SetEvtRcvReq_T;


/* GetEvtRcvRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   RcvSlaveAddr;
    INT8U   RcvLUN;

} PACKED  GetEvtRcvRes_T;

/* Event Message */
typedef struct 
{
   INT8U   RqAddr;
   INT8U   RqLUN;
   INT8U   EvMRev;
   INT8U   SensorType;
   INT8U   SensorNum;
   INT8U   EvtDirType;
   INT8U   EvtData1;
   INT8U   EvtData2;
   INT8U   EvtData3;
} EvtMsg_T;
#pragma pack( )

#endif /* IPMI_EVENTS_H */
