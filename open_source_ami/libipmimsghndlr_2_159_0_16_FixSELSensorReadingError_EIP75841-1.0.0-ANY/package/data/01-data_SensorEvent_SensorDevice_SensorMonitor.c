--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/SensorMonitor.c Tue Jan  3 16:38:33 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/SensorMonitor.c Wed Jan  4 16:26:09 2012
@@ -65,10 +65,10 @@
 #define OTHER_UNITS_SENSOR_TYPE     0x0B
 
 #define LOWER_CRITICAL_GNG_LOW         0x02
-#define UPPER_CRITICAL_GNG_HIGH        0x09 
+#define UPPER_CRITICAL_GNG_HIGH        0x09
 #define LOWER_NON_CRITICAL_GNG_LOW     0x00
 #define UPPER_NON_CRITICAL_GNG_HIGH    0x07
-#define LOWER_NON_RECOVERABLE_GNG_LOW  0x04 
+#define LOWER_NON_RECOVERABLE_GNG_LOW  0x04
 #define UPPER_NON_RECOVERABLE_GNG_HIGH 0x0B
 
 #define SENSOR_MONITOR_INTERVAL     1
@@ -98,7 +98,7 @@
 
 /*
  * @fn InitPowerOnTick
- * @brief Initialize the power on tick counter, 
+ * @brief Initialize the power on tick counter,
  *        should be invoked when ever power on occurs.
  */
 void InitPowerOnTick (int BMCInst)
@@ -110,7 +110,7 @@
 
 /*
  * @fn InitSysResetTick
- * @brief Initialize the System Reset tick counter, 
+ * @brief Initialize the System Reset tick counter,
  *        should be invoked when ever power reset occurs.
  */
 void InitSysResetTick (int BMCInst)
@@ -125,7 +125,7 @@
  *------------------------------------------*/
 int
 InitSensorMonitor (int BMCInst)
-{ 
+{
 
     /* initilize the softprocessor sensor table */
     IPMI_HAL_INIT_SENSOR (BMCInst);
@@ -143,7 +143,7 @@
 
 /*-------------------------------------------
  * GetSensorInfo
- * 
+ *
  * This function will be used by the Sensor
  * hook functions to manipulate Sensor information
  * This function DOES NOT lock sensor information
@@ -152,7 +152,7 @@
 GetSensorInfo (INT8U SensorNum, int BMCInst)
 {
     SensorSharedMem_T* pSMSharedMem = (_FAR_ SensorSharedMem_T*)&g_BMCInfo[BMCInst].SensorSharedMem;
-    return &pSMSharedMem->SensorInfo [SensorNum];	
+    return &pSMSharedMem->SensorInfo [SensorNum];
 }
 
 /*-------------------------------------------
@@ -166,16 +166,16 @@
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
     SensorSharedMem_T* pSMSharedMem = (_FAR_ SensorSharedMem_T*)&g_BMCInfo[BMCInst].SensorSharedMem;
 
-    /* Acquire Shared memory   */ 
-    OS_THREAD_MUTEX_INIT(pBMCInfo->ResetSharedMemMutex, PTHREAD_MUTEX_RECURSIVE); 
+    /* Acquire Shared memory   */
+    OS_THREAD_MUTEX_INIT(pBMCInfo->ResetSharedMemMutex, PTHREAD_MUTEX_RECURSIVE);
     OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->ResetSharedMemMutex, SHARED_MEM_TIMEOUT);
 
 
     /* reset threshold sensors to normal status */
     for (i = 0; i < MAX_SENSOR_NUMBERS; i++)
     {
-        /* Check if sensor present */   
-        if (!pSMSharedMem->SensorInfo [i].IsSensorPresent) { continue; }	
+        /* Check if sensor present */
+        if (!pSMSharedMem->SensorInfo [i].IsSensorPresent) { continue; }
 
         /* Reinitialize based on the event type */
         if (THRESHOLD_SENSOR_CLASS  == pSMSharedMem->SensorInfo [i].EventTypeCode)
@@ -195,7 +195,7 @@
     //OS_RELEASE_MUTEX(ResetSharedMemMutex);
     OS_THREAD_MUTEX_RELEASE(&pBMCInfo->ResetSharedMemMutex);
     //OS_DESTROY_MUTEX(ResetSharedMemMutex);
-    OS_THREAD_MUTEX_DESTROY(&pBMCInfo->ResetSharedMemMutex); 
+    OS_THREAD_MUTEX_DESTROY(&pBMCInfo->ResetSharedMemMutex);
 
 
 }
@@ -213,7 +213,7 @@
     Msg.Param = PARAM_SENSOR_SCAN;
     Msg.Size = 0;
     prctl(PR_SET_NAME,__FUNCTION__,0,0,0);
-     
+
     while(1)
     {
         /* Delay between monitoring cycles */
@@ -247,7 +247,7 @@
             }
         }
     }
