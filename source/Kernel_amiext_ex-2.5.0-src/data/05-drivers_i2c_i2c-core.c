--- linux-3.14.17/drivers/i2c/i2c-core.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/drivers/i2c/i2c-core.c	2014-08-21 12:48:38.773638542 -0400
@@ -49,6 +49,7 @@
 #include <linux/pm_runtime.h>
 #include <linux/acpi.h>
 #include <asm/uaccess.h>
+#include <linux/i2c-dev.h>   // For IOCTL defines
 
 #include "i2c-core.h"
 
@@ -1688,6 +1689,43 @@
 	bus_unregister(&i2c_bus_type);
 }
 
+#ifdef I2C_BUS_RECOVERY
+int i2c_recovery_event(void)
+{
+	int id, ret;
+	struct i2c_adapter	*adap;
+
+	for (id = 0; id < MAX_I2C_ADAPTERS; id++)
+	{
+		adap = i2c_get_adapter(id);
+		if (adap != NULL)
+		{
+			if (adap->algo->i2c_recovery)
+			{
+				if (in_atomic() || irqs_disabled())
+				{
+					ret = rt_mutex_trylock(&adap->bus_lock);
+					if (!ret)
+					{
+						i2c_put_adapter(adap);
+						continue;
+					}
+				}
+				else
+				{
+					rt_mutex_lock(&adap->bus_lock);
+				}
+				ret = adap->algo->i2c_recovery(adap);
+				rt_mutex_unlock(&adap->bus_lock);
+			}
+			i2c_put_adapter(adap);
+		}
+	}
+
+	return 0;
+}
+#endif
+
 /* We must initialize early, because some subsystems register i2c drivers
  * in subsys_initcall() code, but are linked (and initialized) before i2c.
  */
@@ -1793,6 +1831,95 @@
 }
 EXPORT_SYMBOL(i2c_transfer);
 
+/* AMI Extended IOCTLS */
+int i2c_ioctl(struct i2c_adapter * adap,unsigned long cmd, unsigned long arg)
+{
+	switch (cmd)
+	{
+		case I2C_SLAVEREAD:
+			/* IMPORTANT NOTE:
+			   Why we don't lock slave xfer functions?
+
+			   All SPX compliant i2c hardware should never access hardware.
+			   Instead they sleep for slave buffers gets filled up.
+
+			   Since they does access hardware, locking is not needed to
+			   prevent master and slave access to hardware. Also if we lock
+			   the master xfers cannot be done as the slave routines does not
+			   return if if does not have slave data.
+			*/
+			if (adap->algo->slave_recv)
+			{
+				char *tmp;
+				int ret;
+				tmp = kmalloc(MAX_IPMB_MSG_SIZE,GFP_KERNEL);
+				if (!tmp)
+					return -ENOMEM;
+				ret = adap->algo->slave_recv(adap,tmp,MAX_IPMB_MSG_SIZE);
+				if (ret > 0)
+					ret = copy_to_user((char *)arg,tmp,ret)?-EFAULT:ret;
+				kfree(tmp);
+				return ret;
+			}
+			break;
+
+		case SMBUS_HOST_NOTIFY:
+			if (adap->algo->smb_slave_recv)
+			{
+				char 	*tmp;
+				int 	ret = 0;
+				int     count = 0;
+				tmp = kmalloc(MAX_IPMB_MSG_SIZE, GFP_KERNEL);
+				if (tmp==NULL)
+					return -ENOMEM;
+
+				ret = adap->algo->smb_slave_recv(adap,tmp,count);
+				if (ret > 0)
+					ret = copy_to_user((unsigned char __user *)arg,tmp,ret)?-EFAULT:ret;
+#ifdef DEBUG
+				printk (KERN_DEBUG "ret value returned in slave receive call = 0x%x\n", ret);
+#endif
+				kfree(tmp);
+				return ret;
+			}
+			break;
+
+		/* UNTESTED CODE :We don't have any scenario to test slave writes */
+#if 1
+		case I2C_SLAVEWRITE:
+			if (adap->algo->slave_send)
+			{
+				char *tmp;
+				int ret;
+				tmp = kmalloc(MAX_IPMB_MSG_SIZE,GFP_KERNEL);
+				if (!tmp)
+					return -ENOMEM;
+				ret = copy_from_user(tmp,(char *)arg,MAX_IPMB_MSG_SIZE)?-EFAULT:0;
+				if (ret == -EFAULT)
+				{
+					kfree(tmp);
+					return -EFAULT;
+				}
+				return adap->algo->slave_send(adap,tmp,MAX_IPMB_MSG_SIZE);
+			}
+			break;
+#endif
+		default:
+			if (adap->algo->i2c_hw_ioctl)
+				return adap->algo->i2c_hw_ioctl(adap,cmd,arg);
+			break;
+	}
+
+	/* NOTE:  returning a fault code here could cause trouble
+	 * in buggy userspace code.  Some old kernel bugs returned
+	 * zero in this case, and userspace code might accidentally
+	 * have depended on that bug.
+	 */
+	return -ENOTTY;
+}
+
+EXPORT_SYMBOL(i2c_ioctl);
+
 /**
  * i2c_master_send - issue a single I2C message in master transmit mode
  * @client: Handle to slave device
@@ -2039,7 +2166,17 @@
 {
 	struct i2c_adapter *adapter;
 
-	mutex_lock(&core_lock);
+//	mutex_lock(&core_lock);
+	if (in_atomic() || irqs_disabled())
+	{
+		int ret;
+		ret = mutex_trylock(&core_lock);
+		if (!ret)
+			return NULL;
+	}
+	else
+		mutex_lock(&core_lock);
+
 	adapter = idr_find(&i2c_adapter_idr, nr);
 	if (adapter && !try_module_get(adapter->owner))
 		adapter = NULL;
@@ -2082,7 +2219,7 @@
 }
 
 /* Assume a 7-bit address, which is reasonable for SMBus */
