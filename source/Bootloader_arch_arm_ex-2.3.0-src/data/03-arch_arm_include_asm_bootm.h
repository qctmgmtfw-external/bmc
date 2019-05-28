--- u-boot-2013.07/arch/arm/include/asm/bootm.h	2013-12-13 13:49:25.444958498 -0500
+++ uboot.new/arch/arm/include/asm/bootm.h	2013-12-05 12:17:55.087503948 -0500
@@ -28,12 +28,19 @@
 		defined(CONFIG_CMDLINE_TAG) || \
 		defined(CONFIG_INITRD_TAG) || \
 		defined(CONFIG_SERIAL_TAG) || \
-		defined(CONFIG_REVISION_TAG)
+		defined(CONFIG_REVISION_TAG)|| \
+		defined(CONFIG_ENETADDR_TAG)
 # define BOOTM_ENABLE_TAGS		1
 #else
 # define BOOTM_ENABLE_TAGS		0
 #endif
 
+#ifdef CONFIG_ENETADDR_TAG
+# define BOOTM_ENABLE_ENETADDR_TAGS	1
+#else
+# define BOOTM_ENABLE_ENETADDR_TAGS	0
+#endif
+
 #ifdef CONFIG_SETUP_MEMORY_TAGS
 # define BOOTM_ENABLE_MEMORY_TAGS	1
 #else
