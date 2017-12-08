--- .pristine/libipmi-2.38.0-src/data/include/libipmi_AMIOEM.h Thu Sep 15 12:23:01 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_AMIOEM.h Thu Sep 15 14:24:08 2011
@@ -4,11 +4,14 @@
 /* LIBIPMI core header files */
 #include "libipmi_session.h"
 #include "libipmi_errorcodes.h"
+#include "libipmi_StorDevice.h"
 #include "libipmiifc.h"
 
 /* command specific header files */
 #include "IPMI_AMIDevice.h"
 #include "IPMI_AMIConf.h"
+
+#define MAX_SEL_RETRIEVAL_SIZE 1024 * 60
 
 #ifdef __cplusplus
 extern "C" {
@@ -207,6 +210,12 @@
 
 #endif//SNMP_SUPPORT
 
+LIBIPMI_API uint16 IPMICMD_AMIGetSELEntires(IPMI20_SESSION_T *pSession,AMIGetSELEntriesReq_T *pAMIGetSelEntriesReq,
+                                                   AMIGetSELEntriesRes_T *pAMIGetSelEntiresRes,int timeout);
+
+LIBIPMI_API uint16 LIBIPMI_HL_AMIGetSELEntires(IPMI20_SESSION_T *pSession,SELEventRecordWithSensorName_T *pSELEntriesBuffer,
+                                                   uint32 *nNumSelEntries,int timeout);
+
 LIBIPMI_API uint16 LIBIPMI_HL_AMISetPreserveConfStatus(IPMI20_SESSION_T *pSession, INT8U selector, INT8U status, int timeout);
 
 #ifdef __cplusplus
