--- u-boot-2013.07/arch/arm/cpu/astcommon/interrupts.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/interrupts.c	2013-12-05 12:17:55.083503948 -0500
@@ -0,0 +1,142 @@
+#include <common.h>
+#include <command.h>
+#include <linux/types.h>
+#include "soc_hw.h"
+
+#ifdef CONFIG_USE_IRQ
+
+typedef unsigned long UINT32;
+
+extern int timer_init(void);
+
+#define MAX_INTR (32)
+
+struct irq_action {
+	interrupt_handler_t *handler;
+	void *arg;
+	int count;
+	int spurious;
+};
+
+static struct irq_action vectors[MAX_INTR];
+
+void SpuriousHandler(void *IntNum)
+{
+	if ((int)IntNum < MAX_INTR)
+		printf("Spurious IRQ %ld received \n",(UINT32)IntNum);
+}
+
+
+int arch_interrupt_init(void)
+{
+	UINT32 i;
+
+	/* Initialize to Dummy Handler */
+	for (i = 0; i < MAX_INTR; i ++) {
+		vectors[i].handler = SpuriousHandler;
+		vectors[i].arg = (void*)i;
+		vectors[i].count = 0;
+		vectors[i].spurious = 0;
+		/* Set the default Trig Mode and Trig Level here for each*/
+	}
+
+	/* Clear All Current and Pending IRQ */
+	*(UINT32 *)(IRQ_CLEAR_REG)= 0xFFFFFFFF;
+	*(UINT32 *)(IRQ_CLEAR_REG)= 0xFFFFFFFF;
+
+	/* Disable All IRQ */
+	*(UINT32*)(IRQ_ENABLE_REG) = 0x0;
+
+	/* Enable IRQ in PSR */
+	enable_interrupts();
+
+	/* Enable Timer */
+	timer_init();
+	return 0;
+}
+
+void HandleIntr(void)
+{
+	UINT32 IntNum;
+
+	/* Get the Interrupts */
+	IntNum = *(UINT32*)(IRQ_STATUS_REG) & 0x1F;
+
+	/* Spurious Interrupt on IntNum */
+	if (vectors[IntNum].handler == SpuriousHandler) {
+		vectors[IntNum].spurious ++;
+		(*vectors[IntNum].handler)(vectors[IntNum].arg);
+	} else { /* Good Interrupt on IntNum */
+		vectors[IntNum].count ++;
+		(*vectors[IntNum].handler)(vectors[IntNum].arg);
+	}
+
+	/* Clear the Interrupt */
+	*(UINT32 *)(IRQ_CLEAR_REG) = (1 << IntNum);
+}
+
+void irq_install_handler(int IntNum, interrupt_handler_t *handler, void *arg)
+{
+	if (IntNum >=MAX_INTR) {
+		printf("ERROR: Unsupported INTR Number %d\n",IntNum);
+		return;
+	}
+
+	if (handler == NULL) {
+		printf("WARNING: NULL Handler. Freeing Interrupt %d\n",IntNum);
+		irq_free_handler(IntNum);
+		return;
+	}
+
+	if (vectors[IntNum].handler != SpuriousHandler) {
+		if (vectors[IntNum].handler == handler) {
+			printf("WARNING: Same vector is installed for INT %d\n",IntNum);
+			return;
+		}
+		printf("Replacing vector for INT %d\n",IntNum);
+	}
+
+	disable_interrupts();
+	vectors[IntNum].handler = handler;
+	vectors[IntNum].arg = arg;
+	enable_interrupts();
+
+	/* Enable the specfic IRQ */
+	*(UINT32*)(IRQ_CLEAR_REG) |=  (1 << IntNum);
+}
+
+void irq_free_handler(int IntNum)
+{
+	/* Disable the specfic IRQ */
+	*(UINT32*)(IRQ_CLEAR_REG) &=  ~(1 << IntNum);
+
+	disable_interrupts();
+	vectors[IntNum].handler = SpuriousHandler;
+	vectors[IntNum].arg = (void *)IntNum;
+	enable_interrupts();
+}
+
+#endif  /* CONFIG_USE_IRQ */
+
+#ifdef CONFIG_CMD_IRQ
+
+int do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
+{
+	int vec;
+
+	printf ("\nInterrupt-Information:\n");
+	printf ("Num  Routine   Arg       Count  Spurious\n");
+
+	for (vec=0; vec<MAX_INTR; vec ++) {
+		printf ("%02d   %08lx  %08lx      %d         %d\n",
+				vec,
+				(ulong)vectors[vec].handler,
+				(ulong)vectors[vec].arg,
+				vectors[vec].count,
+				vectors[vec].spurious);
+	}
+	return 0;
+}
+
+#endif  /* CONFIG_CMD_IRQ */
+
