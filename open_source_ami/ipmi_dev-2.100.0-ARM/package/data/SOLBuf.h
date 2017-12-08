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
 * SOLBuf.h
 * SOL Buffer handler
 *
 * Author: Govind Kothandapani <govindk@ami.com>
 *       : Gowtham Shanmukham<gowthams@ami.com>
 *
 *****************************************************************/
#ifndef SOLBUF_H
#define SOLBUF_H
#include "Types.h"

#define SOL_SEND_BUF	0x00
#define SOL_RECV_BUF	0x01

extern INT8U InitSOLBuf  		(int send_buf_size, int recv_buf_size,int BMCInst,INT8U *SendBuf,INT8U *RecvBuf);
extern INT8U ClearSOLBuf 		(int BMCInst);
extern int   GetSOLBufSize 		(int buf_ix,int BMCInst);
extern int   AddToSOLBuf 		(int buf_ix, _FAR_ INT8U* p_buf, int size,int BMCInst);
extern int	 RemoveFromSOLBuf 	(int buf_ix, int size,int BMCInst);
extern int	 ReadFromSOLBuf     (int buf_ix, int offset, int size, _FAR_ INT8U* p_buf,int BMCInst);

#endif	/* SOLBUF_H */
