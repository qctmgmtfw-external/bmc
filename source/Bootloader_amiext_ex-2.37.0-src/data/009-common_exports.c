--- u-boot-2013.07/common/exports.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/exports.c	2013-12-05 12:17:55.223503948 -0500
@@ -16,7 +16,8 @@
 /* Reuse _exports.h with a little trickery to avoid bitrot */
 #define EXPORT_FUNC(sym) gd->jt[XF_##sym] = (void *)sym;
 
-#if !defined(CONFIG_X86) && !defined(CONFIG_PPC)
+//#if !defined(CONFIG_X86) && !defined(CONFIG_PPC) && !defined(CONFIG_ARM)
+#ifndef CONFIG_USE_IRQ
 # define install_hdlr      dummy
 # define free_hdlr         dummy
 #else /* kludge for non-standard function naming */
@@ -24,6 +25,7 @@
 # define free_hdlr         irq_free_handler
 #endif
 #ifndef CONFIG_CMD_I2C
+# define i2c_probe	   dummy
 # define i2c_write         dummy
 # define i2c_read          dummy
 #endif
@@ -35,6 +37,13 @@
 # define spi_release_bus   dummy
 # define spi_xfer          dummy
 #endif
+#ifndef CONFIG_EXPORT_ETH_FNS
+# define eth_init              dummy
+# define eth_halt              dummy
+# define eth_send              dummy
+# define eth_rx                dummy
+# define net_set_udp_handler   dummy
+#endif
 
 void jumptable_init(void)
 {
