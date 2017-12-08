--- .pristine/goahead-1.62.0-src/data/sockGen.c Thu Jan 19 14:28:12 2012
+++ source/goahead-1.62.0-src/data/sockGen.c Thu Jan 19 15:12:09 2012
@@ -22,6 +22,7 @@
 #include	<fcntl.h>
 #include	<string.h>
 #include	<stdlib.h>
+#include	<ncml.h>
 
 #ifdef UEMF
 	#include	"uemf.h"
@@ -42,6 +43,7 @@
 extern int			socketMax;				/* Maximum size of socket */
 extern int			socketHighestFd;		/* Highest socket fd opened */
 static int			socketOpenCount = 0;	/* Number of task using sockets */
+extern SERVICE_CONF_STRUCT g_serviceconf;
 
 /***************************** Forward Declarations ***************************/
 
@@ -233,6 +235,11 @@
 /*
  *		Bind to the socket endpoint and the call listen() to start listening
  */
+    	if (strcmp(g_serviceconf.InterfaceName, "both") != 0)
+    	{
+    		setsockopt(sp->sock, SOL_SOCKET, SO_BINDTODEVICE, g_serviceconf.InterfaceName, sizeof(g_serviceconf.InterfaceName) + 1);
+    	}
+
 		rc = 1;
 		setsockopt(sp->sock, SOL_SOCKET, SO_REUSEADDR, (char *)&rc, sizeof(rc));
 		if (bind(sp->sock, r->ai_addr,r->ai_addrlen) < 0)
@@ -451,6 +458,12 @@
 			}
 		}
 	} else {
+
+		if (strcmp(g_serviceconf.InterfaceName, "both") != 0)
+		{
+			setsockopt(sp->sock, SOL_SOCKET, SO_BINDTODEVICE, g_serviceconf.InterfaceName, sizeof(g_serviceconf.InterfaceName) + 1);
+		}
+
 /*
  *		Bind to the socket endpoint and the call listen() to start listening
  */
