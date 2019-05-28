--- u-boot-2013.07/oem/ami/ncsi/cmd_ncsi.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/ncsi/cmd_ncsi.h	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,15 @@
+#ifndef __AMI_CMD_NCSI_H__
+#define __AMI_CMD_NCSI_H__
+
+/* U-boot's cmd function start ncsi discovery */
+extern int  do_ncsi (cmd_tbl_t *, int, int, char * const []);
+
+#ifdef CONFIG_NCSI_SUPPORT
+U_BOOT_CMD(				
+	ncsi,	1,	0,	do_ncsi,				
+	"Configure NCSI Interfaces",
+	""	
+);
+#endif
+
+#endif
