--- linux/fs/namespace.c	2015-07-22 16:20:09.683157513 -0400
+++ linux/fs/namespace.c	2014-08-13 21:38:34.000000000 -0400
@@ -2612,9 +2612,9 @@
 		char __user *, type, unsigned long, flags, void __user *, data)
 {
 	int ret;
-	char *kernel_type;
+	char *kernel_type=NULL;
 	struct filename *kernel_dir;
-	char *kernel_dev;
+	char *kernel_dev=NULL;
 	unsigned long data_page;
 
 	ret = copy_mount_string(type, &kernel_type);
