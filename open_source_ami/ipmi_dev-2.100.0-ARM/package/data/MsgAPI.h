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
 * MsgApi.h
 * Messaging API.
 *
 *  Author: Govind Kothandapani <govindk@ami.com>
 ******************************************************************/
#ifndef MSGAPI_H
#define MSGAPI_H
#include "Types.h"
#include "Message.h"

/*-----------------------------------------------------------------
 * @fn API_PostMsg
 * @brief Post a message to the destination task.
 * @param pMsgPkt  - Pointer to the message to be posted.
 * @param Queue	   - Queue to post this message to.
 * @return   0 if success, -1 if failed.
 *-----------------------------------------------------------------*/
extern int API_PostMsg(MsgPkt_T* pMsgPkt, HQueue_T Queue,int BMCInst);


/*---------------------------------------------------------------------------
 * @fn GetMsg
 * @brief Gets the message posted to this task.
 * @param pMsgPkt  - Pointer to the buffer to hold the message packet.
 * @param Queue    - Queue to fetch the message from.
 * @param NumMs    - Number of milli-seconds to wait.
 *  				 WAIT_NONE     - If the function has to return immediately.
 *					 WAIT_INFINITE - If the function has to wait infinitely.
 * @return   0 if success, -1 if failed.
 *--------------------------------------------------------------------------*/
extern int API_GetMsg(MsgPkt_T* pMsgPkt, HQueue_T Queue, INT16S NumMs,int BMCInst);




#endif	/* MSGAPI_H */
