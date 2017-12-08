--- linux/arch/arm/include/asm/soc-ast2300/system.h	2011-05-25 14:59:15.839790485 +0800
+++ linux.new/arch/arm/include/asm/soc-ast2300/system.h	2013-04-26 16:35:00.000000000 +0800
@@ -38,6 +38,22 @@
 
 static inline void arch_reset(char mode)
 {
+    unsigned long reg;
+
+    /* 
+     * Fix Linux Host OS hang. 
+     * BMC reset will clear LADR3H/LADR3L/LADR12H/LADR12L. 
+     * If BMC reset coincidentally launches at the 2nd LPC clock of a 
+     * decoded KCS/BT turn-around cycle, AST2300 LPC controller will enter a wait state.
+     * And the wait state is not initialized by the BMC reset. It causes Linux Host OS hang
+     */
+    reg = *((volatile unsigned long*) AST_LPC_VA_BASE); /* HICR0 = Disable LPC channel #1 #2 #3 */ 
+    reg &= 0xffffff1f; /* HICR0 = Disable LPC channel #1 #2 #3 */
+    *((volatile unsigned long*) AST_LPC_VA_BASE) = reg;
+    reg = *((volatile unsigned long*) AST_LPC_VA_BASE + 0x10); /* HICR4 = Disable KCS interface and BT interface in channel #3*/
+    reg &= 0xfffffffa; /* HICR4 = Disable Enable KCS interface and BT interface in channel #3*/
+    *((volatile unsigned long*) AST_LPC_VA_BASE + 0x10) = reg;
+
 	/*
 	 * Use WDT to restart system
 	 */
