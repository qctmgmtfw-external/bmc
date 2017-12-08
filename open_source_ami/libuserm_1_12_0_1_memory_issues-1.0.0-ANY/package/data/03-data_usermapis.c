--- .pristine/libuserm-1.12.0-src/data/usermapis.c Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/usermapis.c Wed Sep 14 15:42:30 2011
@@ -1339,10 +1339,11 @@
 
     /*-----convert the list to asciiz-------*/
     localasciiz = ListToAsciiz(list,*nNumUsers);
-    if(asciiz == NULL)
+    if(localasciiz == NULL)
     {
         TCRIT("Memory alloc error\n");
         retval = ENOMEM;
+        FreeFullList(list);
         goto error_out;
     }
     retval = 0;
@@ -1415,6 +1416,7 @@
     {
         TCRIT("Memory alloc error\n");
         retval = ENOMEM;
+        FreeFullList(list);
         goto error_out;
 
     }
