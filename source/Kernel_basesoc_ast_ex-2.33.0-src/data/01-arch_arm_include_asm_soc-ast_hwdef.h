--- linux-3.14.17/arch/arm/include/asm/soc-ast/hwdef.h	1970-01-01 08:00:00.000000000 +0800
+++ linux-3.14.17-ami/arch/arm/include/asm/soc-ast/hwdef.h	2014-12-18 10:22:04.211017956 +0800
@@ -0,0 +1,198 @@
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
+#ifndef     _HWDEF_ASTCOMMON_H
+#define     _HWDEF_ASTCOMMON_H_
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
+
+/*--------------------------------------------------------
+ *  Timer definitions
+ * ------------------------------------------------------
+ */
+/* Timer conrol reg */
+#define TIMER3_INTR_ENABLE		0x400
+#define TIMER3_CLOCK_SELECT		0x200
+#define TIMER3_ENABLE			0x100
+#define TIMER2_INTR_ENABLE		0x040
+#define TIMER2_CLOCK_SELECT		0x020
+#define TIMER2_ENABLE			0x010
+#define TIMER1_INTR_ENABLE		0x004
+#define TIMER1_CLOCK_SELECT		0x002
+#define TIMER1_ENABLE			0x001
+
+
+/* --------------------------------------------------------
+ *  Interrupt Controllers
+ * --------------------------------------------------------
+ */
+#define IRQ_VIC2			IRQ_GPIO
+#define IRQMASK_VIC2			IRQMASK_GPIO
+
+/*
+ * VIC Register (VA)
+ */
+#define VIC_IRQ_ENABLE_VA				IRQ_ENABLE_REG
+#define VIC_IRQ_DISABLE_VA				IRQ_DISABLE_REG
+#define VIC_SOFT_INT_CLEAR_VA			SOFT_INT_CLEAR_REG
+
+#define VIC_SENSE_VA                   INTERRUPT_SENS_REG
+#define VIC_EVENT_VA                   INTERRUPT_EVENT_REG
+#define VIC_EDGE_CLEAR_VA              EDGE_TRIGGERED_INTCLEAR_REG
+
+#define EXT_VIC_SENSE_VA               EXT_INTERRUPT_SENS_REG
+#define EXT_VIC_EVENT_VA               EXT_INTERRUPT_EVENT_REG
+#define EXT_VIC_EDGE_CLEAR_VA          EXT_EDGE_TRIGGERED_INTCLEAR_REG
+
+#define IRQ_ENABLE(irq_no)             *((volatile unsigned long*)VIC_IRQ_ENABLE_VA)    |= 1 << (irq_no)
+#define IRQ_DISABLE(irq_no)            *((volatile unsigned long*)VIC_IRQ_DISABLE_VA)   |= 1 << (irq_no)
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
+#define AST_UART_CLOCK     UART_CLOCK/13 /* defined in board config */
+#define AST_BAUD_115200    (AST_UART_CLOCK /1843200)
+#define AST_BAUD_57600     (AST_UART_CLOCK /921600)
+#define AST_BAUD_38400     (AST_UART_CLOCK /614400)
+#define AST_BAUD_19200     (AST_UART_CLOCK /307200)
+#define AST_BAUD_14400     (AST_UART_CLOCK /230400)
+#define AST_BAUD_9600      (AST_UART_CLOCK /153600)
+
+
+/*----------------------------------------------------------------
+ *   UART DMA Controllers
+ *----------------------------------------------------------------
+ */
+#define	UART_TX_UDMA_EN					0x00
+#define	UART_RX_UDMA_EN					0x04
+#define	UART_UDMA_CONF					0x08
+#define	UART_UDMA_TIMER					0x0C
+
+#define	UART_TX_UDMA_REST				0x20
+#define	UART_RX_UDMA_REST				0x24
+
+#define	UART_TX_UDMA_IER				0x30
+#define	UART_TX_UDMA_ISR				0x34
+#define	UART_RX_UDMA_IER				0x38
+#define	UART_RX_UDMA_ISR				0x3C
+#define	UART_TX_R_POINT(x)				(0x40 + (x*0x20))
+#define	UART_TX_W_POINT(x)				(0x44 + (x*0x20))
+#define	UART_TX_UDMA_ADDR(x)			(0x48 + (x*0x20))
+#define	UART_RX_R_POINT(x)				(0x50 + (x*0x20))
+#define	UART_RX_W_POINT(x)				(0x54 + (x*0x20))
+#define	UART_RX_UDMA_ADDR(x)			(0x58 + (x*0x20))
+
+/* UART_TX_UDMA_EN	-0x00 : UART TX DMA Enable */
+/* UART_RX_UDMA_EN	-0x04 : UART RX DMA Enable */
+#define UDMA_CH_EN(x)					(0x1 << (x))
+
+/* UART_UDMA_CONF - 0x08 : Misc, Buffer size  */
+#define UDMA_TX_BUFF_SIZE_MASK			(0x3)
+#define UDMA_SET_TX_BUFF_SIZE(x)		(x)
+#define UDMA_BUFF_SIZE_1KB				(0x0)
+#define UDMA_BUFF_SIZE_4KB				(0x1)
+#define UDMA_BUFF_SIZE_16KB				(0x2)
+#define UDMA_BUFF_SIZE_64KB				(0x3)
+#define UDMA_RX_BUFF_SIZE_MASK			(0x3 << 2)
+#define UDMA_SET_RX_BUFF_SIZE(x)		(x << 2)
+#define UDMA_TIMEOUT_DIS				(0x1 << 4)
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
+
+
+#endif /* _HWDEF_AST2400_H_ */
