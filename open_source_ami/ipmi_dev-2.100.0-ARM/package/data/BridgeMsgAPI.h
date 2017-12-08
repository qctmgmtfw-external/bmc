/*******************************************************************
********************************************************************
****                                                              **
****    (C)Copyright 2008-2009, American Megatrends Inc.          **
****                                                              **
****    All Rights Reserved.                                      **
****                                                              **
****    5555 , Oakbrook Pkwy, Norcross,                           **
****                                                              **
****    Georgia - 30093, USA. Phone-(770)-246-8600.               **
****                                                              **
********************************************************************
********************************************************************
********************************************************************
**
** BridgeMsgAPI.h
** Bridge Messaging API
**
** Author: Manimehalai S (manimehalais@amiindia.co.in)
*******************************************************************/
#ifndef BRIDGEMSGAPI_H
#define BRIDGEMSGAPI_H
#include "Types.h"

extern int
API_BridgeInternal(MsgPkt_T* pMsgPkt, INT8U DestAddr, int ChNum, BOOL ResponseTracking, int BMCInst);
#endif

