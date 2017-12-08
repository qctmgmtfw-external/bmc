--- .pristine/libvmedia-1.13.0-src/data/comm.c Tue Sep 20 18:29:25 2011
+++ source/libvmedia-1.13.0-src/data/comm.c Wed Sep 21 11:55:13 2011
@@ -25,11 +25,13 @@
 #include "comm.h"
 #include "dbgout.h"
 
+#define BOTH_INTERFACES        "both"
+
 typedef struct sockaddr *LPSOCKADDR;
 #define SOCKET_ERROR (-1)
 
 SOCKET
-InitializeServer(u_short PortNo)
+InitializeServer(u_short PortNo, char *InterfaceName)
 {
 	SOCKET sock;			/* Server Socket */
 	int reuseaddr=1;
@@ -91,6 +93,12 @@
             continue;
         }
 
+		if (strcasecmp(InterfaceName, BOTH_INTERFACES) != 0) 
+		{ 
+			if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, InterfaceName, strlen(InterfaceName)) == -1) 
+				TWARN("Setsockopt(SO_BINDTODEVICE) Failed for server socket\n"); 
+		} 
+
         if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(int)) == -1)
         TWARN("Setsockopt(SO_REUSEADDR) Failed for server socket\n");
 
@@ -134,6 +142,12 @@
    	myname.sin_port 	    = htons(PortNo);		/* Listening Port   */
    	myname.sin_addr.s_addr  = INADDR_ANY;			/* Any Interface	*/
 
+	if (strcasecmp(InterfaceName, BOTH_INTERFACES) != 0) 
+	{ 
+		if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, InterfaceName, strlen(InterfaceName)) == -1) 
+			TWARN("Setsockopt(SO_BINDTODEVICE) Failed for server socket\n"); 
+	} 
+
 	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(int)) == -1)
 		TWARN("Setsockopt(SO_REUSEADDR) Failed for server socket\n");
 
