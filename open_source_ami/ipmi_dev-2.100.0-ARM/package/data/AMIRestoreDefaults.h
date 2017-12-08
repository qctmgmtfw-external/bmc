/*****************************************************************
 *****************************************************************
 ***                                                            **
 ***    (C)Copyright 2005-2006, American Megatrends Inc.        **
 ***                                                            **
 ***            All Rights Reserved.                            **
 ***                                                            **
 ***        6145-F, Northbelt Parkway, Norcross,                **
 ***                                                            **
 ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 ***                                                            **
 *****************************************************************
 ******************************************************************
 *
 * AmiRestoreDefaults.h
 * AMI Restore Defaults command Macros
 *
 * Author: Gokula Kannan. S	<gokulakannans@amiindia.co.in>
 *
 ******************************************************************/
#ifndef __AMIRESTOREDEF_H__ 
#define __AMIRESTOREDEF_H__

#include "Types.h"

/* GetPreserveConfigRes_T */
typedef struct
{
	INT8U Selector;
} GetPreserveConfigReq_T;

/* GetPreserveConfigRes_T */
typedef struct
{
	INT8U   CompletionCode;
	INT8U	Status;
} GetPreserveConfigRes_T;

/* SetPreserveConfigReq_T */
typedef struct 
{
    INT8U Selector;
	INT8U	Status;
} SetPreserveConfigReq_T;

/* SetPreserveConfigRes_T */
typedef struct 
{
    INT8U   CompletionCode;
} SetPreserveConfigRes_T;

extern int AMIRestoreDefaults (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

extern int AMIGetPreserveConfStatus(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

extern int AMISetPreserveConfStatus(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

#endif // __AMIRESTOREDEF_H__ 
