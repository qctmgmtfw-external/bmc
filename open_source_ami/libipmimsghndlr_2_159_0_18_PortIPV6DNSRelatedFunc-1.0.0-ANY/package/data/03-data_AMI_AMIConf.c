--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Fri Feb  3 16:18:15 2012
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Mon Feb 13 19:17:15 2012
@@ -106,6 +106,17 @@
                             6, /*Bond Interface*/
 };
 
+/*
+ * @brief DNS configuration Request parameter length
+ */
+static const INT8U NwDNSConfigParamLength[] = {
+                            66,
+                            3,
+                            4,
+                            64,
+                            3,
+};
+
 extern IfcName_T Ifcnametable[MAX_LAN_CHANNELS];
 /*
  * DNS related commands
@@ -133,192 +144,364 @@
 
     return 0;
 }
+
 int
 AMIGetDNSConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst)
 {
+    _NEAR_  AMIGetDNSConfReq_T* pAMIGetDNSConfReq = (_NEAR_ AMIGetDNSConfReq_T*) pReq;
     _NEAR_  AMIGetDNSConfRes_T*  pAMIGetDNSConfRes = (_NEAR_ AMIGetDNSConfRes_T*) pRes;
-
     HOSTNAMECONF HostnameConfig;
     DOMAINCONF DomainConfig;
-    DNSIPV4CONF Dnsv4IPConfig;
-    DNSIPV6CONF Dnsv6IPConfig;
-    
-/*    char hostname[MAX_HOSTNAME_LEN];
-    int hostsetting=0;
-    int  hostnamelen=0;
-*/    
-    INT8U registerBMC[MAX_LAN_CHANNELS];
-    INT8U registerBMCFQDN[MAX_LAN_CHANNELS];
-    
-    DNS_CONFIG* dnscfg = &pAMIGetDNSConfRes->dnscfg;
-    int ret = 0;
-
-    //Get current hostname
-/*    memset(hostname , '\0', sizeof(hostname));
-    if(GetHostNameConf(hostname, &hostsetting) < 0)
-    {
-        TCRIT("Error getting DNS RAC Name using gethostname\n");
-        strcpy(hostname,"");
-    }
-    hostnamelen = strlen(hostname);
-*/          
+    DNSCONF DnsIPConfig;
+    INT8U regBMC_FQDN[MAX_LAN_CHANNELS];
+
     memset(&HostnameConfig, 0, sizeof(HostnameConfig));
     memset(&DomainConfig, 0, sizeof(DomainConfig));
