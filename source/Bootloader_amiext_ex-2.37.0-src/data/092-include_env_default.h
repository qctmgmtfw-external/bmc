--- u-boot-2013.07/include/env_default.h	2013-07-23 07:58:13.000000000 -0400
+++ uboot/include/env_default.h	2015-09-18 15:42:48.232556974 -0400
@@ -119,6 +119,27 @@
 #ifdef	CONFIG_CLOCKS_IN_MHZ
 	"clocks_in_mhz=1\0"
 #endif
+#ifdef	CONFIG_MEMTEST_ENABLE
+	"do_memtest=" __stringify(CONFIG_MEMTEST_ENABLE)	"\0"
+#endif
+#ifdef	CONFIG_MEMTEST_RESULT
+	"memtest_pass="  CONFIG_MEMTEST_RESULT	"\0"
+#endif
+#ifdef	CONFIG_BOOT_SELECTOR
+	"bootselector="  __stringify(CONFIG_BOOT_SELECTOR)	"\0"
+#endif
+#ifdef	CONFIG_MOST_RECENTLY_PROG_FW
+	"recentlyprogfw=" __stringify(CONFIG_MOST_RECENTLY_PROG_FW)	"\0"
+#endif
+
+#ifdef CONFIG_SD_BOOT_SELECTOR
+	"sdbootselector=" __stringify(CONFIG_SD_BOOT_SELECTOR) "\0"
+#endif
+
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+        "bootretry="  __stringify(CONFIG_SPX_FEATURE_BMC_FIRMWARE_REBOOT_RETRY_COUNT)    "\0"
+#endif
+
 #if defined(CONFIG_PCI_BOOTDELAY) && (CONFIG_PCI_BOOTDELAY > 0)
 	"pcidelay="	__stringify(CONFIG_PCI_BOOTDELAY)"\0"
 #endif
