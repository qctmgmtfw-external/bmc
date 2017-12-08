--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Mon Sep 19 17:22:40 2011
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Mon Sep 19 17:27:41 2011
@@ -3399,7 +3399,28 @@
     }
     else if (USER_ID == pNvrUserInfo->ID)//modifying users
     {
-    	RenameUserDir((char *)pUserInfo->UserName, (char *)pSetUserNameReq->UserName);
+        RenameUserDir((char *)pUserInfo->UserName, (char *)pSetUserNameReq->UserName);
+        AddUsrtoIPMIGrp((char *)pSetUserNameReq->UserName); 
+        for (Ch = 0; Ch < MAX_NUM_CHANNELS; Ch++) 
+        {
+            if(!IsChannelSuppGroups(Ch,BMCInst))
+            {
+                continue;
+            }
+
+            pChannelInfo = getChannelInfo (Ch, BMCInst); 
+            pChUserInfo = getChUserIdInfo(pSetUserNameReq->UserID, &Index, pChannelInfo->ChannelUserInfo,BMCInst); 
+            if (pChUserInfo != NULL) 
+            {
+                AddUsr_To_Grp((char *)pUserInfo->UserName,pChannelInfo->ChannelNumber,pChUserInfo->AccessLimit,0); 
+                AddUsr_To_Grp((char *)pSetUserNameReq->UserName,pChannelInfo->ChannelNumber,0,pChUserInfo->AccessLimit); 
+            } 
+            else 
+            {
+                AddUsr_To_Grp((char *)pUserInfo->UserName,Ch,PRIV_LEVEL_NO_ACCESS,0); 
+                AddUsr_To_Grp((char *)pSetUserNameReq->UserName,Ch,0,PRIV_LEVEL_NO_ACCESS); 
+            }
+        } 
     }    
 		_fmemcpy (pUserInfo->UserName, pSetUserNameReq->UserName, MAX_USERNAME_LEN);
 		_fmemcpy (pNvrUserInfo->UserName, pSetUserNameReq->UserName, MAX_USERNAME_LEN);
