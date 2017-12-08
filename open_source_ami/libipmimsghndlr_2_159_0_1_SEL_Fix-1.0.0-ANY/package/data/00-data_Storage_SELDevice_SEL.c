--- .pristine/libipmimsghndlr-2.159.0-src/data/Storage/SELDevice/SEL.c Wed Sep  7 19:38:09 2011
+++ source/libipmimsghndlr-2.159.0-src/data/Storage/SELDevice/SEL.c Wed Sep  7 19:46:57 2011
@@ -200,11 +200,9 @@
             pBMCInfo->SELConfig.SELEventMsg [13] = EVENT_SEL_IS_FULL;
             pBMCInfo->SELConfig.SELEventMsg [15] = pBMCInfo->SELConfig.SELEventMsg [14] = 0xFF;
 
-            OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->SELMutex, WAIT_INFINITE);
             /* Add SEL Full Event */
             LockedAddSELEntry (&pBMCInfo->SELConfig.SELEventMsg[0], sizeof (pBMCInfo->SELConfig.SELEventMsg), (INT8U*)&AddSelRes,
                            (SYS_IFC_CHANNEL == (g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF) ? TRUE : FALSE),BMCInst);	
-            OS_THREAD_MUTEX_RELEASE(&pBMCInfo->SELMutex);
         }
     }
 
@@ -335,10 +333,10 @@
         if (LastProcessedID)
         {
             _FAR_  SELRec_T*    pSelRec;
-            for (i = 0; i < (pPEFConfig->LastSELRecordID - LastProcessedID); i++)
-            {
-                pSelRec = &m_sel->SELRecord [LastProcessedID + i - 1];
-                if (VALID_RECORD == pSelRec->Valid)
+            for (i = LastProcessedID + 1; i < (pPEFConfig->LastSELRecordID - LastProcessedID); i++)
+            {
+                pSelRec = GetSELRec(i,BMCInst);
+                if (pSelRec != 0)
                 {
                     MsgPkt_T        MsgToPEF;
                     MsgToPEF.Param = PARAM_SENSOR_EVT_MSG;
@@ -855,7 +853,7 @@
 
             
             /* Check if SEL is full or not  */
-            if (!CircularSEL && m_sel->NumRecords == (pBMCInfo->SELConfig.MaxSELRecord ))
+            if (!CircularSEL && m_sel->NumRecords == (pBMCInfo->SELConfig.MaxSELRecord - 1))
             {
                 SetSELSensorReading (1 << EVENT_SEL_IS_FULL,BMCInst);
                 LogSELFullEvent(BMCInst);
