--- linux/include/linux/i2c.h	2017-03-07 12:54:12.057510772 +0800
+++ linux/include/linux/new_i2c.h	2017-03-08 09:32:48.951994278 +0800
@@ -34,7 +34,7 @@
 #include <linux/swab.h>		/* for swab16 */
 #include <uapi/linux/i2c.h>
 
-#define MAX_IPMB_MSG_SIZE  128
+#define MAX_IPMB_MSG_SIZE  256
 #define MAX_I2C_ADAPTERS   16
 
 
