diff -Naur linux.old/include/uapi/linux/i2c.h linux/include/uapi/linux/i2c.h
--- linux.old/include/uapi/linux/i2c.h	2015-03-03 10:32:05.333984866 +0800
+++ linux/include/uapi/linux/i2c.h	2015-03-03 10:31:44.973985066 +0800
@@ -124,7 +124,7 @@
 /*
  * Data for SMBus Messages
  */
-#define I2C_SMBUS_BLOCK_MAX	32	/* As specified in SMBus standard */
+#define I2C_SMBUS_BLOCK_MAX	240	/* As specified in SMBus standard */
 union i2c_smbus_data {
 	__u8 byte;
 	__u16 word;