-    memset(&Dnsv4IPConfig, 0, sizeof(Dnsv4IPConfig));
-    memset(&Dnsv6IPConfig, 0, sizeof(Dnsv6IPConfig));
-    memset(registerBMC, 0, sizeof(registerBMC));  
-    memset(registerBMCFQDN,0,sizeof(registerBMCFQDN));
-         
-    ret = nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &Dnsv4IPConfig, &Dnsv6IPConfig, registerBMC, registerBMCFQDN);
-       
-    /* 
-     * convert data into DNS_CONFIG 
-     */
-    memset(dnscfg,0,sizeof(DNS_CONFIG));
-       
-    dnscfg->DNSDhcpEnable = Dnsv4IPConfig.dhcpEnable;
-    dnscfg->DNSServerEthIndex = Dnsv4IPConfig.EthIndex;
-             
-    /*
-     * Now convert IP strings using intet_ntoa
-     * inet_aton returns 0 if address is invalid
-     */
-    //DNS SERVER IP 
-    struct in_addr tempin;
-    if(inet_aton((char *)&Dnsv4IPConfig.v4DNSIP1,&tempin) == 0)
-    {
-        TCRIT("Invalid address received from resolv.conf!!!\n");
-        return -1;
-    }
-    else
-    {
-        memcpy(&dnscfg->DNSServer1,&tempin,4);
-    }
-
-    if(inet_aton((char *)&Dnsv4IPConfig.v4DNSIP2,&tempin) == 0)
-    {
-        TCRIT("Invalid address received from resolv.conf!!!\n");
-        return -1;
-    }
-    else
-    {
-        memcpy(&dnscfg->DNSServer2,&tempin,4);
-    }
-    
-    //DOMAIN NAME
-    dnscfg->DNSDomainNameDhcpEnable = DomainConfig.dhcpEnable;
-    dnscfg->DNSDomainNameEthIndex = DomainConfig.EthIndex;
-    dnscfg->v4v6                    = DomainConfig.v4v6;
-    dnscfg->DNSDomainNameLen = DomainConfig.domainnamelen;
-    if(DomainConfig.domainnamelen <= MAX_DOMAIN_NAME_STRING_SIZE)
-    {
-        strncpy((char *)&dnscfg->DNSDomainName, (char *)&DomainConfig.domainname, DomainConfig.domainnamelen);
-    }
-    else
-    {
-        strncpy((char *)&dnscfg->DNSDomainName, (char *)&DomainConfig.domainname, MAX_DOMAIN_NAME_STRING_SIZE);
-    }
-        
-    //Register BMC flag
-    memcpy(dnscfg->DNSRegisterBMC, registerBMC, MAX_LAN_CHANNELS);
-    memcpy(dnscfg->RegisterBMCFQDN, registerBMCFQDN, MAX_LAN_CHANNELS);
-
-    //HOST NAME
-    dnscfg->DNSBMCHostNameLen = HostnameConfig.HostNameLen;
-    dnscfg->DNSBMCHostSetting = HostnameConfig.HostSetting;
-    if(HostnameConfig.HostNameLen <= MAX_HOST_NAME_STRING_SIZE)
-    {   
-        strncpy((char *)&dnscfg->DNSBMCHostName, (char *)&HostnameConfig.HostName, HostnameConfig.HostNameLen);
-    }
-    else
-    {
-        strncpy((char *)&dnscfg->DNSBMCHostName, (char *)&HostnameConfig.HostName, MAX_HOST_NAME_STRING_SIZE);
-    }
-    
-    TDBG("AMIGETDNS: DNSIP:  dhcp=%d,      eth=%d, [%d.%d.%d.%d], [%d.%d.%d.%d],\n"
-                    "HOST:   dhcp=[%d,%d],         %d,%s,\n"
-                    "DOMAIN: dhcp=%d,      eth=%d, %d,%s \n",       
-          dnscfg->DNSDhcpEnable,
-          dnscfg->DNSServerEthIndex,
-          dnscfg->DNSServer1[0],dnscfg->DNSServer1[1],dnscfg->DNSServer1[2],dnscfg->DNSServer1[3],
-          dnscfg->DNSServer2[0],dnscfg->DNSServer2[1],dnscfg->DNSServer2[2],dnscfg->DNSServer2[3],       
-          dnscfg->DNSRegisterBMC[0],
-          dnscfg->DNSRegisterBMC[1],
-          dnscfg->DNSBMCHostNameLen,dnscfg->DNSBMCHostName,
-          dnscfg->DNSDomainNameDhcpEnable,
-          dnscfg->DNSDomainNameEthIndex,
-          dnscfg->DNSDomainNameLen,dnscfg->DNSDomainName);
-    
-    if(ret < 0)
-    {
-        *pRes = CC_UNSPECIFIED_ERR;
-        return sizeof (INT8U);
-    }
-    else
-        pAMIGetDNSConfRes->CompletionCode = CC_SUCCESS;
-    
+    memset(&DnsIPConfig, 0, sizeof(DnsIPConfig));
+    memset(regBMC_FQDN, 0, sizeof(regBMC_FQDN));
+
+    nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &DnsIPConfig,regBMC_FQDN );
+    pAMIGetDNSConfRes->CompletionCode = CC_SUCCESS;
+
+    switch(pAMIGetDNSConfReq->Param)
+    {
+        case AMI_DNS_CONF_HOST_NAME:
+            memset(&pAMIGetDNSConfRes->DNSCfg.HName,0,sizeof(HOSTNAMECONF));
+            pAMIGetDNSConfRes->DNSCfg.HName.HostSetting = HostnameConfig.HostSetting;
+            pAMIGetDNSConfRes->DNSCfg.HName.HostNameLen= HostnameConfig.HostNameLen;
+            memcpy(pAMIGetDNSConfRes->DNSCfg.HName.HostName,HostnameConfig.HostName,HostnameConfig.HostNameLen);
+            return sizeof(INT8U) + sizeof(HOSTNAMECONF);
+
+            break;
+        case AMI_DNS_CONF_REGISTER:
+
+            memcpy(pAMIGetDNSConfRes->DNSCfg.RegDNSConf,regBMC_FQDN,MAX_CHANNEL);
+            return sizeof(INT8U) + MAX_CHANNEL;
+
+            break;
+        case AMI_DNS_CONF_DOMAIN_SETTINGS:
+
+            pAMIGetDNSConfRes->DNSCfg.DomainConf.DomainDHCP = DomainConfig.dhcpEnable;
+            pAMIGetDNSConfRes->DNSCfg.DomainConf.DomainIndex = DomainConfig.EthIndex;
+            pAMIGetDNSConfRes->DNSCfg.DomainConf.Domainpriority= DomainConfig.v4v6;
+            pAMIGetDNSConfRes->DNSCfg.DomainConf.DomainLen= DomainConfig.domainnamelen;
+
+            return sizeof(INT8U) + sizeof(DomainSetting);
+
+            break;
+        case AMI_DNS_CONF_DOMAIN_NAME:
+
+            memcpy(pAMIGetDNSConfRes->DNSCfg.DomainName,&DomainConfig.domainname[MAX_DOMAIN_BLOCK_SIZE * (pAMIGetDNSConfReq->Blockselect -1)],MAX_DOMAIN_BLOCK_SIZE);
+            return sizeof(INT8U) + MAX_DOMAIN_BLOCK_SIZE;
+
+            break;
+        case AMI_DNS_CONF_DNS_SETTING:
+
+            pAMIGetDNSConfRes->DNSCfg.DNSConf.IPPriority = DnsIPConfig.IPPriority;
+            pAMIGetDNSConfRes->DNSCfg.DNSConf.DNSDHCP = DnsIPConfig.DNSDHCP;
+            pAMIGetDNSConfRes->DNSCfg.DNSConf.DNSIndex = DnsIPConfig.DNSIndex;
+            return sizeof(INT8U) + sizeof(DNSSetting);
+
+            break;
+        case AMI_DNS_CONF_DNS_IP:
+
+            if(pAMIGetDNSConfReq->Blockselect == 1)
+            {
+                if(IN6_IS_ADDR_V4MAPPED(&DnsIPConfig.DNSIP1))
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,&DnsIPConfig.DNSIP1[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                    return sizeof(INT8U) + IP_ADDR_LEN;
+                }
+                else
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,DnsIPConfig.DNSIP1,IP6_ADDR_LEN);
+                    return sizeof(INT8U) + IP6_ADDR_LEN;
+                }
+            }
+            else if(pAMIGetDNSConfReq->Blockselect == 2)
+            {
+                if(IN6_IS_ADDR_V4MAPPED(&DnsIPConfig.DNSIP2))
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,&DnsIPConfig.DNSIP2[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                    return sizeof(INT8U) + IP_ADDR_LEN;
+                }
+                else
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,DnsIPConfig.DNSIP2,IP6_ADDR_LEN);
+                    return sizeof(INT8U) + IP6_ADDR_LEN;
+                }
+            }
+            else if(pAMIGetDNSConfReq->Blockselect == 3)
+            {
+                if(IN6_IS_ADDR_V4MAPPED(&DnsIPConfig.DNSIP3))
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,&DnsIPConfig.DNSIP3[IP6_ADDR_LEN - IP_ADDR_LEN],IP_ADDR_LEN);
+                    return sizeof(INT8U) + IP_ADDR_LEN;
+                }
+                else
+                {
+                    memcpy(pAMIGetDNSConfRes->DNSCfg.DNSIPAddr,DnsIPConfig.DNSIP3,IP6_ADDR_LEN);
+                    return sizeof(INT8U) + IP6_ADDR_LEN;
+                }
+            }
+            else
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            break;
+        case AMI_DNS_CONF_DNS_RESTART:
+            *pRes = OEMCC_ATTEMPT_TO_GET_WO_PARAM;
+            return sizeof(INT8U);
+            break;
+        default:
+            *pRes = CC_PARAM_NOT_SUPPORTED;
+            return sizeof(INT8U);
+            break;
+    }
+
     return(sizeof(AMIGetDNSConfRes_T));
 }
