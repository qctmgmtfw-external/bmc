/*
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2009-2015, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 */
#ifndef _PIPE_H_
#define _PIPE_H_


#include <coreTypes.h>
#include "setup.h"

uint16 GetPipeReportSize(void);
int PipeReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *DevRequest);
void PipeRemHandler(uint8 DevNo,uint8 ifnum);
void PipeTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum);
void PipeRxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum);


ssize_t  PipeRead(struct file * file , char * buf, size_t count, loff_t *ppos);
ssize_t  PipeWrite(struct file * file , const char __user *buf, size_t count, loff_t *ppos);
#endif /* _PIPE_H_ */