-static u8 i2c_smbus_msg_pec(u8 pec, struct i2c_msg *msg)
+u8 i2c_smbus_msg_pec(u8 pec, struct i2c_msg *msg)
 {
 	/* The address will be sent first */
 	u8 addr = (msg->addr << 1) | !!(msg->flags & I2C_M_RD);
@@ -2093,7 +2230,7 @@
 }
 
 /* Used for write only transactions */
-static inline void i2c_smbus_add_pec(struct i2c_msg *msg)
+void i2c_smbus_add_pec(struct i2c_msg *msg)
 {
 	msg->buf[msg->len] = i2c_smbus_msg_pec(0, msg);
 	msg->len++;
@@ -2104,7 +2241,7 @@
    partial CRC from the write part into account.
    Note that this function does modify the message (we need to decrease the
    message length to hide the CRC byte from the caller). */
-static int i2c_smbus_check_pec(u8 cpec, struct i2c_msg *msg)
+int i2c_smbus_check_pec(u8 cpec, struct i2c_msg *msg)
 {
 	u8 rpec = msg->buf[--msg->len];
 	cpec = i2c_smbus_msg_pec(cpec, msg);
@@ -2551,7 +2688,13 @@
 	return i2c_smbus_xfer_emulated(adapter, addr, flags, read_write,
 				       command, protocol, data);
 }
+EXPORT_SYMBOL(i2c_smbus_msg_pec);
+EXPORT_SYMBOL(i2c_smbus_add_pec);
+EXPORT_SYMBOL(i2c_smbus_check_pec);
 EXPORT_SYMBOL(i2c_smbus_xfer);
+#ifdef I2C_BUS_RECOVERY
+EXPORT_SYMBOL(i2c_recovery_event);
+#endif
 
 MODULE_AUTHOR("Simon G. Vogl <simon@tk.uni-linz.ac.at>");
 MODULE_DESCRIPTION("I2C-Bus main module");