-
 
 int
 AMISetDNSConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst)
 {
     _NEAR_ AMISetDNSConfRes_T *pAMISetDNSConfRes = (_NEAR_ AMISetDNSConfRes_T *)pRes;
-    _NEAR_ AMISetDNSConfReq_T * dns = (_NEAR_ AMISetDNSConfReq_T*)pReq;
-
-
-    TDBG("AMISETDNS: DNSIP:  dhcp=%d,      eth=%d, [%d.%d.%d.%d], [%d.%d.%d.%d],\n"
-                    "HOST:   dhcp=[%d,%d],         %d,%s,\n"
-                    "DOMAIN: dhcp=%d,      eth=%d, %d,%s \n",     
-          dns->dnscfg.DNSDhcpEnable,
-          dns->dnscfg.DNSServerEthIndex,
-          dns->dnscfg.DNSServer1[0],dns->dnscfg.DNSServer1[1],dns->dnscfg.DNSServer1[2],dns->dnscfg.DNSServer1[3],
-          dns->dnscfg.DNSServer2[0],dns->dnscfg.DNSServer2[1],dns->dnscfg.DNSServer2[2],dns->dnscfg.DNSServer2[3],
-          dns->dnscfg.DNSRegisterBMC[0],
-          dns->dnscfg.DNSRegisterBMC[1],
-          dns->dnscfg.DNSBMCHostNameLen,dns->dnscfg.DNSBMCHostName,
-          dns->dnscfg.DNSDomainNameDhcpEnable,
-          dns->dnscfg.DNSDomainNameEthIndex,
-          dns->dnscfg.DNSDomainNameLen,dns->dnscfg.DNSDomainName);
-
-    if(dns->dnscfg.DNSBMCHostNameLen == (MAX_HOSTNAME_LEN -1))
-    {
-        dns->dnscfg.DNSBMCHostName[MAX_HOSTNAME_LEN - 1] = '\0';
-    }
-
-    SetPendStatus(PEND_OP_SET_ALL_DNS_CFG,PEND_STATUS_PENDING);
-    PostPendTask(PEND_OP_SET_ALL_DNS_CFG, (INT8U *)&dns->dnscfg, sizeof(DNS_CONFIG),g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF,BMCInst);
+    _NEAR_ AMISetDNSConfReq_T *pAMISetDNSConfReq = (_NEAR_ AMISetDNSConfReq_T*)pReq;
+//        _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
+    int i;
+
+    if(ReqLen >= 2)
+    {
+        ReqLen -= 2;
+    }
+    else
+    {
+        *pRes = CC_REQ_INV_LEN;
+        return sizeof(INT8U);
+    }
+
+    if( 0x5 > pAMISetDNSConfReq->ParamSelect)
+    {
+        if( ReqLen != NwDNSConfigParamLength[pAMISetDNSConfReq->ParamSelect - 1])
+        {
+            TDBG("ReqLen %ld\n",ReqLen);
+            *pRes = CC_REQ_INV_LEN;
+            return sizeof(INT8U);
+        }
+    }
+
+    switch(pAMISetDNSConfReq->ParamSelect)
+    {
+        case AMI_DNS_CONF_HOST_NAME:
+            /*Reserved bit checking*/
+            if(pAMISetDNSConfReq->DnsConfig.HName.HostSetting > 0x1)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            /*Check the Maximum host name length i.e user can send 63 bytes and last one is null character*/
+            if(pAMISetDNSConfReq->DnsConfig.HName.HostNameLen > MAX_HOSTNAME_LEN - 1)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            TDBG("HostName Setting %d and Length %d\n",pAMISetDNSConfReq->DnsConfig.HName.HostSetting,pAMISetDNSConfReq->DnsConfig.HName.HostNameLen);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostSetting = pAMISetDNSConfReq->DnsConfig.HName.HostSetting;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostNameLen = pAMISetDNSConfReq->DnsConfig.HName.HostNameLen;
+            memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostName,pAMISetDNSConfReq->DnsConfig.HName.HostName,pAMISetDNSConfReq->DnsConfig.HName.HostNameLen);
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+
+            break;
+        case AMI_DNS_CONF_REGISTER:
+
+            for(i = 0; i < MAX_LAN_CHANNELS; i++)
+            {
+                if(pAMISetDNSConfReq->DnsConfig.RegDNSConf[i] & 0xEE)
+                {
+                    TDBG("Register value %d\n",pAMISetDNSConfReq->DnsConfig.RegDNSConf[i]);
+                    *pRes = CC_INV_DATA_FIELD;
+                    return sizeof(INT8U);
+                }
+            }
+
+            TDBG("DNS Register Settings %d %d %d\n",pAMISetDNSConfReq->DnsConfig.RegDNSConf[0],pAMISetDNSConfReq->DnsConfig.RegDNSConf[1],pAMISetDNSConfReq->DnsConfig.RegDNSConf[2]);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.RegisterBMC,pAMISetDNSConfReq->DnsConfig.RegDNSConf,MAX_LAN_CHANNELS);
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+
+            break;
+        case AMI_DNS_CONF_DOMAIN_SETTINGS:
+
+            /*Check the reserve bit*/
+            if(pAMISetDNSConfReq->DnsConfig.DomainConf.DomainDHCP > 1)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            TDBG("Domain DHCP %d\nDomainIndex %d\nDomainLen %d\nDomainpriority%d\n",
+                                                                                    pAMISetDNSConfReq->DnsConfig.DomainConf.DomainDHCP,
+                                                                                    pAMISetDNSConfReq->DnsConfig.DomainConf.DomainIndex,
+                                                                                    pAMISetDNSConfReq->DnsConfig.DomainConf.DomainLen,
+                                                                                    pAMISetDNSConfReq->DnsConfig.DomainConf.Domainpriority);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainDHCP = pAMISetDNSConfReq->DnsConfig.DomainConf.DomainDHCP;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainIndex = pAMISetDNSConfReq->DnsConfig.DomainConf.DomainIndex;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainLen = pAMISetDNSConfReq->DnsConfig.DomainConf.DomainLen;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.Domainpriority = pAMISetDNSConfReq->DnsConfig.DomainConf.Domainpriority;
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+
+            break;
+        case AMI_DNS_CONF_DOMAIN_NAME:
+
+            if( pAMISetDNSConfReq->Blockselector == 0x0 || pAMISetDNSConfReq->Blockselector > MAX_BLOCK)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            TDBG("Block Selector %d \n",pAMISetDNSConfReq->Blockselector);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            memcpy(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainName[ MAX_DOMAIN_BLOCK_SIZE * (pAMISetDNSConfReq->Blockselector - 1)],pAMISetDNSConfReq->DnsConfig.DomainName,MAX_DOMAIN_BLOCK_SIZE);
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+
+            break;
+        case AMI_DNS_CONF_DNS_SETTING:
+
+            /*Check the reserve bit*/
+            if(pAMISetDNSConfReq->DnsConfig.DNSConf.DNSDHCP > 1)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            if(pAMISetDNSConfReq->DnsConfig.DNSConf.IPPriority > 2)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            TDBG("IPPriority %d\n",pAMISetDNSConfReq->DnsConfig.DNSConf.IPPriority);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.IPPriority = pAMISetDNSConfReq->DnsConfig.DNSConf.IPPriority;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSDHCP = pAMISetDNSConfReq->DnsConfig.DNSConf.DNSDHCP;
+            BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIndex = pAMISetDNSConfReq->DnsConfig.DNSConf.DNSIndex;
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+
+
+            break;
+        case AMI_DNS_CONF_DNS_IP:
+
+            if( pAMISetDNSConfReq->Blockselector == 0x0 || pAMISetDNSConfReq->Blockselector > MAX_DNS_IP_ADDRESS)
+            {
+                *pRes = CC_INV_DATA_FIELD;
+                return sizeof(INT8U);
+            }
+
+            if((ReqLen != IP_ADDR_LEN) && (ReqLen != IP6_ADDR_LEN))
+            {
+                TDBG("Reqlen %ld\n",ReqLen);
+                *pRes = CC_REQ_INV_LEN;
+                return sizeof(INT8U);
+            }
+
+            TDBG("Block Selector %d\n",pAMISetDNSConfReq->Blockselector);
+            LOCK_BMC_SHARED_MEM(BMCInst);
+            if(pAMISetDNSConfReq->Blockselector == 1)
+            {
+                memset(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1,0,IP6_ADDR_LEN);
+                if(ReqLen == IP_ADDR_LEN)
+                {
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1[10] = 0xFF;
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1[11] = 0xFF;
+                    memcpy(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1[12],pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                    TDBG("DNS ipv4 address %d %d %d %d\n",pAMISetDNSConfReq->DnsConfig.DNSIPAddr[0],pAMISetDNSConfReq->DnsConfig.DNSIPAddr[1],
+                                                                                    pAMISetDNSConfReq->DnsConfig.DNSIPAddr[2],pAMISetDNSConfReq->DnsConfig.DNSIPAddr[3]);
+                }
+                else
+                {
+                    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1,pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                }
+            }
+            else if(pAMISetDNSConfReq->Blockselector == 2)
+            {
+                memset(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2,0,IP6_ADDR_LEN);
+                if(ReqLen == IP_ADDR_LEN)
+                {
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2[10] = 0xFF;
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2[11] = 0xFF;
+                    memcpy(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2[12],pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                }
+                else
+                {
+                    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2,pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                }
+            }
+            else if(pAMISetDNSConfReq->Blockselector == 3)
+            {
+                memset(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3,0,IP6_ADDR_LEN);
+                if(ReqLen == IP_ADDR_LEN)
+                {
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3[10] = 0xFF;
+                    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3[11] = 0xFF;
+                    memcpy(&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3[12],pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                    TDBG("DNS ipv4 address %d %d %d %d\n",pAMISetDNSConfReq->DnsConfig.DNSIPAddr[0],pAMISetDNSConfReq->DnsConfig.DNSIPAddr[1],
+                                                                                    pAMISetDNSConfReq->DnsConfig.DNSIPAddr[2],pAMISetDNSConfReq->DnsConfig.DNSIPAddr[3]);
+                }
+                else
+                {
+                    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3,pAMISetDNSConfReq->DnsConfig.DNSIPAddr,ReqLen);
+                }
+            }
+            UNLOCK_BMC_SHARED_MEM(BMCInst);
+            break;
+        case AMI_DNS_CONF_DNS_RESTART:
+
+            if(ReqLen != 0)
+            {
+                *pRes = CC_REQ_INV_LEN;
+                return sizeof(INT8U);
+            }
+
+            SetPendStatus(PEND_OP_SET_ALL_DNS_CFG,PEND_STATUS_PENDING);
+            PostPendTask(PEND_OP_SET_ALL_DNS_CFG, 0, 0,g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF,BMCInst);
+            break;
+        default:
+            *pRes = CC_PARAM_NOT_SUPPORTED;
+            return sizeof(INT8U);
+            break;
+    }
 
     pAMISetDNSConfRes->CompletionCode = CC_SUCCESS;
 
     return(sizeof(AMISetDNSConfRes_T));
 }
