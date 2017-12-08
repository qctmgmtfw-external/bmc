--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Fri Sep  9 16:07:09 2011
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Fri Sep  9 20:45:00 2011
@@ -3032,9 +3032,6 @@
         pChUserInfo->UserId = pSetUserAccessReq->UserID;
         pNVRChUserInfo[Index].UserId = pSetUserAccessReq->UserID;
 
-        /* IPMIMessaging is independent of  User enable / disable in Particular  channel */
-        pChannelInfo->NoCurrentUser++;  /* update in Volitile       */
-        pNvrChInfo->NoCurrentUser++;    /* update in NV Memory      */
         pChUserInfo->AccessLimit = PRIV_LEVEL_NO_ACCESS;
         /* Initial depends on the  Request Bit */
          pChUserInfo->IPMIMessaging=FALSE;
@@ -3327,8 +3324,11 @@
                     pChUserInfo->UserId = 0;
                     pChUserInfo->IPMIMessaging=FALSE;
                     pChUserInfo->ActivatingSOL=FALSE;
-                    pChannelInfo->NoCurrentUser--;
-                    pNvrChInfo->NoCurrentUser--;
+                    if( pNvrUserInfo->UserStatus == TRUE)
+                    {
+                        pChannelInfo->NoCurrentUser--;
+                        pNvrChInfo->NoCurrentUser--;
+                    }
                     FlushChConfigs((INT8U*)pNvrChInfo,pNvrChInfo->ChannelNumber,BMCInst);
                 }
                 else
@@ -3366,7 +3366,7 @@
         pNvrUserInfo->ID            = USER_ID;
         pNvrUserInfo->UserId        = pSetUserNameReq->UserID;
         /* allow for IPMI Mesaging  */
-        pNvrUserInfo->UserStatus    = TRUE;
+        pNvrUserInfo->UserStatus    = FALSE;
         /* set default max session  */
         pNvrUserInfo->MaxSession    = pBMCInfo->IpmiConfig.MaxSession;
 
