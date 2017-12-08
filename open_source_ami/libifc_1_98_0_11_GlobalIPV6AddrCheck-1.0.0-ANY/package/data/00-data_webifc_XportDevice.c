--- .pristine/libifc-1.98.0-src/data/webifc_XportDevice.c Wed May  9 17:00:40 2012
+++ source/libifc-1.98.0-src/data/webifc_XportDevice.c Thu May 10 14:48:19 2012
@@ -34,6 +34,7 @@
 #include "libncsiconf.h"
 #include "ncml.h"
 #include "lanchcfg.h"
+#include "nwcfg.h"
 
 typedef struct {
 	INT8U IPAddrSource;
@@ -467,7 +468,13 @@
 					retval = INVALID_V6IP;
 					goto error_out;
 				}
-				TDBG ("Success converting v6 IP Address::%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
+				if(0 != IsValidGlobalIPv6Addr((struct in6_addr*)&iV6IPAddress))
+				{
+					TCRIT("Invalid Global IPv6 Address\n");
+					retval = INVALID_V6IP;
+					goto error_out;
+				}
+				printf("Success converting v6 IP Address::%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
 							iV6IPAddress[0],iV6IPAddress[1],iV6IPAddress[2],iV6IPAddress[3],
 							iV6IPAddress[4],iV6IPAddress[5],iV6IPAddress[6],iV6IPAddress[7],
 							iV6IPAddress[8],iV6IPAddress[9],iV6IPAddress[10],iV6IPAddress[11],
