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
 * AmiSmtp.h
 * AMI smtp commands Macros
 *
 * Author: shivaranjanic <shivaranjanic@amiindia.co.in>
 *
 ******************************************************************/
#ifndef __AMISMTP_H__ 
#define __AMISMTP_H__

#include "Types.h"
#include "smtpclient.h"

extern int g_enablesmtpauth;

extern int  SetSMTPConfigParams (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int  GetSMTPConfigParams (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);
extern int GetSMTP_PrimaryServer(SMTP_STRUCT *mail, INT8U SetSelector, INT8U EthIndex, int BMCInst);
extern int GetSMTP_SecondaryServer(SMTP_STRUCT *mail, INT8U SetSelector, INT8U EthIndex, int BMCInst);


#endif // __AMISMTP_H__ 
