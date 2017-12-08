--- uboot.org/common/env_common.c	2007-10-31 13:00:18.000000000 -0400
+++ uboot/common/env_common.c	2007-11-06 08:59:55.000000000 -0500
@@ -59,6 +59,8 @@
 static uchar env_get_char_init (int index);
 uchar (*env_get_char)(int) = env_get_char_init;
 
+int gDefaultEnvironment = 0;
+
 /************************************************************************
  * Default settings to be used when no valid environment is found
  */
@@ -200,6 +200,7 @@
 
 void env_relocate (void)
 {
+        gDefaultEnvironment = 0;
 	DEBUGF ("%s[%d] offset = 0x%lx\n", __FUNCTION__,__LINE__,
 		gd->reloc_off);
 
@@ -232,6 +235,7 @@
 		puts ("Using default environment\n\n");
 #else
 		puts ("*** Warning - bad CRC, using default environment\n\n");
+		gDefaultEnvironment = 1;
 		SHOW_BOOT_PROGRESS (-1);
 #endif
 
