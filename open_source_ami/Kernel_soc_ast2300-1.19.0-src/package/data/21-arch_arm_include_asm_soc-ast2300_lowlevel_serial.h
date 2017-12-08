--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast2300/lowlevel_serial.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/lowlevel_serial.h	2011-05-25 14:59:15.836790734 +0800
@@ -0,0 +1,25 @@
+/*
+ * linux/include/asm-arm/arch-xxx/lowlevel_serial.h
+ *
+ * Copyright (C) 2005 American Megatrends Inc.
+ */
+
+/* Used during bootup till the serial driver is initialized */
+/* Included in debug_macro.S and uncompress.h */
+#include  "mach/platform.h"
+
+#define UART_BASE	AST_UART5_BASE /* before MMU */
+#define UART_MMU_BASE	AST_UART5_VA_BASE /* after MMU */
+#define BAUD_RATE	38400
+
+#define SERIAL_THR	0x00
+#define SERIAL_RBR	0x00
+#define SERIAL_DLL	0x00
+#define SERIAL_DLM	0x04
+#define SERIAL_IER	0x04
+#define SERIAL_IIR  0x08
+#define SERIAL_FCR  0x08
+#define SERIAL_LCR  0x0C
+#define SERIAL_LSR  0x14
+#define SERIAL_MSR	0x18
+#define SERIAL_SPR	0x1C
