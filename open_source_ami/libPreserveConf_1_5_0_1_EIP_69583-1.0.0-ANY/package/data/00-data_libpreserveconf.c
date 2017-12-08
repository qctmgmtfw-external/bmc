--- .pristine/libPreserveConf-1.5.0-src/data/libpreserveconf.c Mon Sep 12 16:36:00 2011
+++ source/libPreserveConf-1.5.0-src/data/libpreserveconf.c Mon Sep 12 17:25:25 2011
@@ -43,7 +43,8 @@
 #endif                
                 {7, "SSH", 0},                  // to preserve all SSH related files
                 {8, "KVM", 0},                  //to preserve KVM & Vmedia related files
-                {9, "Authentication", 0}        //to preserve Authentication related files
+                {9, "Authentication", 0},       //to preserve Authentication related files
+                {10,"Services", 0}              //to preserve Services related  files
 };
 
 /*----------------------------------------------------------------------------