-    
+
     return NULL;
 }
 /*-------------------------------------------
@@ -274,30 +274,30 @@
 
     /* Get the Sensor Shared Memory */
     pSMSharedMem = (_FAR_ SensorSharedMem_T*)&pBMCInfo->SensorSharedMem;
-    
+
     /* Diable Global Sensor monitoring during initialization
      * Global Scanning will be enabled when Msghandler
      * does all initialization */
     pSMSharedMem->GlobalSensorScanningEnable = FALSE;
-    
+
     /* Create mutex for Sensor shared memory */
-    OS_THREAD_MUTEX_INIT(pBMCInfo->m_hSMSharedMemMutex, PTHREAD_MUTEX_RECURSIVE); 
+    OS_THREAD_MUTEX_INIT(pBMCInfo->m_hSMSharedMemMutex, PTHREAD_MUTEX_RECURSIVE);
 
     InitSensorMonitor (BMCInst);
 
-    
+
 
     /* This table helps us to reduce unnecessary looping in sensor scanning  */
     pBMCInfo->SenConfig.ValidSensorCnt = 0;
     for ( i = 0; i < MAX_SENSOR_NUMBERS; i++)
     {
-        /* Check if sensor present */   
+        /* Check if sensor present */
         if (!pSMSharedMem->SensorInfo [i].IsSensorPresent) { continue; }
         pBMCInfo->SenConfig.ValidSensorList [pBMCInfo->SenConfig.ValidSensorCnt] = i;
-        pBMCInfo->SenConfig.ValidSensorCnt++;	
-    }
-
-    /* Acquire Shared memory   */ 
+        pBMCInfo->SenConfig.ValidSensorCnt++;
+    }
+
+    /* Acquire Shared memory   */
     OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->m_hSMSharedMemMutex, SHARED_MEM_TIMEOUT);
 
     /* Update Sensor Properties & initialize sensors */
@@ -313,6 +313,7 @@
         ReInitSensor(pSensorInfo,BMCInst);
 #ifdef CONFIG_SPX_FEATURE_ENABLE_INTERNAL_SENSOR
         InitInternalSensors (pSensorInfo,BMCInst);
+        InitSensorHook (pSensorInfo,BMCInst);
 #endif
     }
 
@@ -362,7 +363,7 @@
             {
                 IPMI_DBG_PRINT ("PDK_PreMonitorAllSensors returned -1\n");
                 //Clear the MonitorBusy flag before continue to GetMsg
-                //This is required to avoid the break of Sensor Monitor thread in middle 
+                //This is required to avoid the break of Sensor Monitor thread in middle
                 pBMCInfo->SenConfig.MonitorBusy = 0;
                 continue;
             }
@@ -405,18 +406,18 @@
                     /* Release mutex for Sensor shared memory */
                     OS_THREAD_MUTEX_RELEASE(&pBMCInfo->m_hSMSharedMemMutex);
                     //Clear the MonitorBusy flag before continue to GetMsg
-                    //This is required to avoid the break of Sensor Monitor thread in middle 
-                    pBMCInfo->SenConfig.MonitorBusy = 0;    
+                    //This is required to avoid the break of Sensor Monitor thread in middle
+                    pBMCInfo->SenConfig.MonitorBusy = 0;
                     continue;
                 }
                 else
                 {
-                    pSensorInfo->Err = 0;						
+                    pSensorInfo->Err = 0;
                 }
 
                 if(g_PDKHandle[PDK_PREMONITORINDIVIDUALSENSOR] != NULL)
                 {
-                	// Invoke the Check Sensortick to verify whether scan time reached. 
+                	// Invoke the Check Sensortick to verify whether scan time reached.
                 	if (-1 == ((int(*)(SensorInfo_T*,int)) g_PDKHandle[PDK_PREMONITORINDIVIDUALSENSOR])(pSensorInfo,BMCInst))
                 	{
                 		/* Clear the Error status for this scan */
@@ -425,33 +426,33 @@
                 		/* Release mutex for Sensor shared memory */
                 		OS_THREAD_MUTEX_RELEASE(&pBMCInfo->m_hSMSharedMemMutex);
                 		//Clear the MonitorBusy flag before continue to GetMsg
-                		//This is required to avoid the break of Sensor Monitor thread in middle 
-                		pBMCInfo->SenConfig.MonitorBusy = 0;                		
+                		//This is required to avoid the break of Sensor Monitor thread in middle
+                		pBMCInfo->SenConfig.MonitorBusy = 0;
                 		continue;
                 	}
                 }
