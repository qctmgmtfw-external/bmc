--- linux.org/arch/arm/mach-astevb/include/mach/lowlevel_serial.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/lowlevel_serial.h	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,26 @@
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
+
+#define BAUD_RATE	CONFIG_SPX_FEATURE_BAUDRATE_CONSOLE_TTY
+
+#define SERIAL_THR	0x00
+#define SERIAL_RBR	0x00
+#define SERIAL_DLL	0x00
+#define SERIAL_DLM	0x04
+#define SERIAL_IER	0x04
+#define SERIAL_IIR	0x08
+#define SERIAL_FCR	0x08
+#define SERIAL_LCR	0x0C
+#define SERIAL_LSR	0x14
+#define SERIAL_MSR	0x18
+#define SERIAL_SPR	0x1C
