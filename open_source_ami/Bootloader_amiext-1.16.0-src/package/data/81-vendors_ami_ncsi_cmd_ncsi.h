--- u-boot-1.1.6/vendors/ami/ncsi/cmd_ncsi.h	1970-01-01 08:00:00.000000000 +0800
+++ u-boot-1.1.6-ami/vendors/ami/ncsi/cmd_ncsi.h	2010-12-30 16:15:16.000000000 +0800
@@ -0,0 +1,15 @@
+#ifndef __AMI_CMD_NCSI_H__
+#define __AMI_CMD_NCSI_H__
+
+/* U-boot's cmd function start ncsi discovery */
+extern int  do_ncsi (cmd_tbl_t *, int, int, char *[]);
+
+#ifdef CONFIG_NCSI_SUPPORT
+U_BOOT_CMD(				
+	ncsi,	1,	0,	do_ncsi,				
+	"ncsi     - Configure NCSI Interfaces\n",	
+	"    - Configure NCSI Interfaces\n"		
+);
+#endif
+
+#endif
