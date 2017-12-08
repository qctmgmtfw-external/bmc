--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/SensorMonitor.c Wed Dec 21 21:08:57 2011
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/SensorMonitor.c Wed Dec 21 21:55:39 2011
@@ -946,10 +946,37 @@
             /* Call PDK_PreMonitorSensor and skip default sensor reading if it returns -1 or non zero */
             if (0 == pSensorInfo->pPreMonitor (pSensorInfo,&ReadFlags,BMCInst))
             {
+               #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+                {
+                    _FAR_ FullSensorRec_T*      sfs=NULL;
+                    _FAR_ CompactSensorRec_T*   scs=NULL;
+
+                    if (FULL_SDR_REC==pSensorInfo->SDRRec->Type)
+                    {
+                        sfs = (_FAR_ FullSensorRec_T*)pSensorInfo->SDRRec;
+                        if(sfs->OwnerID != pBMCInfo->NMConfig.NMDevSlaveAddress)
+                        {
+                            pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber,  &SensorReading,BMCInst);
+                            pSensorInfo->SensorReading = SensorReading;
+                        }
+                    }
+                    else if (COMPACT_SDR_REC==pSensorInfo->SDRRec->Type)
+                    {
+                        scs = (_FAR_ CompactSensorRec_T*)pSensorInfo->SDRRec;
+                        if(scs->OwnerID != pBMCInfo->NMConfig.NMDevSlaveAddress)
+                        {
+                            pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber,  &SensorReading,BMCInst);
+                            pSensorInfo->SensorReading = SensorReading;
+                        }
+                    }
+
+                }
+               #else
                 /* read the sensor */
                 pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber,  &SensorReading,BMCInst);
                 pSensorInfo->SensorReading = SensorReading;
-                /* Do we have some error  in reading ?? */	
+               #endif
+                /* Do we have some error  in reading ?? */
                 if ( 0 == pSensorInfo->Err )
                 {
                     // get the power state again 
@@ -1785,13 +1812,52 @@
                 /* Call PreMonitorSensor and skip default sensor reading if it returns -1 or non zero */
                 if (0 == pSensorInfo->pPreMonitor(pSensorInfo,&ReadFlags,BMCInst))
                 {
+
+                #ifdef CONFIG_SPX_FEATURE_INTEL_INTELLIGENT_POWER_NODE_MANAGER
+                  {
+                    _FAR_ FullSensorRec_T*      sfs=NULL;
+                    _FAR_ CompactSensorRec_T*   scs=NULL;
+
+                    if (FULL_SDR_REC==pSensorInfo->SDRRec->Type)
+                    {
+                        sfs = (_FAR_ FullSensorRec_T*)pSensorInfo->SDRRec;
+                        if(sfs->OwnerID != pBMCInfo->NMConfig.NMDevSlaveAddress)
+                        {
+                            pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber, &SensorReading, BMCInst);
+                            if( pBMCInfo->IpmiConfig.OPMASupport == 1)
+                                pSensorInfo->SensorReading = SensorReading + pSensorInfo->SenReadingOffset;
+                            else
+                                pSensorInfo->SensorReading = SensorReading;
+
+                            IPMI_DBG_PRINT("owner id ==BMC, keep read \n");
+                        }
+                        else
+                        {
+                            IPMI_DBG_PRINT("owner id ==NM, skip read \n");
+                        }
+                    }
+                    else if (COMPACT_SDR_REC==pSensorInfo->SDRRec->Type)
+                    {
+                        scs = (_FAR_ CompactSensorRec_T*)pSensorInfo->SDRRec;
+                        if(scs->OwnerID != pBMCInfo->NMConfig.NMDevSlaveAddress)
+                        {
+                            pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber, &SensorReading, BMCInst);
+                            if( pBMCInfo->IpmiConfig.OPMASupport == 1)
+                                pSensorInfo->SensorReading = SensorReading + pSensorInfo->SenReadingOffset;
+                            else
+                                pSensorInfo->SensorReading = SensorReading;
+                        }
+                    }
+                  }
+                #else
                     /* read the sensor using softprocessor */
                     pSensorInfo->Err  = IPMI_HAL_GET_SENSOR_READING (pSensorInfo->SensorNumber, &SensorReading, BMCInst);
                     if( pBMCInfo->IpmiConfig.OPMASupport == 1)
                         pSensorInfo->SensorReading = SensorReading + pSensorInfo->SenReadingOffset;   
                     else
-                        pSensorInfo->SensorReading = SensorReading;  
-    
+                        pSensorInfo->SensorReading = SensorReading;
+                #endif
+
                     if ( 0 == pSensorInfo->Err )
                     {
                         // Get the power state again and check
