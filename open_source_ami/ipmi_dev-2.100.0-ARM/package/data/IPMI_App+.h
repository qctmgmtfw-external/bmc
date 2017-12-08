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
#include "PMConfig.h"
#include "Session.h"

/*** Macro Definitions ***/
#define MAX_PLD_ENABLES     4

/* ActivatePayloadReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   AuxData [4];

} ActivatePayloadReq_T;

/* ActivatePayloadRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   AuxData [4];
    INT16U  InboundPayldSize;
    INT16U  OutboundPayldSize;
    INT16U  UDPPort;
    INT16U  VLANNo;

} ActivatePayloadRes_T;

/* DeactivatePayloadReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   AuxData [4];

} DeactivatePayloadReq_T;

/* GetPayldActStatRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   InstCap;
    INT16U  ActivatedInst;

} GetPayldActStatRes_T;

/* GetPayldInstInfoReq_T */
typedef struct 
{
    INT8U   PayldType;
    INT8U   PayldInst;

} GetPayldInstInfoReq_T;

/* GetPayldInstInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  SessionID;
    INT8U   SpecificInfo [8];

} GetPayldInstInfoRes_T;

/* SetUsrPayldAccReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   UserId;
    INT8U   PayloadEnables [MAX_PLD_ENABLES];

} SetUsrPayldAccReq_T;

/* GetUsrPayldAccReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   UserId;

} GetUsrPayldAccReq_T;

/* GetUsrPayldAccRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   PayloadEnables [MAX_PLD_ENABLES];

} GetUsrPayldAccRes_T;


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

} GetChPayldSupRes_T;

/* GetChPayldVerReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadNum;

} GetChPayldVerReq_T;

/* GetChPayldVerRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   FormatVer;

} GetChPayldVerRes_T;

/* GetChOemPayldInfoReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadNum;
    INT8U   OemIANA [3];
    INT8U   OemPyldId [2];

} GetChOemPayldInfoReq_T;

/* GetChOemPayldInfoRes_T */
typedef struct
{

    INT8U   CompletionCode;
    INT8U   PayloadNum;
    INT8U   OemIANA [3];
    INT8U   OemPyldId [2];
    INT8U   FormatVer;

} GetChOemPayldInfoRes_T;

/* SusResPayldEncryptReq_T */
typedef struct
{
    INT8U   PayldType;
    INT8U   PayldInst;
    INT8U   Operation;

} SusResPayldEncryptReq_T;

/* SetChSecurityKeysReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   Operation;
    INT8U   KeyID;

} SetChSecurityKeysReq_T;

/* SetChSecurityKeysRes_T */
typedef struct
{

    INT8U   CompletionCode;

} SetChSecurityKeysRes_T;

/* GetSysIfcCapsRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Rsrvd;

} GetSysIfcCapsRes_T;

/* GetChCipherSuitesReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PayloadType;
    INT8U   ListIndex;

} GetChCipherSuitesReq_T;

/* GetChCipherSuitesRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;

} GetChCipherSuitesRes_T;

#endif /* IPMI_APPDEV_PLUS_H */
