--- uboot/common/board_r.c	2017-06-27 18:04:59.025280140 +0800
+++ uboot/common/board_r_new.c	2017-06-28 13:51:29.000000000 +0800
@@ -711,7 +711,7 @@
 	return 0;
 }

-#ifdef CONFIG_BOARDID_IN_EEPROM
+#if 0
 static int boardid_init (void)
 {
 	int    i, offs, retry, ret = 0;
@@ -1015,7 +1015,7 @@
 #ifdef CONFIG_MODEM_SUPPORT
 	initr_modem,
 #endif
-#ifdef CONFIG_BOARDID_IN_EEPROM
+#if 0
 	boardid_init,
 #endif
 	run_main_loop,
