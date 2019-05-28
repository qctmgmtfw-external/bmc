--- linux/arch/arm/mach-astevb/include/mach/system.h	1969-12-31 19:00:00.000000000 -0500
+++ linux_ast/arch/arm/mach-astevb/include/mach/system.h	2014-07-31 11:17:04.351079138 -0400
@@ -0,0 +1,84 @@
+/*
+ *  linux/include/asm-arm/arch-xxx/system.h
+ *
+ *  Copyright (C) 1999 ARM Limited
+ *  Copyright (C) 2000 Deep Blue Solutions Ltd
+ *	Copyright (C) 2005 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+#ifndef __ASM_ARCH_SYSTEM_H
+#define __ASM_ARCH_SYSTEM_H
+
+#include <mach/platform.h>
+
+/* Some Debug Functions */
+void CheckPrintk(char *);
+
+static inline void arch_idle(void)
+{
+	/*
+	 * This should do all the clock switching
+	 * and wait for interrupt tricks
+	 */
+	cpu_do_idle();
+}
+
+static inline void arch_reset(char mode)
+{
+#if !defined(CONFIG_SOC_AST3100) && !defined(CONFIG_SOC_AST3200)
+    /* 
+     * Fix Linux Host OS hang. 
+     * BMC reset will clear LADR3H/LADR3L/LADR12H/LADR12L. 
+     * If BMC reset coincidentally launches at the 2nd LPC clock of a 
+     * decoded KCS/BT turn-around cycle, AST2300/2400 LPC controller will enter a wait state.
+     * And the wait state is not initialized by the BMC reset. It causes Linux Host OS hang
+     */
+ 	unsigned long reg;
+
+    reg = *((volatile unsigned long*)(AST_LPC_VA_BASE + 0x150));
+    if (!(reg & 0x00000080))
+    {
+        reg |= 0x00000080;
+    }
+    *((volatile unsigned long *)(AST_LPC_VA_BASE + 0x150)) = reg;
+
+    reg = *((volatile unsigned long *)(AST_LPC_VA_BASE + 0x140));
+    reg &= ~(0x00000003);
+    *((volatile unsigned long *)(AST_LPC_VA_BASE + 0x140)) = reg;
+
+    reg = *((volatile unsigned long*) AST_LPC_VA_BASE); /* HICR0 = Disable LPC channel #1 #2 #3 */ 
+    reg &= 0xffffff1f; /* HICR0 = Disable LPC channel #1 #2 #3 */
+    *((volatile unsigned long*) AST_LPC_VA_BASE) = reg;
+    reg = *((volatile unsigned long*) AST_LPC_VA_BASE + 0x10); /* HICR4 = Disable KCS interface and BT interface in channel #3*/
+    reg &= 0xfffffffa; /* HICR4 = Disable Enable KCS interface and BT interface in channel #3*/
+    *((volatile unsigned long*) AST_LPC_VA_BASE + 0x10) = reg;
+#endif
+
+	/*
+	 * Use WDT to restart system
+	 */
+#ifdef CONFIG_SPX_FEATURE_SELECT_WDT2
+    *(volatile unsigned long *) (WDT_RELOAD_REG) = 0x10;
+    *(volatile unsigned long *) (WDT_CNT_RESTART_REG) = 0x4755;
+    *(volatile unsigned long *) (WDT_CONTROL_REG) = 0x03;
+#else
+    *(volatile unsigned long *) (WDT2_RELOAD_REG) = 0x10;
+    *(volatile unsigned long *) (WDT2_CNT_RESTART_REG) = 0x4755;
+    *(volatile unsigned long *) (WDT2_CONTROL_REG) = 0x03;
+#endif
+}
+
+#endif
