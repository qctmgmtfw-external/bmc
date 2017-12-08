--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c Tue Sep 20 18:31:59 2011
+++ source/cdserverapp-1.16.0-src/data/cdserver.c Wed Sep 21 11:39:38 2011
@@ -1327,7 +1327,7 @@
 	}
 
    	/* Create a server socket to listen*/
-	gMasterSock = InitializeServer(portnum);
+	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
       	TCRIT("Server Initialization failure %d\n",errno);
