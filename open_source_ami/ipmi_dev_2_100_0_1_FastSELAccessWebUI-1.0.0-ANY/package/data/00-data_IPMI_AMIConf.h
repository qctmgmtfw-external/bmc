--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Thu Sep 15 22:48:04 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Thu Sep 15 22:58:35 2011
@@ -51,6 +51,9 @@
 #define SERVICE_NAME_SIZE 16
 #define MAX_LAN_CHANNEL             0x05
 #define MAX_IFACE_NAME                16
+#define FULL_SEL_ENTRIES     0xFF
+#define PARTIAL_SEL_ENTRIES   0x00
+#define SEL_EMPTY_REPOSITORY 0x00
 
 #pragma pack (1)
 
@@ -385,6 +388,19 @@
     INT8U SleepSeconds;
 }PACKED RestartService_T;
 
+typedef struct
+{
+    INT32U Noofentretrieved;
+}PACKED AMIGetSELEntriesReq_T;
+
+typedef struct
+{
+    INT8U   CompletionCode;
+    INT32U   Noofentries;
+    INT16U   LastRecID;
+    INT8U   Status;
+}PACKED AMIGetSELEntriesRes_T;
+
 
 #pragma pack ()
 
