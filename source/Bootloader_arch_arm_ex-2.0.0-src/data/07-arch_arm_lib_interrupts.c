--- u-boot-2013.07/arch/arm/lib/interrupts.c	2013-12-13 13:49:25.392958498 -0500
+++ uboot.new/arch/arm/lib/interrupts.c	2013-12-05 12:17:55.071503948 -0500
@@ -195,4 +195,10 @@
 	show_regs (pt_regs);
 	bad_mode ();
 }
+#else
+extern void HandleIntr(void);
+void do_irq (struct pt_regs *pt_regs)
+{
+	HandleIntr();
+}
 #endif
