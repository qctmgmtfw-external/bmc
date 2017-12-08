--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/internal_sensor.c Tue Jan  3 16:38:32 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/internal_sensor.c Wed Jan  4 16:18:49 2012
@@ -15,31 +15,31 @@
  *
  * intenal_sensor.c
  * Internal Sensor functions.
- * 
+ *
  *  Any internal Sensor implementation requires following
- * 
+ *
  *  SensorTypeCode		  - As per IPMI specificaiton. This
- * 							sensor will be registered only 
+ * 							sensor will be registered only
  *  				        if it is found in SDRs
- * 
+ *
  *  InitHook              - This hook will be used to initialize
  *                          the sensor for its default state.
- * 
- *  PreMonitorSensorHook  - this hook should provide the 
+ *
+ *  PreMonitorSensorHook  - this hook should provide the
  *                          sensor reading for the core sensor
  *                          and should return -1 to stop
  *                          actual sensor reading in SensorMonitor.c
- * 
- *  PostMonitorSensorHook - this hook should provide the 
+ *
+ *  PostMonitorSensorHook - this hook should provide the
  * 							event generation capability for this
- *                          sensor and return -1 to stop 
+ *                          sensor and return -1 to stop
  *                          generic event generation.
- * 
+ *
  *  SetSensorReadingHook  - This function will used wherever
- *                          applicable in the core to set a new state 
+ *                          applicable in the core to set a new state
  *                          for this sensor.
- * 
- * 
+ *
+ *
  *
  *  Author: Vinothkumar S <vinothkumars@ami.com>
  ******************************************************************/
@@ -80,7 +80,7 @@
 #ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
     INTERNAL_SENSOR_OPSTATE,
 #endif
-	
+
 } InternalSensor_E;
 
 
@@ -91,16 +91,16 @@
 /*** Local Variables ***/
  InternalSensorTbl_T m_internal_sensor_tbl [] = {
 
-    { 	SENSOR_TYPE_EVT_LOGGING, 
+    { 	SENSOR_TYPE_EVT_LOGGING,
         0,
-        0xFF,  
+        0xFF,
         0,
         0,
         0,
         0,
-        SELSensorInit, 
-        SELMonitor, 
-        SELEventLog, 
+        SELSensorInit,
+        SELMonitor,
+        SELEventLog,
         SELEventLogExt  			/* Post monitor Ext hook  */
     },	// change sensor# to 0x07
 
@@ -109,7 +109,7 @@
         0xFF,  						/* Sensor Number          */
         0,							/* Assertion tracking     */
         0,                          /* WDT use                */
-        0,							/* Assertion tracking     */	
+        0,							/* Assertion tracking     */
         0,							/* WDT restarted          */
         WD2SensorInit, 				/* Init Sensor hook       */
         WD2Monitor, 				/* Pre monitor hook       */
@@ -148,7 +148,7 @@
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
     /* Register core internal sensor handler
     * if we have a valid SDR for that sensor
-    *  */	
+    *  */
     for (i = 0; i < pBMCInfo->InternalSensorTblSize; i++)
     {
         /* Check for a match */
@@ -203,7 +203,7 @@
         /* Check if we have a match for this sensor type */
         if (pBMCInfo->InternalSensorTbl[i].SensorTypeCode == SensorType)
         {
-            u8SensorNumber = pBMCInfo->InternalSensorTbl[i].SensorNum;		
+            u8SensorNumber = pBMCInfo->InternalSensorTbl[i].SensorNum;
         }
     }
 
@@ -215,7 +215,7 @@
         pSensorInfo = GetSensorInfo(u8SensorNumber, BMCInst);
     }
 
-    return (void*)pSensorInfo;	
+    return (void*)pSensorInfo;
 }
 
 
@@ -228,7 +228,7 @@
 /*-----------------------------------------
  * SELSensorInit
  *-----------------------------------------*/
