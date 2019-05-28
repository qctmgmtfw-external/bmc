--- u-boot-2013.07/oem/ami/fmh/cmd.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/oem/ami/fmh/cmd.h	2013-12-05 12:17:54.975503948 -0500
@@ -0,0 +1,16 @@
+
+/*
+ * Copyright 2005 Sun Microsystems, Inc.  All rights reserved.
+ * Use is subject to license terms.
+ */
+
+#ifndef __CMD_H__
+#define __CMD_H__
+
+extern int do_bootm (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern int do_bootelf (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern int do_go (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern int do_jffs2_chpart (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+extern int do_jffs2_fsload (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
+
+#endif
