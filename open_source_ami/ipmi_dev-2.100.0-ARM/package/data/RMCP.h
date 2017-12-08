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
 *****************************************************************
 *
 * rmcp.h
 * RMCP Packet Handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef RMCP_H
#define RMCP_H

#include "Types.h"
#include "IPMI_RMCP.h"
#include "MsgHndlr.h"
#include "Session.h"


/* Integrity Algorithm Numbers */
#define AUTH_NONE                   0
#define AUTH_HMAC_SHA1_96           1
#define AUTH_HMAC_MD5_128           2
#define AUTH_MD5_128                3
#define AUTH_HMAC_SHA256_128        4

#define CONF_AES_CBC_128            1
#define CONF_xCR4_128               2
#define CONF_xCR4_40                3

/* Login Event OEMType definitions */
#define EVENT_LOGIN 	  0x9
#define EVENT_LOGOUT 	  0xA
#define EVENT_AUTO_LOGOUT 0xB
#define EVENT_CONN_LOST   0xC
/**
 * This structure holds various params/flags that are collected/accumulated/set/reset on the way during the flow of packets in API chain.
 * 
*/
typedef struct {
    INT8U	IsPktFromLoopBack; /** packet is received thru LoopBack */
    INT8U	IsPktFromVLAN; /** packet is received thru VLAN */
}MiscParams_T;

/**
 * @var _FAR_ INT32U g_DeleteThisLANSSession
 * @brief Flag indicates LAN ifc to delete the session after sending response
 **/
//extern _FAR_    INT32U      g_DeleteThisLANSessionID;


/**
 * @brief Process RMCP request received over LAN interface.
 * @param pRMCPReq - Request message.
 * @param pRMCPRes - Response message.
 * @return Message length.
**/
extern INT32U ProcessRMCPReq(_NEAR_ RMCPHdr_T* pRMCPReq, _NEAR_ RMCPHdr_T* pRMCPRes, MiscParams_T *pParams,INT8U Channel, int BMCInst);


/**
 * @brief Compute authentication code.
 * @param pPassword     - User password.
 * @param pSessionHdr   - Session header RMCP message.
 * @param pIPMIMsg      - IPMI message payload.
 * @param pAuthCode     - Authentication Code being generated.
 * @param ChannelType   - Channel Type.
**/
extern void  ComputeAuthCode (_FAR_ INT8U* pPassword, _NEAR_ SessionHdr_T* pSessionHdr,
                              _NEAR_ IPMIMsgHdr_T* pIPMIMsg, _NEAR_ INT8U* pAuthCode,
                              INT8U ChannelType);


/**
 * @brief Frame IPMI 2.0 payload.
 * @param PayloadType - Payload Type.
 * @param pRMCPPkt    - RMCP data.
 * @param pPayload    - Payload data.
 * @param PayloadLen  - Payload Length.
 * @param pSessionInfo - Session information.
 * @return 0 if success, -1 if error.
**/
extern int   Frame20Payload     (INT8U PayloadType, _NEAR_ RMCPHdr_T* pRMCPPkt,
                                 _FAR_ INT8U* pPayload,  INT32U PayloadLen, _FAR_
                                 SessionInfo_T* pSessionInfo, int BMCInst);


extern int AddLoginEvent ( SessionInfo_T *pRMCPSession, unsigned char EvtType, int BMCInst);


#endif /* RMCP_H */
