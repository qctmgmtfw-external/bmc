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
 * IPMI_BrMgmt.h
 * Bridge Management Commands
 *
 * Author: Gowtham.M.S <gowthamms@amiindia.co.in>
 *
 *****************************************************************/
#ifndef IPMI_BR_MGMT_H
#define IPMI_BR_MGMT_H

#include "Types.h"

#pragma pack( 1 )

/*** External Definitions ***/
#define BRIDGE_DISABLED         0
#define BRIDGE_ENABLED          1
#define BRIDGE_ASSIGNING        2
#define BRIDGE_ERROR            3

#define ICMB_ID_STR_LEN         0x10


/* GetBridgeStateRes_T */
typedef struct 
{
    INT8U CompletionCode;   
    INT8U State;

} PACKED  GetBridgeStateRes_T;


/* SetBridgeStateRes_T */
typedef struct 
{
    INT8U State;

} PACKED  SetBridgeStateReq_T; 


/* GetICMBAddrRes_T */
typedef struct 
{
    INT8U CompletionCode;   
    INT16U  Addr;

} PACKED  GetICMBAddrRes_T; 
    

/* SetICMBAddrReq_T */
typedef struct 
{
    INT16U  Addr;

} PACKED  SetICMBAddrReq_T; 


/* GetBrStatisticsReq_T */
typedef struct 
{
    INT8U StatSelector;

} PACKED  GetBrStatisticsReq_T; 


/* ICMBStats0_T */
typedef struct 
{
    INT16U  RuntCmd;
    INT16U  BadCmd;
    INT16U  SpuriousResp;

} PACKED  ICMBStats0_T; 


/* ICMBStats1_T */
typedef struct 
{
    INT16U  Sent;
    INT16U  Received;
    INT16U  OtherAddr;
    INT16U  RuntPkt;
    INT16U  BadChecksum;
    INT16U  UnkType;
    INT16U  NotStart;
    INT16U  Restart;
    INT16U  ArbWins;
    INT16U  ArbLosses;

} PACKED  ICMBStats1_T; 


/* ICMBStats_T */
typedef union  
{
    ICMBStats0_T    ICMBStats0;
    ICMBStats1_T    ICMBStats1;

} ICMBStats_T;


/* GetBrStatisticsRes_T */
typedef struct 
{
    INT8U           CompletionCode; 
    INT8U           StatSelector;
    ICMBStats_T     ICMBStats;

} PACKED  GetBrStatisticsRes_T; 


/* GetICMBCapbRes_T */
typedef struct 
{
    INT8U   CompletionCode; 
    INT8U   ICMBVersion;
    INT8U   ICMBRevision;
    INT8U   FeatureSupport;

} PACKED  GetICMBCapbRes_T; 


/* GetBrProxyAddrRes_T */
typedef struct 
{
    INT8U   CompletionCode; 
    INT16U  ICMBAddr;
    INT8U   IPMBAddr;
    INT8U   LUN;

} PACKED  GetBrProxyAddrRes_T; 


/* SetBrProxyAddrReq_T */
typedef struct 
{
    INT16U  ICMBAddr;
    INT8U   IPMBAddr;
    INT8U   LUN;

} PACKED  SetBrProxyAddrReq_T; 


/* GetICMBConnInfoReq_T */
typedef struct 
{
    INT8U   NumericID;

} PACKED  GetICMBConnInfoReq_T;


/* GetICMBConnInfoRes_T */
typedef struct 
{
    INT8U   CompletionCode; 
    INT8U   Flags;
    INT8U   NoConnectors;
    INT8U   ConnectorType;
    INT8U   TypeLength;
    INT8U   IDString [ICMB_ID_STR_LEN];

} PACKED  GetICMBConnInfoRes_T;


/* GetICMBConnIDRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   NumericID;
    INT8U   TypeLength;
    INT8U   IDString [ICMB_ID_STR_LEN];

} PACKED  GetICMBConnIDRes_T;


/* SendICMBConnIDRes_T */
typedef struct 
{
    INT8U   CompletionCode;
    INT8U   rCompletionCode;
    INT8U   NumericID;
    INT8U   TypeLength;
    INT8U   IDString [ICMB_ID_STR_LEN];

} PACKED  SendICMBConnIDRes_T;

#pragma pack( )

#endif  /*IPMI_BR_MGMT_H*/


