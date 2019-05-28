/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef ETH_MOD_H
#define ETH_MOD_H

#include "usb_core.h"
#include "dbgout.h"

#define DEBUG_ETH		0x01

TDBG_DECLARE_DBGVAR_EXTERN(eth);

extern int NotifyEpNum [];
extern int DataOutEpNum [];
extern int DataInEpNum [];
extern USB_CORE UsbCore;
extern int EthDevNo;



#endif

