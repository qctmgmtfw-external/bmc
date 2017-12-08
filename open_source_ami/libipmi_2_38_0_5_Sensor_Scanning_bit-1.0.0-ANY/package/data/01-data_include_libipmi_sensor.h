--- .pristine/libipmi-2.38.0-src/data/include/libipmi_sensor.h Tue Sep 20 22:05:02 2011
+++ source/libipmi-2.38.0-src/data/include/libipmi_sensor.h Tue Sep 20 22:21:34 2011
@@ -147,6 +147,10 @@
 #define THRESH_UP_NON_RECOV         ( (u16)0x40 )
 #define THRESH_LOW_NON_RECOV        ( (u16)0x80 )
 
+#define SENSOR_NOT_AVAILABLE        0xD5
+#define UNABLE_TO_READ_SENSOR       0x20
+#define SENSOR_SCANNING_BIT             0x40
+
 #pragma pack()
 
 
