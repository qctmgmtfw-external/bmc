--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Thu Oct 23 00:44:42 2014
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c Thu Oct 23 03:43:26 2014
@@ -500,6 +500,7 @@
 
     g_BMCInfo[BMCInst].SetWDTUpdated = TRUE;
     g_BMCInfo[BMCInst].Msghndlr.TmrSet = TRUE;
+	pBMCInfo->WDTPreTmtStat = FALSE;
 
     *pRes = CC_NORMAL;
 
