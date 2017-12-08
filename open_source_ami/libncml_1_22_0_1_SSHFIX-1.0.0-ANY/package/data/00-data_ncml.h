--- .pristine/libncml-1.22.0-src/data/ncml.h Thu Sep  8 17:32:58 2011
+++ source/libncml-1.22.0-src/data/ncml.h Thu Sep  8 17:44:06 2011
@@ -99,10 +99,10 @@
 
 typedef struct
 {
-    INT8S   ServiceName[MAX_SERVICE_NAME_SIZE]; /* Service name */
+    INT8S   ServiceName[MAX_SERVICE_NAME_SIZE +1]; /* Service name */
     INT8U   CurrentState;                       /* 1, to enable the service; 0, to disable the service */
     /* The following fields are meaningful only when the service is enabled */
-    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE]; /* Interface name */
+    INT8S   InterfaceName[MAX_SERVICE_IFACE_NAME_SIZE +1]; /* Interface name */
     INT32U  NonSecureAccessPort;                        /* Non-secure access port number */
     INT32U  SecureAccessPort;                           /* Secure access port number */                        
     INTU    SessionInactivityTimeout;                   /* Service session inactivity yimeout in seconds*/
