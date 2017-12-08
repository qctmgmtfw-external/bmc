--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Dec 22 16:17:17 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Dec 22 16:46:21 2011
@@ -358,6 +358,7 @@
 AMISetV6DNSConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst)
 {
     _NEAR_ AMISetV6DNSConfRes_T*pAMISetDNSConfRes = (_NEAR_ AMISetV6DNSConfRes_T*)pRes;
+#if 0    
     _NEAR_ AMISetV6DNSConfReq_T*pAMISetDNSConfReq = (_NEAR_ AMISetV6DNSConfReq_T*)pReq;
 
     if(pAMISetDNSConfReq->v6dnscfg.v6DNSDhcpEnable == 1)
@@ -380,7 +381,7 @@
           
     SetPendStatus(PEND_OP_SET_DNSV6_CFG,PEND_STATUS_PENDING);
     PostPendTask(PEND_OP_SET_DNSV6_CFG, (INT8U *)pAMISetDNSConfReq, sizeof(V6DNS_CONFIG),g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF,BMCInst);
-
+#endif
     pAMISetDNSConfRes->CompletionCode = CC_SUCCESS;
 
     return(sizeof(AMISetDNSConfRes_T));
