--- .pristine/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Fri Sep 16 23:42:32 2011
+++ source/libipmimsghndlr-2.159.0-src/data/SensorEvent/SensorDevice/Sensor.c Sat Sep 17 00:00:34 2011
@@ -1826,7 +1826,7 @@
  * Returns 0 if val1 == val2
  * Returns 1 if val1 > val2
  */
-static int
+ int
 CompareValues(BOOL isSigned, INT8U val1, INT8U val2)
 {
     int retval = 0; // default to equal
