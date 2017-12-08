--- linux-2.6.28.10-pristine/arch/arm/include/asm/mach/irq.h	2009-05-02 14:54:43.000000000 -0400
+++ linux-2.6.28.10/arch/arm/include/asm/mach/irq.h	2009-10-06 17:45:17.000000000 -0400
@@ -21,14 +21,44 @@
 extern void init_FIQ(void);
 extern int show_fiq_list(struct seq_file *, void *);
 
-/*
- * This is for easy migration, but should be changed in the source
- */
-#define do_bad_IRQ(irq,desc)				\
-do {							\
-	spin_lock(&desc->lock);				\
-	handle_bad_irq(irq, desc);			\
-	spin_unlock(&desc->lock);			\
-} while(0)
+struct irqchip {
+    /*
+ *      * Acknowledge the IRQ.
+ *           * If this is a level-based IRQ, then it is expected to mask the IRQ
+ *                * as well.
+ *                     */
+    void (*ack)(unsigned int);
+    /*
+ *      * Mask the IRQ in hardware.
+ *           */
+    void (*mask)(unsigned int);
+    /*
+ *      * Unmask the IRQ in hardware.
+ *           */
+    void (*unmask)(unsigned int);
+    /*
+ *      * Ask the hardware to re-trigger the IRQ.
+ *           * Note: This method _must_ _not_ call the interrupt handler.
+ *                * If you are unable to retrigger the interrupt, do not
+ *                     * provide a function, or if you do, return non-zero.
+ *                          */
+    int (*retrigger)(unsigned int);
+    /*
+ *      * Set the type of the IRQ.
+ *           */
+    int (*set_type)(unsigned int, unsigned int);
+    /*
+ *      * Set wakeup-enable on the selected IRQ
+ *           */
+    int (*set_wake)(unsigned int, unsigned int);
+
+#ifdef CONFIG_SMP
+    /*
+ *      * Route an interrupt to a CPU
+ *           */
+    void (*set_cpu)(struct irqdesc *desc, unsigned int irq, unsigned int cpu);
+#endif
+};
+
 
 #endif
