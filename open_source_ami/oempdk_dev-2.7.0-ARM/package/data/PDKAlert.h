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
 * PDKAlert.h
 * PDK Alerts Handling 
 *
 * Author:Shivaranjani <shivaranjanic@amiindia.co.in> 
 *
 *****************************************************************/

#ifndef __PDKALERT__H 
#define  __PDKALERT__H 

#include "Types.h"
#include "OSPort.h"
#include "Debug.h"
#include "API.h"

#include "SharedMem.h"
#include "ExtendedConfig.h"
#define EMAILID_NOT_CONFIGURED 5
#define USER_NOT_FOUND 6
#define USER_DISABLED 7

extern int  PDK_Alert ( void* pEventRecord,  int SetSelector ,INT8U EthIndex,int BMCInst);
extern int PDK_AfterSNMPTrap ( void* pEventRecord,  int SetSelector ,int BMCInst);
extern int  PDK_FrameAndSendMail ( void* pEventRecord, INT8U SetSelector ,INT8U EthIndex, int BMCInst);

#endif //__PDKALERT__H 
