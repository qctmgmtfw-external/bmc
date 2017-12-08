--- .pristine/libipmisol-2.8.0-src/data/SOL.c Wed Sep 14 14:29:23 2011
+++ source/libipmisol-2.8.0-src/data/SOL.c Wed Sep 14 16:40:04 2011
@@ -307,6 +307,7 @@
     if(NULL == RecvBuf)
     {
         printf("Cannot allocate memory for Recv Buffer in SOL Task\n");
+        free(SendBuf);   	
         return 0;
     }
 
