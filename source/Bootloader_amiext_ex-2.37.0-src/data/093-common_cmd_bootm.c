--- uboot_old/common/cmd_bootm.c	2013-07-23 07:58:13.000000000 -0400
+++ uboot/common/cmd_bootm.c	2014-12-09 04:40:06.815383919 -0500
@@ -941,8 +941,14 @@
 		printf("## Booting kernel from Legacy Image at %08lx ...\n",
 				img_addr);
 		hdr = image_get_kernel(img_addr, images->verify);
-		if (!hdr)
+		if (!hdr) {
+#if defined(CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY)
+            setenv("imgheadererr","yes");
+            saveenv();
+            do_reset (cmdtp, flag, argc, argv);
+#endif
 			return NULL;
+        }
 		bootstage_mark(BOOTSTAGE_ID_CHECK_IMAGETYPE);
 
 		/* get os_data and os_len */
