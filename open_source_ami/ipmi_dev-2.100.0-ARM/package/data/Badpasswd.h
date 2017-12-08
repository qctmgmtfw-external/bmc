/******************************************************************
 ******************************************************************
 ***                                                                                                           **
 ***    (C)Copyright 2006-2009, American Megatrends Inc.                            **
 ***                                                                                                           **
 ***    All Rights Reserved.                                                                          **
 ***                                                                                                           **
 ***    5555 , Oakbrook Pkwy, Norcross,                                                       **
 ***                                                                                                           **
 ***    Georgia - 30093, USA. Phone-(770)-246-8600.                                  **
 ***                                                                                                           **
 ******************************************************************
 ******************************************************************
 ******************************************************************
 *
 * Badpasswd.h
 * Bad password related prototypes
 *
 *  Author: Winston <winstonv@amiindia.co.in>
 ******************************************************************/
#ifndef _BAD_PASSWORD_
#define _BAD_PASSWORD_

#include "Types.h"
#include "Platform.h"
#include "Session.h"

void     MonitorPassword(int BMCInst);

extern INT32U TimeUpdate();

extern int GenerateLockEvent(INT8U Channel,INT8U UserID, int BMCInst);

extern int LockUser(INT8U Userid,INT8U Channel, int BMCInst); 

extern int UnlockUser(INT8U Userid,INT8U Channel, int BMCInst);

extern int FindUserLockStatus(INT8U Userid,INT8U Channel, int BMCInst);

extern int ClearUserLockAttempts(INT8U SerialorLAN, int BMCInst);


#endif


