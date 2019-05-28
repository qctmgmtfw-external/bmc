--- linux.org/arch/arm/mach-astevb/include/mach/debug-macro.S	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/debug-macro.S	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,41 @@
+/* linux/include/asm-arm/arch-xxx/debug-macro.S
+ *
+ * Debugging macro include header
+ *
+ * Copyright (C) 1994-1999 Russell King
+ *
+ * Ported for ASPEED SOC (Base code taken from arch-integrator)
+ * Copyright (C) 2005 American Megatrends Inc.
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation.
+ *
+*/
+
+#include <mach/lowlevel_serial.h>
+
+mmu_enabled:
+		.word	io_mapping_done
+
+		.macro	addruart, rp, rv, temp
+		ldr	\rp, =UART_BASE		@ physical address
+		ldr	\rv, =UART_MMU_BASE	@ virtual address
+		.endm
+
+		.macro	senduart,rd,rx
+		strb	\rd, [\rx, #SERIAL_THR]
+		.endm
+
+		.macro	waituart,rd,rx
+1001:	ldr		\rd, [\rx, #SERIAL_LSR]
+		tst		\rd, #1 << 5			@ UART_LSR_THRE
+		beq		1001b
+		.endm
+
+/* I dont know what busyuart should do. So I used the same as waituart */
+		.macro	busyuart,rd,rx
+1001:	ldr		\rd, [\rx, #SERIAL_LSR]
+		tst		\rd, #1 << 5			@ UART_LSR_THRE
+		beq		1001b
+		.endm
