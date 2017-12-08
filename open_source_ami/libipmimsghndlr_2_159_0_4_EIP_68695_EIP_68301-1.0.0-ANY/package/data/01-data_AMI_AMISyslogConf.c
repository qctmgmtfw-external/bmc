--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMISyslogConf.c Wed Sep  7 20:23:25 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMISyslogConf.c Thu Sep  8 22:31:24 2011
@@ -110,11 +110,13 @@
     if(pGetSyslogConf->SysStatus == ENABLE_LOCAL)
     {
         GetLogRotate(&(pGetSyslogConf->Config.Local.Rotate), &(pGetSyslogConf->Config.Local.Size));
+        pGetSyslogConf->CompletionCode = CC_NORMAL;
+        return sizeof(AMIGetLogConfRes_T) - (sizeof(pGetSyslogConf->Config) - sizeof(pGetSyslogConf->Config.Local));
     }
 
     pGetSyslogConf->CompletionCode = CC_NORMAL;
 
-    return sizeof(AMIGetLogConfRes_T);
+    return sizeof(AMIGetLogConfRes_T) - (sizeof(pGetSyslogConf->Config) - sizeof(pGetSyslogConf->Config.Remote));
 
 }
 