-
-
 
 int
 AMIGetV6DNSConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst)
 {
-    _NEAR_  AMIGetV6DNSConfRes_T*  pAMIGetV6DNSConfRes = (_NEAR_ AMIGetV6DNSConfRes_T*) pRes;
+//    _NEAR_  AMIGetV6DNSConfRes_T*  pAMIGetV6DNSConfRes = (_NEAR_ AMIGetV6DNSConfRes_T*) pRes;
     
     HOSTNAMECONF HostnameConfig;
     DOMAINCONF DomainConfig;
-    DNSIPV4CONF Dnsv4IPConfig;
-    DNSIPV6CONF Dnsv6IPConfig;
+    DNSCONF Dnsv4IPConfig;
     INT8U registerBMC[MAX_LAN_CHANNELS];
-    INT8U registerBMCFQDN[MAX_LAN_CHANNELS];
-    
+//    INT8U registerBMCFQDN[MAX_LAN_CHANNELS];
+/*    
     V6DNS_CONFIG* pV6dnscfg = &pAMIGetV6DNSConfRes->v6dnscfg;
-    int ret = 0;
+    int ret = 0;*/
 
     memset(&HostnameConfig, 0, sizeof(HostnameConfig));
     memset(&DomainConfig, 0, sizeof(DomainConfig));
     memset(&Dnsv4IPConfig, 0, sizeof(Dnsv4IPConfig));
-    memset(&Dnsv6IPConfig, 0, sizeof(Dnsv6IPConfig));
     memset(registerBMC, 0, sizeof(registerBMC));  
-         
-    ret = nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &Dnsv4IPConfig, &Dnsv6IPConfig, registerBMC ,registerBMCFQDN);
+#if 0
+//    ret = nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &Dnsv4IPConfig, &Dnsv6IPConfig, registerBMC ,registerBMCFQDN);
 
        
     /*  convert data into V6DNS_CONFIG struct */
