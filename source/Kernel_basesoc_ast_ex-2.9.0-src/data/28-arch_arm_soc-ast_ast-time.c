--- linux.old/arch/arm/soc-ast/ast-time.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-3.14.17-ami/arch/arm/soc-ast/ast-time.c	2014-08-22 11:44:24.000000000 -0400
@@ -0,0 +1,124 @@
+/*
+ *  linux/arch/arm/soc-ast/ast-time.c
+ *
+ *  Copyright (C) 2009 American Megatrends Inc
+ *
+ *  AST SOC timer functions
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
+#include <linux/kernel.h>
+#include <linux/interrupt.h>
+#include <linux/time.h>
+#include <linux/init.h>
+#include <linux/timex.h>
+#include <linux/smp.h>
+#include <mach/hardware.h>
+#include <asm/io.h>
+#include <asm/irq.h>
+#include <asm/uaccess.h>
+#include <asm/mach/irq.h>
+#include <asm/mach/time.h>
+
+
+#define TIMER_RELOAD	((1*1000*1000) / 100)
+#define TICKS_PER_uSEC  1		/* ASPEED_EXTCLK / 10 ^ 6 */
+#define TICKS2USECS(x)	((x) / TICKS_PER_uSEC)
+
+#if 0  // Newer kernels  don't use this 
+/* Returns number of us since last clock interrupt. Note the
+   interrupts will be disabled before calling this function */
+static unsigned long ast_gettimeoffset(void)
+{
+	unsigned long ticks1, ticks2, status;
+
+	/*
+	 * Get the current number of ticks.  Note that there is a race
+	 * condition between us reading the timer and checking for
+	 * an interrupt.  We get around this by ensuring that the
+	 * counter has not reloaded between our two reads.
+	 */
+	ticks2 = *(volatile unsigned long*)TIMER1_COUNT_REG;
+	do {
+		ticks1 = ticks2;
+		status = __raw_readl(RAW_INT_STATUS_REG);
+		ticks2 = *(volatile unsigned long*)TIMER1_COUNT_REG;
+	} while (ticks2 > ticks1);
+
+	/*
+	 * Number of ticks since last interrupt.
+	 */
+	ticks1 = TIMER_RELOAD - ticks2;
+
+	/*
+	 * Interrupt pending?  If so, we've reloaded once already.
+	 */
+	if (status & IRQMASK_TIMERINT0)
+		ticks1 += TIMER_RELOAD;
+
+	/*
+	 * Convert the ticks to usecs
+	 */
+	return TICKS2USECS(ticks1);
+}
+#endif
+
+static irqreturn_t ast_timer_interrupt(int irq, void *dev_id)
+{
+	/*
+	 * clear the interrupt
+	 */
+	*(volatile unsigned long *)(EDGE_TRIGGERED_INTCLEAR_REG) |= IRQMASK_TIMERINT0;
+
+	/*
+	 * the clock tick routines are only processed on the
+	 * primary CPU
+	 */
+	timer_tick();
+
+	return IRQ_HANDLED;
+}
+
+static struct irqaction ast_timer_irq = {
+	.name		= "AST Timer Tick",
+	.flags		= IRQF_DISABLED|IRQF_TIMER|IRQF_IRQPOLL,
+	.handler	= ast_timer_interrupt,
+};
+
+void __init ast_timer_init(void)
+{
+	/* Disable all timer and interrups */
+	*((volatile unsigned long *) TIMER_CONTROL_REG) = 0;
+	/* Clear Interrupt */
+	*((volatile unsigned long *) EDGE_TRIGGERED_INTCLEAR_REG) |= IRQMASK_TIMERINT0;
+
+	/* Load counter values */
+	*((volatile unsigned long *) TIMER1_RELOAD_REG) = TIMER_RELOAD - 1;
+	*((volatile unsigned long *) TIMER1_COUNT_REG) = TIMER_RELOAD - 1;
+
+	/* Enable Interrupts and timer */
+	*((volatile uint32_t *) TIMER_CONTROL_REG) = TIMER1_ENABLE | TIMER1_CLOCK_SELECT | TIMER1_INTR_ENABLE;
+	*((volatile uint32_t *) INTERRUPT_SENS_REG) = 0x0;
+	*((volatile uint32_t *) INTERRUPT_BOTH_EDGE_TRIGGER_REG) = 0x0;
+	*((volatile uint32_t *) INTERRUPT_EVENT_REG) = 0x00010000;
+
+	/* Setup Timer Interrupt routine */
+	IRQ_SET_RISING_EDGE(IRQ_TIMERINT0);
+	setup_irq(IRQ_TIMERINT0, &ast_timer_irq);
+
+	printk("AST Timer Enabled\n");
+}
+
