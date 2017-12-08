--- .pristine/libifc-1.98.0-src/data/webifc_misc.c Tue Sep 20 14:15:21 2011
+++ source/libifc-1.98.0-src/data/webifc_misc.c Tue Sep 20 17:44:33 2011
@@ -239,9 +239,12 @@
 		fprintf(stdout,"Unable to open private key file.. retry.");
 		sslValid = UNKNOWN_ERROR;
 	}
-	if (verifySSL_CRT() != RPC_HAPI_SUCCESS)
-	{
-		sslValid = SSL_CERT_TRUSTED;
+	sslValid = verifySSL_CRT();
+	
+	if (sslValid != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in validating the SSL Certificate::%d", sslValid);
+		//sslValid = SSL_CERT_TRUSTED;
 	}
 
 remove_temp:
