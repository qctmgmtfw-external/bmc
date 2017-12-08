--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Mon Apr  2 14:37:55 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Mon Apr  2 15:20:27 2012
@@ -2927,6 +2927,12 @@
          nwMakeIFUp(Ifcnametable[i].Index);
         TDBG(" eth%d up is done........ ",i);    
     }
+    
+    /*Restart services*/
+    for(i=0;i<MAX_RESTART_SERVICE;i++)
+    {
+        safe_system(RestartServices[i]);
+    }
 
     /* While restarting both the interfaces, the first interface (eth0) 
     will not be registered properly to DNS, because of any one of the 
