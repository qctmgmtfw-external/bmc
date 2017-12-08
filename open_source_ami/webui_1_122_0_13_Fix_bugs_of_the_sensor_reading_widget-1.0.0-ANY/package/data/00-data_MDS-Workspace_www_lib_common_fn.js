--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/common_fn.js Wed Aug 29 13:49:44 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/common_fn.js Wed Aug 29 13:57:17 2012
@@ -764,6 +764,7 @@
 function prepareDeviceShutdown()
 {
 	parent.oMenu.disableNavBar();
+	parent.StopPolling();
 	parent.closeAllWidgets();
 	parent.web_alerts.stop();
 	top.headerFrame.clearActions();
