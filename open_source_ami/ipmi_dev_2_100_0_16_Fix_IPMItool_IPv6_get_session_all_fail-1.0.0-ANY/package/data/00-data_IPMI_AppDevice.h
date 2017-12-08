--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AppDevice.h	Fri May 31 16:01:47 2013
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AppDevice.h	Fri May 31 16:22:24 2013
@@ -320,7 +320,7 @@
 /* LANIPv6SesInfoRes_T */
 typedef struct
 {
-    INT8U    IPv6Address[16];
+    INT8U    IPv6Address[4];
     INT8U    MACAddress[6];
     INT16U   PortNumber;
 
