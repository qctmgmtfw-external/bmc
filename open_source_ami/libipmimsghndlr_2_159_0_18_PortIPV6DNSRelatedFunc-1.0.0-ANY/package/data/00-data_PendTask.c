--- .pristine/libipmimsghndlr-2.159.0-src/data/PendTask.c Fri Feb  3 16:18:15 2012
+++ source/libipmimsghndlr-2.159.0-src/data/PendTask.c Mon Feb 13 19:17:30 2012
@@ -469,44 +469,40 @@
     NWCFG_STRUCT        NWConfig;
     NWCFG6_STRUCT        NWConfig6;        
     DOMAINCONF      DomainCfg;
-    DNSIPV4CONF     DNSv4;
-    DNSIPV6CONF     DNSv6;
-    INT8U               registerBMC[MAX_LAN_CHANNELS];
-    INT8U               registerFQDN[MAX_LAN_CHANNELS];
+	DNSCONF			DNS;
+    INT8U           regBMC_FQDN[MAX_LAN_CHANNELS];
     int i;
 
     memset(&DomainCfg,0,sizeof(DOMAINCONF));
-    memset(&DNSv4,0,sizeof(DNSIPV4CONF));
-    memset(&DNSv6,0,sizeof(DNSIPV6CONF));
-    memset(registerBMC,0,sizeof(registerBMC));
-    memset(registerFQDN,0,sizeof(registerFQDN));
+    memset(&DNS,0,sizeof(DNSCONF));
+    memset(regBMC_FQDN,0,sizeof(regBMC_FQDN));
 
     nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, EthIndex);    
     NWConfig.CfgMethod = ((NWCFG_STRUCT*)pData)->CfgMethod;
     if(NWConfig.CfgMethod == CFGMETHOD_STATIC)
     {
-        ReadDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN);
+        ReadDNSConfFile(&DomainCfg, &DNS,regBMC_FQDN);
 
         if(DomainCfg.EthIndex == EthIndex)
         {
             if(DomainCfg.v4v6 == 1)
                 DomainCfg.dhcpEnable= 0;
 
-            if(DNSv4.dhcpEnable == 1)
-                DNSv4.dhcpEnable = 0;
+            if(DNS.DNSDHCP == 1)
+                DNS.DNSDHCP = 0;
 
             for(i=0;i<MAX_LAN_CHANNELS;i++)
             {
-                if(registerFQDN[i] == 1)
-                    registerFQDN[i] = 0;
+                if((regBMC_FQDN[i] & 0x10) == 0x10)
+                    regBMC_FQDN[i] |=0x00;
             }
         }
         else
         {
-            registerFQDN[EthIndex] = 0;
-        }
-
-        WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN);
+            regBMC_FQDN[EthIndex] |= 0x0;
+        }
+
+        WriteDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN);
     }
 
     nwWriteNWCfg_ipv4_v6 ( &NWConfig, &NWConfig6, EthIndex);    
@@ -557,17 +553,13 @@
     int retValue = 0;
     char Ifcname[16];
     DOMAINCONF      DomainCfg;
-    DNSIPV4CONF     DNSv4;
-    DNSIPV6CONF     DNSv6;
-    INT8U               registerBMC[MAX_LAN_CHANNELS];
-    INT8U               registerFQDN[MAX_LAN_CHANNELS];
+    DNSCONF     DNS;
+    INT8U               regBMC_FQDN[MAX_LAN_CHANNELS];
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
 
     memset(&DomainCfg,0,sizeof(DOMAINCONF));
-    memset(&DNSv4,0,sizeof(DNSIPV4CONF));
-    memset(&DNSv6,0,sizeof(DNSIPV6CONF));
-    memset(registerBMC,0,sizeof(registerBMC));
-    memset(registerFQDN,0,sizeof(registerFQDN));
+    memset(&DNS,0,sizeof(DNSCONF));
+    memset(regBMC_FQDN,0,sizeof(regBMC_FQDN));
     memset(Ifcname,0,sizeof(Ifcname));
 
     nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, EthIndex);
@@ -581,7 +573,7 @@
         return -1;
     }
 
-    if(ReadDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN) != 0)
+    if(ReadDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN) != 0)
     {
         TCRIT("Error While Reading DNS configuration File\n");
     }
@@ -613,7 +605,7 @@
 
     nwWriteNWCfg_ipv4_v6 ( &NWConfig, &NWConfig6, EthIndex);
 
