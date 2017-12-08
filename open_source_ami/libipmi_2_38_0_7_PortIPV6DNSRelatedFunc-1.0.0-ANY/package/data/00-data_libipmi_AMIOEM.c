--- .pristine/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Fri Feb  3 16:17:41 2012
+++ source/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Wed Jan 18 16:11:26 2012
@@ -2161,106 +2161,339 @@
 return 0;
 }
 
-uint16 IPMICMD_AMIGetAllDNSConf( IPMI20_SESSION_T *pSession,
-                             AMIGetDNSConfRes_T *pGetAllDNSConfRes,
+uint16 IPMICMD_Set_DNS_Conf( IPMI20_SESSION_T *pSession,
+                             AMISetDNSConfReq_T* pSetDNSConfReq,
+                             unsigned int ReqDataLen,
+                             AMISetDNSConfRes_T *pSetDNSConfRes,
                              int timeout )
 {
     uint16 wRet;
-    uint8 *pReq = NULL;
+    uint32 dwResLen;
+    dwResLen = sizeof(AMISetDNSConfRes_T);
+
+    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
+                                                                            (NETFN_AMI << 2), CMD_AMI_SET_DNS_CONF,
+                                                                            (uint8*)pSetDNSConfReq, ReqDataLen,
+                                                                            (uint8*)pSetDNSConfRes, &dwResLen,
+                                                                            timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Configuration in raw IPMI command layer::%d\n", wRet);
+    }
+    return (wRet);
+}
+
+uint16 IPMICMD_Get_DNS_Conf( IPMI20_SESSION_T *pSession,
+                             AMIGetDNSConfReq_T *pGetDNSConfReq,
+                             AMIGetDNSConfRes_T *pGetDNSConfRes,
+                             uint32* ResLen, 
+                             int timeout )
+{
+    uint16 wRet;
     uint32 dwResLen;
     dwResLen = sizeof(AMIGetDNSConfRes_T);
 
     wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
                         (NETFN_AMI << 2), CMD_AMI_GET_DNS_CONF,
-                        pReq, 0,
-                        (uint8*)pGetAllDNSConfRes, &dwResLen,
-                        timeout);
-
-    if(wRet != 0)
-    {
-        TCRIT("Error in getting AllDNS Configuration in raw IPMI command layer::%d\n", wRet);
-    }
-    return (wRet);
-}
-
-uint16 LIBIPMI_HL_AMIGetAllDNSConf(IPMI20_SESSION_T *pSession,INT8U *pGetDNSConf,int timeout)
-{
-    uint16 wRet;
-    AMIGetDNSConfRes_T Res = {0};
-
-    wRet = IPMICMD_AMIGetAllDNSConf(pSession,
-                             &Res,
-                             timeout );
-    if(wRet != 0)
-    {
-        TCRIT("Error While getting DNS configuration %d\n",wRet);
-    }
-
-    memcpy(pGetDNSConf,&Res.dnscfg,sizeof(Res.dnscfg));
-
-    return wRet;
-
-}
-
-uint16 IPMICMD_AMIGetV6DNSConf( IPMI20_SESSION_T *pSession,
-                             AMIGetV6DNSConfRes_T*pGetV6DNSConfRes,
-                             int timeout )
-{
-    uint16 wRet;
-    uint8 *pReq= NULL;
-    uint32 dwResLen;
-    dwResLen = sizeof(AMIGetV6DNSConfRes_T);
-
-    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
-                        (NETFN_AMI << 2), CMD_AMI_GET_V6DNS_CONF,
-                        pReq, 0,
-                        (uint8*)pGetV6DNSConfRes, &dwResLen,
+                        (uint8*)pGetDNSConfReq, sizeof(AMIGetDNSConfReq_T),
+                        (uint8*)pGetDNSConfRes, &dwResLen,
                         timeout);
     if(wRet != 0)
     {
-        TCRIT("Error in getting IPv6 DNS Configuration in raw IPMI command layer::%d\n", wRet);
-    }
+        TCRIT("Error in getting DNS configuration in raw IPMI command layer::%d\n", wRet);
+    }
+
+    *ResLen = dwResLen;
     return (wRet);
 }
 
