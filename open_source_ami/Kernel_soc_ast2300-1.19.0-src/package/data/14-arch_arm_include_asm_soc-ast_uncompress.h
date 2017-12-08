--- linux-2.6.28.10/arch/arm/include/asm/soc-ast/uncompress.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10-ami/arch/arm/include/asm/soc-ast/uncompress.h	2010-05-10 16:31:48.000000000 +0800
@@ -0,0 +1,57 @@
+/*
+ * linux/include/asm-arm/arch-xxx/uncompress.h
+ *
+ * Copyright (C) 2005 American Megatrends Inc.
+ */
+
+#include <mach/hardware.h>
+#include <mach/lowlevel_serial.h>
+#include <linux/serial_reg.h>
+
+#define flush()  do { } while(0)
+
+static void putc(const char c)
+{
+	volatile unsigned int status;
+
+	/* Wait for Ready */
+	do {
+    	status = *((volatile unsigned char *) (UART_BASE + SERIAL_LSR));
+    } while (!(status & UART_LSR_THRE));
+
+	/* Write Character */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_THR)) = c;
+}
+
+#if 0
+static void putstr(const char *s)
+{
+	while (*s) {
+		putc(*s);
+		if (*s == '\n')
+			putc('\r');
+		s++;
+	}
+}
+#endif
+
+static __inline__ void arch_decomp_setup(void)
+{
+	/* Enable DLAB, 8 Bits and One Stop Bit */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_LCR)) = 0x83;
+
+	/* Set the Baud Rate */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_DLM)) = ((CONFIG_UART_CLK / (13 * BAUD_RATE * 16)) & 0xFF00) >> 8;
+	*((volatile unsigned char *) (UART_BASE + SERIAL_DLL)) = ((CONFIG_UART_CLK / (13 * BAUD_RATE * 16)) & 0x00FF);
+
+	/* Disable  DLAB, 8 Bits and 1 Stop Bit */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_LCR)) = 0x03;
+
+	/* Enable FIFO */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_FCR)) = 0xC1;
+}
+
+/*
+ * nothing to do
+ */
+#define arch_decomp_wdog()
