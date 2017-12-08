--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Wed Jan 11 15:02:55 2012
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Wed Jan 11 15:09:48 2012
@@ -1384,8 +1384,8 @@
 
     if (0 == (pSensorEvtEnReq->Flags & SCAN_MASK))
     {
-        /* DisableScanning () */
-        pSensorInfo->EventFlags |= BIT6;
+        /* Disable the sensor  */
+        pSensorInfo->EventFlags |= BIT5;  ///* Bit 5 -  Unable to read           */
     }
 
     pSensorInfo->EventFlags &= ~(EVENT_MSG_MASK | SCAN_MASK);
