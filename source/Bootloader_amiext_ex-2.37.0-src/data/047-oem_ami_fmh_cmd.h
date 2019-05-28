--- u-boot-2013.07/oem/ami/fmh/cmd.h	2015-05-21 19:12:37.563579771 -0400
+++ uboot/oem/ami/fmh/cmd.h	2015-05-21 12:35:38.738830540 -0400
@@ -0,0 +1,17 @@
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
+extern int do_ext4_load(cmd_tbl_t *cmdtp, int flag, int argc,char *const argv[]);
+
+#endif
