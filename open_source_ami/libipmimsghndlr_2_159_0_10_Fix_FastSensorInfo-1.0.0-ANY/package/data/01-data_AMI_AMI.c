--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Fri Sep 16 23:42:32 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Fri Sep 16 23:59:47 2011
@@ -135,6 +135,9 @@
   { CMD_AMI_SET_PRESERVE_CONF,	   PRIV_ADMIN,	AMI_SET_PRESERVE_CONF,	sizeof(SetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
   { CMD_AMI_GET_PRESERVE_CONF,		PRIV_USER,  AMI_GET_PRESERVE_CONF,	 sizeof(GetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
 
+    /*---------------------- AMI Sensor Info Commands ------------------------------------------------------*/
+    { CMD_AMI_GET_SENSOR_INFO,   PRIV_USER,     AMI_GET_SENSOR_INFO, 0x00 , 0xAAAA, 0xFFFF},
+
    { 0 , 0 , 0 , 0, 0 ,0}, // End of Table
 
 
