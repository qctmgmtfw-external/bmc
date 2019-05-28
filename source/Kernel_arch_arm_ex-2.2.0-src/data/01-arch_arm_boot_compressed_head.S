--- linux-3.14.17/arch/arm/boot/compressed/head.S	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/arch/arm/boot/compressed/head.S	2014-08-21 13:11:45.709664580 -0400
@@ -674,6 +674,13 @@
 		mov	pc, lr
 ENDPROC(__setup_mmu)
 
+__armv4_no_cache:
+       orr     r0,r0,#0
+       orr     r0,r0,#0
+       mov     pc,lr
+       orr     r0,r0,#0
+       orr     r0,r0,#0
+
 @ Enable unaligned access on v6, to allow better code generation
 @ for the decompressor C code:
 __armv6_mmu_cache_on:
