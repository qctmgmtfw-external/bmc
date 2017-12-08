--- .pristine/ipmi_dev-2.100.0-src/data/Support.h Fri Sep 16 23:40:54 2011
+++ source/ipmi_dev-2.100.0-src/data/Support.h Sat Sep 17 00:04:02 2011
@@ -421,6 +421,8 @@
 #define AMI_SET_SEL_POLICY          AMISetSELPolicy         /* UNIMPLEMENTED */
 #define AMI_GET_SEL_ENTRIES           AMIGetSELEntires       /* UNIMPLEMENTED */
 
+/*------------------------------AMI SensorInfo Commands -----------------------*/
+#define AMI_GET_SENSOR_INFO         AMIGetSenforInfo        /*UNIMPLEMENTED*/
 /*--------------------------APML Specific Commands ------------------------------------*/
 //SB-RMI
 #define APML_GET_INTERFACE_VERSION	ApmlGetInterfaceVersion   /*UNIMPLEMENTED*/ //APML
