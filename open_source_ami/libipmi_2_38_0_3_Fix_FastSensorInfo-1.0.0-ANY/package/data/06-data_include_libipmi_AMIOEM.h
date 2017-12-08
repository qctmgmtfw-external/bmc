--- .pristine/libipmi-2.38.0-src/data/include/libipmi_AMIOEM.h Fri Sep 16 23:41:46 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_AMIOEM.h Fri Sep 16 23:56:08 2011
@@ -10,8 +10,10 @@
 /* command specific header files */
 #include "IPMI_AMIDevice.h"
 #include "IPMI_AMIConf.h"
+#include "SensorMonitor.h"
 
 #define MAX_SEL_RETRIEVAL_SIZE 1024 * 60
+#define MAX_SENSOR_INFO_SIZE    1024 * 60
 
 #ifdef __cplusplus
 extern "C" {
@@ -218,6 +220,11 @@
 
 LIBIPMI_API uint16 LIBIPMI_HL_AMISetPreserveConfStatus(IPMI20_SESSION_T *pSession, INT8U selector, INT8U status, int timeout);
 
+LIBIPMI_API uint16 IPMICMD_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,
+                                                   AMIGetSensorInfoRes_T *pAMIGetSensorInfoRes,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSensorInfo(IPMI20_SESSION_T *pSession,SenInfo_T *pSensorInfo,uint32 *nNumSensor,int timeout);
+
 #ifdef __cplusplus
 }
 #endif
