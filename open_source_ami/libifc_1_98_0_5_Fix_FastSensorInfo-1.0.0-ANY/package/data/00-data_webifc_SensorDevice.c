--- .pristine/libifc-1.98.0-src/data/webifc_SensorDevice.c Fri Sep 16 23:49:44 2011
+++ source/libifc-1.98.0-src/data/webifc_SensorDevice.c Fri Sep 16 23:59:17 2011
@@ -48,6 +48,7 @@
 	struct sensor_data *pSensorBuff = NULL;
 	long int reading,lownr,lowct,lownc,highnc,highct,highnr = 0;
 	unsigned char *pStartBuff = NULL;
+        uint32 nNumSensor = 0;
 
 	TDBG("in get all sensors\n");
 
@@ -99,7 +100,7 @@
 		memset(pSensorBuff, 0, nSensors * sizeof(struct sensor_data));
 		pStartBuff = (unsigned char *) pSensorBuff;
 
-		wRet = LIBIPMI_HL_GetAllSensorReadings(&((wc_get_ipmi_session(wp))->IPMISession),pSensorBuff,DEFAULT_IPMITIMEOUT);
+		wRet = LIBIPMI_HL_GetAllSensorReadings(&((wc_get_ipmi_session(wp))->IPMISession),pSensorBuff, &nNumSensor,DEFAULT_IPMITIMEOUT);
 		if(wRet != 0)
 		{
 			TCRIT("Error reading All Sensors\n");
@@ -108,7 +109,7 @@
 		else
 		{
 			/* Write entries here */
-			while(nSensors)
+			while(nNumSensor)
 			{
 				reading = pSensorBuff->sensor_reading * 1000;
 				lownr = pSensorBuff->low_non_recov_thresh * 1000;
@@ -140,7 +141,7 @@
 							pSensorBuff->SensorAccessibleFlags);
 
 				pSensorBuff++;
-				nSensors--;
+				nNumSensor--;
 			}
 		}
 		free(pStartBuff);
