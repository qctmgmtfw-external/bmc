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
 * AmiResetPass.h
 * AMI Reset Password commands Macros
 *
 * Author: Gokula Kannan. S	<gokulakannans@amiindia.co.in>
 *
 ******************************************************************/
#ifndef __AMIRESETPASS_H__ 
#define __AMIRESETPASS_H__

#include "Types.h"
#include "smtpclient.h"


extern int AMIResetPassword(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int AMIGetEmailForUser(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int AMISetEmailForUser(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int AMIGetEmailFormatUser(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int AMISetEmailFormatUser(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);


#endif // __AMISMTP_H__ 
