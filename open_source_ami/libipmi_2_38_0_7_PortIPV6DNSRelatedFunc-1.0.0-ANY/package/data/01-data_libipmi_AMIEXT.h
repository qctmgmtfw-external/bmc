--- .pristine/libipmi-2.38.0-src/data/libipmi_AMIEXT.h Fri Feb  3 16:17:40 2012
+++ source/libipmi-2.38.0-src/data/libipmi_AMIEXT.h Wed Jan 18 15:50:12 2012
@@ -163,26 +163,17 @@
                              AMIGetDNSConfRes_T *pGetLANEnableRes,
                              int timeout );
 
-LIBIPMI_API uint16 IPMICMD_AMIGetV6DNSConf( IPMI20_SESSION_T *pSession,
-                             AMIGetV6DNSConfRes_T*pGetLANEnableRes,
-                             int timeout );
-
-LIBIPMI_API uint16 IPMICMD_AMISetAllDNSConf(IPMI20_SESSION_T *pSession,
-                            AMISetDNSConfReq_T *pAMISetDNSconfReq,
-                            AMISetDNSConfRes_T *pAMISetDNSConfRes,int timeout);
-
-LIBIPMI_API uint16 IPMICMD_AMISetIPV6DNSConf(IPMI20_SESSION_T *pSession,
-                            AMISetV6DNSConfReq_T* pAMISetIPV6DNSconfReq,
-                            AMISetV6DNSConfRes_T* pAMISetIPV6DNSConfRes,int timeout);
-
-LIBIPMI_API uint16 LIBIPMI_HL_AMIGetAllDNSConf(IPMI20_SESSION_T *pSession,
-                            INT8U *pGetDNSConf,int timeout);
-
-LIBIPMI_API uint16 LIBIPMI_HL_AMIGetV6DNSConf(IPMI20_SESSION_T *pSession,
-                            INT8U *pGetV6DNS,int timeout);
-
-LIBIPMI_API uint16 LIBIPMI_HL_AMISetAllDNSConf(IPMI20_SESSION_T *pSession,
-                            INT8U *pSetDNSReq,int timeout);
+LIBIPMI_API uint16 IPMICMD_Set_DNS_Conf( IPMI20_SESSION_T *pSession,
+                             AMISetDNSConfReq_T* pSetDNSConfReq,
+                             unsigned int ReqDataLen,
+                             AMISetDNSConfRes_T *pSetDNSConfRes,
+                             int timeout );
+
+LIBIPMI_API uint16 IPMICMD_Get_DNS_Conf( IPMI20_SESSION_T *pSession,
+                             AMIGetDNSConfReq_T *pGetDNSConfReq,
+                             AMIGetDNSConfRes_T *pGetDNSConfRes,
+                             uint32* ResLen,
+                             int timeout );
 
 LIBIPMI_API uint16 LIBIPMI_HL_GetAllSeviceConf (IPMI20_SESSION_T *pSession, 
                            GetServiceInfo_T *GetServiceInfo, int timeout);
@@ -193,8 +184,6 @@
 LIBIPMI_API uint16 LIBIPMI_HL_AMISetServiceConf(IPMI20_SESSION_T *pSession, 
                              AMIServiceConfig_T *pSetServiceCfgReq,int timeout);
 
-LIBIPMI_API uint16 LIBIPMI_HL_AMISetV6DNSConf(IPMI20_SESSION_T *pSession,
-                            INT8U *pSetV6DNSReq,int timeout);
 LIBIPMI_API uint16 LIBIPMI_HL_AMIGetEthIndex(IPMI20_SESSION_T *pSession,
                             INT8U Channel,INT8U *EthIndex,int timeout);
 
@@ -203,6 +192,32 @@
 
 LIBIPMI_API uint16 LIBIPMI_HL_AMIGetPamOrder (IPMI20_SESSION_T *pSession, 
                              INT8U *PamOrder, int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSHostSetting(IPMI20_SESSION_T *pSession,INT8U* HostSetting,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSRegister(IPMI20_SESSION_T *pSession,INT8U* RegisterBMC,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSDomainSetting(IPMI20_SESSION_T *pSession,INT8U *DomainConf,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSDomainName(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *DomainName,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSSetting(IPMI20_SESSION_T *pSession,INT8U* DNSConf,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSSetting(IPMI20_SESSION_T *pSession,INT8U *DNSConf,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U AddrLen,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetDNSIPAddress(IPMI20_SESSION_T *pSession,INT8U Block,INT8U *IPAddr,INT8U* AddrLen,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMISetDNSRestart(IPMI20_SESSION_T *pSession,int timeout);
 
 #ifdef CONFIG_SPX_FEATURE_SNMP_SUPPORT
 LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSNMPConf(IPMI20_SESSION_T *pSession,
