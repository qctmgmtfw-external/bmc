--- .pristine/ipmimain-2.61.0-src/data/HostReset.c Thu Apr 26 21:17:20 2012
+++ source/ipmimain-2.61.0-src/data/HostReset.c Thu Apr 26 21:32:28 2012
@@ -59,8 +59,9 @@
         	if( g_PDKHandle[PDK_LPCRESET] != NULL )
        	        ((void(*)(int))g_PDKHandle[PDK_LPCRESET])((int)(*BMCInst));
             
-            pBMCInfo->PefConfig.PEFTmrMgr.ResetFlag = 1;
-
+            pBMCInfo->PefConfig.PEFTmrMgr.StartDlyResetFlag = 1;
+            pBMCInfo->PefConfig.PEFTmrMgr.PEFTimerResetFlag = 1;
+            
         	if(pBMCInfo->TriggerEvent.LPCResetFlag)
         	{
         		if(0 == send_signal_by_name("Adviserd",SIGUSR2))
