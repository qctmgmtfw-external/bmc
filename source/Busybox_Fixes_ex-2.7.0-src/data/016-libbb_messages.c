--- busybox.org/libbb/messages.c	2015-07-28 14:11:41.360055716 +0530
+++ busybox/libbb/messages.c	2015-07-28 14:17:11.904051288 +0530
@@ -40,7 +40,7 @@
 /* util-linux manpage says /sbin:/bin:/usr/sbin:/usr/bin,
  * but I want to save a few bytes here. Check libbb.h before changing! */
 const char bb_PATH_root_path[] ALIGN1 =
-	"PATH=/sbin:/usr/sbin:/bin:/usr/bin" BB_ADDITIONAL_PATH;
+	"PATH=/sbin:/usr/sbin:/bin:/usr/bin:/usr/local/bin" BB_ADDITIONAL_PATH;
 
 
 const int const_int_1 = 1;
