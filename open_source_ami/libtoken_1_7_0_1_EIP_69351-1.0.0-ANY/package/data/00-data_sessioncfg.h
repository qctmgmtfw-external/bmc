--- .pristine/libtoken-1.7.0-src/data/sessioncfg.h Mon Sep 19 10:35:44 2011
+++ source/libtoken-1.7.0-src/data/sessioncfg.h Mon Sep 19 18:44:23 2011
@@ -40,6 +40,7 @@
 #define BANDWIDTH_1MBPS					(128 * 1024)
 
 #ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_CONSOLE_PREVIEW  
+#define WEB_PREVIEWER_HOST_POWER_OFF			-5
 #define WEB_PREVIEWER_CAP_IN_PROGRESS			-2
 #define WEB_PREVIEWER_CAP_FAILURE			-1
 #define WEB_PREIVEW_CAPTURE_INTERVAL			10 	//seconds
