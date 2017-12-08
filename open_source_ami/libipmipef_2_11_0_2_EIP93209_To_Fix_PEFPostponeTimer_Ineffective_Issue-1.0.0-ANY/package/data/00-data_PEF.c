--- .pristine/libipmipef-2.11.0-src/data/PEF.c Fri Aug 24 11:19:16 2012
+++ source/libipmipef-2.11.0-src/data/PEF.c Fri Aug 24 15:18:57 2012
@@ -100,6 +100,7 @@
 
 #define SEL_MAX_RECORD_SIZE     0x10
 
+#define ARM_PEF_TEMP_DISABLE        0xFE
  _FAR_ Semaphore_T   hPETAckSem;
 
 static INT8U m_SysEventMsg [] = {
@@ -519,6 +520,13 @@
             case PARAM_PLATFORM_EVT_MSG:
 
             case PARAM_SENSOR_EVT_MSG:
+            	/* If PEF is temporary disabled */ 
+                if (g_BMCInfo[BMCInst].PefConfig.PEFTmrMgr.TmrInterval == ARM_PEF_TEMP_DISABLE) 
+                {
+                    IPMI_DBG_PRINT ("PEF Temporary Disabled\n");
+                    break;
+                }
+				
                 /* If PEF is disabled */
                 if (0 == (pPEFConfig->PEFControl & 0x01))
                 {
