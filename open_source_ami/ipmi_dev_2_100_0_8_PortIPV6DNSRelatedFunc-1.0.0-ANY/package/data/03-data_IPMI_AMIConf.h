--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Fri Feb  3 16:17:30 2012
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Wed Jan 18 20:42:39 2012
@@ -58,6 +58,18 @@
 #define THRESHOLD_RESERVED_BIT  0xC0
 #define DISCRETE_RESERVED_BIT   0x80
 
+#define AMI_DNS_CONF_HOST_NAME                      0x01
+#define AMI_DNS_CONF_REGISTER                          0x02
+#define AMI_DNS_CONF_DOMAIN_SETTINGS           0x03
+#define AMI_DNS_CONF_DOMAIN_NAME                  0x04
+#define AMI_DNS_CONF_DNS_SETTING                    0x05
+#define AMI_DNS_CONF_DNS_IP                               0x06
+#define AMI_DNS_CONF_DNS_RESTART                    0x07
+
+#define MAX_DNS_IP_ADDRESS                            0x03
+#define MAX_DOMAIN_BLOCK_SIZE                      64
+#define MAX_BLOCK                                   0x04
+
 #pragma pack (1)
 
 #define MAX_BOND_IFACE_SLAVES       4
@@ -126,27 +138,59 @@
 typedef struct
 {
     INT8U   CompletionCode;
-    DNS_CONFIG dnscfg;
-    
+    V6DNS_CONFIG v6dnscfg;
+    
+}PACKED AMIGetV6DNSConfRes_T;
+
+typedef struct
+{
+    INT8U   DomainDHCP;
+    INT8U   DomainIndex;
+    INT8U   Domainpriority;
+    INT8U   DomainLen;
+}PACKED DomainSetting;
+
+typedef struct
+{
+    INT8U   DNSDHCP;
+    INT8U   DNSIndex;
+    INT8U   IPPriority;
+}PACKED DNSSetting;
+
+typedef union
+{
+    HOSTNAMECONF        HName;
+    INT8U                       RegDNSConf[MAX_LAN_CHANNEL];
+    DomainSetting           DomainConf;
+    INT8U                       DomainName[MAX_DOMAIN_BLOCK_SIZE];
+    DNSSetting                DNSConf;
+    INT8U                       DNSIPAddr[IP6_ADDR_LEN];
+} DNSConf_T;
+
+typedef struct
+{
+    INT8U   ParamSelect;
+    INT8U   Blockselector;
+    DNSConf_T    DnsConfig;
+}PACKED AMISetDNSConfReq_T;
+
+typedef struct
+{
+    INT8U   CompletionCode;
+    
+}PACKED AMISetDNSConfRes_T;
+
+typedef struct
+{
+    INT8U   CompletionCode;
+    DNSConf_T   DNSCfg;
 }PACKED AMIGetDNSConfRes_T;
 
 typedef struct
 {
-    INT8U   CompletionCode;
-    V6DNS_CONFIG v6dnscfg;
-    
-}PACKED AMIGetV6DNSConfRes_T;
-
-typedef struct
-{
-    DNS_CONFIG dnscfg;
-}PACKED AMISetDNSConfReq_T;
-
-typedef struct
-{
-    INT8U   CompletionCode;
-    
-}PACKED AMISetDNSConfRes_T;
+    INT8U   Param;
+    INT8U   Blockselect;
+}PACKED AMIGetDNSConfReq_T;
 
 typedef struct
 {
