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
 * IPMI_App.h
 * AppDevice Commands Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_APPDEV_PLUS_H
#define IPMI_APPDEV_PLUS_H

#include "Types.h"

#pragma pack( 1 )

/*** Macro Definitions ***/
#define MAX_PLD_ENABLES     4

/* ActivatePayloadReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   AuxData [4];

} PACKED  ActivatePayloadReq_T;

/* ActivatePayloadRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   AuxData [4];
    INT16U  InboundPayldSize;
    INT16U  OutboundPayldSize;
    INT16U  UDPPort;
    INT16U  VLANNo;

} PACKED  ActivatePayloadRes_T;

/* DeactivatePayloadReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   AuxData [4];

} PACKED  DeactivatePayloadReq_T;

/* GetPayldActStatRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   InstCap;
    INT16U  ActivatedInst;

} PACKED  GetPayldActStatRes_T;

/* GetPayldInstInfoReq_T */
typedef struct 
{
    INT8U   PayldType;
    INT8U   PayldInst;

} PACKED  GetPayldInstInfoReq_T;

/* GetPayldInstInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  SessionID;
    INT8U   SpecificInfo [8];

} PACKED  GetPayldInstInfoRes_T;

/* SetUsrPayldAccReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   UserId;
    INT8U   PayloadEnables [MAX_PLD_ENABLES];

} PACKED  SetUsrPayldAccReq_T;

/* SetUsrPayldAccRes_T */
typedef struct
{
    INT8U CompletionCode;

} PACKED  SetUsrPayldAccRes_T;


/* GetUsrPayldAccReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   UserId;

} PACKED  GetUsrPayldAccReq_T;

/* GetUsrPayldAccRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   PayloadEnables [MAX_PLD_ENABLES];

} PACKED  GetUsrPayldAccRes_T;


/* GetChPayldSupRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   StdPldtype1;
    INT8U   StdPldtype2;
    INT8U   SessStpPldtype1;
    INT8U   SessStpPldtype2;
    INT8U   OemPldtype1;
    INT8U   OemPldtype2;
    INT16U  Rsvd;

} PACKED  GetChPayldSupRes_T;

/* GetChPayldVerReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadNum;

} PACKED  GetChPayldVerReq_T;

/* GetChPayldVerRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   FormatVer;

} PACKED  GetChPayldVerRes_T;

/* GetChOemPayldInfoReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadNum;
    INT8U   OemIANA [3];
    INT8U   OemPyldId [2];

} PACKED  GetChOemPayldInfoReq_T;

/* GetChOemPayldInfoRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   PayloadNum;
    INT8U   OemIANA [3];
    INT8U   OemPyldId [2];
    INT8U   FormatVer;

} PACKED  GetChOemPayldInfoRes_T;

/* SusResPayldEncryptReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   Operation;

} PACKED  SusResPayldEncryptReq_T;

/* SetChSecurityKeysReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   Operation;
    INT8U   KeyID;

} PACKED  SetChSecurityKeysReq_T;

/* SetChSecurityKeysRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   LockStatus;

} PACKED  SetChSecurityKeysRes_T;

/* GetSysIfcCapsRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Rsrvd;

} PACKED  GetSysIfcCapsRes_T;

/* GetChCipherSuitesReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadType;
    INT8U   ListIndex;

} PACKED  GetChCipherSuitesReq_T;

/* GetChCipherSuitesRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;

} PACKED  GetChCipherSuitesRes_T;

#pragma pack( )

#endif /* IPMI_APPDEV_PLUS_H */
