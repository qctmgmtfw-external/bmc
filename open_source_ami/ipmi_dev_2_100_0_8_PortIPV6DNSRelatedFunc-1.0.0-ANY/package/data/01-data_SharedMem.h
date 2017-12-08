--- .pristine/ipmi_dev-2.100.0-src/data/SharedMem.h Fri Feb  3 16:17:29 2012
+++ source/ipmi_dev-2.100.0-src/data/SharedMem.h Wed Jan 18 20:25:20 2012
@@ -54,6 +54,29 @@
 #pragma pack( 1 )
 
 #define DISABLED_EVENT_RECEIVER 0xFF
+#define MAX_HOSTNAME_LEN    64
+#define MAX_DNS_IP_ADDRESS                            0x03
+#define MAX_DOMAIN_BLOCK_SIZE                      64
+#define MAX_BLOCK                                               0x04
+
+typedef struct
+{
+    INT8U   HostSetting;
+    INT8U   HostNameLen;
+    INT8U   HostName[MAX_HOSTNAME_LEN];
+    INT8U   RegisterBMC[MAX_LAN_CHANNELS];
+    INT8U   DomainDHCP;
+    INT8U   DomainIndex;
+    INT8U   Domainpriority;
+    INT8U   DomainLen;
+    INT8U   DNSDHCP;
+    INT8U   DNSIndex;
+    INT8U   DomainName[MAX_DOMAIN_BLOCK_SIZE * MAX_BLOCK];
+    INT8U   IPPriority;
+    INT8U   DNSIPAddr1[IP6_ADDR_LEN];
+    INT8U   DNSIPAddr2[IP6_ADDR_LEN];
+    INT8U   DNSIPAddr3[IP6_ADDR_LEN];
+}PACKED DNS_T;
 
 /**
  * @struct BMCSharedMem_T
@@ -136,6 +159,7 @@
     BOOL                AdminDisableIPMBSecondary;
     BOOL                PowerActionInProgress;
 #endif
+    DNS_T               DNSconf;
 } PACKED  BMCSharedMem_T;
 
 #pragma pack( )
