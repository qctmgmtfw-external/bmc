--- linux-2.6.28.10-pristine/arch/arm/soc-ast2300/ast2300-irq.c	1970-01-01 08:00:00.000000000 +0800
+++ linux-2.6.28.10/arch/arm/soc-ast2300/ast2300-irq.c	2011-05-26 16:51:48.467614000 +0800
@@ -0,0 +1,91 @@
+/*
+ *  linux/arch/arm/mach-ast2300evb/ast2300-irq.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ *  AST2300 SOC IRQ handling functions
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
+static void ast2050_irq_mask (unsigned int irq)
+{
+	if (irq > NR_IRQS)
+		return;
+	
+	if(irq < BW_CMP_NR_IRQS)
+		*((volatile unsigned long *) IRQ_DISABLE_REG) |= (1 << irq);
+	else
+		*((volatile unsigned long *) EXT_IRQ_DISABLE_REG) |= (1 << (irq - BW_CMP_NR_IRQS));
+}
+
+static void ast2050_irq_unmask (unsigned int irq)
+{
+	if (irq > NR_IRQS)
+		return;
+	if(irq < BW_CMP_NR_IRQS)
+		*((volatile unsigned long *) IRQ_ENABLE_REG) |= (1 << irq);
+	else
+		*((volatile unsigned long *) EXT_IRQ_ENABLE_REG) |= (1 << (irq - BW_CMP_NR_IRQS));			
+}
+
+struct irq_chip ext_chip = {
+	.ack    = ast2050_irq_mask,
+	.mask   = ast2050_irq_mask,
+	.unmask = ast2050_irq_unmask,
+};
+
+void __init ast2050_init_irq(void)
+{
+	int i;
+
+	printk("AST2300 Interrupt Controller Enabled\n");
+	/* Set the trigger level and modes of the interrupts */
+	*(volatile unsigned long *) (IRQ_SELECT_REG)  = 0;
+	*(volatile unsigned long *) (IRQ_ENABLE_REG) =0;
+	*(volatile unsigned long *) (IRQ_DISABLE_REG) = 0xFFFFFFFF;
+	*(volatile unsigned long *) (EDGE_TRIGGERED_INTCLEAR_REG) = 0xFFFFFFFF;
+
+	*(volatile unsigned long *) (EXT_IRQ_SELECT_REG)  = 0;
+	*(volatile unsigned long *) (EXT_IRQ_ENABLE_REG) =0;
+	*(volatile unsigned long *) (EXT_IRQ_DISABLE_REG) = 0xFFFFFFFF;
+	*(volatile unsigned long *) (EXT_EDGE_TRIGGERED_INTCLEAR_REG) = 0xFFFFFFFF;
+
+	for (i = 0; i < NR_IRQS; i ++) {
+		if(i < BW_CMP_NR_IRQS)
+		{
+        	if ((1 << i) & AST2050_VIC_VALID_INTMASK) {
+            	set_irq_chip(i, &ext_chip);
+	            set_irq_handler(i, handle_level_irq);
+    	        set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
+        	}
+		}else{
+			if ((1 << (i - BW_CMP_NR_IRQS)) & EXT_AST2050_VIC_VALID_INTMASK) {
+            	set_irq_chip(i, &ext_chip);
+	            set_irq_handler(i, handle_level_irq);
+    	        set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
+        	}
+		}
+	}
+}
