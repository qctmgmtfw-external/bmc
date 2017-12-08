--- linux-2.6.28.10-pristine/arch/arm/include/asm/mach/arch.h	2009-05-02 14:54:43.000000000 -0400
+++ linux-2.6.28.10/arch/arm/include/asm/mach/arch.h	2009-10-06 17:45:17.000000000 -0400
@@ -20,6 +20,7 @@
 	 * by assembler code in head.S, head-common.S
 	 */
 	unsigned int		nr;		/* architecture number	*/
+	unsigned int        phys_ram;   /* start of physical ram */
 	unsigned int		phys_io;	/* start of physical io	*/
 	unsigned int		io_pg_offst;	/* byte offset for io 
 						 * page tabe entry	*/
