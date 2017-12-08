--- .pristine/ipmimain-2.61.0-src/data/SharedMem.c Fri Dec 30 16:24:12 2011
+++ source/ipmimain-2.61.0-src/data/SharedMem.c Fri Dec 30 16:50:57 2011
@@ -162,7 +162,7 @@
     }
 
     /*Initialize the Session Table memory */
-    memset(pBMCInfo->SessionTblInfo.SessionTbl,0,sizeof(SessionTblInfo_T)*(  pBMCInfo->IpmiConfig.MaxSession + 1));
+    memset(pBMCInfo->SessionTblInfo.SessionTbl,0,sizeof(SessionInfo_T)*(  pBMCInfo->IpmiConfig.MaxSession + 1));
 
     pSensorSharedMem->GlobalSensorScanningEnable = TRUE;
 
