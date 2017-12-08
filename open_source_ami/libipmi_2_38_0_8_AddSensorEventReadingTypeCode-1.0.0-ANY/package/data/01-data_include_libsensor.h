--- .pristine/libipmi-2.38.0-src/data/include/libsensor.h	Mon Oct  1 12:55:31 2012
+++ source/libipmi-2.38.0-src/data/include/libsensor.h	Tue Oct  2 09:19:44 2012
@@ -32,6 +32,7 @@
 	*/
     UINT8 sensor_num;
     char sensor_name[ 20 ];
+    UINT8 sensor_event_type;
     UINT8 owner_id;
     UINT8 owner_lun;
     UINT8 sensor_type;
