--- .pristine/libifc-1.98.0-src/data/webifc_SensorDevice.c	Mon Oct  1 12:56:06 2012
+++ source/libifc-1.98.0-src/data/webifc_SensorDevice.c	Tue Oct  2 09:22:55 2012
@@ -56,6 +56,7 @@
 	{
 		JSONFIELD(SensorNumber,JSONFIELD_TYPE_INT8U),
 		JSONFIELD(SensorName,JSONFIELD_TYPE_STR),
+		JSONFIELD(SensorEventType,JSONFIELD_TYPE_INT8U),
 		JSONFIELD(OwnerID,JSONFIELD_TYPE_INT8U),
 		JSONFIELD(OwnerLUN,JSONFIELD_TYPE_INT8U),
 		JSONFIELD(SensorType,JSONFIELD_TYPE_INT8U),
@@ -122,6 +123,7 @@
 				WEBPAGE_WRITE_JSON_RECORD(HL_GETALLSENSORS,
 							pSensorBuff->sensor_num,
 							pSensorBuff->sensor_name,
+							pSensorBuff->sensor_event_type,
 							pSensorBuff->owner_id,
 							pSensorBuff->owner_lun,
 							pSensorBuff->sensor_type,
