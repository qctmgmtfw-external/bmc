--- u-boot-2013.07/arch/arm/cpu/astcommon/reset.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/reset.c	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,13 @@
+/* AST2X00 WDT Routines for u-boot reboot */
+
+#include <common.h>
+#include <linux/types.h>
+#include "soc_hw.h"
+
+void reset_cpu(ulong addr)
+{
+	*(unsigned long *)(WDT2_RELOAD_REG) = 0x10;
+   *(unsigned long *)(WDT2_CNT_RESTART_REG) = 0x4755;
+	*(unsigned long *)(WDT2_CONTROL_REG) = WDT_TIMER_EN | WDT_RESET_EN;
+	while (1);
+}
