--- .pristine/ipmi_dev-2.100.0-src/data/Sensor.h Fri Sep 16 23:40:54 2011
+++ source/ipmi_dev-2.100.0-src/data/Sensor.h Sat Sep 17 00:03:47 2011
@@ -145,7 +145,7 @@
 extern int GetSensorEventEnable (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
 extern int GetSensorReading (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
 extern int SetSensorReading (_NEAR_ INT8U* pReq, INT8U ReqLen, _NEAR_ INT8U* pRes,_NEAR_ int BMCInst);
-
+int CompareValues(BOOL isSigned, INT8U val1, INT8U val2);
 /** @} */
 
 /**
