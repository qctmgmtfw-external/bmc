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
 ****************************************************************/
/*****************************************************************
 *
 * MsgHndlr.h
 * Message Handler.
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 * 		 : Rama Bisa <ramab@ami.com>
 *       : Basavaraj Astekar <basavaraja@ami.com>
 *       : Bakka Ravinder Reddy <bakkar@ami.com>
 *
 *****************************************************************/
#ifndef MSG_HNDLR_H
#define MSG_HNDLR_H

#include "Types.h"
#include "OSPort.h"
#include "Message.h"
#include "IPMIDefs.h"

#define BMC_LUN_00                      0
#define BMC_LUN_01                      1
#define BMC_LUN_10                      2
#define BMC_LUN_11                      3

#define LOOP_BACK_REQ                     0x10

#define LAN_IFC_SUP 0x0001
#define SOL_IFC_SUP 0x0002
#define SERIAL_IFC_SUP 0x0004
#define AMI_DEVICE_SUP 0x0008
#define USER_LOCKED                    1
#define USER_UNLOCKED                0


/*-----------------------
 * Command Handler Type
 *----------------------*/
typedef int (*pCmdHndlr_T) (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

/*----------------------
 * Command Handler map
 *----------------------*/
typedef struct
{
    INT8U		Cmd;
    INT8U		Privilege;
    pCmdHndlr_T	CmdHndlr;
    INT8U		ReqLen;		/* 0xFF - Any Length */
    INT16U		FFConfig;
    INT16U            IfcSupport;

}  CmdHndlrMap_T;

/*-------------------------------
 * Extended Command Handler Type
 *-------------------------------*/
typedef int (*pExCmdHndlr_T) (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

/*------------------------------
 * Extended Command Handler map
 *------------------------------*/
typedef struct
{
    INT8U		Cmd;
    INT8U		Privilege;
    pExCmdHndlr_T	CmdHndlr;
    INT8U		ReqLen;		/* 0xFF - Any Length */
    INT16U		FFConfig;
    INT16U            IfcSupport;

} ExCmdHndlrMap_T;



/*-------------------------
 * Message Handler Table
 *-------------------------*/
typedef struct
{
    INT8U						NetFn;
    _NEAR_ const CmdHndlrMap_T*	CmdHndlrMap;

}  MsgHndlrTbl_T;

typedef struct
{
    INT8U			NetFn;
    INT8U			GroupExtnCode;
    const CmdHndlrMap_T*	CmdHndlrMap;

} GroupExtnMsgHndlrTbl_T;


#pragma pack( 1 )

/*------------------------------
 * Pending Bridged Response Tbl
 *-----------------------------*/
#define MAX_PENDING_BRIDGE_RES 50
typedef struct
{
    INT8U			Used;
    INT8U			TimeOut;
    INT8U			SeqNum;
    INT8U			ChannelNum;
    INT8U			DstSessionHandle;
    INT8U			SrcSessionHandle;
    INT8U			DestQ[PIPE_NAME_LEN];
    IPMIMsgHdr_T	ReqMsgHdr;
    IPMIMsgHdr_T	ResMsgHdr;
} PACKED  PendingBridgedResTbl_T;

/*------------------------------
 * Pending Sequence Number
 *-----------------------------*/
//#define		NO_IPMB_PROTOCOL_CHANNEL	3
#define		MAX_PENDING_SEQ_NO			50
#define		SEQ_NO_EXPIRATION_TIME		5

typedef struct
{
    INT8U			Used;
    INT8U			TimeOut;
    INT8U			SeqNum;
    INT8U			NetFn;
    INT8U			Cmd;
} PACKED  PendingSeqNoTbl_T;

#define MAX_IPMI_CMD_BLOCKS         3
typedef struct
{
    INT8U			Channel;		/**< Originator's channel number 		*/
    HQueue_T		hSrcQ;			/**< Originator Queue 					*/
    INT8U			Param;			/**< Parameter							*/
    INT8U			Cmd;			/**< Command that needs to be processed */
    INT8U			NetFnLUN;		/**< Net function and LUN of command    */
    INT8U			Privilege;		/**< Current privilege level			*/
    INT32U			SessionID;		/**< Session ID if any					*/
    INT8U			RequestSize;						/**< Size of the Request	*/
    INT8U			Request	 [MSG_PAYLOAD_SIZE];		/**< Request Data	buffer	*/
    INT8U			ResponseSize;						/**< Size of the response	*/
    INT8U			Response [MSG_PAYLOAD_SIZE];		/**< Response Data buffer	*/
    _FAR_ INT8U*    pRequest;                   /**< Pointer to Request data    */
    _FAR_ INT8U*    pResponse;                  /**< Pointer to Response data   */

} PACKED  IPMICmdMsg_T;

#pragma pack( )

/*------------------ Timer Task Table --------------------------*/
typedef void (*pTimerFn_T) (int BMCInst);
typedef struct
{
    INT8U           NumSecs;
    pTimerFn_T      TimerFn;

} TimerTaskTbl_T;



/* Extern declaration */
extern int GetCmdHndlr(MsgPkt_T* pReq,MsgPkt_T* pRes,CmdHndlrMap_T* pCmdHndlrMap,
                                INT32U HdrOffset,INT8U CmdOverride,CmdHndlrMap_T** CmdHndrl );
extern int	  GetMsgHndlrMap (INT8U NetFn, _FAR_ CmdHndlrMap_T ** pCmdHndlrMap,int BMCInst);

/**
*@fn GroupExtnGetMsgHndlrMap
*@brief Gets the exact command Handler by comparing NetFn
*@param Netfn -NetFunction of the Cmd to execute
*@GroupExtnCode - Group Extension code
*@param pCmdHndlrMap Pointer to the Command Handler
*/
extern int 	  GroupExtnGetMsgHndlrMap (INT8U NetFn, INT8U GroupExtnCode, CmdHndlrMap_T ** pCmdHndlrMap,int BMCInst);

extern _FAR_  PendingBridgedResTbl_T	m_PendingBridgedResTbl[MAX_PENDING_BRIDGE_RES];
extern INT32U CalculateCheckSum2 (_FAR_ INT8U* Pkt, INT32U Len);
extern void	  SwapIPMIMsgHdr (_NEAR_ IPMIMsgHdr_T* pIPMIMsgReq, _NEAR_ IPMIMsgHdr_T* pIPMIMsgRes);


/**
* @fn GetIfcSupport
* @brief This function checks the support of Interface before 
*            Interface specific commands are executed
* @param IfcSupt - Interface support variable to be verified
* @param IfcSupport - Gives the Interface presence support
*/
extern int GetIfcSupport(INT16U IfcSupt,INT8U *IfcSupport,int BMCInst);

/*------------------------
 * Privilege Levels
 *------------------------*/
#define PRIV_NONE		0x00
#define PRIV_CALLBACK	0x01
#define PRIV_USER		0x02
#define PRIV_OPERATOR	0x03
#define PRIV_ADMIN		0x04
#define PRIV_OEM		0x05
#define PRIV_LOCAL		0x81
#define PRIV_SYS_IFC	0x82

/*------------------------------
* Channel type used for BadPassword
*------------------------------*/
#define LAN_CHANNEL_BADP       0x1
#define SERIAL_CHANNEL_BADP  0x2

/*----------------------------
 * Command Handler Example (app.c)
 *----------------------------*/
/*
#ifdef	APP_DEVICE

 .... Code ....

const CmdHndlrMap_T	g_APP_CmdHndlr [] =
{
	{ CMD_GET_DEV_ID,			PRIV_USER,		GET_DEV_ID,			  0x00 },
	{ CMD_BROADCAST_GET_DEV_ID, PRIV_LOCAL,		BROADCAST_GET_DEV_ID, 0x00 },
	...
	...
	{ 0x00, 					0x00, 			0x00,				  0x00 }
};

#else
const CmdHndlrMap_T	g_APP_CmdHndlr [] = { 0x00, 0x00, 0x00, 0x00 };
#endif	// APP_DEVICE
*/

/*---------------------------
 * app.h
 *---------------------------*/
/*
#ifndef APP_H
#define APP_H
#include "MsgHndlr.h"

extern const CmdHndlrMap_T g_APP_CmdHndlr [];

#endif
*/

/*---------------------------------------------------
 * Parameters passed by through the message handler Q.
 *---------------------------------------------------*/
#define PARAM_IFC			        0x01
#define PARAM_TIMER			        0x02
#define PARAM_EXECUTE_SP	        0x03
#define PARAM_SENSOR_MONITOR        0x04
#define PARAM_SENSOR_MONITOR_DONE   0x05
#define PARAM_IFC_READY				0x06
#define RCVMSGQ_LENGTH              100


#define NO_RESPONSE			        0x10
#define NORMAL_RESPONSE		        0x11
#define BRIDGING_REQUEST	        0x12

/*---------------------------------------------------
 * @def GetCurPrivLevel
 * @ret The current Privilege level.
 *---------------------------------------------------*/
#define GetCurPrivLevel()	g_BMCInfo[BMCInst].Msghndlr.CurPrivLevel

/*----------------------------------------------------
 * Receive Message Queue Names
 *----------------------------------------------------*/
extern _FAR_ char g_RcvMsgQ [3][RCVMSGQ_LENGTH];

extern void FillIPMIResFailure (_NEAR_ MsgPkt_T* pReq, _NEAR_ MsgPkt_T* pRes, int BMCInst);

#endif	/* MSG_HNDLR_H */
