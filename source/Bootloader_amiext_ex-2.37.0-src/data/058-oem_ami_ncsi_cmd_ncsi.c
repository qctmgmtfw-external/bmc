--- u-boot-2013.07/oem/ami/ncsi/cmd_ncsi.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/ncsi/cmd_ncsi.c	2013-12-05 12:17:54.975503948 -0500
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
+do_ncsi (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	NetLoop(NCSI);
+	return 0;
+}
+#endif
