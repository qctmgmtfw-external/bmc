--- u-boot-2013.07/oem/ami/r2c/cmd_r2c.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/r2c/cmd_r2c.c	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,32 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <net.h>
+# include "cmd_r2c.h"
+# include "externs.h"
+
+/* U-boot's cmd function enter remote recovery console session */
+int  
+do_r2c (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	return rconsole();
+}
+
+
+/* Actual function implementing r2c protocol*/
+/* Never Returns. Can reset card either using r2c 
+ * reset command or ctrl-c on the terminal console*/
+int  
+rconsole(void)
+{
+#ifdef CONFIG_CMD_NET
+	while (1)
+	{
+		printf("Entering AMI r2c console\n");
+		CallFromNetLoop = 1;
+		NetLoop(R2C);
+	}
+#endif
+	return 0;
+}	
+
