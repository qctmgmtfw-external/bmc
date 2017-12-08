--- .pristine/cdserverapp-1.16.0-src/data/cdserver.c Mon Sep  5 17:06:31 2011
+++ source/cdserverapp-1.16.0-src/data/cdserver.c Wed Sep  7 17:47:51 2011
@@ -445,10 +445,11 @@
 	gMachineState [Instance] = STATE_NOEVENT; 
 	gMachineInited [Instance] = 1;
 	int activesession = 0;
+	int maxsession = 0;
 	
 	if (get_service_configurations(CDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 	{
-		printf("Unable to get  the  FDMedia Serviec Conf\n");
+		printf("Unable to get  the  CDMedia Serviec Conf\n");
 	}
 	/*Checking for KVM CurrentActiveSession and setting the client count*/
 	if (isNotApplicable((unsigned char *)&(MediaConf.CurrentActiveSession),sizeof(MediaConf.CurrentActiveSession)) ) 
@@ -464,18 +465,24 @@
 			activesession = activesession+1;
 			getNotEditableMaskData((unsigned char *)&(activesession),sizeof(MediaConf.CurrentActiveSession), (unsigned char *)&(MediaConf.CurrentActiveSession));
 			
-			TDBG("FD Server Session, setting CurrentActiveSession value After Mask::%d\n", MediaConf.CurrentActiveSession);
+			TDBG("CD Server Session, setting CurrentActiveSession value After Mask::%d\n", MediaConf.CurrentActiveSession);
 		}
 		else
 		{
-			
 			MediaConf.CurrentActiveSession =activesession++;
 		}
-		if((activesession) <=  MediaConf.MaxAllowSession)
+		
+		// get cd-media MaxAllowSession count from MediaConf and assign the value to maxsession.
+		if(isNotEditable((unsigned char *)&(MediaConf.MaxAllowSession),sizeof(MediaConf.MaxAllowSession)) )
+		{
+			getNotEditableData((unsigned char *)&MediaConf.MaxAllowSession/*IN*/,sizeof(MediaConf.CurrentActiveSession)/*IN*/, (unsigned char *)&maxsession/*OUT*/);
+		}
+		
+		if(activesession <= maxsession)
 		{
 			if (set_service_configurations(CDMEDIA_SERVICE_NAME, &MediaConf) != 0)
 			{
-				printf("Unable to set the Serviec Conf for FD Server\n");
+				printf("Unable to set the Service Conf for CD Server\n");
 			}
 		}
 	}
