--- .pristine/ipmi_dev-2.100.0-src/data/BridgeMsgAPI.h Tue Dec 20 22:01:31 2011
+++ source/ipmi_dev-2.100.0-src/data/BridgeMsgAPI.h Wed Dec 21 00:43:53 2011
@@ -23,6 +23,6 @@
 #include "Types.h"
 
 extern int
-API_BridgeInternal(MsgPkt_T* pMsgPkt, char *QueueName, int* ChNum, int* IPMBBusNum,BOOL ResponseTracking, int BMCInst);
+API_BridgeInternal(MsgPkt_T* pMsgPkt, INT8U DestAddr, int ChNum, BOOL ResponseTracking, int BMCInst);
 #endif
 
