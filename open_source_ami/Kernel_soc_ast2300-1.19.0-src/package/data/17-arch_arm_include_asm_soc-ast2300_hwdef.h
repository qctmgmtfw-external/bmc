--- linux-2.6.28.10-pristine/arch/arm/include/asm/soc-ast2300/hwdef.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/include/asm/soc-ast2300/hwdef.h	2011-05-25 14:59:15.828790840 +0800
@@ -0,0 +1,155 @@
+/*
+ *  This file contains the AST2300 SOC Register definitions
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
+#ifndef     _HWDEF_AST2300_H_
+#define     _HWDEF_AST2300_H_
+
+
+/* -----------------------------------------------------------------
+ *                  SRAM Registers
+ * -----------------------------------------------------------------
+ */
+
+
+/*
+ * -------------------------------------------------------------
+ *                     SDRAM Registers
+ * -------------------------------------------------------------
+ */
+
+/*--------------------------------------------------------
+ *  Timer definitions
+ * ------------------------------------------------------
+ */
+/* Timer conrol reg */
+#define TIMER3_INTR_ENABLE         0x400
+#define TIMER3_CLOCK_SELECT        0x200
+#define TIMER3_ENABLE          0x100
+#define TIMER2_INTR_ENABLE         0x040
+#define TIMER2_CLOCK_SELECT        0x020
+#define TIMER2_ENABLE          0x010
+#define TIMER1_INTR_ENABLE         0x004
+#define TIMER1_CLOCK_SELECT        0x002
+#define TIMER1_ENABLE          0x001
+
+/* --------------------------------------------------------
+ *  Interrupt Controllers
+ * --------------------------------------------------------
+ */
+#define IRQ_VIC2           IRQ_GPIO
+#define IRQMASK_VIC2       IRQMASK_GPIO
+
+/*
+ * VIC Register (VA)
+ */
+#define VIC_IRQ_ENABLE_VA              IRQ_ENABLE_REG
+#define VIC_IRQ_DISABLE_VA             IRQ_DISABLE_REG
+#define VIC_SOFT_INT_CLEAR_VA          SOFT_INT_CLEAR_REG
+
+#define VIC_SENSE_VA                    INTERRUPT_SENS_REG
+#define VIC_EVENT_VA                    INTERRUPT_EVENT_REG
+#define VIC_EDGE_CLEAR_VA               EDGE_TRIGGERED_INTCLEAR_REG
+
+#define EXT_VIC_SENSE_VA                EXT_INTERRUPT_SENS_REG
+#define EXT_VIC_EVENT_VA                EXT_INTERRUPT_EVENT_REG
+#define EXT_VIC_EDGE_CLEAR_VA           EXT_EDGE_TRIGGERED_INTCLEAR_REG
+
+
+#define IRQ_ENABLE(irq_no)             *((volatile unsigned long*)VIC_IRQ_ENABLE_VA)    |= 1 << (irq_no)
+#define IRQ_DISABLE(irq_no)                *((volatile unsigned long*)VIC_IRQ_DISABLE_VA)   |= 1 << (irq_no)
+#define IRQ_SOFT_INT_CLEAR(irq_no)     *((volatile unsigned long*)VIC_SOFT_INT_CLEAR_VA)|= 1 << (irq_no)
+
+#define IRQ_SET_LEVEL_TRIGGER(irq_no)   *((volatile unsigned long*)VIC_SENSE_VA) |= 1 << (irq_no)
+#define IRQ_SET_EDGE_TRIGGER(irq_no)    *((volatile unsigned long*)VIC_SENSE_VA) &= ~(1 << (irq_no))
+#define IRQ_SET_RISING_EDGE(irq_no)     *((volatile unsigned long*)VIC_EVENT_VA) |= 1 << (irq_no)
+#define IRQ_SET_FALLING_EDGE(irq_no)    *((volatile unsigned long*)VIC_EVENT_VA) &= ~(1 << (irq_no))
+#define IRQ_SET_HIGH_LEVEL(irq_no)      *((volatile unsigned long*)VIC_EVENT_VA) |= 1 << (irq_no)
+#define IRQ_SET_LOW_LEVEL(irq_no)       *((volatile unsigned long*)VIC_EVENT_VA) &= ~(1 << (irq_no))
+#define IRQ_EDGE_CLEAR(irq_no)          *((volatile unsigned long*)VIC_EDGE_CLEAR_VA) |= 1 << (irq_no)
+
+#define EXT_IRQ_SET_LEVEL_TRIGGER(irq_no)   *((volatile unsigned long*)EXT_VIC_SENSE_VA) |= 1 << (irq_no - BW_CMP_NR_IRQS)
+#define EXT_IRQ_SET_EDGE_TRIGGER(irq_no)    *((volatile unsigned long*)EXT_VIC_SENSE_VA) &= ~(1 << (irq_no - BW_CMP_NR_IRQS))
+#define EXT_IRQ_SET_RISING_EDGE(irq_no)     *((volatile unsigned long*)EXT_VIC_EVENT_VA) |= 1 << (irq_no - BW_CMP_NR_IRQS)
+#define EXT_IRQ_SET_FALLING_EDGE(irq_no)    *((volatile unsigned long*)EXT_VIC_EVENT_VA) &= ~(1 << (irq_no - BW_CMP_NR_IRQS))
+#define EXT_IRQ_SET_HIGH_LEVEL(irq_no)      *((volatile unsigned long*)EXT_VIC_EVENT_VA) |= 1 << (irq_no - BW_CMP_NR_IRQS)
+#define EXT_IRQ_SET_LOW_LEVEL(irq_no)       *((volatile unsigned long*)EXT_VIC_EVENT_VA) &= ~(1 << (irq_no - BW_CMP_NR_IRQS))
+#define EXT_IRQ_EDGE_CLEAR(irq_no)          *((volatile unsigned long*)EXT_VIC_EDGE_CLEAR_VA) |= 1 << (irq_no - BW_CMP_NR_IRQS)
+
+#define VIC2_SENSE_VA                   VIC2_SENSE
+#define VIC2_EVENT_VA                   VIC2_EVENT
+#define VIC2_EDGE_CLEAR_VA              VIC2_EDGE_CLEAR
+
+#define VIC2_IRQ_SET_LEVEL_TRIGGER(irq_no)     *((volatile unsigned long*)VIC2_SENSE_VA) |= 1 << (irq_no - VIC2_IRQ_SHIFT)
+#define VIC2_IRQ_SET_EDGE_TRIGGER(irq_no)      *((volatile unsigned long*)VIC2_SENSE_VA) &= ~(1 << (irq_no - VIC2_IRQ_SHIFT))
+#define VIC2_IRQ_SET_RISING_EDGE(irq_no)       *((volatile unsigned long*)VIC2_EVENT_VA) |= 1 << (irq_no - VIC2_IRQ_SHIFT)
+#define VIC2_IRQ_SET_FALLING_EDGE(irq_no)      *((volatile unsigned long*)VIC2_EVENT_VA) &= ~(1 << (irq_no - VIC2_IRQ_SHIFT))
+#define VIC2_IRQ_SET_HIGH_LEVEL(irq_no)        *((volatile unsigned long*)VIC2_EVENT_VA) |= 1 << (irq_no - VIC2_IRQ_SHIFT)
+#define VIC2_IRQ_SET_LOW_LEVEL(irq_no)         *((volatile unsigned long*)VIC2_EVENT_VA) &= ~(1 << (irq_no - VIC2_IRQ_SHIFT))
+#define VIC2_IRQ_EDGE_CLEAR(irq_no)            *((volatile unsigned long*)VIC_EDGE_CLEAR_VA) |= 1 << (irq_no - VIC2_IRQ_SHIFT)
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
+#define AST_UART_CLOCK      UART_CLOCK/13 /* defined in board config */
+#define AST_BAUD_115200        (AST_UART_CLOCK /1843200)
+#define AST_BAUD_57600     (AST_UART_CLOCK /921600)
+#define AST_BAUD_38400     (AST_UART_CLOCK /614400)
+#define AST_BAUD_19200     (AST_UART_CLOCK /307200)
+#define AST_BAUD_14400     (AST_UART_CLOCK /230400)
+#define AST_BAUD_9600      (AST_UART_CLOCK /153600)
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
+#define WDT_TIMER_EN       0x01
+#define WDT_RESET_EN       0x02
+#endif
