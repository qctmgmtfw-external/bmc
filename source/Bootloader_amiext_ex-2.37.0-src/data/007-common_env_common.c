--- u-boot-2013.07/common/env_common.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/env_common.c	2013-12-05 12:17:55.223503948 -0500
@@ -34,6 +34,8 @@
 
 DECLARE_GLOBAL_DATA_PTR;
 
+int gDefaultEnvironment = 0;
+
 /************************************************************************
  * Default settings to be used when no valid environment is found
  */
@@ -192,6 +194,7 @@
 
 void env_relocate(void)
 {
+	gDefaultEnvironment = 0;
 #if defined(CONFIG_NEEDS_MANUAL_RELOC)
 	env_reloc();
 	env_htab.change_ok += gd->reloc_off;
@@ -201,6 +204,7 @@
 		/* Environment not changable */
 		set_default_env(NULL);
 #else
+		gDefaultEnvironment = 1;
 		bootstage_error(BOOTSTAGE_ID_NET_CHECKSUM);
 		set_default_env("!bad CRC");
 #endif
