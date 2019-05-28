--- uboot/board/ast2400evb/ast2400evb.c	2016-08-04 15:38:10.159578137 +0800
+++ uboot/board/ast2400evb/ast2400evb_new.c	2016-08-04 15:40:29.783576739 +0800
@@ -61,6 +61,17 @@
 #define AST_CE2_SEGMENT_ADDRESS 0x58540000      // 0x2A000000 - 0x2BFFFFFF
 #endif
 
+// Quanta +++
+#define AST2400_A0_CHIPID  0x02000303
+#define AST2400_A1_CHIPID  0x02010303
+#define AST2500_A0_CHIPID  0x04000303
+#define AST2510_A0_CHIPID  0x04000103
+#define AST2520_A0_CHIPID  0x04000203
+#define AST2530_A0_CHIPID  0x04000403
+#define AST2500_A1_CHIPID  0x04010303
+#define AST2510_A1_CHIPID  0x04010103
+// +++ Quanta
+
 extern void soc_init(void);
 
 DECLARE_GLOBAL_DATA_PTR;
@@ -84,6 +95,7 @@
 
 int board_init (void)
 {
+	ulong reg, chip_id;  // Quanta
     
 	/*Should match with linux mach type for this board */
 	gd->bd->bi_arch_number = 900;
@@ -95,6 +107,30 @@
 
 	icache_enable ();
 
+	// Quanta +++
+	*((volatile ulong*) 0x1e6e2000) = 0x1688A8A8;  // unlock SCU
+	reg = *((volatile ulong*) 0x1e6e2008);         // LHCLK = HPLL/8
+	reg &= 0x1c0fffff;                             // PCLK  = HPLL/8
+#ifdef CONFIG_AST2400
+	reg |= 0x61800000;                             // BHCLK = HPLL/8
+#else
+	reg |= 0x61b00000;                             // BHCLK = HPLL/8
+#endif
+	*((volatile ulong*) 0x1e6e2008) = reg;
+
+	QUANTA_OEM_INIT;
+	*((volatile ulong*) SCU_KEY_CONTROL_REG) = 0;  // lock SCU
+
+	// SCU7C
+	chip_id = *((volatile ulong*) 0x1e6e207c);
+	if ((chip_id == AST2400_A0_CHIPID) || (chip_id == AST2400_A1_CHIPID) || (chip_id == AST2500_A0_CHIPID) || (chip_id == AST2510_A0_CHIPID) ||
+		(chip_id == AST2520_A0_CHIPID) || (chip_id == AST2530_A0_CHIPID) || (chip_id == AST2500_A1_CHIPID) || (chip_id == AST2510_A1_CHIPID)) {
+		*((volatile ulong*) 0x1e6ed07c) = 0x000000A8;  // unlock
+		*((volatile ulong*) 0x1e6ed030) = 0x00040000;  // disable common clock
+		*((volatile ulong*) 0x1e6ed07c) = 0x00000000;  // lock
+	}
+	// +++ Quanta
+
 	return 0;
 }
 
@@ -136,6 +172,12 @@
 	
 #endif	
 
+// Quanta +++
+#ifdef QUANTA_OEM_MISC_INIT
+	QUANTA_OEM_MISC_INIT;
+#endif
+// +++ Quanta
+
 	setenv("verify", "n");
 	return (0);
 }
