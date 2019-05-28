--- linux.pristine/kernel/kallsyms.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/kernel/kallsyms.c	2014-02-03 15:11:00.007029096 -0500
@@ -21,6 +21,9 @@
 #include <linux/proc_fs.h>
 #include <linux/sched.h>	/* for cond_resched */
 #include <linux/mm.h>
+#ifdef CONFIG_BIGPHYS_AREA
+#include <linux/bigphysarea.h>
+#endif
 #include <linux/ctype.h>
 #include <linux/slab.h>
 
@@ -612,3 +615,11 @@
 	return 0;
 }
 device_initcall(kallsyms_init);
+
+#ifdef CONFIG_BIGPHYS_AREA
+EXPORT_SYMBOL(bigphysarea_alloc);
+EXPORT_SYMBOL(bigphysarea_free);
+EXPORT_SYMBOL(bigphysarea_alloc_pages);
+EXPORT_SYMBOL(bigphysarea_free_pages);
+EXPORT_SYMBOL(bigphysarea_get_max_size);
+#endif
