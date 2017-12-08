--- .pristine/libnetwork-1.58.0-src/data/protoendpoint.c Mon Sep 12 13:03:09 2011
+++ source/libnetwork-1.58.0-src/data/protoendpoint.c Mon Sep 12 16:58:25 2011
@@ -119,8 +119,8 @@
 			}
 			ptrTempNetData->next = NULL;
 			
-			strcpy(ptrTempNetData->localAddress, strLocalAddr);
-			strcpy(ptrTempNetData->remoteAddress, strRemoteAddr);
+			strncpy(ptrTempNetData->localAddress, strLocalAddr, sizeof(ptrTempNetData->localAddress));
+			strncpy(ptrTempNetData->remoteAddress, strRemoteAddr, sizeof(ptrTempNetData->remoteAddress));
 			ptrTempNetData->ulLocalPort = ulLPort;
 			ptrTempNetData->ulRemotePort = ulRPort;
 			
@@ -167,6 +167,8 @@
 	unsigned long ulLocalPort = 0, ulRemotePort = 0;
 	unsigned long ulPort = 0;
 	char strRemoteAddr[32] = {0};
+	char *pTmpRemoteAddr = NULL;
+	char *pPortNo = NULL;
 	
 	/**
 	* Procedure:
@@ -179,8 +181,21 @@
 	 *    all the properties and return the same.
 	 **/
 	
-	strcpy(strRemoteAddr, (strrchr(pstrName, '-')+1));
-	ulPort = (atol)(strtok(strchr(pstrName, ':')+1, "-"));
+	pTmpRemoteAddr = strrchr(pstrName, '-') + 1;
+	if(NULL == pTmpRemoteAddr)
+		return FAILURE;
+	
+	strncpy(strRemoteAddr, pTmpRemoteAddr, sizeof(strRemoteAddr));
+
+	pPortNo = strchr(pstrName, ':') + 1;
+	if(NULL == pPortNo)
+		return FAILURE;
+
+	pPortNo = strtok(pPortNo, "-");
+	if(NULL == pPortNo)
+		return FAILURE;
+
+	ulPort = atol(pPortNo);
 	
 	sprintf(strPipe, "%s %s | %s %s | %s ESTABLISHED | %s %s | %s %s|%s %s",
 		NETSTAT_BIN_PATH,