-int 
+int
 SELSensorInit (void*  pSenInfo,int BMCInst)
 {
     SensorInfo_T*  pSensorInfo = pSenInfo;
@@ -242,20 +242,26 @@
     pSensorInfo->PreviousState = 0;
     pSensorInfo->SensorReading = 0;
 
-    RecordCount = (INT16U)((g_BMCInfo[BMCInst].SELConfig.MaxSELRecord/4)* 3);  
+    RecordCount = (INT16U)((g_BMCInfo[BMCInst].SELConfig.MaxSELRecord/4)* 3);
 
     /* Initialize the SEL Sensor based on current state */
     if (SelInfo.RecCt == 0)
     {
         /* The SEL has been cleared  */
         pSensorInfo->PreviousState = 0;
-        pSensorInfo->SensorReading = EVENT_LOG_AREA_RESET;
+        pSensorInfo->SensorReading = 0;
+    }
+    else if (SelInfo.RecCt == g_BMCInfo[BMCInst].SELConfig.MaxSELRecord)
+    {
+        /* SEL is full*/
+        pSensorInfo->PreviousState = 0;
+        pSensorInfo->SensorReading = EVENT_SEL_IS_FULL;
     }
     else if (SelInfo.OpSupport & OVERFLOW_FLAG)
     {
         /* SEL is full*/
     }
-    else if (SelInfo.RecCt >=  RecordCount)
+    else if((SelInfo.RecCt >=  RecordCount) && (SelInfo.RecCt <g_BMCInfo[BMCInst].SELConfig.MaxSELRecord ))
     {
         /* SEL is almost full */
         pSensorInfo->PreviousState = 0;
@@ -264,15 +270,15 @@
 
     pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_SEL].SensorReading = pSensorInfo->SensorReading;
 
-    
+
     return 0;
 
 }
 
 /*-----------------------------------------
- * SELMonitor 
- *-----------------------------------------*/
-int 
+ * SELMonitor
+ *-----------------------------------------*/
+int
 SELMonitor (void *pSenInfo, INT8U* pReadFlags,int BMCInst)
 {
 	SensorInfo_T*  pSensorInfo = pSenInfo;
@@ -290,7 +296,7 @@
 /*-----------------------------------------
  * GetSELFullPercentage
  *-----------------------------------------*/
-INT8U 
+INT8U
 GetSELFullPercentage(int BMCInst)
 {
     SELInfo_T	SelInfo;
@@ -309,11 +315,11 @@
 /*-----------------------------------------
  * SELEventLog
  *-----------------------------------------*/
-int 
+int
 SELEventLog (void *pSenInfo, INT8U* pReadFlags, int BMCInst)
 {
     INT8U			EventMsg [EVENT_MSG_LENGTH];
-    SensorInfo_T*   pSensorInfo,*pReqSensorInfo = pSenInfo;	
+    SensorInfo_T*   pSensorInfo,*pReqSensorInfo = pSenInfo;
 
 
     /* Get Sensor information */
@@ -325,6 +331,7 @@
     if (0 == (pReqSensorInfo->SensorReading ^ pSensorInfo->PreviousState))
     {
         /* No state !! No Events !!1, but still PostMonitorExt for futher process */
+        pSensorInfo->SensorReading = (pReqSensorInfo->SensorReading == 0)? pReqSensorInfo->SensorReading : (1 << pReqSensorInfo->SensorReading);
         *pReadFlags = 0;
         return -1;
     }
@@ -333,7 +340,8 @@
     if ((pSensorInfo->AssertionEventEnablesByte1 & (1 << pSensorInfo->SensorReading)) == 0)
     {
         // event is not supported but still need PostMonitorExt to do futher process
-        *pReadFlags = 0;		
+        pSensorInfo->SensorReading = (pReqSensorInfo->SensorReading == 0)? pReqSensorInfo->SensorReading : (1 << pReqSensorInfo->SensorReading);
+        *pReadFlags = 0;
         return -1;
     }
 
@@ -349,27 +357,29 @@
 
     /* event direction|type */
     EventMsg [5] = pSensorInfo->EventTypeCode;
-    EventMsg [6] = pReqSensorInfo->SensorReading;//(INT8U)*pSensorReading;	
-    
+    EventMsg [6] = pReqSensorInfo->SensorReading;//(INT8U)*pSensorReading;
+
     switch (pSensorInfo->SensorReading)
     {
         case EVENT_LOG_AREA_RESET:
         case EVENT_SEL_IS_FULL:		/* Intentional fall thru !!!! */
-            /* Event Data 2 and 3 are unspecified */    	
-            EventMsg [7] = 0xFF;		
+            /* Event Data 2 and 3 are unspecified */
+            EventMsg [7] = 0xFF;
             EventMsg [8] = 0xFF;
             break;
 
         case EVENT_SEL_ALMOST_FULL:
-            /* Event Data 2 and 3 are unspecified */    	
-            EventMsg [7] = 0xFF;		
+            /* Event Data 2 and 3 are unspecified */
+            EventMsg [7] = 0xFF;
             EventMsg [8] = GetSELFullPercentage(BMCInst);
             break;
 
     }
-    
+
     /* Set Previous state to new state */
-    pSensorInfo->PreviousState = pSensorInfo->SensorReading = pReqSensorInfo->SensorReading;//*pSensorReading;
+    //pSensorInfo->PreviousState = pSensorInfo->SensorReading = pReqSensorInfo->SensorReading;//*pSensorReading;
+    pSensorInfo->PreviousState = pReqSensorInfo->SensorReading;//*pSensorReading;
+    pSensorInfo->SensorReading = (pReqSensorInfo->SensorReading == 0)? pReqSensorInfo->SensorReading : (1 << pReqSensorInfo->SensorReading);
 
     /* We can't delay in logging clear and Full Event so this will be
     * taken care in SEL.c itself */
@@ -390,7 +400,7 @@
  * SELEventLogExt
  *
  * This function is called by the PostMonitorExt hook function with the
- * local sensor data struction passed from the Sensor Monioring Task. 
+ * local sensor data struction passed from the Sensor Monioring Task.
  * Any changes in the sensor data structure will be cpoied into the master
  * sensor storage.
  *
@@ -404,7 +414,7 @@
     pSensorInfo->EventFlags &= 0xDF;
 
     // it is an event only sensor, the sensor event should be automatically deasserted.
-    // clear the assertion occured bits, and setup the de-assertion occured bits only 
+    // clear the assertion occured bits, and setup the de-assertion occured bits only
     // when the change is occured.
     if (pSensorInfo->SensorReading ^ pSensorInfo->PreviousState)
     {
@@ -454,7 +464,7 @@
 /*-----------------------------------------
  * SetSELSensorReading
  *-----------------------------------------*/
-int 
+int
 SetSELSensorReading (INT16U Reading,int BMCInst)
 {
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
@@ -470,7 +480,7 @@
 /*-----------------------------------------
  * WD2SensorInit
  *-----------------------------------------*/
-int 
+int
 WD2SensorInit (void*  pSenInfo,int BMCInst)
 {
     SensorInfo_T*  pSensorInfo = pSenInfo;
@@ -486,16 +496,16 @@
 
     //printf ("WD2 Sensor Init done\n");
     //printf ("Previous state = %x\n", pSensorInfo->PreviousState);
-    //printf ("Reading        = %x\n", m_internal_sensor_tbl [INTERNAL_SENSOR_WD2].SensorReading);    
-    
+    //printf ("Reading        = %x\n", m_internal_sensor_tbl [INTERNAL_SENSOR_WD2].SensorReading);
+
     return 0;
 
 }
 
 /*-----------------------------------------
- * WD2Monitor 
- *-----------------------------------------*/
-int 
+ * WD2Monitor
+ *-----------------------------------------*/
+int
 WD2Monitor (void* pSenInfo, INT8U* pReadFlags,int BMCInst)
 {
 	 SensorInfo_T*  pSensorInfo = pSenInfo;
@@ -504,14 +514,14 @@
     pSensorInfo->SensorReading = pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_WD2].SensorReading;
 
     /* Sensor Reading is calculated don't proceed regular monitoring */
-    *pReadFlags = 1;		
+    *pReadFlags = 1;
     return -1;
 
 }
 /*---------------------------------------------------------------------
  * WD2EventLog
  *--------------------------------------------------------------------*/
-int 
+int
 WD2EventLog (void* pSenInfo, INT8U* pReadFlags,int BMCInst)
 {
     SensorInfo_T*  		pSensorInfo = pSenInfo;
@@ -542,7 +552,7 @@
         /* event direction|type */
         EventMsg [5] = pSensorInfo->EventTypeCode;
 
-        /* Event Data 3 are unspecified */    	
+        /* Event Data 3 are unspecified */
         EventMsg [8] = 0xFF;
 
         //-------------------------------------------------------------------
@@ -556,7 +566,7 @@
         u8EventOccured = pSensorInfo->AssertionEventOccuredByte2;
         u8EventEnabled = pSensorInfo->AssertionEventEnablesByte2;
 
-        // do the pre-timeout interrupt SEL event first in offset 8, only if 
+        // do the pre-timeout interrupt SEL event first in offset 8, only if
         // the pre-timeout occured but the event has not been generated
         if (((u16EventHistory >> EVENT_TIMER_INT) & 0x1) && (u8EventOccured & 0x1) == 0)
         {
@@ -564,10 +574,10 @@
             if (0 == (u8TmrUse & 0x80) && (u8EventEnabled & 0x1))
             {
                 // event offset plus the ED2 support
-                EventMsg [6] = EVENT_TIMER_INT | 0xC0;	
-
-                /* Event Data 2 provides more information on Timer int */    	
-                EventMsg [7] = (u8TmrUse & 0x07) | (u8TmrActions & 0xF0);		
+                EventMsg [6] = EVENT_TIMER_INT | 0xC0;
+
+                /* Event Data 2 provides more information on Timer int */
+                EventMsg [7] = (u8TmrUse & 0x07) | (u8TmrActions & 0xF0);
 
                 /* Post Event Message */
                 if (0 == (g_WDTTmrMgr.WDTTmr.TmrUse & 0x80))
@@ -580,7 +590,7 @@
             pSensorInfo->AssertionEventOccuredByte2 |= 0x1;
 
             // clear the pre-timeout interrupt event generated flag
-            pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_WD2].u16AssertionHistory &= 0xFEFF; 
+            pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_WD2].u16AssertionHistory &= 0xFEFF;
         }
 
         //-------------------------------------------------------------------
@@ -600,13 +610,13 @@
             {
                 if ((u16EventHistory & (1 << i)) && (u8EventOccured & (1 << i)) == 0)
                 {
-                    // make sure the WDT Log is enabled, and event logging is enabled 
+                    // make sure the WDT Log is enabled, and event logging is enabled
                     if (0 == (u8TmrUse & 0x80) && (u8EventEnabled & (1 << i)))
                     {
                         // event offset plus the ED2 support
                         EventMsg [6] = i | 0xC0;
 
-                        /* Event Data 2 provides more information on Timer int */    	
+                        /* Event Data 2 provides more information on Timer int */
                         EventMsg [7] = (u8TmrUse & 0x07) | (u8TmrActions & 0xF0);
 
                         /* Post Event Message */
@@ -627,7 +637,7 @@
             }
 
             // Clear the "Don't Log" bit after WDT timeout action or system reset.
-            // Don't clear the "Don't Log" bit if the WDT SET / RESET came after 
+            // Don't clear the "Don't Log" bit if the WDT SET / RESET came after
             // the timeout was occurred but before the event was generated. That is,
             // we are processing the pending events here while a new WDT is set.
             if (pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_WD2].u8Restarted == 0)
@@ -641,7 +651,7 @@
     }
 
     // when we get here, all pending events are generated.
-    // if the WDT is restarted, re-arm the WDT sensor by 
+    // if the WDT is restarted, re-arm the WDT sensor by
     // clear the event occurred in the sensorInfo, and reset
     // the WDT re-arm flag
     if (pBMCInfo->InternalSensorTbl[INTERNAL_SENSOR_WD2].u8Restarted)
@@ -675,12 +685,12 @@
  * WD2EventLogExt
  *
  * This function is called by the PostMonitorExt hook function with the
- * local sensor data struction passed from the Sensor Monioring Task. 
+ * local sensor data struction passed from the Sensor Monioring Task.
  * Any changes in the sensor data structure will be cpoied into the master
  * sensor storage.
  *
  *--------------------------------------------------------------------*/
-int 
+int
 WD2EventLogExt (void* pSenInfo, INT8U* pReadFlags,int BMCInst)
 {
 
@@ -691,11 +701,11 @@
  *
  * SetWD2SensorReading
  *
- * This function stores the current WDT sensor reading, and keep 
+ * This function stores the current WDT sensor reading, and keep
  * tracking those un-logged assertions.
  *
  *--------------------------------------------------------------------*/
-int 
+int
 SetWD2SensorReading (INT16U Reading, INT8U u8TmrUse, INT8U u8TmrActions,int BMCInst)
 {
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
@@ -723,11 +733,11 @@
  * RestartWD2Sensor
  *
  * This function is called when the WDT2 sensor structure needs to be
- * initialized, including the internal sensor assertion flag. Mostly, 
+ * initialized, including the internal sensor assertion flag. Mostly,
  * the SetWDT / ResetWDT command or the sensor init is executed.
  *
  *--------------------------------------------------------------------*/
-int 
+int
 RestartWD2Sensor(int BMCInst)
 {
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
@@ -750,7 +760,7 @@
 /*-----------------------------------------
  * OpStateSensorInit
  *-----------------------------------------*/
-int 
+int
 OpStateSensorInit (void* pSenInfo, int BMCInst)
 {
     SensorInfo_T* pSensorInfo = pSenInfo;
@@ -765,9 +775,9 @@
 }
 
 /*-----------------------------------------
- * OpStateMonitor 
- *-----------------------------------------*/
-int 
+ * OpStateMonitor
+ *-----------------------------------------*/
+int
 OpStateMonitor (void *pSenInfo, INT8U* pReadFlags, int BMCInst)
 {
     SensorInfo_T* pSensorInfo = pSenInfo;
@@ -799,7 +809,7 @@
 /*-----------------------------------------
  * OpStateEventLog
  *-----------------------------------------*/
-int 
+int
 OpStateEventLog (void *pSenInfo, INT8U* pReadFlags, int BMCInst)
 {
     BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
