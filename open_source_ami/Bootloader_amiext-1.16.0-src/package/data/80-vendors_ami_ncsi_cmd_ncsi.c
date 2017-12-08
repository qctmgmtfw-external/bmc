--- u-boot-1.1.6/vendors/ami/ncsi/cmd_ncsi.c	1970-01-01 08:00:00.000000000 +0800
+++ u-boot-1.1.6-ami/vendors/ami/ncsi/cmd_ncsi.c	2010-12-30 16:15:11.000000000 +0800
@@ -0,0 +1,15 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <net.h>
+# include "cmd_ncsi.h"
+
+#ifdef CONFIG_NCSI_SUPPORT
+/* U-boot's cmd function enter ncsi discovery */
+int  
+do_ncsi (cmd_tbl_t *cmdtp, int flag , int argc, char *argv[])
+{
+	NetLoop(NCSI);
+	return 0;
+}
+#endif
