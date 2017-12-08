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
 * SOLRecv.h
 * SOL Receive functions
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Gowtham Shanmukham<gowthams@ami.com>
 *
 *****************************************************************/
#ifndef SOL_RECV_H
#define SOL_RECV_H
#include "Types.h"
#include "SOL.h"

extern int SR_Init (int BMCInst);
extern int SR_OnSOLPkt (SOLPayLoad_T* p_pkt, INT16U size,int BMCInst);
extern int SR_IsPktReady  (int BMCInst);
extern int SR_FormSOLPkt (SOLPayLoad_T* p_pkt, _FAR_ INT16U* p_size,int BMCInst);

#endif	/* SOL_RECV_H */
