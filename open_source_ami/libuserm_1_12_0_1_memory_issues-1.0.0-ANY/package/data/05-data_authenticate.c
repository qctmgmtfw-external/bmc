--- .pristine/libuserm-1.12.0-src/data/authenticate.c Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/authenticate.c Wed Sep 14 15:40:22 2011
@@ -63,6 +63,7 @@
     if(si == NULL)
     {
         TCRIT("Insufficient memory for session info allocation\n");
+        return NULL;
     }
 
     memset(si,0,sizeof(SESSION_INFO));
