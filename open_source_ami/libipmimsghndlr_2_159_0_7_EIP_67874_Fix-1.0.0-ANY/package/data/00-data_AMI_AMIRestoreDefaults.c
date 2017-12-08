--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIRestoreDefaults.c Wed Sep 14 13:16:17 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIRestoreDefaults.c Wed Sep 14 17:44:23 2011
@@ -56,7 +56,12 @@
     }
     
     system (RESTORE_CMD);
-	
+
+    //Restart adviser if preserve conf support enable else "RESTORE_CMD" will take care
+    #ifdef CONFIG_SPX_FEATURE_PRESERVE_CONF_SUPPORT
+        TINFO("Stopping adviserd... ");
+        system("/etc/init.d/adviserd.sh stop");
+    #endif	
     TINFO("Restoring to default configuration... Done");
 
     //Post BMC Reboot task to Pend task    
