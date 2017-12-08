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
 * rmcp.h
 * RMCP Packet Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_RMCP_PLUS_H
#define IPMI_RMCP_PLUS_H
#include "Types.h"
#include "coreTypes.h"

#pragma pack( 1 )

#define MAX_PAYLOAD_ALGO_SUPPORTED				3



/**** RSSP and RAKP Message Status Codes *****/
#define SC_NO_ERROR								0
#define SC_INSUFFICIENT_RESOURCE				1
#define SC_INV_SESSION_ID						2
#define SC_INV_PAYLOAD_TYPE						3
#define SC_INV_AUTH_ALGORITHM					4
#define SC_INV_INTEGRITY_ALGORITHM				5
#define SC_NO_MATCHED_AUTH_PAYLOAD				6
#define SC_NO_MATCHED_INTEGRITY_PAYLOAD			7
#define SC_INACTIVE_SESSION_ID					8
#define SC_INV_ROLE								9
#define SC_UNAUTHORISED_ROLE					10
#define SC_INSUFFICIENT_RESOURCE_AT_ROLE		11
#define SC_INV_NAME_LEN							12
#define SC_UNAUTHORISED_NAME					13
#define SC_UNAUTHORISED_GUID					14
#define SC_INV_INTEGRITY_CHECK					15
#define SC_INV_CONFIDENTIALITY_ALGORTHM			16
#define SC_NO_CIPHER_SUITE_MATCH				17

#define SESSION_INTEGRITY_KEY_SIZE              20
#define SESSION_HMAC_MD5_I_KEY_SIZE              16
#define SESSION_MD5_KEY_SIZE    16
#define HASH_KEY1_SIZE                          20
#define HASH_KEY2_SIZE                          20
#define SHA2_HASH_KEY_SIZE						32
#define MAX_HASH_KEY_SIZE       SHA2_HASH_KEY_SIZE    


/**** RSSP Open Session Request ****/
typedef struct
{
    INT8U   PayloadType;
    INT16U   Reserved1;
    INT8U   PayloadLen;
    INT8U   Algorithm;
    INT8U   Reserved2 [3];

} PACKED RSSPPayloadInfo_T;

typedef struct
{
    INT8U				MsgTag;
    INT8U				Role;
    INT8U				Reserved [2];
    uint32				RemConSessionID;
    RSSPPayloadInfo_T	Auth;
    RSSPPayloadInfo_T	Integrity;
    RSSPPayloadInfo_T	Confidentiality;

} PACKED RSSPOpenSessionReq_T;

/**** RSSP Open Session Response ****/
typedef struct
{
    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Role;    
    INT8U			Reserved;
    uint32			RemConSessionID;

} PACKED RSSPOpenSessionErrRes_T;

typedef struct
{
    INT8U				MsgTag;
    INT8U				StatusCode;
    INT8U				Role;
    INT8U				Reserved;
    uint32				RemConSessionID;
    uint32				MgdSysSessionID;
    RSSPPayloadInfo_T	Auth;
    RSSPPayloadInfo_T	Integrity;
    RSSPPayloadInfo_T	Confidentiality;


} PACKED RSSPOpenSessionRes_T;


/**** RAKP Message1 ****/
typedef struct
{

    INT8U			MsgTag;
    INT8U			Reserved1 [3];
    uint32			ManSysSessionID;
    INT8U			RemConRandomNo [16];
    INT8U			Role;
    INT8U			Reserved2 [2];
    INT8U			UsrNameLen;
    INT8U			UsrName [16];

} PACKED RAKPMsg1Req_T;

/**** RAKP Message2 ****/
typedef struct
{
    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Reserved1 [2];
    uint32			RemConSessionID;

} PACKED RAKPMsg2ErrRes_T;

typedef struct
{

    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Reserved1 [2];
    uint32			RemConSessionID;
    INT8U			ManSysRandomNo [16];
    INT8U			ManSysGUID [16];

} PACKED RAKPMsg2Res_T;

typedef struct
{

    uint32			RemConSessionID;
    uint32			MgdSysSessionID;
    INT8U			RemConRandNo [16];
    INT8U			MgdSysRandNo [16];
    INT8U			MgdSysGUID [16];
    INT8U			Role;
    INT8U			UsrNameLen;
    INT8U			UsrName [16];

} PACKED RAKPMsg1HMAC_T;

/**** RAKP Message3 ****/
typedef struct
{
    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Reserved1 [2];
    uint32			ManSysSessionID;

} PACKED RAKPMsg3Req_T;


typedef struct
{
    INT8U			MgdSysRandNo [16];
    uint32			RemConSessionID;
    INT8U			Role;
    INT8U			UsrNameLen;
    INT8U			UsrName [16];

} PACKED Msg3hmac_T;


typedef struct
{
    INT8U			RemConRandNo [16];
    INT8U			MgdSysRandNo [16];
    INT8U			Role;
    INT8U			UsrNameLen;
    INT8U			UsrName [16];

} PACKED SIKhmac_T;

/**** RAKP Message4 ****/
typedef struct
{
    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Reserved1 [2];
    uint32			RemConSessionID;

} PACKED RAKPMsg4ErrRes_T;

typedef struct
{

    INT8U			MsgTag;
    INT8U			StatusCode;
    INT8U			Reserved1 [2];
    uint32			RemConSessionID;

} PACKED RAKPMsg4Res_T;

typedef struct
{
    INT8U			RemConRandNo [16];
    uint32			MgdSysSessionID;
    INT8U			MgdSysGUID [16];

} PACKED RAKPMsg4hmac_T;



#pragma pack( )

#endif /* RMCP_PLUS_H */
