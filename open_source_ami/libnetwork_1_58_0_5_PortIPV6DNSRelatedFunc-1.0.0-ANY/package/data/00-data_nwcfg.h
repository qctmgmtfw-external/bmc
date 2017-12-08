--- .pristine/libnetwork-1.58.0-src/data/nwcfg.h Fri Feb  3 16:17:34 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.h Mon Feb 13 19:18:34 2012
@@ -84,6 +84,7 @@
 #define VLAN_PROC_SYS_RAC_NCSI_ENABLE_LAN    "echo 1 > /proc/sys/ractrends/ncsi/Enable"
 
 #define VLAN_NETWORK_IP_CONFIGFILE    "/etc/init.d/vlannetworking enableip"
+#define IF_STATIC_ADDR_STR " pre-up ip addr add"
 
 /*Bond*/
 #define BONDING_CONF_FILE   "/conf/bond.conf"
@@ -112,7 +113,7 @@
 #define MAX_IPV6ADDRS  16
 
 /* DNS */
-#define MAX_HOST_NAME_STRING_SIZE       (64+1)
+#define MAX_HOST_NAME_STRING_SIZE       64
 #define MAX_DOMAIN_NAME_STRING_SIZE     256
 #define HOSTNAME_SETTING_MANUAL			0
 #define HOSTNAME_SETTING_AUTO			1
@@ -177,7 +178,6 @@
                          | NWEXT_IPCFG_IP \
                          | NWEXT_IPCFG_MASK \
                          | NWEXT_IPCFG_GW)
-
 
 /*Used to check the flag status*/
 #define CHECK_FLAG(in,level) ((in&level) == level)
@@ -435,6 +435,17 @@
 
 typedef struct
 {
+    INT8U   DNSIP1[IP6_ADDR_LEN];
+    INT8U   DNSIP2[IP6_ADDR_LEN];
+    INT8U   DNSIP3[IP6_ADDR_LEN];
+    INT8U   DNSDHCP;
+    INT8U   DNSIndex;
+    INT8U   IPPriority;
+}PACKED DNSCONF;
+
+/*
+typedef struct
+{
     INT8U    dhcpEnable;
     INT8U    EthIndex;
     INT8U    v4DNSIP1[INET_ADDRSTRLEN];
@@ -450,7 +461,7 @@
     INT8U    v6DNSIP2[INET6_ADDRSTRLEN];
     
 }PACKED DNSIPV6CONF;
-
+*/
 
 typedef struct
 {
@@ -458,7 +469,7 @@
     INT8U   BondIndex;                                                      /*Index value of Bond Interface*/
     INT8U   BondMode;                                                       /*Bond Mode*/
     INT16U   MiiInterval;                                                     /*MII Interval*/
-    INT8U   Slaves;                                                           /*Each bit represents the interface Index value i.e 0-7*/
+    INT8U   Slaves;                                                           /*Each bit represents the interface Index value i.e 0-7*/    
 }PACKED BondIface;
 
 
@@ -535,7 +546,7 @@
 void ConvertIP6numToStr(unsigned char *var, unsigned int len,unsigned char *string) ;
 int nwReadNWCfg_IPv6(NWCFG6_STRUCT *cfg,INT8U EthIndex);
 int GetDefaultGateway_ipv6(unsigned char *gw,INT8U *Interface);
-int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS1v6, char *DNS2v6, char* domain,unsigned char* domainnamelen);
+int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS3, INT8U DNSIPPriority,char* domain,unsigned char* domainnamelen);
 
 /**
 *@fn nwReadNWCfg_v4_v6
@@ -634,18 +645,12 @@
 
 
 /* DNS */
-int nwSetResolvConf_v4_v6(char* dns1,char* dns2,char* dnsv6_1,char* dnsv6_2,char* domain);
-
-int ReadDNSConfFile ( DOMAINCONF *DomainConfig, DNSIPV4CONF *Dnsv4IPConfig,
-					  DNSIPV6CONF *Dnsv6IPConfig, INT8U *registerBMC ,INT8U *registerFQDN);
-int WriteDNSConfFile ( DOMAINCONF *DomainConfig, DNSIPV4CONF *Dnsv4IPConfig, 
-		               DNSIPV6CONF *Dnsv6IPConfig, INT8U *registerBMC,INT8U *registerFQDN );
-int nwGetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, 
-				     DNSIPV4CONF *Dnsv4IPConfig, DNSIPV6CONF *Dnsv6IPConfig, 
-				     INT8U *registerBMC,INT8U *registerFQDN );
-int nwSetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, 
-					 DNSIPV4CONF *Dnsv4IPConfig, DNSIPV6CONF *Dnsv6IPConfig, 
-					 INT8U *registerBMC,INT8U *registerFQDN);
+int nwSetResolvConf_v4_v6(char* dns1,char* dns2,char* dnsv3,char* domain);
+
+int ReadDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN);
+int WriteDNSConfFile ( DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig, INT8U *regBMC_FQDN);
+int nwGetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN );
+int nwSetAllDNSConf( HOSTNAMECONF *HostnameConfig, DOMAINCONF *DomainConfig, DNSCONF *DnsIPConfig,INT8U *regBMC_FQDN );
 
 /* Library for AMI Extended DNS command  */
 int nwSetHostName(char *name);
