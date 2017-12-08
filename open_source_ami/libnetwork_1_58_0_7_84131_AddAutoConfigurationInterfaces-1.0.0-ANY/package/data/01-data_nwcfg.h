--- .pristine/libnetwork-1.58.0-src/data/nwcfg.h Wed Mar  7 21:55:03 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.h Fri Mar  9 11:46:46 2012
@@ -179,6 +179,10 @@
                          | NWEXT_IPCFG_MASK \
                          | NWEXT_IPCFG_GW)
 
+#define MAX_RESTART_SERVICE     5
+
+#define MAX_SERVICE    5
+
 /*Used to check the flag status*/
 #define CHECK_FLAG(in,level) ((in&level) == level)
 
@@ -469,7 +473,8 @@
     INT8U   BondIndex;                                                      /*Index value of Bond Interface*/
     INT8U   BondMode;                                                       /*Bond Mode*/
     INT16U   MiiInterval;                                                     /*MII Interval*/
-    INT8U   Slaves;                                                           /*Each bit represents the interface Index value i.e 0-7*/    
+    INT8U   Slaves;                                                           /*Each bit represents the interface Index value i.e 0-7*/
+    INT8U   AutoConf;
 }PACKED BondIface;
 
 
