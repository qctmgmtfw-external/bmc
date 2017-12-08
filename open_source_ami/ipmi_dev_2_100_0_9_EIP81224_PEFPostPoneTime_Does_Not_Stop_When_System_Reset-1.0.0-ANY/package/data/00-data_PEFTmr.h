--- .pristine/ipmi_dev-2.100.0-src/data/PEFTmr.h Thu Apr 26 21:16:36 2012
+++ source/ipmi_dev-2.100.0-src/data/PEFTmr.h Thu Apr 26 21:31:18 2012
@@ -59,7 +59,8 @@
     INT8U   TakePEFAction;  /**< Action to be taken on timeout */
     INT8U   StartDlyTmr;        /**< Count Startup delay*/
     INT8U   AlertStartDlyTmr;   /**< Count Alert Startup delay*/
-    INT8U   ResetFlag;
+    INT8U   StartDlyResetFlag;
+    INT8U	PEFTimerResetFlag;
 
 } PACKED  PEFTmrMgr_T;
 
