--- .pristine/hdserverapp-1.17.0-src/data/hdserver.c Mon Sep  5 17:06:34 2011
+++ source/hdserverapp-1.17.0-src/data/hdserver.c Wed Sep  7 18:02:37 2011
@@ -464,6 +464,7 @@
 	gMachineInited[Instance] = 1;
 
 	int activesession = 0;
+	int maxsession = 0;
 	
 	if (get_service_configurations(HDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 	{
@@ -483,18 +484,25 @@
 			activesession = activesession+1;
 			getNotEditableMaskData((unsigned char *)&(activesession),sizeof(MediaConf.CurrentActiveSession), (unsigned char *)&(MediaConf.CurrentActiveSession));
 			
-			TDBG("FD Server Session, setting CurrentActiveSession value After Mask::%d\n", MediaConf.CurrentActiveSession);
+			TDBG("HD Server Session, setting CurrentActiveSession value After Mask::%d\n", MediaConf.CurrentActiveSession);
 		}
 		else
 		{
 			
 			MediaConf.CurrentActiveSession =activesession++;
 		}
-		if(activesession <= MediaConf.MaxAllowSession)
+
+		// get hd-media MaxAllowsession count from MediaConf and assgn the value to maxsession.
+		if(isNotEditable((unsigned char *)&(MediaConf.MaxAllowSession),sizeof(MediaConf.MaxAllowSession)) )
+		{
+			getNotEditableData((unsigned char *)&MediaConf.MaxAllowSession/*IN*/,sizeof(MediaConf.CurrentActiveSession)/*IN*/, (unsigned char *)&maxsession/*OUT*/);
+		}
+		
+		if(activesession <= maxsession)
 		{
 			if (set_service_configurations(HDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 			{
-				printf("Unable to set the Serviec Conf for FD Server\n");
+				printf("Unable to set the Serviec Conf for HD Server\n");
 			}
 		}
 	}