-    if(WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN) != 0)
+    if(WriteDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN) != 0)
     {
         TCRIT("Error While Writing DNS configuration File\n");
     }
@@ -639,23 +631,19 @@
     NWCFG6_STRUCT        NWConfig6 ;
     char str[40]=VLAN_NETWORK_DECONFIG_FILE;
     DOMAINCONF      DomainCfg;
-    DNSIPV4CONF     DNSv4;
-    DNSIPV6CONF     DNSv6;
-    INT8U               registerBMC[MAX_LAN_CHANNELS];
-    INT8U               registerFQDN[MAX_LAN_CHANNELS];
+    DNSCONF     DNS;
+    INT8U               regBMC_FQDN[MAX_LAN_CHANNELS];
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
 
     memset(&DomainCfg,0,sizeof(DOMAINCONF));
-    memset(&DNSv4,0,sizeof(DNSIPV4CONF));
-    memset(&DNSv6,0,sizeof(DNSIPV6CONF));
-    memset(registerBMC,0,sizeof(registerBMC));
-    memset(registerFQDN,0,sizeof(registerFQDN));
+    memset(&DNS,0,sizeof(DNSCONF));
+    memset(regBMC_FQDN,0,sizeof(regBMC_FQDN));
 
     //nwReadNWCfg(&NWConfig, EthIndex);
     nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, EthIndex);
     NWConfig.VLANID=0;
 
-    if(ReadDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN) != 0)
+    if(ReadDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN) != 0)
     {
         TCRIT("Error While Reading DNS configuration File\n");
     }
@@ -696,7 +684,7 @@
     }
 
     nwWriteNWCfg_ipv4_v6 ( &NWConfig, &NWConfig6, EthIndex);
-    if(WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN) != 0)
+    if(WriteDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN) != 0)
     {
         TCRIT("Error While Writing DNS configuration File\n");
     }
@@ -777,68 +765,46 @@
  */
 int PendSetAllDNSCfg(INT8U* pData, INT32U DataLen, INT8U EthIndex,int BMCInst)
 {
-    DNS_CONFIG* dnscfg = (DNS_CONFIG*)pData;
 
     HOSTNAMECONF HostnameConfig;
     DOMAINCONF DomainConfig;
-    DNSIPV4CONF Dnsv4IPConfig;
-    DNSIPV6CONF Dnsv6IPConfig;
-
-/*    char hostname[MAX_HOST_NAME_STRING_SIZE];
-    int hostsetting=0;
-    int hostnamelen=0;
-*/
-    INT8U registerBMC[MAX_LAN_CHANNELS];
-    INT8U registerFQDN[MAX_LAN_CHANNELS];
-	/*
-    int hostsetting=0;
-    int hostnamelen=0;
-	*/
-
-/*
-    //Get current hostname
-    memset(hostname , '\0', sizeof(MAX_HOST_NAME_STRING_SIZE));
-    if(GetHostNameConf(hostname, &hostsetting) < 0)
-        TCRIT("Error getting DNS RAC Name using gethostname\n");
-
-    hostnamelen = strlen(hostname);
-*/
+    DNSCONF DnsIPConfig;
+    INT8U regBMC_FQDN[MAX_LAN_CHANNELS];
 
     memset(&HostnameConfig, 0, sizeof(HostnameConfig));
     memset(&DomainConfig, 0, sizeof(DomainConfig));
-    memset(&Dnsv4IPConfig, 0, sizeof(Dnsv4IPConfig));
-    memset(&Dnsv6IPConfig, 0, sizeof(Dnsv6IPConfig));
-    memset(registerBMC, 0, sizeof(registerBMC));
-        memset(registerFQDN, 0, sizeof(registerFQDN));
-
-    nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &Dnsv4IPConfig, &Dnsv6IPConfig, registerBMC,registerFQDN );
+    memset(&DnsIPConfig, 0, sizeof(DnsIPConfig));
+    memset(regBMC_FQDN, 0, sizeof(regBMC_FQDN));
+
+    nwGetAllDNSConf( &HostnameConfig, &DomainConfig, &DnsIPConfig,regBMC_FQDN );
+
+    LOCK_BMC_SHARED_MEM(BMCInst);
 
     /*
      * Start of converting data
      */
     //DNS SERVER IP