-uint16 LIBIPMI_HL_AMIGetV6DNSConf(IPMI20_SESSION_T *pSession,INT8U *pGetV6DNS,int timeout)
-{
-    uint16 wRet;
-    AMIGetV6DNSConfRes_T Res = {0};
-
-    wRet = IPMICMD_AMIGetV6DNSConf(pSession,
-                             &Res,
-                             timeout );
+uint16 LIBIPMI_HL_AMISetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_HOST_NAME;
+    memcpy(&SetDNSConfReq.DnsConfig.HName,HostSetting,sizeof(HOSTNAMECONF));
+
+    ReqDataLen = 2 + sizeof(HOSTNAMECONF);
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
     if(wRet != 0)
     {
-        TCRIT("Error While getting IPV6 DNS configuration %d\n",wRet);
-    }
-
-    memcpy(pGetV6DNS ,&Res.v6dnscfg,sizeof(Res.v6dnscfg));
+        TCRIT("Error in Setting DNS Host Configuration\n");
+    }
 
     return wRet;
-
-}
-
-uint16 IPMICMD_AMISetAllDNSConf(IPMI20_SESSION_T *pSession,AMISetDNSConfReq_T *pAMISetDNSconfReq
-                                                                        ,AMISetDNSConfRes_T *pAMISetDNSConfRes,int timeout)
-{
-    uint16 wRet;
-    uint32 dwResLen;
-    dwResLen = sizeof(AMISetDNSConfRes_T);
-
-    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
-                        (NETFN_AMI << 2), CMD_AMI_SET_DNS_CONF,
-                        (uint8 *)pAMISetDNSconfReq, sizeof(AMISetDNSConfReq_T),
-                        (uint8*)pAMISetDNSConfRes, &dwResLen,
-                        timeout);
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U*HostSetting,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_HOST_NAME;
+    GetDNSConfReq.Blockselect = 0;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
     if(wRet != 0)
     {
-        TCRIT("Error in Setting All DNS Configuration in raw IPMI command layer::%d\n", wRet);
-    }
-    return (wRet);
-}
-
+        TCRIT("Error in getting DNS Host Configuration\n");
+    }
+
+    memcpy(HostSetting,&GetDNSConfRes.DNSCfg.HName,sizeof(HOSTNAMECONF));
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_REGISTER;
+    memcpy(SetDNSConfReq.DnsConfig.RegDNSConf, RegisterBMC,MAX_CHANNEL);
+
+    ReqDataLen = 2 + MAX_CHANNEL;
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Register Configuration\n");
+    }
+
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_REGISTER;
+    GetDNSConfReq.Blockselect = 0;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in getting DNS Register Configuration\n");
+    }
+
+    memcpy(RegisterBMC,GetDNSConfRes.DNSCfg.RegDNSConf,MAX_CHANNEL);
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DOMAIN_SETTINGS;
+    memcpy(&SetDNSConfReq.DnsConfig.DomainConf,DomainConf,sizeof(DomainSetting));
+
+    ReqDataLen = 2 + sizeof(DomainSetting);
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Domain settings Configuration\n");
+    }
+
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_DOMAIN_SETTINGS;
+    GetDNSConfReq.Blockselect = 0;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in getting DNS Host Configuration\n");
+    }
+
+    memcpy(DomainConf,&GetDNSConfRes.DNSCfg.DomainConf,sizeof(DomainSetting));
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DOMAIN_NAME;
+    SetDNSConfReq.Blockselector = Block;
+    memcpy(SetDNSConfReq.DnsConfig.DomainName,DomainName,MAX_DOMAIN_BLOCK_SIZE);
+
+    ReqDataLen = 2 + MAX_DOMAIN_BLOCK_SIZE;
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Domain Name Configuration\n");
+    }
+
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_DOMAIN_NAME;
+    GetDNSConfReq.Blockselect = Block;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in getting DNS Host Configuration\n");
+    }
+
+    memcpy(DomainName,GetDNSConfRes.DNSCfg.DomainName,MAX_DOMAIN_BLOCK_SIZE);
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_SETTING;
+    memcpy(&SetDNSConfReq.DnsConfig.DNSConf,DNSConf,sizeof(DNSSetting));
+
+    ReqDataLen = 2 + sizeof(DNSSetting);
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Host Configuration\n");
+    }
+
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_DNS_SETTING;
+    GetDNSConfReq.Blockselect = 0;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in getting DNS Host Configuration\n");
+    }
+
+    memcpy(DNSConf,&GetDNSConfRes.DNSCfg.DNSConf,sizeof(DNSSetting));
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr, INT8U AddrLen,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_IP;
+    SetDNSConfReq.Blockselector = Block;
+    memcpy(SetDNSConfReq.DnsConfig.DNSIPAddr,IPAddr,AddrLen);
+
+    ReqDataLen = 2 + AddrLen;
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Host Configuration\n");
+    }
+
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMIGetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U* AddrLen,int timeout)
+{
+    uint16 wRet;
+    AMIGetDNSConfReq_T GetDNSConfReq;
+    AMIGetDNSConfRes_T GetDNSConfRes;
+    uint32 ResLen= 0 ;
+
+    memset(&GetDNSConfReq,0,sizeof(AMIGetDNSConfReq_T));
+
+    GetDNSConfReq.Param = AMI_DNS_CONF_DNS_IP;
+    GetDNSConfReq.Blockselect = Block;
+
+    wRet = IPMICMD_Get_DNS_Conf(pSession,&GetDNSConfReq,&GetDNSConfRes,&ResLen,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in getting DNS Host Configuration\n");
+    }
+
+    memcpy(IPAddr,GetDNSConfRes.DNSCfg.DNSIPAddr,ResLen -1);
+    *AddrLen = (INT8U)(ResLen -1);
+    return wRet;
+}
+
+uint16 LIBIPMI_HL_AMISetDNSRestart(IPMI20_SESSION_T *pSession,int timeout)
+{
+    uint16 wRet;
+    AMISetDNSConfReq_T SetDNSConfReq;
+    AMISetDNSConfRes_T SetDNSConfRes;
+    unsigned int ReqDataLen = 0;
+
+    memset(&SetDNSConfReq,0,sizeof(AMISetDNSConfReq_T));
+
+    SetDNSConfReq.ParamSelect = AMI_DNS_CONF_DNS_RESTART;
+
+    ReqDataLen = 2;
+    wRet = IPMICMD_Set_DNS_Conf(pSession,&SetDNSConfReq,ReqDataLen,&SetDNSConfRes,timeout);
+    if(wRet != 0)
+    {
+        TCRIT("Error in Setting DNS Host Configuration\n");
+    }
+
+    return wRet;
+}
 
 uint16 IPMICMD_AMISetServiceConf(IPMI20_SESSION_T *pSession, void* p_SetServiceConf_Req
                                                , void* p_SetServiceConf_Res, int timeout)
