--- u-boot-2013.07/common/cmd_boot.c	2013-12-13 13:49:25.704958498 -0500
+++ uboot.new/common/cmd_boot.c	2013-12-05 12:17:55.227503948 -0500
@@ -38,7 +38,7 @@
 	return entry (argc, argv);
 }
 
-static int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
+int do_go(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
 {
 	ulong	addr, rc;
 	int     rcode = 0;
