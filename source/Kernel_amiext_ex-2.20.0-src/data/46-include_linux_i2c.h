--- linux-3.14.17/include/linux/i2c.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/linux/i2c.h	2014-08-21 12:41:22.625630354 -0400
@@ -34,6 +34,10 @@
 #include <linux/swab.h>		/* for swab16 */
 #include <uapi/linux/i2c.h>
 
+#define MAX_IPMB_MSG_SIZE  128
+#define MAX_I2C_ADAPTERS   16
+
+
 extern struct bus_type i2c_bus_type;
 extern struct device_type i2c_adapter_type;
 
@@ -70,6 +74,10 @@
 extern int __i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs,
 			  int num);
 
+/* Extended IOCTLs -implemented in i2c-core */
+extern int i2c_ioctl(struct i2c_adapter *adap, unsigned long cmd, unsigned long arg);
+
+
 /* This is the very generalized SMBus access routine. You probably do not
    want to use this, though; one of the functions below may be much easier,
    and probably just as fast.
@@ -82,6 +90,10 @@
 /* Now follow the 'nice' access routines. These also document the calling
    conventions of i2c_smbus_xfer. */
 
+extern u8 i2c_smbus_msg_pec(u8 pec, struct i2c_msg *msg);
+extern void i2c_smbus_add_pec(struct i2c_msg *msg);
+extern int i2c_smbus_check_pec(u8 cpec, struct i2c_msg *msg);
+
 extern s32 i2c_smbus_read_byte(const struct i2c_client *client);
 extern s32 i2c_smbus_write_byte(const struct i2c_client *client, u8 value);
 extern s32 i2c_smbus_read_byte_data(const struct i2c_client *client,
@@ -377,6 +389,19 @@
 
 	/* To determine what the adapter supports */
 	u32 (*functionality) (struct i2c_adapter *);
+
+   /* AMI Extended Functions */
+   /* i2c recovery */
+   int (*i2c_recovery) (struct i2c_adapter *adap);
+
+   /* Slave Receive and Slave Send Functions */
+   int (*slave_send)(struct i2c_adapter *adap , char *data ,int size);
+   int (*slave_recv)(struct i2c_adapter *adap , char *data ,int size);
+   int (*smb_slave_recv)(struct i2c_adapter *adap , char *data , int size);
+
+   /* Extended IOCTL Calls */
+   int (*i2c_hw_ioctl)(struct i2c_adapter *adap, unsigned long cmd , unsigned long arg);
+
 };
 
 /**
@@ -418,6 +443,42 @@
 int i2c_generic_gpio_recovery(struct i2c_adapter *adap);
 int i2c_generic_scl_recovery(struct i2c_adapter *adap);
 
+/****************************************************/
+/* 			AMI Recovery Begin 					    */
+/* Keeping this for backward compatability. May be  */
+/* from 3.14.x onwards we can use generic recovery? */		
+/****************************************************/
+extern int i2c_recovery_event(void);
+
+/*
+ * i2c_recovery is the structure used to specify the recovery parameters for the bus
+ */
+#define DEFAULT_NUM_PULSES     16
+#define DEFAULT_PULSE_PERIOD       5
+#define DEFAULT_FREQ           100000
+#define ENABLE_SMBUS_RESET     1
+#define DISABLE_SMBUS_RESET        0
+#define ENABLE_CLOCK_PULSE     1
+#define DISABLE_CLOCK_PULSE        0
+#define ENABLE_FORCE_STOP      1
+#define DISABLE_FORCE_STOP     0
+#define ENABLE_RECOVERY            1
+#define DISABLE_RECOVERY       0   /* Recovery should not be done on multi-master bus - Disable Recovery */
+typedef struct
+{
+   int enable_recovery;
+   int is_smbus_reset_enabled;
+   int is_generate_clock_pulses_enabled;
+   int is_force_stop_enabled;
+   int num_clock_pulses;
+   int frequency;
+} bus_recovery_info_T;
+
+/**************************************************/
+/*               AMI Recovery End				  */
+/**************************************************/
+
+
 /*
  * i2c_adapter is the structure used to identify a physical i2c bus along
  * with the access algorithms necessary to access it.
