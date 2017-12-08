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
 * solsend.h
 * SOL Send Routines
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Gowtham Shanmukham<gowthams@ami.com>
 *
 *****************************************************************/
#ifndef SOLSEND_H
#define SOLSEND_H
#include "Types.h"
#include "SOL.h"

#define MAX_SOL_READ_SERIAL_BUF     256

extern int SS_Init (int BMCInst);
extern int SS_OnSOLPkt (SOLPayLoad_T* p_pkt, INT16U size,int BMCInst);
extern int SS_IsPktReady  (int BMCInst);
extern int SS_FormSOLPkt (SOLPayLoad_T* p_pkt, _FAR_ INT16U* p_size,int BMCInst);
extern void OnSOLRedirection (INT8U b,int BMCInst);

extern void*  ReadSerialPkt	  (void* pData);


#endif /* SOLSEND_H */

