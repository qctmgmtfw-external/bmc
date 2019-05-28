--- linux/kernel/reboot.c	2014-08-14 09:38:34.000000000 +0800
+++ linux/kernel/reboot_new.c	2017-11-10 13:56:45.768868868 +0800
@@ -132,6 +132,20 @@
  */
 void kernel_restart(char *cmd)
 {
+	// Quanta ---
+	struct file   *filp;
+	mm_segment_t  oldfs;
+	char          buf[] = {'5', '\n', 0};
+
+	filp = filp_open("/conf/system_reboot", O_RDWR|O_CREAT, 0644);
+	if ((filp != NULL) && !IS_ERR(filp)) {
+		oldfs = get_fs();
+		set_fs(KERNEL_DS);
+		filp->f_op->write(filp, buf, sizeof(buf), &filp->f_pos);
+		set_fs(oldfs);
+		filp_close(filp, NULL);
+	}
+	// --- Quanta
 	kernel_restart_prepare(cmd);
 	migrate_to_reboot_cpu();
 	syscore_shutdown();
