--- .pristine/libipmi-2.38.0-src/data/libipmi_sensor.c Tue Sep 20 22:05:02 2011
+++ source/libipmi-2.38.0-src/data/libipmi_sensor.c Tue Sep 20 22:21:49 2011
@@ -884,6 +884,16 @@
             /*Get the Sensor state*/
             SensorState(pSensorBuff->SensorReading,&sensor_list[i].sensor_state,pSensorBuff->hdr.Type,pSensorBuff->Linearization,pSensorBuff->Units1,pSensorBuff->EventTypeCode,pSensorBuff->AssertionEventByte1,pSensorBuff->AssertionEventByte2,pSensorBuff->DeassertionEventByte1,pSensorBuff->DeassertionEventByte2);
         }
+
+        /* If sensor scanning is disabled or the reading is unavailable... */
+        if( !( pSensorBuff->Flags & SENSOR_SCANNING_BIT)  || (pSensorBuff->Flags & UNABLE_TO_READ_SENSOR) )
+        {
+            /* Return an error code to let the caller know */
+            sensor_list[i].SensorAccessibleFlags = SENSOR_NOT_AVAILABLE;
+            sensor_list[i].sensor_reading = 0;
+            sensor_list[i].discrete_state = 0;
+            sensor_list[i].raw_reading = 0;
+        }
         pSensorBuff++;
     }
     free(pSensor);
