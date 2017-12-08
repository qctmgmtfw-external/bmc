--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c Tue Sep 20 18:32:01 2011
+++ source/fdserverapp-1.17.0-src/data/fdserver.c Wed Sep 21 11:40:50 2011
@@ -1307,7 +1307,7 @@
 	}
 
    	/* Create a server socket to listen*/
-	gMasterSock = InitializeServer(portnum);
+	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
       	TCRIT("Server Initialization failure %d\n",errno);
