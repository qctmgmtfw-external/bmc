--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Fri Sep 16 23:40:54 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Sat Sep 17 00:02:26 2011
@@ -28,6 +28,7 @@
 #include "Iptables.h"
 #include "IPMI_LANConfig.h"
 #include "IPMI_AppDevice.h"
+#include "IPMI_SDRRecord.h"
 
 
 /* Network Interface EnableState */
@@ -54,6 +55,8 @@
 #define FULL_SEL_ENTRIES     0xFF
 #define PARTIAL_SEL_ENTRIES   0x00
 #define SEL_EMPTY_REPOSITORY 0x00
+#define THRESHOLD_RESERVED_BIT  0xC0
+#define DISCRETE_RESERVED_BIT   0x80
 
 #pragma pack (1)
 
@@ -401,7 +404,49 @@
     INT8U   Status;
 }PACKED AMIGetSELEntriesRes_T;
 
-
+typedef struct
+{
+    INT8U   CompletionCode;
+    INT8U   Noofentries;
+}PACKED AMIGetSensorInfoRes_T;
+
+typedef struct
+{
+    SDRRecHdr_T    hdr;
+    INT8U               OwnerID;
+    INT8U               OwnerLUN;
+    INT8U               SensorNumber;
+    INT8U               SensorReading;
+    INT8U               MaxReading;
+    INT8U               MinReading;
+    INT8U               Flags;
+    INT8U               ComparisonStatus;
+    INT8U               OptionalStatus;
+    INT8U               SensorTypeCode;
+    INT8U               EventTypeCode;
+    INT8U               Units1;
+    INT8U               Units2;
+    INT8U               Units3;
+    INT8U               Linearization;
+    INT8U               M_LSB;
+    INT8U               M_MSB_Tolerance;
+    INT8U               B_LSB;
+    INT8U               B_MSB_Accuracy;
+    INT8U               Accuracy_MSB_Exp;
+    INT8U               RExp_BExp;
+    INT8U               LowerNonCritical;
+    INT8U               LowerCritical;
+    INT8U               LowerNonRecoverable;
+    INT8U               UpperNonCritical;
+    INT8U               UpperCritical;
+    INT8U               UpperNonRecoverable;
+    INT8U               AssertionEventByte1;
+    INT8U               AssertionEventByte2;
+    INT8U               DeassertionEventByte1;
+    INT8U               DeassertionEventByte2;
+    INT8S               SensorName[MAX_ID_STR_LEN];
+
+}PACKED SenInfo_T;
 #pragma pack ()
 
 
