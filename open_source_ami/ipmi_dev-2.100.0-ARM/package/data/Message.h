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
 * message.h
 * Inter task messaging functions.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef MESSAGE_H
#define MESSAGE_H
#include "Types.h"
#include "OSPort.h"
#include <time.h>

#pragma pack( 1 )

#define IP_ADDR_LEN 4
#define IP6_ADDR_LEN 16

/*-----------------------------------
 * Definitions
 *-----------------------------------*/
#define MSG_PAYLOAD_SIZE	1024 * 60
#define PIPE_NAME_LEN       	64

#ifdef CONFIG_SPX_FEATURE_IPMICMD_RESPONSE_TIMEOUT
#define GETMSG_TIMEOUT CONFIG_SPX_FEATURE_IPMICMD_RESPONSE_TIMEOUT_VALUE
#else
#define GETMSG_TIMEOUT WAIT_INFINITE
#endif

/*-----------------------------------
 * Type definitions
 *-----------------------------------*/
typedef struct
{
    INT32U			Param;			        /* Parameter                         */
    INT8U			Channel;		        /* Originator's channel number 		 */
    INT8U			SrcQ [PIPE_NAME_LEN];	/* Originator Queue 				 */
#ifdef CONFIG_SPX_FEATURE_IPMICMD_RESPONSE_TIMEOUT
    time_t			ReqTime;   				/* Request Timestamp 				 */
#endif
    INT8U			Cmd;			        /* Command that needs to be processed*/
    INT8U			NetFnLUN;		        /* Net function and LUN of command   */
    INT8U			Privilege;		        /* Current privilege level			 */
    INT32U			SessionID;		        /* Session ID if any				 */
#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPMI_IPV6
    INT8U			IPAddr [IP6_ADDR_LEN];   /* IPv6 Address*/
#else
    INT8U			IPAddr [IP_ADDR_LEN];   /* IPv4 Address                        */
#endif
    INT16U			UDPPort;                /* UDP Port Number                   */
    INT16U			Socket;                 /* socket handle                     */
    INT32U			Size;			        /* Size of the data 				 */
    INT8U			Data [MSG_PAYLOAD_SIZE];/* Data						         */

} PACKED MsgPkt_T;


#pragma pack( )

/**
 * @brief Initialisation
 * @return   0 if success, -1 if failed.
**/
extern int InitMsg (void);

/**
 * @fn PostMsg
 * @brief Post a message to the destination task.
 * @param MsgPkt   - Message to be posted.
 * @param Queue	   - Queue to post this message to.
 * @return   0 if success, -1 if failed.
**/
extern int PostMsg (_FAR_ MsgPkt_T* pMsgPkt, INT8S *Queuepath ,int BMCInst);

/**
 * @fn PostMsgNonBlock
 * @brief Post a message to the destination task without blocking.
 * @param MsgPkt   - Message to be posted.
 * @param Queue	   - Queue to post this message to.
 * @return   0 if success, -1 if failed.
**/
extern int PostMsgNonBlock (_FAR_ MsgPkt_T* pMsgPkt, INT8S *Queuepath, int BMCInst );

/**
 * @fn AddToQueue
 * @brief Post a buffer to the destination task.
 * @param pBuf   - Buffer to be posted.
 * @param Queuepath    - Queuepath to post this buffer to.
 * @return   0 if success, -1 if failed.
**/
extern int AddToQueue (void* pBuf, INT8S *Queuepath,INT32U Size,int BMCInst);


/**
 * @fn GetMsg
 * @brief Gets the message posted to this task.
 * @param MsgPkt   - Pointer to the buffer to hold the message packet.
 * @param Queue    - Queue to fetch the message from.
 * @param NumMs    - Number of milli-seconds to wait.
 *  				 WAIT_NONE     - If the function has to return immediately.
 *					 WAIT_INFINITE - If the function has to wait infinitely.
 * NOTE :
 * @return   0 if success, -1 if failed.
**/
extern int GetMsg (_FAR_ MsgPkt_T*	pMsgPkt, INT8S *Queuepath, INT16S Timeout, int BMCInst);

/**
 * @fn GetMsgInMsec
 * @brief Gets the message posted to this task.
 * @param MsgPkt   - Pointer to the buffer to hold the message packet.
 * @param Queue    - Queue to fetch the message from.
 * @param NumMs    - Number of milli-seconds to wait.
 *  				 WAIT_NONE     - If the function has to return immediately.
 *					 WAIT_INFINITE - If the function has to wait infinitely.
 * NOTE :
 * @return   0 if success, -1 if failed.
**/
extern int GetMsgInMsec (_FAR_ MsgPkt_T*	pMsgPkt, INT8S *Queuepath, INT16S Timeout, int BMCInst); 


/**
 * @brief Returns the number of messages in the Queue.
 * @param Queue Queue.
**/
extern int NumMsg (HQueue_T Queue);

/**
 * @fn AddQueue
 * @brief Adds the Queue to the common Queue Handle structure
 * @param Queuepath - Queue Path
**/
extern int AddQueue(INT8S *Queuepath);

/**
 * @fn GetQueue
 * @brief Opens the Created Queue Handle from the Common
 *            Queue Handle Structure
 * @param Queuepath - Queue Path
 * @return Return 0
**/
extern int GetQueue(INT8S *Queuepath,int flags);

/**
 * @fn GetQueueHandle
 * @brief Gets the Queue Handle from the Common
 *            Queue Handle Structure
 * @param Queuepath - Queue Path
 * @param IfcQ  Handle for the needed Queue
 * @return Return 0
**/
extern int GetQueueHandle(INT8S *Queuepath,HQueue_T *IfcQ,int BMCInst);

extern time_t GetRequestTime (INT8U NetFn, INT8U Cmd);

#endif	/* MESSAGE_H */
