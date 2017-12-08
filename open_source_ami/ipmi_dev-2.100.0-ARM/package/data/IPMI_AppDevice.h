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
 *****************************************************************
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
#ifndef IPMI_APPDEV_H
#define IPMI_APPDEV_H

#include "Types.h"

/*** Macro definition ***/
#define CHALLENGE_STR_LEN       16
#define AUTH_CODE_LEN           16
#define MAX_APP_CONF_PARAM				4
#define MIN_SYSINFO_OEM_CONF_PARAM		192
#define MAX_SYSINFO_OEM_CONF_PARAM		255

/* Added to Support IPMI v2.0/RMCP+ Integrity Algorithm Number */ 
#define AUTH_CODE_HASH_LEN      20 
#define HASH_DATA_LEN           16 
#define MAX_USERNAME_LEN        16
#define MAX_PASSWORD_LEN        20
#define  MAX_I2C_BUFFER_SIZE    50
#define IPMI15_MAX_PASSWORD_LEN 16
#define SIXTEEN_COUNT_WINDOW_LEN 16
#define SEQNUM_ROLLOVER 0xFFFFFFFF
#define RMCPPLUS_SEQLOWLIMIT 16
#define RMCPPLUS_SEQUPLIMIT 15
#define EIGHT_COUNT_WINDOW_LEN 8
#define	MAX_FW_VER_SELECTOR  	16
#define MAX_SYS_NAME_SELECTOR	16
#define MAX_PRIM_OS_NAME_SELECTOR 16
#define MAX_OS_NAME_SELECTOR	16
#define MAX_BLOCK_SIZE			16

#define SYS_INFO_SET_COMPLETE    0x00
#define SYS_INFO_SET_IN_PROGRESS 0x01
#define SYS_INFO_COMMIT_WRITE	 0x02
#define PARAM_PRESENT_REVISION   0x11
#define PARAM_OLDER_REVISION 	 0x00

#define SET_IN_PROGRESS_PARAM	0x00
#define SYS_FW_VERSION_PARAM	0x01
#define SYS_NAME_PARAM			0x02
#define PRIM_OS_NAME_PARAM		0x03
#define OS_NAME_PARAM			0x04
#define MAX_PARAM_SELECTOR		OS_NAME_PARAM +1 /* Last Parameter + 1 */

#define GET_PARAM_REV_ONLY		0x80




#pragma pack( 1 )

/* SetWDTReq_T */
typedef struct
{
    INT8U   TmrUse;
    INT8U   TmrActions;
    INT8U   PreTimeOutInterval;
    INT8U   ExpirationFlag;
    INT16U  InitCountDown;

} PACKED SetWDTReq_T;

/* GetWDTRes_T */
typedef struct
{
    INT8U       CompletionCode;
    SetWDTReq_T CurrentSettings;
    INT16U      PresentCountDown;

} PACKED GetWDTRes_T;

/* GetBMCGblEnblRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   BMCGblEnblByte;

} PACKED GetBMCGblEnblRes_T;

/* ClearMsgsFlagReq_T */
typedef struct
{
    INT8U   Flag;
} PACKED ClearMsgsFlagReq_T;

/* GetMsgFlagsRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   MsgFlags;

} PACKED GetMsgFlagsRes_T;

/* EnblMsgChRcvReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   ChannelState;

} PACKED EnblMsgChRcvReq_T;

/* EnblMsgChRcvRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;
    INT8U   ChannelState;

} PACKED EnblMsgChRcvRes_T;

/* GetMsgRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;

} PACKED GetMsgRes_T;

/* SendMsgReq_T */
typedef struct
{
    INT8U   ChNoTrackReq;

} PACKED SendMsgReq_T;

/* SendMsgRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ResData;

} PACKED SendMsgRes_T;

/* ReadEvtMsgBufRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ResData[16];

} PACKED ReadEvtMsgBufRes_T;

/* GetBTIfcCapRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   NumReqSupported;
    INT8U   InputBufSize;
    INT8U   OutputBufSize;
    INT8U   RespTime;
    INT8U   Retries;

} PACKED GetBTIfcCapRes_T;

/* GetSysGUIDRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Node[6];
    INT16U  ClockSeq;
    INT16U  TimeHigh;
    INT16U  TimeMid;
    INT32U  TimeLow;

} PACKED GetSysGUIDRes_T;

/* GetChAuthCapReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   PrivLevel;

} PACKED GetChAuthCapReq_T;

/* GetChAuthCapRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;
    INT8U   AuthType;
    INT8U   PerMsgUserAuthLoginStatus;
    INT8U   ExtCap;
    INT8U   OEMID[3];
    INT8U   OEMAuxData;

} PACKED GetChAuthCapRes_T;

/* GetSesChallengeReq_T */
typedef struct
{
    INT8U   AuthType;
    INT8U   UserName[16];

} PACKED GetSesChallengeReq_T;

/* GetSesChallengeRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT32U  TempSessionID;
    INT8U   ChallengeString[16];

} PACKED GetSesChallengeRes_T;

/* ActivateSesReq_T */
typedef struct
{
    INT8U   AuthType;
    INT8U   Privilege;
    INT8U   ChallengeString[CHALLENGE_STR_LEN];
    INT32U  OutboundSeq;

} PACKED ActivateSesReq_T;

/* ActivateSesRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   AuthType;
    INT32U  SessionID;
    INT32U  InboundSeq;
    INT8U   Privilege;

} PACKED ActivateSesRes_T;

/* SetSesPrivLevelReq_T */
typedef struct
{
    INT8U   Privilege;

} PACKED SetSesPrivLevelReq_T;

/* SetSesPrivLevelRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Privilege;

} PACKED SetSesPrivLevelRes_T;

/* GetSesInfoReq_T */
typedef struct
{
    INT8U   SessionIndex;
    INT8U   SessionHandleOrID [4];

} PACKED GetSesInfoReq_T;

/* LANSesInfoRes_T */
typedef struct
{
    INT8U    IPAddress[4];
    INT8U    MACAddress[6];
    INT16U   PortNumber;

} PACKED LANSesInfoRes_T;

/* SerialSesInfoRes_T */
typedef struct
{
    INT8U    SessionActivityType;
    INT8U    DestinationSelector;
    INT8U    IPAddress[4];
    INT16U   PortNumber;

} PACKED SerialSesInfoRes_T;

/* Union SessionInfoRes_T */
typedef union
{
    LANSesInfoRes_T     LANSesInfo;
    SerialSesInfoRes_T  SerialSesInfo;

} PACKED SessionInfoRes_T;

/* ActiveSesInfo_T */
typedef struct
{
    INT8U       UserID;
    INT8U       Privilege;
    INT8U       ChannelNum;

} PACKED ActiveSesInfo_T;

/* GetSesInfoRes_T */
typedef struct
{
    INT8U               CompletionCode;
    INT8U               SessionHandle;
    INT8U               NumPossibleActiveSession;
    INT8U               NumActiveSession;
    ActiveSesInfo_T     ActiveSesinfo;
    SessionInfoRes_T    SesInfo;

} PACKED GetSesInfoRes_T;

#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPMI_IPV6
/* LANIPv6SesInfoRes_T */
typedef struct
{
    INT8U    IPv6Address[4];
    INT8U    MACAddress[6];
    INT16U   PortNumber;

} PACKED LANIPv6SesInfoRes_T;
#endif
/* CloseSesReq_T */
typedef struct
{
    INT32U  SessionID;
    INT8U   SessionHandle;

} PACKED CloseSesReq_T;

/* GetAuthCodeReq_T */
typedef struct
{
    INT8U   AuthType;
    INT8U   ChannelNum;
    INT8U   UserID;
    INT8U   HashData[HASH_DATA_LEN];

} PACKED GetAuthCodeReq_T;


/* GetAuthCodeRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   AuthCode[AUTH_CODE_HASH_LEN];

} PACKED GetAuthCodeRes_T;

/* SetChAccessReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   ChannelAccess;
    INT8U   Privilege;

} PACKED SetChAccessReq_T;

typedef struct
{
	INT8U		CompletionCode;
} PACKED SetChAccessRes_T;

/* GetChAccessReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   AccessFlag;

} PACKED GetChAccessReq_T;

/* GetChAccessRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelAccess;
    INT8U   Privilege;

} PACKED GetChAccessRes_T;

typedef struct
{
	INT8U ChannelNum;
} PACKED GetChInfoReq_T;

/* GetChInfoRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ChannelNum;
    INT8U   ChannelMedium;
    INT8U   ChannelProtocol;
    INT8U   SessionActiveSupport;
    INT8U   VendorID[3];
    INT8U   AuxiliaryInfo[2];

} PACKED GetChInfoRes_T;

/* SetUserAccessReq_T */
typedef struct
{
    INT8U   ChannelNoUserAccess;
    INT8U   UserID;
    INT8U   AccessLimit;
    INT8U   SessionLimit;

} PACKED SetUserAccessReq_T;

