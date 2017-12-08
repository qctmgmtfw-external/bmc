--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMI.h Fri Sep 16 23:40:54 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMI.h Sat Sep 17 00:02:04 2011
@@ -183,5 +183,8 @@
 /* Retrive SEL Entries Command */
 #define CMD_AMI_GET_SEL_ENTIRES 0x85
 
+/* Retrive Sensor Info Command*/
+#define CMD_AMI_GET_SENSOR_INFO 0x86
+
 #endif /* IPMI_AMI_H */
 
