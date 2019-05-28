--- linux.pristine/init/do_mounts.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/init/do_mounts.c	2014-02-03 15:11:17.155028666 -0500
@@ -511,7 +511,13 @@
 			return;
 
 		printk(KERN_ERR "VFS: Unable to mount root fs via NFS, trying floppy.\n");
+
+// Patch by AMI for CRAMFS Ramdisk
+#ifdef CONFIG_BLK_DEV_FD
 		ROOT_DEV = Root_FD0;
+#else
+		ROOT_DEV = Root_RAM0;
+#endif
 	}
 #endif
 #ifdef CONFIG_BLK_DEV_FD