typedef struct
{
    INT8U CompletionCode;
} PACKED SetUserAccessRes_T;


/* GetUserAccessReq_T */
typedef struct
{
    INT8U   ChannelNum;
    INT8U   UserID;

} PACKED GetUserAccessReq_T;

/* GetUserAccessRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   MaxNoUserID;
    INT8U   CurrentUserID;
    INT8U   FixedUserID;
    INT8U   ChannelAccess;

} PACKED GetUserAccessRes_T;

/* SetUserNameReq_T */
typedef struct
{
    INT8U   UserID;
    INT8U   UserName[MAX_USERNAME_LEN];

} PACKED SetUserNameReq_T;

typedef struct
{
    INT8U   CompletionCode;
} PACKED SetUserNameRes_T;


/*GetUserNameReq_T*/
typedef struct
{
    INT8U   UserID;
} PACKED GetUserNameReq_T;


/* GetUserNameRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   UserName[MAX_USERNAME_LEN];

} PACKED GetUserNameRes_T;



/* SetUserPswdReq_T */
typedef struct
{
    INT8U   UserID;
    INT8U   Operation;
    INT8U   Password[MAX_PASSWORD_LEN];

} PACKED SetUserPswdReq_T;

/* SetUserPswdReq_T */
typedef struct
{
    INT8U   CompletionCode;
} PACKED SetUserPswdRes_T;

/* MasterWriteReadReq_T */
typedef struct
{
    INT8U   BusTypeChNo;
    INT8U   SlaveAddress;
    INT8U   ReadCount;
    INT8U   Data[MAX_I2C_BUFFER_SIZE];

} PACKED MasterWriteReadReq_T;

/* MasterWriteReadRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   Data[MAX_I2C_BUFFER_SIZE];

} PACKED MasterWriteReadRes_T;

typedef struct {

    INT8U   SetSelector;
    INT8U   SysFWVersion[MAX_FW_VER_SELECTOR][MAX_BLOCK_SIZE];

} PACKED SysVerInfo_T;

typedef struct {

    INT8U   SetSelector;
    INT8U   SysName[MAX_SYS_NAME_SELECTOR][MAX_BLOCK_SIZE];

} PACKED SysNameInfo_T;

typedef struct {

    INT8U   SetSelector;
    INT8U   PrimOSName[MAX_PRIM_OS_NAME_SELECTOR][MAX_BLOCK_SIZE];

} PACKED PrimOSInfo_T;


typedef struct {

    INT8U   SetSelector;
    INT8U   OSName[MAX_OS_NAME_SELECTOR][MAX_BLOCK_SIZE];

} PACKED OSInfo_T;

typedef union {

    INT8U   SetInProgress;
    SysVerInfo_T    SysVerInfo;
    SysNameInfo_T   SysNameInfo;
    PrimOSInfo_T    PrimOSInfo;
    OSInfo_T    OSInfo;
} PACKED  SysInfoUN_T	;


/* GetSystemInfoParamReq_T */
typedef struct
{
    INT8U   ParamRev;
    INT8U   ParamSelector;
    INT8U   SetSelector;
    INT8U   BlockSelector;

} PACKED GetSystemInfoParamReq_T;


/* GetSystemInfoParamRes_T */
typedef struct
{
    INT8U   CompletionCode;
    INT8U   ParamRevision;
    SysInfoUN_T     SysInfo;

} PACKED GetSystemInfoParamRes_T;

/* GetSystemInfoParamOEMRes_T */
typedef struct
{
    INT8U           CompletionCode;
    INT8U           ParamRevision;
    /* OEM Info */
} PACKED GetSystemInfoParamOEMRes_T;


/* SetSystemInfoParamReq_T */
typedef struct
{
    INT8U   		ParamSelector;
    SysInfoUN_T		SysInfo;
} PACKED SetSystemInfoParamReq_T;


/* SetSystemInfoParamRes_T */
typedef struct
{
    INT8U   CompletionCode;
} PACKED SetSystemInfoParamRes_T;

/* SetSystemInfoParamOEMReq_T */
typedef struct
{
    INT8U   ParamSelector;
    /* OEM parameter */
} PACKED SetSystemInfoParamOEMReq_T;

#pragma pack( )

#endif /* IPMI_APPDEV_H */
