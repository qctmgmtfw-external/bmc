--- .pristine/libipmi-2.38.0-src/data/libipmi_sensor.c Fri Sep 16 23:41:45 2011
+++ source/libipmi-2.38.0-src/data/libipmi_sensor.c Sat Sep 17 16:06:05 2011
@@ -629,10 +629,23 @@
 LIBIPMI_API uint16
 LIBIPMI_HL_GetSensorCount( IPMI20_SESSION_T *pSession, int *sdr_count, int timeout )
 {
-    return( LIBIPMI_HL_LoadSensorSDRs( pSession, NULL, sdr_count, timeout ) );
-}
-
-
+    uint16 wRet;
+    SDRRepositoryInfo_T pResSDRRepoInfo;
+
+    wRet =  IPMICMD_GetSDRRepositoryInfo( pSession,
+                                                        &pResSDRRepoInfo,
+                                                        timeout);
+    if(wRet != LIBIPMI_E_SUCCESS)
+    {
+        TWARN("Error in Getting Num of Sensor infon\n");
+        return wRet;
+    }
+
+    *sdr_count = pResSDRRepoInfo.RecCt;
+    return wRet;
+}
+
+#if 0
 LIBIPMI_API uint16
 LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
                                  struct sensor_data *sensor_list, int timeout )
@@ -767,3 +780,112 @@
     free( sdr_buffer );
     return( 0 );
 }
+#endif
+
+/**
+ * @fn LIBIPMI_HL_GetAllSensorReadings
+ * @brief High Level API to get all the sensor Info
+ * @params Session, sensor_list, nNumSensor, timout
+ * @return proper completion code on success
+ */
+
+LIBIPMI_API uint16
+LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
+                                 struct sensor_data *sensor_list, uint32* nNumSensor,int timeout )
+{
+    uint16 wRet;
+    int sdr_count;
+    int i ;
+    SenInfo_T * pSensorBuff = NULL;
+    unsigned char* pSensor = NULL;
+
+    /* Load the SDRs related to sensors into memory */
+    wRet = LIBIPMI_HL_GetSensorCount( pSession,&sdr_count, timeout );
+    if( wRet != LIBIPMI_E_SUCCESS )
+    {
+        TWARN( "Cannot load sensor SDRs\n" );
+        return( wRet );
+    }
+
+    pSensorBuff = (SenInfo_T*) malloc(sdr_count* sizeof(SenInfo_T));
+    if(pSensorBuff == NULL)
+    {
+        TWARN("Error in Allocating the Memory\n");
+        return -1;
+    }
+    pSensor = (unsigned char *)pSensorBuff;
+
+    wRet = LIBIPMI_HL_AMIGetSensorInfo(pSession, pSensorBuff, nNumSensor, timeout);
+    if ( wRet != LIBIPMI_E_SUCCESS )
+    {
+        free(pSensorBuff);
+        if( wRet == LIBIPMI_E_SENSOR_INFO_EMPTY)
+        {
+            TINFO("Sensor Info is empty\n");
+        }
+        else
+        {
+            TWARN("Error in getting sensor info\n");
+        }
+        return (wRet);
+    }
+
+    for(i = 0; i < *nNumSensor; i++)
+    {
+        sensor_list[ i ].sensor_num = pSensorBuff->SensorNumber;
+        sensor_list[ i ].owner_id = pSensorBuff->OwnerID;
+        sensor_list[ i ].owner_lun = pSensorBuff->OwnerLUN;
+        sensor_list[ i ].sensor_type = pSensorBuff->SensorTypeCode;
+        sensor_list[ i ].sensor_units[ 0 ] = pSensorBuff->Units1;
+        sensor_list[ i ].sensor_units[ 1 ] = pSensorBuff->Units2;
+        sensor_list[ i ].sensor_units[ 2 ] = pSensorBuff->Units3;
+        sensor_list[ i ].raw_reading = pSensorBuff->SensorReading;
+        strncpy(sensor_list[ i ].sensor_name,pSensorBuff->SensorName,MAX_ID_STR_LEN);
+        sensor_list[i].SensorAccessibleFlags = 0;
+
+        if(pSensorBuff->EventTypeCode > 1)
+        {
+            /*Updating the discrete sensor details*/
+            sensor_list[i].discrete_state = pSensorBuff->EventTypeCode;
+            sensor_list[i].sensor_state = 0;
+            sensor_list[ i ].sensor_reading = (float)(pSensorBuff->ComparisonStatus | (pSensorBuff->OptionalStatus << 8));
+        }
+        else
+        {
+            sensor_list[ i ].discrete_state = 0;
+            /*Convert Sensor Reading*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->SensorReading, &sensor_list[i].sensor_reading, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert LowerNonRecoverable*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerNonRecoverable, &sensor_list[i].low_non_recov_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert LowerCritical*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerCritical, &sensor_list[i].low_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert LowerNonCritical*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->LowerNonCritical, &sensor_list[i].low_non_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert UpperNonCritical*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperNonCritical, &sensor_list[i].high_non_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert UpperCritical*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperCritical, &sensor_list[i].high_crit_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Convert UpperNonRecoverable*/
+            ipmi_conv_reading(pSensorBuff->hdr.Type,pSensorBuff->UpperNonRecoverable, &sensor_list[i].high_non_recov_thresh, pSensorBuff->MinReading, pSensorBuff->MaxReading, pSensorBuff->Units1, 
+                                    pSensorBuff->Linearization, pSensorBuff->M_LSB, pSensorBuff->B_LSB, pSensorBuff->M_MSB_Tolerance, pSensorBuff->B_MSB_Accuracy,pSensorBuff->RExp_BExp);
+
+            /*Get the Sensor state*/
+            SensorState(pSensorBuff->SensorReading,&sensor_list[i].sensor_state,pSensorBuff->hdr.Type,pSensorBuff->Linearization,pSensorBuff->Units1,pSensorBuff->EventTypeCode,pSensorBuff->AssertionEventByte1,pSensorBuff->AssertionEventByte2,pSensorBuff->DeassertionEventByte1,pSensorBuff->DeassertionEventByte2);
+        }
+        pSensorBuff++;
+    }
+    free(pSensor);
+    return wRet ;
+}
