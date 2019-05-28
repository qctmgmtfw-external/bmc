--- linux.org/arch/arm/mach-astevb/include/mach/uncompress.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/mach-astevb/include/mach/uncompress.h	2014-01-31 15:42:23.587921284 -0500
@@ -0,0 +1,63 @@
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
+
+	/* Set the proper clock Divisor - Clear bit 12 */
+	*((volatile unsigned long *) (0x1e6e2000)) = 0x1688A8A8;
+	*((volatile unsigned long *) (0x1e6e202c)) &= 0xFFFFEFFF;
+	*((volatile unsigned long *) (0x1e6e2000)) = 0;
+
+	/* Enable DLAB, 8 Bits and One Stop Bit */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_LCR)) = 0x83;
+
+	/* Set the Baud Rate */
+	*((volatile unsigned char *) (UART_BASE + SERIAL_DLM)) = ((CONFIG_UART_CLK / (BAUD_RATE * 16)) & 0xFF00) >> 8;
+	*((volatile unsigned char *) (UART_BASE + SERIAL_DLL)) = ((CONFIG_UART_CLK / (BAUD_RATE * 16)) & 0x00FF);
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
