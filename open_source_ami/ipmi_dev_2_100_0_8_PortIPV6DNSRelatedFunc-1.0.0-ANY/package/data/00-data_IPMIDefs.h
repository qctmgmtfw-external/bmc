--- .pristine/ipmi_dev-2.100.0-src/data/IPMIDefs.h Fri Feb  3 16:17:30 2012
+++ source/ipmi_dev-2.100.0-src/data/IPMIDefs.h Wed Jan 18 20:18:50 2012
@@ -199,6 +199,7 @@
 #define OEMCC_SEL_CLEARED                                           0x85
 #define OEMCC_SENSOR_INFO_EMPTY                                 0x87
 
+#define OEMCC_ATTEMPT_TO_GET_WO_PARAM              0x82
 
 #define	CC_POWER_LIMIT_OUT_OF_RANGE						0x84
 #define	CC_CORRECTION_TIME_OUT_OF_RANGE				    0x85
