--- .pristine/i2c_hw-1.12.0-ARM-AST-src/data/i2c-transfer.c Fri Oct 26 19:51:50 2012
+++ source/i2c_hw-1.12.0-ARM-AST-src/data/i2c-transfer.c Fri Oct 26 19:57:36 2012
@@ -307,7 +307,10 @@
 	
 		
 	retval = as_send_bytes(i2c_adap, msgs[0],num-2);
-
+	if(retval != 0)
+	{
+		return -EIO;
+	}
 
 	
 	retval = send_slave_addr(i2c_adap, msgs[1].addr, msgs[1].flags);
