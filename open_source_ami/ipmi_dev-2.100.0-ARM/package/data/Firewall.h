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
 * Firewall.h
 * IPMI firmware firewall commands
 *
 *  Author: Ravinder Reddy <bakkar@ami.com>
 *          Basavaraj Astekar <basavaraja@ami.com>
 *          
 ******************************************************************/
#ifndef FIREWALL_H
#define FIREWALL_H
#include "Types.h"

#pragma pack(1)
typedef struct{
  INT8U IANA[3];
}PACKED IANA_T;
#pragma pack()
/*** Function Prototypes ***/

/**
 * @defgroup apcf5 Firmware Firewall Commands
 * @ingroup apcf
 * IPMI IPM Device Command Handlers. Invoked by the message handler
 * (IPMI 2.0 feature)
 * @{
 **/
extern int GetNetFnSup      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetCmdSup        (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSubFnSup      (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetConfigCmds    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetConfigSubFns  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetCmdEnables    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetCmdEnables    (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int SetSubFnEnables  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetSubFnEnables  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
extern int GetOEMNetFnIANASupport  (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
/** @} */

#endif /* FIREWALL_H */
