--- linux.org/arch/arm/soc-ast/ast-irq.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/arch/arm/soc-ast/ast-irq.c	2014-01-31 15:42:23.383921289 -0500
@@ -0,0 +1,96 @@
+/*
+ *  linux/arch/arm/soc-ast/ast-irq.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ *  AST SOC IRQ handling functions
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
+#include <linux/init.h>
+#include <linux/interrupt.h>
+#include <linux/list.h>
+#include <linux/irq.h>
+
+#include <asm/irq.h>
+#include <mach/hardware.h>
+#include <asm/mach-types.h>
+
+static void ast_irq_mask (struct irq_data *d)
+{
+	if (d->irq > NR_IRQS)
+		return;
+	
+	if(d->irq < BW_CMP_NR_IRQS)
+		*((volatile unsigned long *) IRQ_DISABLE_REG) |= (1 << d->irq);
+	else
+		*((volatile unsigned long *) EXT_IRQ_DISABLE_REG) |= (1 << (d->irq - BW_CMP_NR_IRQS));
+}
+
+static void ast_irq_unmask (struct irq_data *d)
+{
+	if (d->irq > NR_IRQS)
+		return;
+	if(d->irq < BW_CMP_NR_IRQS)
+		*((volatile unsigned long *) IRQ_ENABLE_REG) |= (1 << d->irq);
+	else
+		*((volatile unsigned long *) EXT_IRQ_ENABLE_REG) |= (1 << (d->irq - BW_CMP_NR_IRQS));			
+}
+
+struct irq_chip ext_chip = {
+	.irq_ack    = ast_irq_mask,
+	.irq_mask   = ast_irq_mask,
+	.irq_unmask = ast_irq_unmask,
+};
+
+void __init ast_init_irq(void)
+{
+	int i;
+
+	printk("AST Interrupt Controller Enabled\n");
+	/* Set the trigger level and modes of the interrupts */
+	*(volatile unsigned long *) (IRQ_SELECT_REG)  = 0;
+	*(volatile unsigned long *) (IRQ_ENABLE_REG) = 0;
+	*(volatile unsigned long *) (IRQ_DISABLE_REG) = 0xFFFFFFFF;
+	*(volatile unsigned long *) (EDGE_TRIGGERED_INTCLEAR_REG) = 0xFFFFFFFF;
+
+	*(volatile unsigned long *) (EXT_IRQ_SELECT_REG)  = 0;
+	*(volatile unsigned long *) (EXT_IRQ_ENABLE_REG) = 0;
+	*(volatile unsigned long *) (EXT_IRQ_DISABLE_REG) = 0xFFFFFFFF;
+	*(volatile unsigned long *) (EXT_EDGE_TRIGGERED_INTCLEAR_REG) = 0xFFFFFFFF;
+
+	for (i = 0; i < NR_IRQS; i ++)
+	{
+		if(i < BW_CMP_NR_IRQS)
+		{
+        		if ((1 << i) & AST_VIC_VALID_INTMASK)
+			{
+				irq_set_chip_and_handler(i, &ext_chip, handle_level_irq);
+				//irq_set_chip_and_handler(i, &ext_chip, handle_edge_irq);
+				set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
+			}
+		}
+		else
+		{
+			if ((1 << (i - BW_CMP_NR_IRQS)) & EXT_AST_VIC_VALID_INTMASK)
+			{
+				irq_set_chip_and_handler(i, &ext_chip, handle_level_irq);
+				//irq_set_chip_and_handler(i, &ext_chip, handle_edge_irq);
+				set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
+			}
+		}
+	}
+}
