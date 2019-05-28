--- linux.org/arch/arm/mach-astevb/include/mach/entry-macro.S	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/entry-macro.S	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,79 @@
+/* include/asm-arm/arch-xxx/entry-macro.S
+ *
+ * Low-level IRQ helper macros for Aspeed SOC  platforms
+ *
+ * This file is licensed under  the terms of the GNU General Public
+ * License version 2. This program is licensed "as is" without any
+ * warranty of any kind, whether express or implied.
+ */
+
+#include <mach/platform.h>
+
+		.macro	disable_fiq
+		.endm
+
+		.macro	get_irqnr_preamble, base, tmp
+		ldr	\base, =IO_ADDRESS(AST_IC_VA_BASE)
+		.endm
+
+		.macro	arch_ret_to_user, tmp1, tmp2
+		.endm
+
+		.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
+		ldr	\tmp, =IO_ADDRESS(AST_SCU_VA_BASE)
+		ldr	\irqnr, [\tmp, #0x44]
+		cmp	\irqnr, #0
+		beq	2000f
+		
+1000: /* pass1 */
+		cmp	\irqnr, #32
+		ble 1001f
+		ldr	\tmp, =IO_ADDRESS(AST_IC_VA_BASE)
+		ldr	\irqstat, [\tmp, #ASPEED_VIC2_STATUS_OFFSET]
+		sub	\irqnr, \irqnr, #32		
+		mov	\tmp, #32
+		sub	\tmp, \tmp, \irqnr
+		mov \irqstat, \irqstat, lsl \tmp	/* mask uncompare parts */
+		mov \irqstat, \irqstat, lsr \tmp
+		mov \irqnr, #63
+		clz \tmp, \irqstat
+		cmp \tmp, #32
+		bne 3000f
+		mov \irqnr, #32
+1001:
+		ldr \tmp, =IO_ADDRESS(AST_IC_VA_BASE)
+		ldr \irqstat, [\tmp, #ASPEED_VIC_STATUS_OFFSET]
+		mov \tmp, #32
+		sub \tmp, \tmp, \irqnr		
+		mov \irqstat, \irqstat, lsl \tmp	/* mask uncompare parts */
+		mov \irqstat, \irqstat, lsr \tmp
+		mov \irqnr, #31
+		clz \tmp, \irqstat
+		cmp \tmp, #32
+		bne 3000f
+
+2000: /* pass 2 */
+		ldr \tmp, =IO_ADDRESS(AST_IC_VA_BASE)
+		ldr \irqstat, [\tmp, #ASPEED_VIC2_STATUS_OFFSET]
+		mov \irqnr, #63
+		clz \tmp, \irqstat
+		cmp \tmp, #32
+		bne 3000f
+2001:
+		ldr \tmp, =IO_ADDRESS(AST_IC_VA_BASE)
+		ldr \irqstat, [\tmp, #ASPEED_VIC_STATUS_OFFSET]
+		mov \irqnr, #31
+		clz \tmp, \irqstat
+		cmp \tmp, #32
+		beq 4000f /* not find */
+
+3000: /* find */
+		sub \irqnr, \irqnr, \tmp
+		ldr \tmp, =IO_ADDRESS(AST_SCU_VA_BASE)
+		str \irqnr, [\tmp, #0x44]
+		cmp \irqnr, #64
+4000: /* done */
+		.endm
+
+		.macro	irq_prio_table
+		.endm
