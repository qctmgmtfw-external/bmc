--- .pristine/ipmimain-2.61.0-src/data/IPMI_Main.c Mon Sep 12 15:39:23 2011
+++ source/ipmimain-2.61.0-src/data/IPMI_Main.c Mon Sep 12 15:41:21 2011
@@ -266,7 +266,7 @@
 **/
 void AddUserToChGroups(int BMCInst)
 {
-    INT8U ch;
+    INT8U ch,j;
     int i;
     unsigned int accesslimit;
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
@@ -290,9 +290,9 @@
             {
                 AddUsrtoIPMIGrp((char*)pUserTable[i].UserName);
             }
-            if((0 != pUserTable[i].UserId) && (pChannelInfo->ChannelUserInfo[i].UserId == pUserTable[i].UserId))
+            if(NULL != getChUserIdInfo (pUserTable[i].UserId , &j, pChannelInfo->ChannelUserInfo, BMCInst))
             {
-                accesslimit=(unsigned int)pChannelInfo->ChannelUserInfo[i].AccessLimit;
+                accesslimit=(unsigned int)pChannelInfo->ChannelUserInfo[j].AccessLimit;
                 AddUsr_To_Grp((char*)pUserTable[i].UserName,ch,0,accesslimit);
             }
         }
