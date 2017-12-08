--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Dec  8 13:31:23 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Thu Dec  8 14:01:07 2011
@@ -1886,7 +1886,31 @@
             SensorInfo.MaxReading = FullSDR->MaxReading;
             SensorInfo.MinReading = FullSDR->MinReading;
             SensorInfo.Linearization = FullSDR->Linearization;
+#if 0
+            /* original designed */
             strncpy(SensorInfo.SensorName,FullSDR->IDStr,MAX_ID_STR_LEN);
+#else
+            /* extract_sdr_id, the function's arg 2 is not matched in this case, so expand this function here */
+            
+            /* Low 5 bits of typelengthcode is length of ID */
+            if((FullSDR->IDStrTypeLen & 0x1f) != (u8)0)
+            {
+                /* High 2 bits == 11b, string is 8 bit ascii */
+                if((FullSDR->IDStrTypeLen >> 6) == (u8)3)
+                {
+                    /* Copy the ID string out */
+                    strncpy(SensorInfo.SensorName,FullSDR->IDStr,(size_t)(FullSDR->IDStrTypeLen & 0x1f));
+                }
+                else
+                {
+                    /* Copy the ID string out */
+                	sprintf(SensorInfo.SensorName, "Unknown");
+                	
+                    /* Do handling of 6 bit packed ascii IDs here */
+                    TWARN("Handling for 6 bit packed ascii IDs unimplemented\n");
+                }
+            }            
+#endif            
         }
         else if(pSensor.SDRRec->Type == COMPACT_SDR_REC)   /*Compact SDR*/
         {
@@ -1896,7 +1920,31 @@
             SensorInfo.MaxReading = 0;
             SensorInfo.MinReading = 0;
             SensorInfo.Linearization = 0;
+#if 0
+            /* original designed */
             strncpy(SensorInfo.SensorName,CompSDR->IDStr,MAX_ID_STR_LEN);
+#else
+            /* extract_sdr_id, the function's arg 2 is not matched in this case, so expand this function here */
+            
+            /* Low 5 bits of typelengthcode is length of ID */
+            if((CompSDR->IDStrTypeLen & 0x1f) != (u8)0)
+            {
+                /* High 2 bits == 11b, string is 8 bit ascii */
+                if((CompSDR->IDStrTypeLen >> 6) == (u8)3)
+                {
+                    /* Copy the ID string out */
+                    strncpy(SensorInfo.SensorName,CompSDR->IDStr,(size_t)(CompSDR->IDStrTypeLen & 0x1f));
+                }
+                else
+                {
+                    /* Copy the ID string out */
+                	sprintf(SensorInfo.SensorName, "Unknown");
+                	
+                    /* Do handling of 6 bit packed ascii IDs here */
+                    TWARN("Handling for 6 bit packed ascii IDs unimplemented\n");
+                }
+            }
+#endif            
         }
         memcpy(pValidSensor,(INT8U *)&SensorInfo,sizeof(SenInfo_T));
         pValidSensor = pValidSensor + sizeof(SenInfo_T);
