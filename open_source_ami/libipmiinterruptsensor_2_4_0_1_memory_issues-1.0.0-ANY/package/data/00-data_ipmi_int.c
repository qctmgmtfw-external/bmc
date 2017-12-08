--- .pristine/libipmiinterruptsensor-2.4.0-src/data/ipmi_int.c Wed Sep 14 14:29:20 2011
+++ source/libipmiinterruptsensor-2.4.0-src/data/ipmi_int.c Wed Sep 14 16:33:28 2011
@@ -56,6 +56,8 @@
     int cd_pipe;
     int fd_pipe;
     char buf[1];
+    //initialize the buffer with zero
+	buf[0] = 0;
 
     do 
     {