-    Dnsv4IPConfig.EthIndex = dnscfg->DNSServerEthIndex;
-    Dnsv4IPConfig.dhcpEnable = dnscfg->DNSDhcpEnable;
-
-    if(Dnsv4IPConfig.dhcpEnable == 1)
-    {
-        memset(Dnsv4IPConfig.v4DNSIP1,'\0',INET_ADDRSTRLEN);
-        memset(Dnsv4IPConfig.v4DNSIP2,'\0',INET_ADDRSTRLEN);
+    DnsIPConfig.DNSIndex = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIndex;
+    DnsIPConfig.DNSDHCP = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSDHCP;
+    DnsIPConfig.IPPriority = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.IPPriority;
+
+    if(DnsIPConfig.DNSDHCP== 1)
+    {
+        memset(DnsIPConfig.DNSIP1,'\0',IP6_ADDR_LEN);
+        memset(DnsIPConfig.DNSIP2,'\0',IP6_ADDR_LEN);
+        memset(DnsIPConfig.DNSIP3,'\0',IP6_ADDR_LEN);
     }
     else
     {
-        //convert ip into strings
-        sprintf((char *)&Dnsv4IPConfig.v4DNSIP1, "%d.%d.%d.%d",
-            dnscfg->DNSServer1[0], dnscfg->DNSServer1[1], dnscfg->DNSServer1[2], dnscfg->DNSServer1[3]);
-
-        sprintf((char *)&Dnsv4IPConfig.v4DNSIP2, "%d.%d.%d.%d",
-            dnscfg->DNSServer2[0], dnscfg->DNSServer2[1], dnscfg->DNSServer2[2], dnscfg->DNSServer2[3]);
+        memcpy(DnsIPConfig.DNSIP1,BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1,IP6_ADDR_LEN);
+        memcpy(DnsIPConfig.DNSIP2,BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2,IP6_ADDR_LEN);
+        memcpy(DnsIPConfig.DNSIP3,BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3,IP6_ADDR_LEN);
     }
 
     //DOMAIN NAME
-    DomainConfig.EthIndex = dnscfg->DNSDomainNameEthIndex;
-    DomainConfig.v4v6 = dnscfg->v4v6; //To support both IPv4 and IPv6 in DNS_CONFIG.
-    DomainConfig.dhcpEnable = dnscfg->DNSDomainNameDhcpEnable;
+    DomainConfig.EthIndex = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainIndex;
+    DomainConfig.v4v6 = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.Domainpriority; //To support both IPv4 and IPv6 in DNS_CONFIG.
+    DomainConfig.dhcpEnable = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainDHCP;
     if(DomainConfig.dhcpEnable == 1)
     {
         DomainConfig.domainnamelen = 0;
@@ -846,47 +812,34 @@
     }
     else
     {
-        DomainConfig.domainnamelen = dnscfg->DNSDomainNameLen;
+        DomainConfig.domainnamelen = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainLen;
         if(DomainConfig.domainnamelen <= MAX_DOMAIN_NAME_STRING_SIZE)
         {
             memset(&DomainConfig.domainname , '\0', sizeof(DomainConfig.domainname));
-            strncpy((char *)&DomainConfig.domainname, (char *)&dnscfg->DNSDomainName, DomainConfig.domainnamelen);
+            strncpy((char *)&DomainConfig.domainname, (char *)&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainName, DomainConfig.domainnamelen);
         }
     }
 
     //Register BMC Flag
-    memcpy(registerBMC, dnscfg->DNSRegisterBMC, MAX_LAN_CHANNELS);
-    memcpy(registerFQDN, dnscfg->RegisterBMCFQDN, MAX_LAN_CHANNELS);
-
-    //HOST NAME settings
-	/*
-    //If hostname or hostsetting has been changed, set Hostname
-    if( (strcmp(hostname,(char *)&dnscfg->DNSBMCHostName) != 0) ||
-        (hostsetting != dnscfg->DNSBMCHostSetting) )
-    {
-        if(dnscfg->DNSBMCHostNameLen <= MAX_HOST_NAME_STRING_SIZE)
-        {
-            SetHostNameConf((char *)&dnscfg->DNSBMCHostName, dnscfg->DNSBMCHostSetting);
-        }
-    }
-	*/
-    if(dnscfg->DNSBMCHostNameLen <= MAX_HOST_NAME_STRING_SIZE)
-    {
-        HostnameConfig.HostNameLen = dnscfg->DNSBMCHostNameLen;
+    memcpy(regBMC_FQDN, BMC_GET_SHARED_MEM (BMCInst)->DNSconf.RegisterBMC, MAX_LAN_CHANNELS);
+
+    if(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostNameLen <= MAX_HOST_NAME_STRING_SIZE)
+    {
+        HostnameConfig.HostNameLen = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostNameLen;
     }
     else
     {
         HostnameConfig.HostNameLen = MAX_HOST_NAME_STRING_SIZE;
     }
-    HostnameConfig.HostSetting = dnscfg->DNSBMCHostSetting;
+    HostnameConfig.HostSetting = BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostSetting;
 
     memset(&HostnameConfig.HostName , '\0', sizeof(HostnameConfig.HostName));
-    strncpy((char *)&HostnameConfig.HostName, (char *)&dnscfg->DNSBMCHostName, HostnameConfig.HostNameLen);    
-    //memcpy(&HostnameConfig.HostName, &dnscfg->DNSBMCHostName, HostnameConfig.HostNameLen);
-
+    strncpy((char *)&HostnameConfig.HostName, (char *)&BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostName, HostnameConfig.HostNameLen);
+
+    UNLOCK_BMC_SHARED_MEM(BMCInst);
     /* End of converting data */
 
-    nwSetAllDNSConf( &HostnameConfig, &DomainConfig, &Dnsv4IPConfig, &Dnsv6IPConfig, registerBMC,registerFQDN);
+    nwSetAllDNSConf( &HostnameConfig, &DomainConfig, &DnsIPConfig, regBMC_FQDN);
 
     SetPendStatus (PEND_OP_SET_ALL_DNS_CFG, PEND_STATUS_COMPLETED);
     return 0;
@@ -910,6 +863,7 @@
 
 static int PendSetDNSv6Cfg(INT8U* pData, INT32U DataLen, INT8U EthIndex,int BMCInst)
 {
+#if 0
     HOSTNAMECONF HostnameConfig;
     DOMAINCONF domainConf;
     DNSIPV4CONF dnsIPv4;
@@ -949,7 +903,7 @@
 
     /* Now set All the DNS related configurations */
     nwSetAllDNSConf(&HostnameConfig, &domainConf, &dnsIPv4, &dnsIPv6, registerBMC,registerFQDN);
-
+#endif
     SetPendStatus (PEND_OP_SET_DNSV6_CFG, PEND_STATUS_COMPLETED);
 
     return 0;
@@ -995,6 +949,10 @@
             break;
 
         case ENABLE_V6://Enable IPv6 only is not allowed
+            cfg.enable  = 0;
+            cfg6.enable = 1;
+            break;
+            
         default:
             SetPendStatus (PEND_OP_SET_ETH_IFACE_STATE, PEND_STATUS_COMPLETED);
             return -1;
@@ -1231,31 +1189,27 @@
     NWCFG_STRUCT        NWConfig;
     NWCFG6_STRUCT        NWConfig6;
     DOMAINCONF      DomainCfg;
-    DNSIPV4CONF     DNSv4;
-    DNSIPV6CONF     DNSv6;
-    INT8U               registerBMC[MAX_LAN_CHANNELS];
-    INT8U               registerFQDN[MAX_LAN_CHANNELS];
+    DNSCONF     DNS;
+    INT8U               regBMC_FQDN[MAX_LAN_CHANNELS];
 
     memset(&DomainCfg,0,sizeof(DOMAINCONF));
-    memset(&DNSv4,0,sizeof(DNSIPV4CONF));
-    memset(&DNSv6,0,sizeof(DNSIPV6CONF));
-    memset(registerBMC,0,sizeof(registerBMC));
-    memset(registerFQDN,0,sizeof(registerFQDN));
+    memset(&DNS,0,sizeof(DNSCONF));
+    memset(regBMC_FQDN,0,sizeof(regBMC_FQDN));
 
     nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, EthIndex);
     NWConfig6.CfgMethod= ((NWCFG6_STRUCT*)pData)->CfgMethod;
 
     if(NWConfig6.CfgMethod == CFGMETHOD_STATIC)
     {
-        ReadDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN);
+        ReadDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN);
 
         if(DomainCfg.v4v6 == 2)
             DomainCfg.v4v6 = 0;
 
-        if(DNSv6.dhcpEnable == 1)
-            DNSv6.dhcpEnable = 0;
-
-        WriteDNSConfFile(&DomainCfg, &DNSv4, &DNSv6, registerBMC, registerFQDN);
+        if(DNS.DNSDHCP== 1)
+            DNS.DNSDHCP= 0;
+
+        WriteDNSConfFile(&DomainCfg, &DNS, regBMC_FQDN);
     }
 
     nwWriteNWCfg_ipv4_v6( &NWConfig, &NWConfig6,EthIndex);