-                
+
                 /* Monitoring cycle */
                 if (THRESHOLD_SENSOR_CLASS  == pSensorInfo->EventTypeCode)
-                {
-                    if ((0 == (SensorTick %  pSensorInfo->SensorMonitorInterval)) &&
-                        (pBMCInfo->SenConfig.PowerOnTick >= pSensorInfo->PowerOnDelay) &&
-                        (pBMCInfo->SenConfig.SysResetTick >= pSensorInfo->SysResetDelay)) 
-                    { 
-                        /* Clear the Error status for this scan */
-                        pSensorInfo->Err = 0; 
-                        OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].SensorSharedMemMutex, WAIT_INFINITE);
-                        /* Monitor threshold sensor */
-                        MonitorTSensors (pSensorInfo,BMCInst); 
-                        OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].SensorSharedMemMutex);
-                    }
-                }
-                else
                 {
                     if ((0 == (SensorTick %  pSensorInfo->SensorMonitorInterval)) &&
                         (pBMCInfo->SenConfig.PowerOnTick >= pSensorInfo->PowerOnDelay) &&
                         (pBMCInfo->SenConfig.SysResetTick >= pSensorInfo->SysResetDelay))
-                    { 
+                    {
+                        /* Clear the Error status for this scan */
+                        pSensorInfo->Err = 0;
+                        OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].SensorSharedMemMutex, WAIT_INFINITE);
+                        /* Monitor threshold sensor */
+                        MonitorTSensors (pSensorInfo,BMCInst);
+                        OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].SensorSharedMemMutex);
+                    }
+                }
+                else
+                {
+                    if ((0 == (SensorTick %  pSensorInfo->SensorMonitorInterval)) &&
+                        (pBMCInfo->SenConfig.PowerOnTick >= pSensorInfo->PowerOnDelay) &&
+                        (pBMCInfo->SenConfig.SysResetTick >= pSensorInfo->SysResetDelay))
+                    {
                         /* Clear the Error status for this scan */
                         pSensorInfo->Err = 0;
                         OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].SensorSharedMemMutex, WAIT_INFINITE);
@@ -466,10 +467,10 @@
 
             }
 
-    } 
+    }
 
 #if (TERMINAL_MODE_SUPPORT == 1)
-        /* Calculate Health State for terminal mode */ 
+        /* Calculate Health State for terminal mode */
         UpdateOverallHealthState (BMCInst);
 #endif
 
@@ -501,7 +502,7 @@
             pBMCInfo->SenConfig.SensorMonitorLoopCount++;
         }
 
-    } 
+    }
 
 }
 
@@ -531,7 +532,7 @@
         // Forward the event to the specified slave address.
         IPMI_DBG_PRINT_1 ("Event occured and transfered to 0x%X slave address", pSharedMem->EvRcv_SlaveAddr);
         IPMI_DBG_PRINT_BUF (EventMsg, size);
-        
+
         //Adjustment made due the reason that GeneratorID is a single byte in IPMB.
         EventMsg[1] = EventMsg[0];
 
@@ -624,19 +625,19 @@
 
      pSensorInfo = (SensorInfo_T*)&pSMSharedMem->SensorInfo [SensorNum];
 
-     /* Get Sensor Properties from Softprocessor */ 	
+     /* Get Sensor Properties from Softprocessor */
      IPMI_HAL_GET_SENSOR_PROPERTIES ( pSensorInfo->SensorNumber, &SensorProperties,BMCInst);
 
-     /* Update Sensor properties to shared memory */ 			
-     pSensorInfo->SensorMonitorInterval    = (SensorProperties.MonitorInterval == 0) ? 1 : SensorProperties.MonitorInterval; 
-     pSensorInfo->SensorState 	   	       = SensorProperties.MonitorState; 
-     
+     /* Update Sensor properties to shared memory */
+     pSensorInfo->SensorMonitorInterval    = (SensorProperties.MonitorInterval == 0) ? 1 : SensorProperties.MonitorInterval;
+     pSensorInfo->SensorState 	   	       = SensorProperties.MonitorState;
+
      if (pSensorInfo->SensorReadType != THRESHOLD_SENSOR_CLASS)
      {
         /* Get the normal default value for the digital sensors */
         pSensorInfo->PreviousState 	   = ipmitoh_u16 (SensorProperties.NormalValue);
      }
