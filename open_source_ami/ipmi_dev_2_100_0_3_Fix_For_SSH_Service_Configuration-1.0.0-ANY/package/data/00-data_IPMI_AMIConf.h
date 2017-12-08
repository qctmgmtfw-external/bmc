--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Mon Sep 19 17:00:43 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMIConf.h Mon Sep 19 17:03:50 2011
@@ -73,7 +73,7 @@
     INT32U ServiceID;
     INT8U	 Enable;
     /* The following fields are meaningful only when the service is enabled */
-    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE]; /* Interface name */
+    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE+1]; /* Interface name */
     INT32U  NonSecureAccessPort;               /* Non-secure access port number */
     INT32U  SecureAccessPort;                  /* Secure access port number */                        
     INTU    SessionInactivityTimeout;      	   /* Service session inactivity yimeout in seconds*/
@@ -102,7 +102,7 @@
 
 typedef struct 
 {
-    INT8S   ServiceName [SERVICE_NAME_SIZE]; /* Service name */
+    INT8S   ServiceName [SERVICE_NAME_SIZE+1]; /* Service name */
     AMIServiceConfig_T GetAllSeviceCfg;
 }GetAllServiceConf_T;
 
