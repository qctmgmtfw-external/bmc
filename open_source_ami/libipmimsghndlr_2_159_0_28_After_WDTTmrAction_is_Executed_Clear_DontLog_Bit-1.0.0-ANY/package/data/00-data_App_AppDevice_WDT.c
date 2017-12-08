--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/WDT.c Mon Dec  3 14:05:03 2012
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/WDT.c Mon Dec  3 14:24:12 2012
@@ -361,6 +361,8 @@
         ((void(*)(INT8U,WDTTmrMgr_T*,int))g_PDKHandle[PDK_WATCHDOGACTION]) (TmrAction,&g_WDTTmrMgr,BMCInst);
     }
 
+    /* Clearing don't log bit after Watchdog timeout */ 
+    g_WDTTmrMgr.WDTTmr.TmrUse &= 0x7F;
 }
 
 