-     
+
      pSensorInfo->PowerOnDelay = SensorProperties.PowerOnDelay;
      pSensorInfo->SysResetDelay = SensorProperties.SysResetDelay;
 
@@ -652,9 +653,9 @@
     INT8U				SensorNum = 0;
     struct stat buf;
     char SensorFileName[MAX_SEN_NAME_SIZE];
-    
+
     memset(SensorFileName,0,MAX_SEN_NAME_SIZE);
-        
+
     SENSORTHRESH_FILE(BMCInst, SensorFileName);
 
     /* Get SDR for that sensor */
@@ -662,7 +663,7 @@
 
     if (sr == NULL)
     {
-        IPMI_WARNING ("Invalid Sensor number %x. \n", pSensorInfo->SensorNumber);	
+        IPMI_WARNING ("Invalid Sensor number %x. \n", pSensorInfo->SensorNumber);
         SET_SM_INIT_DONE(pSensorInfo->EventFlags);
         return;
     }
@@ -671,17 +672,17 @@
     /* Save Sensor number in case of Shared count sensor */
     SensorNum = pSensorInfo->SensorNumber;
 
-    /* Get SDR for that sensor */	
+    /* Get SDR for that sensor */
     SR_LoadSDRDefaults (sr,pSensorInfo,BMCInst);
 
     /* Restore Sensor number in case of Shared count sensor */
     pSensorInfo->SensorNumber = SensorNum;
 
     /* Invoke init sensor hooks registered if any */
-    InitSensorHook (pSensorInfo,BMCInst);	
+    InitSensorHook (pSensorInfo,BMCInst);
 
     SET_SM_INIT_DONE(pSensorInfo->EventFlags);
-    
+
     if(0 == stat(SensorFileName,&buf))
         SwapSensorThresholds(BMCInst,SensorNum);
 }
@@ -698,10 +699,10 @@
     /* Initialize PDK Sensor callback routines  */
     for ( i = 0; i < MAX_SENSOR_NUMBERS; i++)
     {
-        /* Check if sensor present */   
+        /* Check if sensor present */
         if (!pSMSharedMem->SensorInfo [i].IsSensorPresent) { continue; }
 
-        pSMSharedMem->SensorInfo [i].InitSensorHookCnt  = 0;		
+        pSMSharedMem->SensorInfo [i].InitSensorHookCnt  = 0;
         pSMSharedMem->SensorInfo [i].PostMonitorExtHookCnt = 0;
         pSMSharedMem->SensorInfo [i].PreEventLogHookCnt = 0;
     }
@@ -715,7 +716,7 @@
 int
 PreMonitorSensor (void*  pSenInfo,INT8U* pReadFlags,int BMCInst)
 {
-	//SensorInfo_T*  pSensorInfo = pSenInfo;	
+	//SensorInfo_T*  pSensorInfo = pSenInfo;
 	//printf("######## Default PostMonitorSensor called for Sensor[%x]---------\n",pSensorInfo->SensorNumber);
 	return 0;
 }
