--- uboot/oem/ami/fmh/cmd_fmh.c	2016-05-24 13:22:49.036117183 +0800
+++ uboot/oem/ami/fmh/cmd_fmh_new.c	2016-05-24 13:22:40.828116957 +0800
@@ -718,7 +718,7 @@
     currentbootretry++;
     sprintf(value,"%d",currentbootretry);
     setenv("currentbootretry",(char *)value);
-    saveenv();
+    //saveenv();  // Quanta remove
 
     if(force_recovery || imgheadererr || (currentbootretry > bootretry) )
     {
@@ -741,6 +741,7 @@
             fwupdate();
             return;
         }
+        saveenv();  // Quanta
     }
     else
     {
