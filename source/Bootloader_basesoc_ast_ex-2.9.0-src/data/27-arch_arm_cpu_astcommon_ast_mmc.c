--- uboot/arch/arm/cpu/astcommon/ast_mmc.c	2015-07-01 20:44:08.563228719 +0800
+++ uboot.new/arch/arm/cpu/astcommon/ast_mmc.c	2015-07-01 20:44:17.619228816 +0800
@@ -127,8 +127,8 @@
 
 int cpu_mmc_init(void)
 {
-		ast_enable_sdhci();
-		ast_register_sdhci (AST_SD_BASE_ADDR+0x100,MAX_CLK,MIN_CLK);
-		ast_register_sdhci (AST_SD_BASE_ADDR+0x200,MAX_CLK,MIN_CLK);
+	//	ast_enable_sdhci();  //Quanta
+	//	ast_register_sdhci (AST_SD_BASE_ADDR+0x100,MAX_CLK,MIN_CLK); //Quanta
+	//	ast_register_sdhci (AST_SD_BASE_ADDR+0x200,MAX_CLK,MIN_CLK); //Quanta
 		return 0;
 }
