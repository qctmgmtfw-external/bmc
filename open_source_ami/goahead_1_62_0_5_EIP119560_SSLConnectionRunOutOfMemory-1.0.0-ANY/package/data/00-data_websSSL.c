--- .pristine/goahead-1.62.0-src/data/websSSL.c	Wed Apr 10 14:28:37 2013
+++ source/goahead-1.62.0-src/data/websSSL.c	Fri Apr 12 10:02:26 2013
@@ -205,6 +205,10 @@
  */
     SSL_CTX_set_tmp_rsa_callback(sslctx, websSSLTempRSACallback);
 
+    // to fix IE9 HTTPS in NAT routing (TLS 1.0) issue
+    SSL_CTX_set_default_read_ahead(sslctx, 1);
+    SSL_CTX_set_mode(sslctx, SSL_CTX_get_mode(sslctx) | SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
+
 /*
  *  Set the verification callback for the SSL context
  */
@@ -688,7 +692,10 @@
 {
     int     rc, len, lenBuf;
     char    c;
-
+    
+	//IE9 HTTPS NAT routing fix
+    int sfd, retry_count = 0;
+	
     a_assert(wsp);
     a_assert(buf);
 
@@ -699,10 +706,31 @@
         return -1;
     }
 
+    sfd = BIO_get_fd(wsp->bio, NULL);
+    fcntl(sfd, F_SETFL, fcntl(sfd, F_GETFL) | O_NONBLOCK);
+
     while (1) {
-
+        //IE9 HTTPS NAT routing fix
+        //Below two lines commented and new block added
+        //BIO_read block and never returns
+
+        /*
         if ((rc = BIO_read(wsp->bio, &c, 1)) < 0) {
             return rc;
+        }
+        */
+        while ((rc = BIO_read(wsp->bio, &c, 1)) < 0) {
+            if (!BIO_should_retry(wsp->bio)) { //This is an error condition
+                fcntl(sfd, F_SETFL, fcntl (sfd, F_GETFL) &  ~O_NONBLOCK);
+                return rc;
+            }
+            retry_count++;
+            usleep(10);
+            if (retry_count > 16) {
+                // for some reason we are not able to receive data from ssl sock
+                fcntl(sfd, F_SETFL, fcntl (sfd, F_GETFL) &  ~O_NONBLOCK);
+                return 0;
+            }
         }
 
         if (rc == 0) {
@@ -712,6 +740,7 @@
             if (len > 0 && BIO_eof(wsp->bio)) {
                 c = '\n';
             } else {
+				fcntl(sfd, F_SETFL, fcntl (sfd, F_GETFL) &  ~O_NONBLOCK);
                 return -1;
             }
         }
@@ -723,6 +752,7 @@
             if ((len > 0) && (len < lenBuf)) {
                 (*buf)[len] = 0;
             }
+			fcntl(sfd, F_SETFL, fcntl (sfd, F_GETFL) &  ~O_NONBLOCK);
             return len;
         } else if (c == '\r') {
             continue;
