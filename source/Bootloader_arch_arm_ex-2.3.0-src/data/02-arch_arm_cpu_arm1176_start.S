--- u-boot-2013.07/arch/arm/cpu/arm1176/start.S	2013-07-23 19:58:13.000000000 +0800
+++ u-boot-2013.07-ami/arch/arm/cpu/arm1176/start.S	2015-07-03 16:21:46.892773497 +0800
@@ -32,11 +32,13 @@
 
 #include <asm-offsets.h>
 #include <config.h>
-#include <version.h>
+//#include <version.h> // TODO : Fix This
 
+/*  TODO: Fix This 
 #ifndef CONFIG_SYS_PHY_UBOOT_BASE
 #define CONFIG_SYS_PHY_UBOOT_BASE	CONFIG_SYS_UBOOT_BASE
 #endif
+*/
 
 /*
  *************************************************************************
@@ -120,6 +122,18 @@
 _end_ofs:
 	.word _end - _start
 
+#ifdef CONFIG_USE_IRQ
+/* IRQ stack memory (calculated at run-time) */
+.globl IRQ_STACK_START
+IRQ_STACK_START:
+	.word	0x0badc0de
+
+/* IRQ stack memory (calculated at run-time) */
+.globl FIQ_STACK_START
+FIQ_STACK_START:
+	.word 0x0badc0de
+#endif
+
 /* IRQ stack memory (calculated at run-time) + 8 bytes */
 .globl IRQ_STACK_START_IN
 IRQ_STACK_START_IN:
@@ -175,8 +189,10 @@
 	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache
 
 	/* Prepare to disable the MMU */
+/*  TODO: Fix This 
 	adr	r2, mmu_disable_phys
 	sub	r2, r2, #(CONFIG_SYS_PHY_UBOOT_BASE - CONFIG_SYS_TEXT_BASE)
+*/
 	b	mmu_disable
 
 	.align 5
@@ -185,7 +201,9 @@
 	mcr	p15, 0, r0, c1, c0, 0
 	nop
 	nop
+/*  TODO: Fix This 
 	mov	pc, r2
+*/
 mmu_disable_phys:
 
 #ifdef CONFIG_DISABLE_TCM
@@ -215,7 +233,9 @@
 	/*
 	 * Go setup Memory and board specific bits prior to relocation.
 	 */
+#if !defined(CONFIG_TWO_STAGES_BOOTLOADER)
 	bl	lowlevel_init		/* go setup pll,mux,memory */
+#endif
 
 	bl	_main
 
