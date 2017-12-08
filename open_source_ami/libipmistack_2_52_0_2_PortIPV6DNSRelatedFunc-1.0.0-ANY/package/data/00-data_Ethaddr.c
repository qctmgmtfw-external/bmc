--- .pristine/libipmistack-2.52.0-src/data/Ethaddr.c Fri Feb  3 16:18:25 2012
+++ source/libipmistack-2.52.0-src/data/Ethaddr.c Thu Jan 19 10:51:48 2012
@@ -282,6 +282,56 @@
     return 0;
 }
 
+int InitDNSConfiguration(int BMCInst)
+{
+    HOSTNAMECONF    HostConf;
+    DOMAINCONF      DomainConf;
+    DNSCONF             DNSConf;
+    INT8U                   RegBMC_FQDN[MAX_CHANNEL];
+    int retval = 0;
+
+    memset(&HostConf,0,sizeof(HOSTNAMECONF));
+    memset(&DomainConf,0,sizeof(DOMAINCONF));
+    memset(&DNSConf,0,sizeof(DNSCONF));
+    memset(RegBMC_FQDN,0,MAX_CHANNEL);
+
+    retval = nwGetAllDNSConf(&HostConf,&DomainConf,&DNSConf,RegBMC_FQDN);
+    if(retval != 0)
+    {
+        TCRIT("Error in Getting All DNS configurtaion\n");
+        return -1;
+    }
+
+    LOCK_BMC_SHARED_MEM(BMCInst);
+
+    /*Update the HostName Settings*/
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostSetting = HostConf.HostSetting;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostNameLen = HostConf.HostNameLen;
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostName,HostConf.HostName,MAX_HOST_NAME_STRING_SIZE);
+
+    /*Update the DNS Configurations*/
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSDHCP = DNSConf.DNSDHCP;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIndex = DNSConf.DNSIndex;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.IPPriority = DNSConf.IPPriority;
+
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr1,DNSConf.DNSIP1,IP6_ADDR_LEN);
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr2,DNSConf.DNSIP2,IP6_ADDR_LEN);
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DNSIPAddr3,DNSConf.DNSIP3,IP6_ADDR_LEN);
+
+    /*Update the Domain Configurations*/
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainDHCP = DomainConf.dhcpEnable;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainIndex = DomainConf.EthIndex;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.Domainpriority = DomainConf.v4v6;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainLen = DomainConf.domainnamelen;
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.DomainName,DomainConf.domainname,MAX_DOMAIN_NAME_STRING_SIZE);
+
+    /*Update the Register BMC and Method*/
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.RegisterBMC,RegBMC_FQDN,MAX_CHANNEL);
+
+    UNLOCK_BMC_SHARED_MEM(BMCInst);
+    return 0;
+}
+
 /*
 *@fn GetLinkStatus
 *@brief This function helps to find the link status i.e link is up or down
