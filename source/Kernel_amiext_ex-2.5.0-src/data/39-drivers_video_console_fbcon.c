--- linux.pristine/drivers/video/console/fbcon.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/video/console/fbcon.c	2014-02-03 15:11:00.019029096 -0500
@@ -584,7 +584,8 @@
 	if (fb_get_color_depth(&info->var, &info->fix) == 1)
 		erase &= ~0x400;
 	logo_height = fb_prepare_logo(info, ops->rotate);
-	logo_lines = DIV_ROUND_UP(logo_height, vc->vc_font.height);
+	//logo_lines = DIV_ROUND_UP(logo_height, vc->vc_font.height);
+	logo_lines = (logo_height + ((info->var.yres-logo_height)/2) + vc->vc_font.height - 1) / vc->vc_font.height;
 	q = (unsigned short *) (vc->vc_origin +
 				vc->vc_size_row * rows);
 	step = logo_lines * cols;
