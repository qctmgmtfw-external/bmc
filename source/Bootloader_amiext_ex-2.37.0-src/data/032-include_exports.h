--- u-boot-2013.07/include/exports.h	2013-12-13 13:49:25.788958498 -0500
+++ uboot.new/include/exports.h	2013-12-05 12:17:55.243503948 -0500
@@ -3,6 +3,10 @@
 
 #ifndef __ASSEMBLY__
 
+#ifdef CONFIG_EXPORT_ETH_FNS
+#include <net.h>
+#endif
+
 /* These are declarations of exported functions available in C code */
 unsigned long get_version(void);
 int  getc(void);
@@ -26,9 +30,17 @@
 unsigned long ustrtoul(const char *cp, char **endp, unsigned int base);
 unsigned long long ustrtoull(const char *cp, char **endp, unsigned int base);
 #if defined(CONFIG_CMD_I2C)
+int i2c_probe (uchar);
 int i2c_write (uchar, uint, int , uchar* , int);
 int i2c_read (uchar, uint, int , uchar* , int);
 #endif
+#ifdef CONFIG_EXPORT_ETH_FNS
+int  eth_init(bd_t *bis);
+void eth_halt(void);
+int  eth_send(void *packet, int length);
+int  eth_rx(void);
+void net_set_udp_handler(rxhand_f*);
+#endif
 
 void app_startup(char * const *);
 
