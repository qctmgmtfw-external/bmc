--- linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/serial.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10-ami/arch/arm/include/asm/soc-ast2300/serial.h	2011-05-25 14:59:15.838790608 +0800
@@ -0,0 +1,37 @@
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
+#define BASE_BAUD	((CONFIG_UART_CLK/13)/16)
+
+#define STD_COM_FLAGS (UPF_BOOT_AUTOCONF | UPF_SKIP_TEST)
+
+#define SERIAL_PORT_DFNS    \
+ {0, BASE_BAUD, AST_UART1_BASE, IRQ_UARTINT0, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART1_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART2_BASE, IRQ_UARTINT1, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART2_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART3_BASE, IRQ_UARTINT2, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART3_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART4_BASE, IRQ_UARTINT3, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART4_VA_BASE, 2},   \
+ {0, BASE_BAUD, AST_UART5_BASE, IRQ_UARTINT4, STD_COM_FLAGS, 0, UPIO_MEM32, (void *) AST_UART5_VA_BASE, 2},
