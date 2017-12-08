--- .pristine/libnetwork-1.58.0-src/data/tcp.c Mon Sep 12 13:03:09 2011
+++ source/libnetwork-1.58.0-src/data/tcp.c Mon Sep 12 16:56:08 2011
@@ -1053,6 +1053,8 @@
 	char strInstName[256] = {0};
 	
 	char strPipe[256] = { 0 };
+	char *pTmpIP = NULL;
+	
 	int nRet = FAILURE;
 
 	/**
@@ -1071,7 +1073,13 @@
 		strcpy(pstrName, (strchr(pstrName, '-') + 1));
 		if(strstr(pstrName, ":") != NULL 
 		&& strtok(pstrName, ":") !=  NULL) 
-			strcpy(strIP, strtok(pstrName, ":"));
+		{
+			pTmpIP = strtok(pstrName, ":");
+			if(NULL == pTmpIP)
+				return FAILURE;
+	
+			strncpy(strIP, pTmpIP, sizeof(strIP));
+		}
 		if(strrchr(strInstName, ':'))
 			ulDataPort = 
 				(atol)(strrchr(strInstName, ':')+1);
@@ -1158,6 +1166,7 @@
 	char strStatus[20] = { 0 };
 	char strLocalAddress[128] = { 0 };
 	char strRemoteAddress[128] = { 0 };
+	char *pTmpIP = NULL;
 	int nRet = FAILURE;
 	
 	/**
@@ -1176,7 +1185,13 @@
 		strcpy(pstrName, (strchr(pstrName, '-') + 1));
 		if(strstr(pstrName, ":") != NULL 
 		 && strtok(pstrName, ":") !=  NULL) 
-			strcpy(strIP, strtok(pstrName, ":"));
+		{
+			pTmpIP = strtok(pstrName, ":");
+			if(NULL == pTmpIP)
+				return FAILURE;
+
+			strncpy(strIP, pTmpIP, sizeof(strIP));
+		}
 		if(strrchr(strInstName, ':'))
 			ulDataPort = 
 				(atol)(strrchr(strInstName, ':')+1);
