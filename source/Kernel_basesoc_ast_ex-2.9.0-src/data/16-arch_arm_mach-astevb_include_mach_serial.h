--- linux-3.14.17/arch/arm/mach-astevb/include/mach/serial.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-3.14.17-ami/arch/arm/mach-astevb/include/mach/serial.h	2014-12-17 18:20:07.778919925 +0800
@@ -0,0 +1,54 @@
+/*
+ *  linux/include/asm-arm/arch-xxx/serial.h
+ *
+ *  This file contains the ASPEED SOC Serial port configuration
+ *  This serial port is similiar to UART 8250
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
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
+
+
+#include <linux/serial_core.h>
+#include <mach/hardware.h>
+
+#define BASE_BAUD	((CONFIG_UART_CLK)/16)
+
+#define STD_COM_FLAGS (UPF_BOOT_AUTOCONF | UPF_SKIP_TEST)
+
+#if !defined(CONFIG_SOC_AST2530)
+#define SERIAL_PORT_DFNS    \
+ {0, BASE_BAUD, AST_UART1_BASE, IRQ_UARTINT0, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART1_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART2_BASE, IRQ_UARTINT1, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART2_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART3_BASE, IRQ_UARTINT2, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART3_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART4_BASE, IRQ_UARTINT3, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART4_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART5_BASE, IRQ_UARTINT4, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART5_VA_BASE, 2},
+#else
+#define SERIAL_PORT_DFNS    \
+ {0, BASE_BAUD, AST_UART1_BASE,  IRQ_UARTINT0,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART1_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART2_BASE,  IRQ_UARTINT1,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART2_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART3_BASE,  IRQ_UARTINT2,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART3_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART4_BASE,  IRQ_UARTINT3,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART4_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART5_BASE,  IRQ_UARTINT4,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART5_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART6_BASE,  IRQ_UARTINT5,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART6_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART7_BASE,  IRQ_UARTINT6,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART7_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART8_BASE,  IRQ_UARTINT7,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART8_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART9_BASE,  IRQ_UARTINT8,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART9_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART10_BASE, IRQ_UARTINT9,  STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART10_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART11_BASE, IRQ_UARTINT10, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART11_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART12_BASE, IRQ_UARTINT11, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART12_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART13_BASE, IRQ_UARTINT12, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART13_VA_BASE, 2},
+#endif
