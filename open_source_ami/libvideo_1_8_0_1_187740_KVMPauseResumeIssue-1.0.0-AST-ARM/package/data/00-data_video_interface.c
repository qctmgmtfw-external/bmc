--- .pristine/libvideo-1.8.0-ARM-AST-src/data/video_interface.c Fri Aug 29 18:02:01 2014
+++ source/libvideo-1.8.0-ARM-AST-src/data/video_interface.c Tue Oct 14 15:29:14 2014
@@ -93,6 +93,9 @@
 
 void resume_video(void)
 {
+	g_send_full_screen = 1;
+
+#if 0
     /* Since AST Soc does not support resume video,
      * so clear buffer to emulation resume video function.*/
     ASTCap_Ioctl ioc;
@@ -105,6 +108,7 @@
 	if (ioc.ErrCode != ASTCAP_IOCTL_SUCCESS) {
 		printf("resume_video failed\n");
 	}
+#endif
 }
 
 int set_video_engine_configs(void *configs)
