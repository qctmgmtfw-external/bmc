--- u-boot_orig/include/ast/hwdef.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot_mod/include/ast/hwdef.h	2015-12-23 19:39:33.684578952 -0500
@@ -0,0 +1,122 @@
+/*
+ *  This file contains the AST SOC Register definitions
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
+#ifndef     _HWDEF_AST_H_
+#define     _HWDEF_AST_H_
+
+
+/* -----------------------------------------------------------------
+ *      	         SRAM Registers
+ * -----------------------------------------------------------------
+ */
+
+
+/*
+ * -------------------------------------------------------------
+ *  					SDRAM Registers
+ * -------------------------------------------------------------
+ */
+
+/*--------------------------------------------------------
+ *  Timer definitions
+ * ------------------------------------------------------
+ */
+/* Timer conrol reg */
+#define TIMER3_INTR_ENABLE 		0x400
+#define TIMER3_CLOCK_SELECT		0x200
+#define TIMER3_ENABLE		    0x100
+#define TIMER2_INTR_ENABLE 		0x040
+#define TIMER2_CLOCK_SELECT		0x020
+#define TIMER2_ENABLE		    0x010
+#define TIMER1_INTR_ENABLE 		0x004
+#define TIMER1_CLOCK_SELECT		0x002
+#define TIMER1_ENABLE		    0x001
+
+/* --------------------------------------------------------
+ *  Interrupt Controllers
+ * --------------------------------------------------------
+ */
+/* Source control reg */
+#define INTR_LOW_LEVEL	0x00
+#define INTR_HIGH_LEVEL	0x40
+#define INTR_NEG_EDGE	0x80
+#define INRR_POS_EDGE	0xC0
+
+
+/*  --------------------------------------------------------------
+ *   I2S Controllers
+ *  --------------------------------------------------------------
+ */
+
+
+/*  --------------------------------------------------------------
+ *   SSP Controllers
+ *  --------------------------------------------------------------
+ */
+
+
+/*  --------------------------------------------------------------
+ *   I2C Controllers
+ *  --------------------------------------------------------------
+ */
+
+
+/*----------------------------------------------------------------
+ *   DMA Controllers
+ *----------------------------------------------------------------
+ */
+
+
+/*----------------------------------------------------------------
+ *   Serial  Controllers
+ *----------------------------------------------------------------
+ */
+#if 0
+#define AST_UART_CLOCK      UART_CLOCK/13	/* defined in board config */
+#define AST_BAUD_115200	    (AST_UART_CLOCK /1843200)
+#define AST_BAUD_57600	    (AST_UART_CLOCK /921600)
+#define AST_BAUD_38400	    (AST_UART_CLOCK /614400)
+#define AST_BAUD_19200	    (AST_UART_CLOCK /307200)
+#define AST_BAUD_14400	    (AST_UART_CLOCK /230400)
+#define AST_BAUD_9600	    (AST_UART_CLOCK /153600)
+#endif
+
+/*----------------------------------------------------------------
+ *   RTC
+ *----------------------------------------------------------------
+ */
+
+
+/*----------------------------------------------------------------
+ *   WDT
+ *----------------------------------------------------------------
+ */
+#define WDT_TIMER_EN		0x01
+#define WDT_RESET_EN		0x02
+
+/*----------------------------------------------------------------
+ *   USB Reference Clock
+ *----------------------------------------------------------------
+ */
+#define AST_PLL_25MHZ                   25000000
+#define AST_PLL_24MHZ                   24000000
+#define AST_PLL_12MHZ                   12000000
+
+#endif
