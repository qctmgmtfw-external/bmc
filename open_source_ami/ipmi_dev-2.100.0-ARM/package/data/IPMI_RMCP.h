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
 * ipmi_rmcp.h
 * RMCP Packet Handler structures
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef IPMI_RMCP_H
#define IPMI_RMCP_H
#include "Types.h"
#include "coreTypes.h"

#pragma pack( 1 )

/*** External Definitions ***/
#define LAN_RMCP_PORT               623
#define AUTH_CODE_LEN               16
#define PAYLOAD_IPMI_MSG            0x00
#define PAYLOAD_SOL                 0x01
#define PAYLOAD_OEM                 0x02

#define RMCP_PLUS_FORMAT            0x06
#define CONF_AES_CBC_128_HDR_LEN    0x10

#define CONF_BLOCK_SIZE             16
#define MAX_INTEGRITY_LEN           20
#define MAX_HMAC_MD5_INTEGRITY_LEN  16
#define MAX_MD5_INTEGRITY_LEN  16
#define MAX_RANDOM_LEN              10


/**
 * @struct RMCPHdr_T
 * @brief RMCP Header
**/
typedef struct
{
    INT8U   Version;
    INT8U   Reserved;
    INT8U   SeqNumber;
    INT8U   MsgClass;

} PACKED RMCPHdr_T;


/**
 * @struct SessionHdr_T
 * @brief IPMI Session header
**/
typedef struct
{
    INT8U   AuthType;
    uint32  SessionSeqNum;
    uint32  SessionID;

} PACKED SessionHdr_T;


/**
 * @struct SessionHdr2_T
 * @brief IPMI Session Header
**/
typedef struct
{
    INT8U   AuthType;
    INT8U   PayloadType;
    uint32  SessionID;
    uint32  SessionSeqNum;
    INT16U  IPMIMsgLen;

} PACKED SessionHdr2_T;


/**
 * @struct RMCPPingHdr_T
 * @brief RMCP Ping Message Header
**/
typedef struct
{
    INT8U   IANANum[4];
    INT8U   MsgType;
    INT8U   MsgTag;
    INT8U   Reserved;
    INT8U   DataLen;

} PACKED RMCPPingHdr_T;



#pragma pack( )

#endif /* IPMI_RMCP_H */
