/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* AMIConf.h
* AMI specific extended configuration commands 
*
* Author: Benson Chuang <bensonchuang@ami.com.tw>
*           
******************************************************************/

#ifndef __AMICONF_H__ 
#define __AMICONF_H__

#include "Types.h"
#include "ncml.h"

extern char *ServiceNameList[MAX_SERVICE_NUM];
extern int AMIGetServiceConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);
extern int AMISetServiceConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);
extern int AMIGetDNSConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);
extern int AMISetDNSConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);

/* DNS v6 */
extern int AMIGetV6DNSConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);
extern int AMISetV6DNSConf(_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes,int BMCInst);
extern int AMISetActiveSlave(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMILinkDownResilent( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
extern int AMISetIfaceState(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
extern int AMIGetIfaceState(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
extern int AMISetFirewall ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMIGetFirewall ( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes ,int BMCInst );
extern int AMISetSNMPConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst);
extern int AMIGetSNMPConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);

extern int AMIGetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
extern int AMISetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
extern int AMIGetSELEntires(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
extern int AMIGetSenforInfo(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
#endif /* __AMICONF_H__ */