@@ -739,7 +740,7 @@
 InitSensorHook (_FAR_ SensorInfo_T*  pSensorInfo,int BMCInst)
 {
     int 	Hooklist,retval=0;
-            
+
     /* Call all Init sensor Hook   */
     for ( Hooklist = 0; Hooklist < pSensorInfo->InitSensorHookCnt; Hooklist++ )
     {
@@ -759,8 +760,8 @@
 PreEventLog (SensorInfo_T* pSensorInfo, INT8U* pEventData,int BMCInst)
 {
     int 	Hooklist,retval = 0;
-    INT8U   RdFlag = 0; 
-        
+    INT8U   RdFlag = 0;
+
     /* Call all PreEventLog Hook for sensors */
     for (Hooklist = 0; Hooklist < pSensorInfo->PreEventLogHookCnt; Hooklist++)
     {
@@ -786,7 +787,7 @@
 CompareValues(BOOL isSigned, INT8U val1, INT8U val2)
 {
     int retval = 0; // default to equal
-    
+
     /* Do comparison based on isSigned flag */
     if (FALSE == isSigned)
     {
@@ -804,10 +805,10 @@
     {
         // Signed comparison
         INT16  sval1, sval2;
-        
+
         sval1 = (INT8)val1;
         sval2 = (INT8)val2;
-        
+
         if (sval1 < sval2)
         {
             retval = -1;
@@ -828,7 +829,7 @@
 INT16 GetHighDeassertValue(bool IsSigned, INT16 Value, INT8U Hysteresis)
 {
     INT16 deassertValue = (Value - Hysteresis);
-    
+
     if (FALSE == IsSigned)
     {
         if (deassertValue < 0)
@@ -839,7 +840,7 @@
         if (deassertValue < -128)
             deassertValue = -128;
     }
-    
+
     return deassertValue;
 }
 
@@ -860,7 +861,7 @@
         if (deassertValue > 127)
             deassertValue = 127;
     }
-    
+
     return deassertValue;
 }
 
@@ -907,7 +908,7 @@
     INT8U                       DeassertionHistoryByte1;
     INT8U                       DeassertionHistoryByte2;
     INT8U                   OEMField;
-    INT8U			    SensorNum;	
+    INT8U			    SensorNum;
     BOOL                SignedSensor = 0; // 1 if sensor has signed values
     INT16               SensorMax = 255;
     INT16               SensorMin = 0;
@@ -921,7 +922,7 @@
     /* Get Sensor Reading from SP only if it not a settable sensor */
     if (0 == GET_SETTABLE_SENSOR_BIT(pSensorInfo->SensorInit))
     {
-        /* get the power state of host */		
+        /* get the power state of host */
 
         if(g_PDKHandle[PDK_GETPSGOOD] != NULL)
         {
@@ -930,10 +931,10 @@
 
         /* If sensor is not need to monitor on stand by power, and if Server */
         /* is power off then return setting error "sensor not acceble at present state" */
-        if  (( 0 == (pSensorInfo->SensorState & 0x1) ) && ( 0 == PSGood)) 
+        if  (( 0 == (pSensorInfo->SensorState & 0x1) ) && ( 0 == PSGood))
         {
             // Response cannot be provided in this state
-            pSensorInfo->Err = CC_PARAM_NOT_SUP_IN_CUR_STATE ; 
+            pSensorInfo->Err = CC_PARAM_NOT_SUP_IN_CUR_STATE ;
             pSensorInfo->EventFlags |= BIT5;
             return;
         }
@@ -979,12 +980,12 @@
                 /* Do we have some error  in reading ?? */
                 if ( 0 == pSensorInfo->Err )
                 {
-                    // get the power state again 
+                    // get the power state again
                     if(g_PDKHandle[PDK_GETPSGOOD] != NULL)
                     {
                         PSGood = ((int(*)(int))g_PDKHandle[PDK_GETPSGOOD]) (BMCInst);
                     }
-    
+
                     /* check to see the power state of the server to confirm its still in power good state */
                     if  (( 0 == (pSensorInfo->SensorState & 0x01) ) && (0 == PSGood ))
                     {
@@ -997,19 +998,19 @@
                     // Update pSensorInfo here and comment it below where it was being overwritten by the local variable
                     // Maintain full 16 bit value in case post processing needs the full value.
                     if (pBMCInfo->IpmiConfig.OPMASupport == 1)
-                        pSensorInfo->SensorReading += pSensorInfo->SenReadingOffset; 
+                        pSensorInfo->SensorReading += pSensorInfo->SenReadingOffset;
                 }
                 else
                 {
                     pSensorInfo->pPostMonitor  (pSensorInfo,&ReadFlags,BMCInst);
                     pSensorInfo->EventFlags |= BIT5;
-                    return;	
+                    return;
                 }
 
             }
             else
             {
-                // get the power state again 
+                // get the power state again
                 if(g_PDKHandle[PDK_GETPSGOOD] != NULL)
                 {
                     PSGood = ((int(*)(int))g_PDKHandle[PDK_GETPSGOOD]) (BMCInst);
@@ -1027,7 +1028,7 @@
             }
         }
     }
-    
+
     /* Call PDK_PostMonitorSensor and dont monitor the sensor if it returns -1*/
     // Do not call PostMonitor if we are overriding the sensor value since it may change it.
     if ((0 == Override) && (0 != pSensorInfo->pPostMonitor  (pSensorInfo,&ReadFlags,BMCInst)))
@@ -1053,7 +1054,7 @@
 
     // Make sure the reading is only 8 bits.
     pSensorInfo->SensorReading &= 0x00ff;
-    
+
 #if 0
     IPMI_DBG_PRINT_2  ("SensorNo = %02x     Reading = %02x\n", \
                         pSensorInfo->SensorNumber, pSensorInfo->SensorReading);
@@ -1147,13 +1148,13 @@
         }
 
         StartLevel = Level;
-        
+
         switch (Level)
         {
             case SENSOR_STATUS_NORMAL:
                 if (Value <= WarningLow)
                 {
-                    IPMI_DBG_PRINT_3("#%02x LNC(A) - Value: %02x Threshold: %02x\n", 
+                    IPMI_DBG_PRINT_3("#%02x LNC(A) - Value: %02x Threshold: %02x\n",
                     SensorNum, (INT8U)Value, (INT8U)WarningLow);
                     /* deassert WarningLow going High */
                     EvData1 = 0x51;
@@ -1282,7 +1283,7 @@
                     DeassertThreshold = GetLowDeassertValue(SignedSensor, WarningLow, NegHysteresis);
                     if (Value > DeassertThreshold)
                     {
-                        IPMI_DBG_PRINT_3("#%02x LNC(D) - Value: %02x Threshold: %02x\n", 
+                        IPMI_DBG_PRINT_3("#%02x LNC(D) - Value: %02x Threshold: %02x\n",
                         SensorNum, (INT8U)Value, (INT8U)DeassertThreshold);
                         /* deassert WarningLow going Low */
                         EvData1 = 0x50;
@@ -1567,7 +1568,7 @@
         /* event direction|type */
         EventMsg [5] = 0x01;
 
-        EventMsg [6] = 0;	
+        EventMsg [6] = 0;
 
         /* Set Event Data only if it is a settable sensor */
         if (0 == GET_SETTABLE_SENSOR_BIT(pSensorInfo->SensorInit))
@@ -1612,7 +1613,7 @@
         if (SendAssertion)
         {
 
-            if ((EventData1 & 0x0f) < 8)    
+            if ((EventData1 & 0x0f) < 8)
             {
                 AssertionHistoryByte1 = (1 << (EventData1 & 0x0f));
             }
@@ -1622,7 +1623,7 @@
             }
 
 
-            /* For Manual Rearm Sensor - Check for already generated events in the History bytes 
+            /* For Manual Rearm Sensor - Check for already generated events in the History bytes
             For Auto Rearm Sensor -History bytes are not valid */
             if ( (0 == (AssertionHistoryByte1 & pSensorInfo->AssertionHistoryByte1)) &&
                 (0 == (AssertionHistoryByte2 & pSensorInfo->AssertionHistoryByte2)) )
@@ -1658,10 +1659,10 @@
             }
             SendAssertion = 0;
         }
-        
+
         if (SendDeassertion)
         {
-            if ((EvData1 & 0x0f) < 8)    
+            if ((EvData1 & 0x0f) < 8)
             {
                 DeassertionHistoryByte1 = (1 << (EvData1 & 0x0f));
             }
@@ -1670,7 +1671,7 @@
                 DeassertionHistoryByte2 = (1 << ((EvData1 & 0x0f) - 8));
             }
 
-            /* For Manual Rearm Sensor - Check for already generated events in the History bytes 
+            /* For Manual Rearm Sensor - Check for already generated events in the History bytes
             For Auto Rearm Sensor -History bytes are not valid */
             if ( (0 == (DeassertionHistoryByte1 & pSensorInfo->DeassertionHistoryByte1)) &&
             (0 == (DeassertionHistoryByte2 & pSensorInfo->DeassertionHistoryByte2)) )
@@ -1747,7 +1748,7 @@
         HealthLevel = SENSOR_STATUS_NONRECOVERABLE_HIGH;
 
     pSensorInfo->HealthLevel = HealthLevel;
-    
+
     UpdateThresHealthState (pSensorInfo, HealthLevel,BMCInst);
 #endif
 
@@ -1788,7 +1789,7 @@
         /* Get Sensor Reading from SP only if it not a settable sensor */
         if (0 == GET_SETTABLE_SENSOR_BIT(pSensorInfo->SensorInit))
         {
-            /* get the power state of host */		
+            /* get the power state of host */
             if(g_PDKHandle[PDK_GETPSGOOD] != NULL)
             {
                 PSGood = ((int(*)(int))g_PDKHandle[PDK_GETPSGOOD]) (BMCInst);
@@ -1796,9 +1797,9 @@
 
             /* If sensor is not need to monitor on stand by power, and if Server */
             /* is power off then return setting error "sensor not acceble at present state" */
-            if  (( 0 == (pSensorInfo->SensorState & 0x01)) && (0 == PSGood )) 
-            {
-                pSensorInfo->Err = CC_PARAM_NOT_SUP_IN_CUR_STATE ; 
+            if  (( 0 == (pSensorInfo->SensorState & 0x01)) && (0 == PSGood ))
+            {
+                pSensorInfo->Err = CC_PARAM_NOT_SUP_IN_CUR_STATE ;
                 pSensorInfo->EventFlags |= BIT5;
                 return;
             }
@@ -1853,7 +1854,7 @@
                     /* read the sensor using softprocessor */
                     pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber, &SensorReading, BMCInst);
                     if( pBMCInfo->IpmiConfig.OPMASupport == 1)
-                        pSensorInfo->SensorReading = SensorReading + pSensorInfo->SenReadingOffset;   
+                        pSensorInfo->SensorReading = SensorReading + pSensorInfo->SenReadingOffset;
                     else
                         pSensorInfo->SensorReading = SensorReading;
                 #endif
@@ -1865,14 +1866,14 @@
                         {
                             PSGood = ((int(*)(int))g_PDKHandle[PDK_GETPSGOOD]) (BMCInst);
                         }
-    
+
                         /* check to see the power state of the server to confirm its still in power good state */
                         if  (( 0 == (pSensorInfo->SensorState & 0x01) ) && ( 0 == PSGood ))
                         {
                             pSensorInfo->Err = CC_PARAM_NOT_SUP_IN_CUR_STATE ;
                             pSensorInfo->pPostMonitor (pSensorInfo,&ReadFlags,BMCInst);
-                            pSharedSensorInfo 			= API_GetSensorInfo (pSensorInfo->SensorNumber, BMCInst);	
-                            pSensorInfo->PreviousState 	=  pSharedSensorInfo->PreviousState;						 
+                            pSharedSensorInfo 			= API_GetSensorInfo (pSensorInfo->SensorNumber, BMCInst);
+                            pSensorInfo->PreviousState 	=  pSharedSensorInfo->PreviousState;
                             pSensorInfo->EventFlags |= BIT5;
                             return;
                         }
@@ -1880,12 +1881,12 @@
                 }
                 else
                 {
-                    // get the power state again 
+                    // get the power state again
                     if(g_PDKHandle[PDK_GETPSGOOD] != NULL)
                     {
                         PSGood = ((int(*)(int))g_PDKHandle[PDK_GETPSGOOD]) (BMCInst);
                     }
-    
+
                     /* check to see the power state of the server to confirm its still in power good state */
                     if  (( 0 == (pSensorInfo->SensorState & 0x01) ) && (0 == PSGood ))
                     {
@@ -1914,7 +1915,7 @@
         return;
     }
     /* Could be possible that the Sensor callback function could have updated
-    * the previous state */   	 
+    * the previous state */
     pSensorInfo->PreviousState 	=  PreviousState;
 
 
@@ -1924,7 +1925,7 @@
         pSensorInfo->EventFlags |= BIT5;
         return ;
     }
-    
+
 #if 0
     IPMI_DBG_PRINT_3 ("SensorNo = %x     Reading = %x %x\n", \
                       pSensorInfo->SensorNumber, \
@@ -1942,7 +1943,7 @@
     {
         // Make sure the "Reading Not Available" bit is clear.
         pSensorInfo->EventFlags &= ~BIT5;
-        
+
         return;
     }
 
@@ -2116,7 +2117,7 @@
     pSensorInfo->PreviousState = pSensorInfo->SensorReading;
     // Make sure the "Reading Not Available" bit is clear.
     pSensorInfo->EventFlags &= ~BIT5;
-    
+
 }
 
 
@@ -2442,7 +2443,7 @@
             if(g_PDKHandle[PDK_PREMONITORALLSENSORS] != NULL)
             {
                 ((int(*)(bool,int)) g_PDKHandle[PDK_PREMONITORALLSENSORS])(g_BMCInfo[BMCInst].SenConfig.InitAgentRearm,BMCInst);
-               
+
             }
         }
     }
@@ -2461,7 +2462,7 @@
                     if (0 == (pReArmSensorReq->ReArmAllEvents & BIT7))
                     {
                         if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                        {         
+                        {
                             EventsToRearm = ((pSensorInfo->DeassertionEventEnablesByte2<<8)+pSensorInfo->DeassertionEventEnablesByte1)
                                                          & (((pSensorInfo->AssertionHistoryByte2<<8) + pSensorInfo->AssertionHistoryByte1)
                                                         ^ ((pSensorInfo->DeassertionHistoryByte2<<8)+pSensorInfo->DeassertionHistoryByte1) );
@@ -2476,7 +2477,7 @@
                     else
                     {
                         if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                        {            
+                        {
                             EventsToRearm =(((pReArmSensorReq->ReArmAssertionEvents2<<8)+pReArmSensorReq->ReArmAssertionEvents1)
                                                         & ((pSensorInfo->DeassertionEventEnablesByte2<<8)+pSensorInfo->DeassertionEventEnablesByte1))
                                                         & (((pSensorInfo->AssertionHistoryByte2<<8) + pSensorInfo->AssertionHistoryByte1)
@@ -2492,7 +2493,7 @@
                 else  /* Auto Rearm Sensor */
                 {
                     if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                    {         
+                    {
                         EventsToRearm = ((pSensorInfo->AssertionEventOccuredByte2<<8) + pSensorInfo->AssertionEventOccuredByte1)
                                                     & ((pSensorInfo->DeassertionEventEnablesByte2<<8)+pSensorInfo->DeassertionEventEnablesByte1);
                     }
@@ -2506,14 +2507,14 @@
                 pSensorInfo->DeassertionEventOccuredByte2   = 0;
 
                 if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                {         
+                {
                     /* Is event message generation enabled ? */
                     if (0 != (pSensorInfo->EventFlags & BIT7))
                     {
                         GenerateDeassertionEvent( EventsToRearm, pSensorInfo,BMCInst);
                     }
                 }
-            }	
+            }
             else
             {
                 /* Rearm the non threshold sensor */
@@ -2532,7 +2533,7 @@
                     if (0 == (pReArmSensorReq->ReArmAllEvents & BIT7))
                     {
                         if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                        {            
+                        {
                             EventsToRearm =((pSensorInfo->DeassertionEventEnablesByte2<<8)+pSensorInfo->DeassertionEventEnablesByte1)
                                                         & ( ((pSensorInfo->AssertionHistoryByte2<<8) + pSensorInfo->AssertionHistoryByte1)
                                                         ^ ((pSensorInfo->DeassertionHistoryByte2<<8)+pSensorInfo->DeassertionHistoryByte1) );
@@ -2553,7 +2554,7 @@
                         {
                             EventsToRearm =(((pReArmSensorReq->ReArmAssertionEvents2<<8)+pReArmSensorReq->ReArmAssertionEvents1)
                                                         & ((pSensorInfo->DeassertionEventEnablesByte2<<8)+pSensorInfo->DeassertionEventEnablesByte1))
-                                                        & ( ((pSensorInfo->AssertionHistoryByte2<<8) + pSensorInfo->AssertionHistoryByte1) 
+                                                        & ( ((pSensorInfo->AssertionHistoryByte2<<8) + pSensorInfo->AssertionHistoryByte1)
                                                         ^((pSensorInfo->DeassertionHistoryByte2<<8)+pSensorInfo->DeassertionHistoryByte1) );
                         }
                         /* Modify sensors PreviousState value so that appropriate events are generated
@@ -2584,7 +2585,7 @@
                 }
 
                 if(pBMCInfo->IpmiConfig.EventsForRearm == 1)
-                {         
+                {
                     /* Is event message generation enabled ? */
                     if ( 0 != (pSensorInfo->EventFlags & BIT7))
                     {
@@ -2675,7 +2676,7 @@
                 break;
         } /* switch (pMsg->Param) */
     }while(FALSE == exit);
-    
+
     return;
 }
 
@@ -2796,23 +2797,23 @@
 	_FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
 	pSenSharedMem = (_FAR_ SensorSharedMem_T*)&pBMCInfo->SensorSharedMem;
 	char SensorFileName[MAX_SEN_NAME_SIZE];
-	
+
 	memset(SensorFileName,0,MAX_SEN_NAME_SIZE);
-	
+
 	SENSORTHRESH_FILE(BMCInst, SensorFileName);
-	
-	
+
+
 	sensorfp = fopen(SensorFileName,"rb");
 	if(sensorfp == NULL)
 	return -1;
-	
+
 	OS_THREAD_MUTEX_ACQUIRE(&g_BMCInfo[BMCInst].SensorSharedMemMutex, WAIT_INFINITE);
-	
-	
+
+
 	while((sizeof(SensorThresholds) == fread((char *)&SensorRecord,sizeof(char),sizeof(SensorThresholds),sensorfp)))
 	{
 	 if(pSenSharedMem->SensorInfo[SensorNum].EventTypeCode == THRESHOLD_SENSOR_CLASS)
-   {	
+   {
 			if(pSenSharedMem->SensorInfo[SensorNum].SensorNumber == SensorRecord.SensorNum)
 			{
 				pSenSharedMem->SensorInfo[SensorNum].LowerCritical = SensorRecord.LowerCritical;
@@ -2825,12 +2826,12 @@
 		}
 		memset(&SensorRecord,0,sizeof(SensorThresholds));
 	}
-	
+
 	fclose(sensorfp);
-	
+
 	OS_THREAD_MUTEX_RELEASE(&g_BMCInfo[BMCInst].SensorSharedMemMutex);
 
-	
+
 	return 1;
 }
 #endif /*SENSOR_DEVICE == 1*/
