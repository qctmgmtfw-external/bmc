--- .pristine/ipmi_dev-2.100.0-src/data/Pnm.h Tue Dec 20 22:01:31 2011
+++ source/ipmi_dev-2.100.0-src/data/Pnm.h Wed Dec 21 00:43:14 2011
@@ -23,6 +23,7 @@
 
 #include "Types.h"
 #include "MsgHndlr.h"
+#include "hal_defs.h"
 
 /*------------------ PNM Command Numbers -------------------------*/
 #define CMD_PNM_OEM_GET_READING              0xE2
@@ -32,6 +33,10 @@
 #define PNM_PLATFORM_POWER_CONSUMPTION      0
 #define PNM_INLET_AIR_TEMPERATURE           1
 #define PNM_ICC_TDC                         2
+
+#define NM_IPMB_MSG_Q        MSG_PIPES_PATH "NMIPMBQ"
+#define INVALID_RECORD    0xff
+#define MAX_PDK_NM_Sensor 80
 
 /*PNM Command Handler table */
 extern const CmdHndlrMap_T g_PNM_CmdHndlr[];
@@ -43,7 +48,7 @@
     INT8U   ReadingType;
     INT8U   reserved1;
     INT8U   reserved2;
-} PACKED PnmOemGetReadingReq_T;
+} PnmOemGetReadingReq_T;
 
 
 /* PnmOemGetReadingrRes_T */
@@ -53,16 +58,42 @@
     INT8U   ReadingType;
     INT8U   ReadingData1;
     INT8U   ReadingData2;
-} PACKED PnmOemGetReadingrRes_T;
+} PnmOemGetReadingrRes_T;
 
 
 /* PnmMePowerStateChangeRes_T */
 typedef struct
 {
     INT8U   CompletionCode;
-} PACKED PnmMePowerStateChangeRes_T;
+} PnmMePowerStateChangeRes_T;
+
+typedef struct
+{
+    INT8U		TYPE_ENUM;
+    INT8U		ID;
+    INT8U       valid;
+} SensorInfoSYS_T;
+
+typedef int (*Fill_handler) (MsgPkt_T *pMsgPkt ,INT16U *byte_counter );
+typedef int (*Parse_handler)(MsgPkt_T *pMsgPkt, INT8U NM_SensorID, INT8U res_par, int BMCInst);
+
+typedef struct
+{
+    INT8U		TYPE_ENUM;
+    INT8U		BMC_SensorID;
+    INT8U		NetFnLUN;
+    INT8U		cmd;
+    INT8U       NM_SensorID;
+    INT8U       res_par;
+    Fill_handler fill_package_hndlr;
+    Parse_handler parse_package_hndlr;
+} SensorInfoCUST_T;
 
 extern int PnmOemGetReading   (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,int BMCInst);
 extern int PnmOemMePowerStateChange (_NEAR_ INT8U* pReq,INT8U ReqLen,_NEAR_ INT8U* pRes,int BMCInst);
 
+// generic function for NodeManager
+#ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+extern int NM_RegisterOneSensor(hal_t *phal);
+#endif
 #endif //_PNM_H_
