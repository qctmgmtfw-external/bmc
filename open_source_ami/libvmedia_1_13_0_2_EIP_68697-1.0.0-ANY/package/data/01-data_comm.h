--- .pristine/libvmedia-1.13.0-src/data/comm.h Tue Sep 20 18:29:25 2011
+++ source/libvmedia-1.13.0-src/data/comm.h Wed Sep 21 11:46:01 2011
@@ -23,7 +23,7 @@
 #endif
 
 /* Function Prototypes of COMM Module */
-SOCKET	InitializeServer(u_short PortNo);
+SOCKET	InitializeServer(u_short PortNo, char *InterfaceName);
 SOCKET	WaitForClientConnection(SOCKET serv_sock);
 void	ShutDownServer(SOCKET sock);
 
