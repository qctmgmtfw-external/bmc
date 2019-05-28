--- linux-3.14.17/include/uapi/linux/i2c-dev.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/uapi/linux/i2c-dev.h	2014-08-21 12:43:39.245632919 -0400
@@ -51,6 +51,20 @@
 #define I2C_PEC		0x0708	/* != 0 to use PEC with SMBus */
 #define I2C_SMBUS	0x0720	/* SMBus transfer */
 
+/* Extended IOCTLS - Implemented in i2c-core and hardware */
+#define I2C_SLAVEREAD      0x0750  /* Perform Slave Read   */
+#define I2C_SLAVEWRITE     0x0751  /* Perform Slave Write  */
+#define I2C_SET_HOST_ADDR  0x0752  /* Set Slave Address of the Controller */
+#define I2C_GET_HOST_ADDR  0x0753  /* Get Slave Address of the Controller */
+#define I2C_SET_REC_INFO   0x0754  /* Set Recovery Parameters */
+#define I2C_GET_REC_INFO   0x0755  /* Get Recovery Parameters */
+#define I2C_RESET      0x0756  /* Reset Controller */
+#define SMBUS_HOST_NOTIFY  0x0757 /* Perform SMBus Host Notify Protocol */
+#define I2C_SET_SPEED      0x0758  /* Set I2C Bus Speed */
+#define I2C_GET_LINK_STATE      0x0759  /* Get the Status of Clk/Data pin*/
+#define I2C_BUS_TEST            0x0850  /* Perform the I2C Bus test*/
+#define I2C_MCTP_READ           0x0851  /* Perform MCTP Pkt Read*/
+
 
 /* This is the structure as used in the I2C_SMBUS ioctl call */
 struct i2c_smbus_ioctl_data {
