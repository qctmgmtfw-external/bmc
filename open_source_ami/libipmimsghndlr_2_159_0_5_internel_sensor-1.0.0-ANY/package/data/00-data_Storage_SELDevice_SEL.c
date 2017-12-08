--- .pristine/libipmimsghndlr-2.159.0-src/data/Storage/SELDevice/SEL.c Thu Sep  8 22:50:17 2011
+++ source/libipmimsghndlr-2.159.0-src/data/Storage/SELDevice/SEL.c Fri Sep  9 15:24:38 2011
@@ -847,7 +847,7 @@
             if ((GetSELFullPercentage(BMCInst) >= SEL_ALMOST_FULL_PERCENTAGE) && (g_BMCInfo[BMCInst].SELConfig.selalmostfull != 1))
             {
                 /* Set SEL reading to be almost full */
-                SetSELSensorReading (1 << EVENT_SEL_ALMOST_FULL,BMCInst);
+                SetSELSensorReading (EVENT_SEL_ALMOST_FULL,BMCInst);
                 g_BMCInfo[BMCInst].SELConfig.selalmostfull = 1;
             }
 
@@ -855,7 +855,7 @@
             /* Check if SEL is full or not  */
             if (!CircularSEL && m_sel->NumRecords == (pBMCInfo->SELConfig.MaxSELRecord - 1))
             {
-                SetSELSensorReading (1 << EVENT_SEL_IS_FULL,BMCInst);
+                SetSELSensorReading ( EVENT_SEL_IS_FULL,BMCInst);
                 LogSELFullEvent(BMCInst);
             }
 #endif
@@ -1286,21 +1286,21 @@
     pClrSelRes->CompletionCode = CC_NORMAL;
     pClrSelRes->EraseProgress  = SEL_ERASE_COMPLETED;
     pBMCInfo->SELConfig.SELOverFlow = FALSE;
+    pBMCInfo->SELConfig.RsrvIDCancelled = TRUE;
+    pBMCInfo->SELConfig.selalmostfull = 0;
+
+    OS_THREAD_MUTEX_RELEASE(&pBMCInfo->SELMutex);
 
 #ifdef CONFIG_SPX_FEATURE_ENABLE_INTERNAL_SENSOR
     /* Add an Clear SEL Log if needed */
-    SetSELSensorReading (1 << EVENT_LOG_AREA_RESET,BMCInst);
+    SetSELSensorReading ( EVENT_LOG_AREA_RESET,BMCInst);
     LogClearSELEvent(BMCInst);
 #endif
-    pBMCInfo->SELConfig.RsrvIDCancelled = TRUE;
-    pBMCInfo->SELConfig.selalmostfull = 0;
-
 
     if(g_PDKHandle[PDK_POSTCLEARSEL] != NULL)	
     {
         ((INT8U(*)(int)) g_PDKHandle[PDK_POSTCLEARSEL])(BMCInst);
     }
-    OS_THREAD_MUTEX_RELEASE(&pBMCInfo->SELMutex);
 
     return sizeof (ClearSELRes_T);
 }
