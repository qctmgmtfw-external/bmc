--- .pristine/libunix-1.5.0-src/data/proc-invok.c Thu Jan  5 09:40:02 2012
+++ source/libunix-1.5.0-src/data/proc-invok.c Fri Jan  6 15:58:33 2012
@@ -225,13 +225,14 @@
     while ((entry = readdir(dir)))
     {
         /* Convert the file descriptor to integer */
+        fd = atoi( entry->d_name);
         if (fd > 2)
         {
             /* Close it */
             close(fd);
         }
     }
-    
+    closedir(dir);
     return 0;
 }
 
@@ -257,9 +258,9 @@
         ... but close all my pipes 
         because we have seen unwanted write(7, .. ) from /usr/sbin/dhcp6c
         which corrupts /var/MsgHndlrQ1 */
-    if ((child = fork ()) < 0)
-    {
-        perror("fork");
+    if ((child = vfork ()) < 0)
+    {
+        perror("vfork");
         return errno;
     }
     else
@@ -270,7 +271,7 @@
             close_all_fd(getppid());
             execlp("/bin/sh", "sh", "-c", command, NULL);
             perror("execve");
-            return errno;
+            _exit(1); 
         }
         else
         {
