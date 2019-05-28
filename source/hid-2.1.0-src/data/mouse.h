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
#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <coreTypes.h>
#include "setup.h"

uint16 GetMouseReportSize(void);
int MouseReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *DevRequest);
void MouseRemHandler(uint8 DevNo,uint8 ifnum);
int SendMouseEvent (uint8 DevNo,uint8 IfNum,uint8 *Buff);
USB_HID_DESC* GetHidDesc_Mouse (uint8 DevNo,uint8 CfgIndex, uint8 IntIndex);
int SendMouseData (uint8 DevNo,uint8 IfNum, uint8 *Buff);
void MouseTxHandler (uint8 DevNo, uint8 IfNum, uint8 EpNum);


#endif /* _MOUSE_H_ */

