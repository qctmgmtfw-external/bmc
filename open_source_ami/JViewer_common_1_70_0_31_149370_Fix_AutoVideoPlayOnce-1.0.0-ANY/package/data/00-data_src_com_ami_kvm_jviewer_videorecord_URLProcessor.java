--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/videorecord/URLProcessor.java Tue Jan  7 02:37:18 2014
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/videorecord/URLProcessor.java Tue Jan  7 22:28:18 2014
@@ -190,6 +190,7 @@
 			else if(secureConnect == 1)
 				conn = (HttpsURLConnection) url.openConnection();
 
+			conn.setUseCaches(false);
 			conn.setDoOutput(true);
 			conn.setInstanceFollowRedirects(false);
 			if (sesCookie != null) {
