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
 *
 * ipmi_firewall.h
 * IPMI firmware firewall Request and Response structures
 *
 *  Author: Ravinder Reddy <bakkar@ami.com>
 *          Basavaraj Astekar <basavaraja@ami.com>
 *
 ******************************************************************/
#ifndef IPMI_FIREWALL_H
#define IPMI_FIREWALL_H
#include "Types.h"

#pragma pack( 1 )

typedef union
{
	INT8U DefBodyCode;
	INT8U IANA[3];
} PACKED NetFnParams_T;
/*------------------------------
 * GetNetFnSupRes_T
 *------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT8U   LUNSupport;
    INT8U   NetFnPair [16];

} PACKED  GetNetFnSupRes_T;


/*------------------------------
 * GetCmdSupReq_T
 *------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   OpNetFn;
    INT8U   LUN;
    NetFnParams_T NetFnParams;

} PACKED  GetCmdSupReq_T;

/*------------------------------
 * GetCmdSupRes_T
 *------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SupMask [16];

} PACKED  GetCmdSupRes_T;

/*---------------------------------
 * GetSubFnSupReq_T
 *---------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   NetFn;
    INT8U   LUN;
    INT8U   Cmd;
    NetFnParams_T NetFnParams;

} PACKED  GetSubFnSupReq_T;


/*----------------------------------
 * GetSubFnSupRes_T
 *----------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SpecType;
    INT8U   SpecVer;
    INT8U   SpecRev;
    INT32U  SupMask;

} PACKED  GetSubFnSupRes_T;


/*-----------------------------------
 * GetConfigCmdsReq_T
 *-----------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   OpNetFn;
    INT8U   LUN;
    NetFnParams_T NetFnParams;

} PACKED  GetConfigCmdsReq_T;


/*------------------------------------
 * GetConfigCmdsRes_T
 *------------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT8U   SupMask [16];

} PACKED  GetConfigCmdsRes_T;


/*---------------------------------------
 * GetConfigSubFnsReq_T
 *---------------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   NetFn;
    INT8U   LUN;
    INT8U   Cmd;
    NetFnParams_T NetFnParams;

} PACKED  GetConfigSubFnsReq_T;


/*----------------------------------------
 * GetConfigSubFnsRes_T
 *----------------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT32U  SupMask;

} PACKED  GetConfigSubFnsRes_T;


/*---------------------------------------
 * SetCmdEnablesReq_T
 *---------------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   OpNetFn;
    INT8U   LUN;
    INT8U   EnDisMask [16];
    NetFnParams_T NetFnParams;

} PACKED  SetCmdEnablesReq_T;


/*---------------------------------------
 * GetCmdEnablesReq_T
 *---------------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   OpNetFn;
    INT8U   LUN;

} PACKED  GetCmdEnablesReq_T;


/*---------------------------------------
 * GetCmdEnablesRes_T
 *---------------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT8U   EnDisMask [16];
    NetFnParams_T NetFnParams;

} PACKED  GetCmdEnablesRes_T;


/*---------------------------------------
 * SetSubFnEnablesReq_T
 *---------------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   NetFn;
    INT8U   LUN;
    INT8U   Cmd;
    INT32U  Enables;

} PACKED  SetSubFnEnablesReq_T;


/*---------------------------------------
 * GetSubFnEnablesReq_T
 *---------------------------------------*/
typedef struct
{
    INT8U   ChannelNum;
    INT8U   NetFn;
    INT8U   LUN;
    INT8U   Cmd;

} PACKED  GetSubFnEnablesReq_T;


/*---------------------------------------
 * GetSubFnEnablesRes_T
 *---------------------------------------*/
typedef struct
{
    INT8U   CompletionCode;
    INT32U  Enables;

} PACKED  GetSubFnEnablesRes_T;

/*---------------------------------------
 * GetOEMNetFnIANASupportReq_T
 *---------------------------------------*/
typedef struct
{
   INT8U   ChannelNum;
   INT8U   NetFn;
   INT8U   ListIndex;

}PACKED GetOEMNetFnIANASupportReq_T;
/*---------------------------------------
 * GetOEMNetFnIANASupportRes_T
 *---------------------------------------*/
typedef struct
{
   INT8U   CompletionCode;
   INT8U   IANAIndex;
   INT8U   LUNSupport;
   INT8U   IANACode[3];

}PACKED GetOEMNetFnIANASupportRes_T;


#pragma pack( )

#endif /* IPMI_FIREWALL_H */
