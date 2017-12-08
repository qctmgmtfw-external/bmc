--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/WDT.c Thu Oct 23 00:44:42 2014
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/WDT.c Thu Oct 23 19:27:14 2014
@@ -89,7 +89,7 @@
     int *inst = (int*) pArg;
     int BMCInst = *inst;
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
-    int PreTimeAction = FALSE;
+
     INT32U WDTInterval = 0,WDTPreInterval = 0,jiffycmp = 0;
     long long jiffyvalue=0,jiffystart = 0;
     unsigned long Hertz = sysconf(_SC_CLK_TCK);
@@ -170,7 +170,7 @@
 
             if((jiffycmp > WDTInterval) ||(jiffycmp > (WDTInterval-WDTPreInterval)))
             {
-                if((jiffycmp > (WDTInterval-WDTPreInterval)) && (PreTimeAction == FALSE)
+                if((jiffycmp > (WDTInterval-WDTPreInterval)) && (pBMCInfo->WDTPreTmtStat == FALSE)
                     && ((pBMCInfo->WDTConfig.TmrActions & 0x70) != 0))
                 {
                     /* take pretimeout actions */
@@ -187,12 +187,12 @@
                     }
                     WDTTmrAction (g_WDTTmrMgr.WDTTmr.TmrActions & 0x70, BMCInst);
                     pBMCInfo->WDTPreTmtStat = TRUE;
-                    PreTimeAction = TRUE;
+
                 }
                 else if(jiffycmp > WDTInterval)
                 {
                     g_WDTTmrMgr.TmrPresent = FALSE;
-                    PreTimeAction = FALSE;
+                    
                     OS_THREAD_MUTEX_ACQUIRE(&pBMCInfo->hWDTSharedMemMutex, WAIT_INFINITE)
                     BMC_GET_SHARED_MEM(BMCInst)->IsWDTRunning=FALSE;
                     BMC_GET_SHARED_MEM(BMCInst)->IsWDTPresent =FALSE;
