--- linux.pristine/drivers/i2c/i2c-dev.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/i2c/i2c-dev.c	2014-02-03 15:11:00.039029094 -0500
@@ -40,6 +40,10 @@
 #include <linux/jiffies.h>
 #include <linux/uaccess.h>
 
+#ifdef I2C_BUS_RECOVERY
+static struct 	timer_list poll_timer;
+#endif
+
 /*
  * An i2c_dev represents an i2c_adapter ... an I2C or SMBus master, not a
  * slave (i2c_client) with which messages will be exchanged.  It's coupled
@@ -373,7 +377,7 @@
 		/* These are special: we do not use data */
 		return i2c_smbus_xfer(client->adapter, client->addr,
 				      client->flags, data_arg.read_write,
-				      data_arg.command, data_arg.size, NULL);
+				      data_arg.command, data_arg.size, &temp);
 
 	if (data_arg.data == NULL) {
 		dev_dbg(&client->adapter->dev,
@@ -406,6 +410,7 @@
 	}
 	res = i2c_smbus_xfer(client->adapter, client->addr, client->flags,
 	      data_arg.read_write, data_arg.command, data_arg.size, &temp);
+#if 0
 	if (!res && ((data_arg.size == I2C_SMBUS_PROC_CALL) ||
 		     (data_arg.size == I2C_SMBUS_BLOCK_PROC_CALL) ||
 		     (data_arg.read_write == I2C_SMBUS_READ))) {
@@ -413,6 +418,28 @@
 			return -EFAULT;
 	}
 	return res;
+#endif
+	if ( res && (((data_arg.size == I2C_SMBUS_BYTE) ||
+	              (data_arg.size == I2C_SMBUS_BYTE_DATA) ||(data_arg.size == I2C_SMBUS_WORD_DATA))&&
+		      (data_arg.read_write == I2C_SMBUS_READ))) {
+		if (copy_to_user(data_arg.data, &temp, datasize))
+			return -EFAULT;
+	}
+
+	if (  res && (data_arg.size == I2C_SMBUS_BLOCK_DATA)&&(data_arg.read_write == I2C_SMBUS_READ))
+	{
+		if (copy_to_user(data_arg.data, &temp, res))
+			return -EFAULT;
+	}
+
+	if ( res && (((data_arg.size == I2C_SMBUS_PROC_CALL) ||
+		      (data_arg.size == I2C_SMBUS_BLOCK_PROC_CALL)) ||
+		      (data_arg.read_write == I2C_SMBUS_READ))) {
+		if (copy_to_user(data_arg.data, &temp, res * sizeof(data_arg.data->byte)))
+			return -EFAULT;
+	}
+
+	return 0;
 }
 
 static long i2cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
@@ -476,12 +503,8 @@
 		client->adapter->timeout = msecs_to_jiffies(arg * 10);
 		break;
 	default:
-		/* NOTE:  returning a fault code here could cause trouble
-		 * in buggy userspace code.  Some old kernel bugs returned
-		 * zero in this case, and userspace code might accidentally
-		 * have depended on that bug.
-		 */
-		return -ENOTTY;
+		/* Call the core ioctl (AMI extended)functions */
+		return i2c_ioctl(client->adapter,cmd,arg);
 	}
 	return 0;
 }
@@ -617,6 +640,14 @@
 };
 
 /* ------------------------------------------------------------------------- */
+#ifdef I2C_BUS_RECOVERY
+static void recover_i2c_bus (unsigned long ptr)
+{
+	i2c_recovery_event ();
+	mod_timer(&poll_timer,jiffies +(10000 * HZ)/1000);
+	return;
+}
+#endif
 
 /*
  * module load/unload record keeping
@@ -647,6 +678,14 @@
 	/* Bind to already existing adapters right away */
 	i2c_for_each_dev(NULL, i2cdev_attach_adapter);
 
+#ifdef I2C_BUS_RECOVERY
+	init_timer(&poll_timer);
+	poll_timer.function = recover_i2c_bus;
+	poll_timer.data = 0;
+	poll_timer.expires = jiffies +(10000 * HZ)/1000;
+	add_timer(&poll_timer);
+#endif
+
 	return 0;
 
 out_unreg_class:
@@ -664,6 +703,9 @@
 	i2c_for_each_dev(NULL, i2cdev_detach_adapter);
 	class_destroy(i2c_dev_class);
 	unregister_chrdev(I2C_MAJOR, "i2c");
+#ifdef I2C_BUS_RECOVERY
+	del_timer(&poll_timer);
+#endif
 }
 
 MODULE_AUTHOR("Frodo Looijaard <frodol@dds.nl> and "
