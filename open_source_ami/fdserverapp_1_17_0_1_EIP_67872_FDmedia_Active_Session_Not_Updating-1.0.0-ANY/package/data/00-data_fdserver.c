--- .pristine/fdserverapp-1.17.0-src/data/fdserver.c Mon Sep  5 17:06:32 2011
+++ source/fdserverapp-1.17.0-src/data/fdserver.c Wed Sep  7 17:57:38 2011
@@ -437,8 +437,9 @@
 	gMachineState[Instance] = STATE_NOEVENT;
 	gMachineInited[Instance] = 1;
 	int activesession = 0;
+	int maxsession = 0;
 	
-	if (get_service_configurations(CDMEDIA_SERVICE_NAME, &MediaConf) != 0)
+	if (get_service_configurations(FDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 	{
 		printf("Unable to get  the  FDMedia Serviec Conf\n");
 	}
@@ -460,12 +461,18 @@
 		}
 		else
 		{
-			
 			MediaConf.CurrentActiveSession =activesession++;
 		}
-		if(activesession <= MediaConf.MaxAllowSession)
-		{
-			if (set_service_configurations(CDMEDIA_SERVICE_NAME, &MediaConf) != 0)
+
+		// get fd-media MaxAllowsession count from MediaConf and assgn the value to maxsession.
+		if(isNotEditable((unsigned char *)&(MediaConf.MaxAllowSession),sizeof(MediaConf.MaxAllowSession)) )
+		{
+			getNotEditableData((unsigned char *)&MediaConf.MaxAllowSession/*IN*/,sizeof(MediaConf.CurrentActiveSession)/*IN*/, (unsigned char *)&maxsession/*OUT*/);
+		}
+		
+		if(activesession <= maxsession)
+		{
+			if (set_service_configurations(FDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 			{
 				printf("Unable to set the Serviec Conf for FD Server\n");
 			}
