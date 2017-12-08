--- .pristine/goahead-1.62.0-src/data/websSSL.c Thu Jul 24 12:34:16 2014
+++ source/goahead-1.62.0-src/data/websSSL.c Mon Sep  1 18:32:42 2014
@@ -726,7 +726,7 @@
             }
             retry_count++;
             usleep(10);
-            if (retry_count > 16) {
+            if (retry_count > 512) {
                 // for some reason we are not able to receive data from ssl sock
                 fcntl(sfd, F_SETFL, fcntl (sfd, F_GETFL) &  ~O_NONBLOCK);
                 return 0;
