--- .pristine/ipmimain-2.61.0-src/data/SharedMem.c Fri Feb  3 16:18:36 2012
+++ source/ipmimain-2.61.0-src/data/SharedMem.c Thu Jan 19 10:58:57 2012
@@ -290,6 +290,8 @@
     pSharedMem->PowerActionInProgress = FALSE;
 #endif
 
+    InitDNSConfiguration(BMCInst);    
+    
     return 0;
 }
 
