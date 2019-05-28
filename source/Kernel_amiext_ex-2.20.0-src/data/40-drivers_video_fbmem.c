--- linux.pristine/drivers/video/fbmem.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/video/fbmem.c	2014-02-03 15:11:00.027029096 -0500
@@ -510,6 +510,9 @@
 			fb_rotate_logo(info, logo_rotate, &image, rotate);
 	}
 
+	image.dx = (info->var.xres - image.width) / 2;
+	image.dy = (info->var.yres - image.height) / 2;
+
 	fb_do_show_logo(info, &image, rotate, n);
 
 	kfree(palette);
@@ -664,8 +667,11 @@
 {
 	int y;
 
+	// Display only one image even if the num of online cpus > 1
+	// y = fb_show_logo_line(info, rotate, fb_logo.logo, 0,
+	//		      num_online_cpus());
 	y = fb_show_logo_line(info, rotate, fb_logo.logo, 0,
-			      num_online_cpus());
+			      1);
 	y = fb_show_extra_logos(info, y, rotate);
 
 	return y;
