--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c Tue Sep 20 18:32:02 2011
+++ source/hdserverapp-1.17.0-src/data/hdserver.c Wed Sep 21 11:43:39 2011
@@ -1354,7 +1354,7 @@
 	}
 
    	/* Create a server socket to listen*/
-	gMasterSock = InitializeServer(portnum);
+	gMasterSock = InitializeServer(portnum, MediaConf.InterfaceName);
 	if (gMasterSock==INVALID_SOCKET)
    	{
       	TCRIT(" Server Initialization failure %d\n",errno);
