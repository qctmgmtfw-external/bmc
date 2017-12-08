--- .pristine/libipmi-2.38.0-src/data/include/libipmi_StorDevice.h Thu Sep 15 12:23:01 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_StorDevice.h Thu Sep 15 14:24:36 2011
@@ -21,6 +21,7 @@
 #include "IPMI_FRU.h"
 #include "IPMI_IPMB.h"
 #include "IPMI_Storage.h"
+#include "SELRecord.h"
 
 #include "libipmi_sdr.h"
 #include "std_macros.h"
@@ -86,7 +87,7 @@
 we want all sel entires but we also want sensor names*/
 typedef struct _SELEventRecordWithSensorName
 {
-	SELEventRecord_T EventRecord;
+	SELRec_T EventRecord;
 	uint8 SensorName[65]; //65 is the max possible because SensorName len in SDR is indicated by 6 bits
 } PACKED
 SELEventRecordWithSensorName_T;
