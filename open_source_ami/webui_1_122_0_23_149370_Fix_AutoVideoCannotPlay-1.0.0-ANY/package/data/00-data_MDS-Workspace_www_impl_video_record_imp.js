--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/video_record_imp.js Thu Jan  9 01:18:10 2014
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/video_record_imp.js Thu Jan  9 02:30:50 2014
@@ -207,6 +207,7 @@
 		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
 			if(arg.HAPI_STATUS == 0) {
 				if (top.user.isAdmin()) {
+					getConsoles();
 					window.open(top.gPageDir + "jviewer_launch.html?" + 
 						"JNLPSTR=PlayVideo&JNLPNAME=" + top.gVideoJnlp + 
 						"&FILENAME=" + filename, "PlayVideo", "toolbar=0," +
@@ -238,6 +239,7 @@
 		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
 			if(arg.HAPI_STATUS == 0) {
 				if (top.user.isAdmin()) {
+					getConsoles();
 					window.open(top.gPageDir + "jviewer_launch.html?" + 
 						"JNLPSTR=SaveVideo&JNLPNAME=" + top.gVideoJnlp +
 						"&FILENAME=" + filename, "DownloadVideo", "toolbar=0," +
