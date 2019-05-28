--- linux.pristine/drivers/tty/vt/vt.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/tty/vt/vt.c	2014-02-03 15:11:00.023029096 -0500
@@ -1616,7 +1616,8 @@
 	vc->vc_decscnm		= 0;
 	vc->vc_decom		= 0;
 	vc->vc_decawm		= 1;
-	vc->vc_deccm		= global_cursor_default;
+	//vc->vc_deccm		= global_cursor_default;
+	vc->vc_deccm		= 0;
 	vc->vc_decim		= 0;
 
 	vt_reset_keyboard(vc->vc_num);
