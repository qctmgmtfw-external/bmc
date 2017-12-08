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
 ******************************************************************
 *
 * Ipmi_serialmodem.h
 * Structures pertaining to serial/modem configuration, callback & MUX
 *
 *  Author: Govindarajan <govindarajann@amiindia.co.in>
 *          Vinoth Kumar <vinothkumars@amiindia.co.in>
 ******************************************************************/
#ifndef IPMI_SERIAL_MODEM_H
#define IPMI_SERIAL_MODEM_H

#include "Types.h"

#pragma pack( 1 )

/*** External Definitions */
#define MAX_TAP_RESPONSE_CODE_LEN       15

/* SetSerialModemConfigReq_T */
typedef struct 
{
    INT8U       ChannelNo;
    INT8U       ParamSel;

} PACKED  SetSerialModemConfigReq_T;


/* GetSerialModemConfigReq_T */
typedef struct 
{
    INT8U       ParamRevChannelNo;
    INT8U       ParamSel;
    INT8U       SetSel;
    INT8U       BlockSel;

} PACKED  GetSerialModemConfigReq_T;


/* GetSerialModemConfigRes_T */
typedef struct 
{
    INT8U       CompCode;
    INT8U       ParamRev;

} PACKED  GetSerialModemConfigRes_T;


/* SerialModemActivePingReq_T */
typedef struct 
{
    INT8U       SessionState;
    INT8U       IPMIVersion;

} PACKED  SerialModemActivePingReq_T;


/* CallbackReq_T */
typedef struct
{
   INT8U ChannelNum;
   INT8U DestSel;

} PACKED  CallbackReq_T;


/* SetUserCallbackReq_T */
typedef struct
{
      INT8U UserID;
      INT8U ChannelNum;
      INT8U UserCallbackCapabilities;
      INT8U CBCPNegOptions;
      INT8U CallBack1;
      INT8U CallBack2;
      INT8U CallBack3;

} PACKED  SetUserCallbackReq_T;


/* GetUserCallbackReq_T */
typedef struct
{
      INT8U UserID;
      INT8U ChannelNum;

} PACKED  GetUserCallbackReq_T;


/* GetUserCallbackRes_T */
typedef struct
{
      INT8U CompletionCode;
      INT8U UserCallbackCapabilities;
      INT8U CBCPNegOptions;
      INT8U CallBack1;
      INT8U CallBack2;
      INT8U CallBack3;

} PACKED  GetUserCallbackRes_T;


/* SetMuxReq_T */
typedef struct 
{

    INT8U ChannelNo;
    INT8U MuxSettingReq;

} PACKED  SetMuxReq_T;


/* SetMuxRes_T */
typedef struct 
{
    INT8U CompCode;
    INT8U MuxSetting;

} PACKED  SetMuxRes_T;


/* GetTAPResCodeReq_T */
typedef struct
{
    INT8U ChannelNo;

} PACKED  GetTAPResCodeReq_T;


/* TAPResCode_T */
typedef struct 
{
    INT8U Code [MAX_TAP_RESPONSE_CODE_LEN];

} PACKED  TAPResCode_T;


/* GetTAPResCodeRes_T */
typedef struct 
{
    INT8U           CompCode;
    TAPResCode_T    Res;

} PACKED  GetTAPResCodeRes_T;

#pragma pack( )

#endif  /* IPMI_SERIAL_MODEM_H */