@@ -349,18 +532,25 @@
     {
         pAMIGetV6DNSConfRes->CompletionCode = CC_SUCCESS;
     }
-    
+#endif
     return(sizeof(AMIGetV6DNSConfRes_T));
 }
-
 
 int
 AMISetV6DNSConf( _NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes,int BMCInst)
 {
     _NEAR_ AMISetV6DNSConfRes_T*pAMISetDNSConfRes = (_NEAR_ AMISetV6DNSConfRes_T*)pRes;
-#if 0    
     _NEAR_ AMISetV6DNSConfReq_T*pAMISetDNSConfReq = (_NEAR_ AMISetV6DNSConfReq_T*)pReq;
 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
+   if(IsCardInActiveFlashMode())
+    {
+            IPMI_WARNING("Card in Active flash mode, Not safe to set any of DNS configurations...\n");
+            *pRes = CC_UNSPECIFIED_ERR;
+            return sizeof (*pRes);
+    }
+#endif    
+    
     if(pAMISetDNSConfReq->v6dnscfg.v6DNSDhcpEnable == 1)
     {
         if(ReqLen >2)
@@ -381,7 +571,7 @@
           
     SetPendStatus(PEND_OP_SET_DNSV6_CFG,PEND_STATUS_PENDING);
     PostPendTask(PEND_OP_SET_DNSV6_CFG, (INT8U *)pAMISetDNSConfReq, sizeof(V6DNS_CONFIG),g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF,BMCInst);
-#endif
+
     pAMISetDNSConfRes->CompletionCode = CC_SUCCESS;
 
     return(sizeof(AMISetDNSConfRes_T));
