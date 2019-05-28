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
#ifndef _IUSB_HID_H_
#define _IUSB_HID_H_

#include <coreTypes.h>
#include "iusb.h"

extern int KeybdLedWaitStatus(IUSB_HID_PACKET *UserModePkt, uint8 IfNum, uint8 NoWait);
extern int KeybdExitWait(uint8 IfNum);
extern int KeybdLedRemoteCall(uint8 DevNo,uint8 IfNum,uint8 Data);
extern int KeybdPrepareData(IUSB_HID_PACKET *UserModePkt, uint8 *KeybdData);
extern int MousePrepareData(IUSB_HID_PACKET *UserModePkt, uint8 MouseMode, uint8 *MouseData);

#endif /* _IUSB_HID_H_ */
