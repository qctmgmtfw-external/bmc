--- .pristine/libipmi-2.38.0-src/data/libipmi_sensor.c	Mon Oct  1 12:55:32 2012
+++ source/libipmi-2.38.0-src/data/libipmi_sensor.c	Tue Oct  2 09:20:37 2012
@@ -864,6 +864,7 @@
     for(i = 0; i < *nNumSensor; i++)
     {
         sensor_list[ i ].sensor_num = pSensorBuff->SensorNumber;
+        sensor_list[ i ].sensor_event_type = pSensorBuff->EventTypeCode;
         sensor_list[ i ].owner_id = pSensorBuff->OwnerID;
         sensor_list[ i ].owner_lun = pSensorBuff->OwnerLUN;
         sensor_list[ i ].sensor_type = pSensorBuff->SensorTypeCode;
