--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Fri Sep 16 11:25:17 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Fri Sep 16 14:21:24 2011
@@ -316,10 +316,10 @@
     INT8U   CompletionCode;
     INT8U   Username[MAX_USERNAME_LEN];
     //INT8U   Password[MAX_PASSWORD_LEN];
-    INT32U snmp_enable;                          
-    INT32U snmp_access_type;                     
-    INT32U snmp_enc_type_1;
-    INT32U snmp_enc_type_2;
+    INT8U snmp_enable;                          
+    INT8U snmp_access_type;                     
+    INT8U snmp_enc_type_1;
+    INT8U snmp_enc_type_2;
     
 }PACKED AMIGetSNMPConfRes_T;
 
@@ -328,10 +328,10 @@
     INT8U   CompletionCode;
     INT8U   Username[MAX_USERNAME_LEN];
     //INT8U   Password[MAX_PASSWORD_LEN];
-    INT32U snmp_enable;                          
-    INT32U snmp_access_type;                     
-    INT32U snmp_enc_type_1;
-    INT32U snmp_enc_type_2;
+    INT8U snmp_enable;                          
+    INT8U snmp_access_type;                     
+    INT8U snmp_enc_type_1;
+    INT8U snmp_enc_type_2;
     
 }PACKED AMIGetSNMPConfReq_T;
 
@@ -339,10 +339,10 @@
 {
     INT8U   Username[MAX_USERNAME_LEN];
     INT8U   Password[MAX_PASSWORD_LEN];
-    INT32U snmp_enable;                          
-    INT32U snmp_access_type;                     
-    INT32U snmp_enc_type_1;
-    INT32U snmp_enc_type_2;
+    INT8U snmp_enable;                          
+    INT8U snmp_access_type;                     
+    INT8U snmp_enc_type_1;
+    INT8U snmp_enc_type_2;
 }PACKED AMISetSNMPConfReq_T;
 
 typedef struct
