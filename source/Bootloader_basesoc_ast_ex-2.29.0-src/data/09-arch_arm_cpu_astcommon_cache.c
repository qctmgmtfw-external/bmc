--- u-boot-2013.07/arch/arm/cpu/astcommon/cache.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/cache.c	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,14 @@
+#include <common.h>
+#include <asm/errno.h>
+#include <linux/compiler.h>
+
+void enable_caches(void)
+{
+#ifndef CONFIG_SYS_ICACHE_OFF
+	icache_enable();
+#endif
+#ifndef CONFIG_SYS_DCACHE_OFF
+	dcache_enable();		
+#endif
+}
+
