--- .pristine/libvmedia-1.13.0-src/data/comm.h Mon Oct 10 16:29:23 2011
+++ source/libvmedia-1.13.0-src/data/comm.h Mon Oct 10 16:39:00 2011
@@ -22,6 +22,8 @@
 #define INVALID_SOCKET (-1)
 #endif
 
+#define BOTH_INTERFACES "both"
+
 /* Function Prototypes of COMM Module */
 SOCKET	InitializeServer(u_short PortNo, char *InterfaceName);
 SOCKET	WaitForClientConnection(SOCKET serv_sock);
