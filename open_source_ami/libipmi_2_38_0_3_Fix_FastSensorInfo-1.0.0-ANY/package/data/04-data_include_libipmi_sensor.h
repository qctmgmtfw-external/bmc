--- .pristine/libipmi-2.38.0-src/data/include/libipmi_sensor.h Fri Sep 16 23:41:46 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_sensor.h Fri Sep 16 23:56:24 2011
@@ -164,7 +164,7 @@
 
 LIBIPMI_API uint16
 LIBIPMI_HL_GetAllSensorReadings( IPMI20_SESSION_T *pSession,
-                                 struct sensor_data *sensor_list, int timeout );
+                                 struct sensor_data *sensor_list, uint32* nNumSensor,int timeout );
 
 #ifdef  __cplusplus
 }
