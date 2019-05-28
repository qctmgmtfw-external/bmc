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
#ifndef _KEYBD_H_
#define _KEYBD_H_


#include <coreTypes.h>
#include "setup.h"

uint16 GetKeybdReportSize(void);
int KeybdReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *DevRequest);
void KeybdRemHandler(uint8 DevNo,uint8 ifnum);
int SendKeybdEvent (uint8 DevNo,uint8 IfNum,uint8 *Buff);
int SetKeybdSetReport(uint8 DevNo,uint8 IfNum,uint8 *Data,uint16 DataLen);
int SendKeybdData (uint8 DevNo,uint8 IfNum, uint8 *Buff);
void KeybdTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum);


#endif /* _KEYBD_H_ */