@@ -2388,72 +2621,6 @@
     }
     GetServiceInfo->ServiceCnt = MAX_SERVICES;
     return 0;
-}
-
-uint16 LIBIPMI_HL_AMISetAllDNSConf(IPMI20_SESSION_T *pSession,INT8U *pSetDNSReq,int timeout)
-{
-    uint16 wRet;
-    AMISetDNSConfRes_T Res = {0};
-   
-    wRet = IPMICMD_AMISetAllDNSConf(pSession,
-                             (AMISetDNSConfReq_T *)pSetDNSReq,
-                             &Res,
-                             timeout );
-    if(wRet != 0)
-    {
-        TCRIT("Error While Setting All DNS configuration %d\n",wRet);
-        return wRet;
-    }
-
-    return (uint16)Res.CompletionCode;
-
-}
-
-uint16 IPMICMD_AMISetIPV6DNSConf(IPMI20_SESSION_T *pSession,AMISetV6DNSConfReq_T* pAMISetIPV6DNSconfReq
-                                                                        ,AMISetV6DNSConfRes_T* pAMISetIPV6DNSConfRes,int timeout)
-{
-    uint16 wRet;
-    uint32 dwResLen,dwReqLen;
-    dwResLen = sizeof(AMISetV6DNSConfRes_T);
-
-    if(pAMISetIPV6DNSconfReq->v6dnscfg.v6DNSDhcpEnable == 1)
-    {
-    	dwReqLen = 2;
-    }
-    else
-    {
-    	dwReqLen = sizeof(AMISetV6DNSConfReq_T);
-    }
-    
-    wRet = LIBIPMI_Send_RAW_IPMI2_0_Command(pSession, PAYLOAD_TYPE_IPMI,
-                        (NETFN_AMI << 2), CMD_AMI_SET_V6DNS_CONF,
-                        (uint8 *)pAMISetIPV6DNSconfReq, dwReqLen,
-                        (uint8*)pAMISetIPV6DNSConfRes, &dwResLen,
-                        timeout);
-    if(wRet != 0)
-    {
-        TCRIT("Error in Setting IPV6 DNS Configuration in raw IPMI command layer::%d\n", wRet);
-    }
-    return (wRet);
-}
-
-uint16 LIBIPMI_HL_AMISetV6DNSConf(IPMI20_SESSION_T *pSession,INT8U *pSetV6DNSReq,int timeout)
-{
-    uint16 wRet;
-    AMISetV6DNSConfRes_T Res = {0};
-
-    wRet = IPMICMD_AMISetIPV6DNSConf(pSession,
-                             (AMISetV6DNSConfReq_T *)pSetV6DNSReq,
-                             &Res,
-                             timeout );
-    if(wRet != 0)
-    {
-        TCRIT("Error While Setting IPV6 DNS configuration %d\n",wRet);
-        return wRet;
-    }
-   
-    return (uint16)Res.CompletionCode;
-
 }
 
 /*
