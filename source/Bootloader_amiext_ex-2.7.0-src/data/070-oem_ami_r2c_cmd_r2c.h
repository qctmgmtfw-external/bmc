--- u-boot-2013.07/oem/ami/r2c/cmd_r2c.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/r2c/cmd_r2c.h	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,17 @@
+#ifndef __AMI_CMD_R2C_H__
+#define __AMI_CMD_R2C_H__
+
+/* U-boot's cmd function enter remote recovery console session */
+extern int  do_r2c (cmd_tbl_t *, int, int, char * const []);
+
+U_BOOT_CMD(				
+	r2c,	1,	0,	do_r2c,				
+	"r2c     - Enter AMI's Remote Recovery Console Session\n",
+	"    - Enter AMI's Remote Recovery Console Session\n"
+);
+
+
+/* Actual function implementing r2c protocol*/
+extern int  rconsole(void);	
+
+#endif	
