--- u-boot-2013.07/arch/arm/cpu/astcommon/ast_fb.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot.new/arch/arm/cpu/astcommon/ast_fb.c	2015-01-16 16:13:39.000000000 +0800
@@ -0,0 +1,44 @@
+#include <common.h>
+
+#ifdef CONFIG_BOOTLOGO_SUPPORT
+#include <malloc.h>
+#include <miiphy.h>
+#include <net.h>
+
+#include "soc_hw.h"
+
+unsigned int enable_framebuffer(void)
+{
+
+        /* System Control Unit (SCU) */
+        *(unsigned long *)(SCU_KEY_CONTROL_REG) = 0x1688A8A8;
+        *(unsigned long *)(SCU_CLK_STOP_REG) &= ~(0x00000402);
+        udelay(10000);
+
+        *(unsigned long *)(SCU_MISC_CONTROL_REG) &= ~(0x00070098);
+        *(unsigned long *)(SCU_MISC_CONTROL_REG) |= 0x00050080;
+        *(unsigned long *)(SCU_CLK_STOP_REG) &= ~(0x00000400);
+        udelay(10000);
+
+        *(unsigned long *)(SCU_SYS_RESET_REG) &= ~(0x00002080);
+        udelay(10000);
+
+        *(unsigned long *)(SCU_KEY_CONTROL_REG) = 0x00000000;
+
+        /* Graphics Display Controller - (800x600, 32bpp, 60Hz) */
+        *(unsigned long *)(0x1E6E6000 + 0x6C) = 0x00046C50;     // CRT Video PLL Setting
+        *(unsigned long *)(0x1E6E6000 + 0x70) = 0x031F041F;     // CRT Horizontal Total & Display Enable End
+        *(unsigned long *)(0x1E6E6000 + 0x74) = 0x03C70347;     // CRT Horizontal Retrace Start & End 
+        *(unsigned long *)(0x1E6E6000 + 0x78) = 0x02570273;     // CRT Vertical Total & Display Enable End
+        *(unsigned long *)(0x1E6E6000 + 0x7C) = 0x025C0258;     // CRT Vertical Retrace Start & End
+        *(unsigned long *)(0x1E6E6000 + 0x84) = 0x01900C80;     // CRT Display Offset & Terminal Count
+        *(unsigned long *)(0x1E6E6000 + 0x80) = CONFIG_DISPLAY_ADDRESS; // CRT Display Starting Address
+        *(unsigned long *)(0x1E6E6000 + 0x88) = 0x00001E12;     // CRT Threshold
+        *(unsigned long *)(0x1E6E6000 + 0x64) = 0x00000081;     // CRT Control-2
+        *(unsigned long *)(0x1E6E6000 + 0x60) = 0x00000101;     // CRT Control-1
+        udelay(10000);
+
+	return CONFIG_DISPLAY_ADDRESS;
+}
+
+#endif /* CONFIG_BOOTLOGO_SUPPORT */
