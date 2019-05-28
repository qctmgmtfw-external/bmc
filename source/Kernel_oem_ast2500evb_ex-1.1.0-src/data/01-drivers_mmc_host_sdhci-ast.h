--- /home/josephwei/MDS/MDS3_7_10c/MDS/workspace/ast2500evb_r11-5-FB_workspace/Build/kernel/linux/drivers/mmc/host/sdhci-ast.h	Fri Aug  5 13:55:03 2016
+++ linux/drivers/mmc/host/sdhci-ast.h	Tue Aug  9 11:30:56 2016
@@ -40,6 +40,6 @@
 #ifdef CONFIG_SPX_FEATURE_MMC_OEM_PLATFORM
 #define AST_SD_MIN_CLOCK                (1 * 1000000) /* 1 Mhz */
 #else
-#define AST_SD_MIN_CLOCK		(20 * 1000000) /* 20 Mhz */
+#define AST_SD_MIN_CLOCK		 (400000) /* 400 Khz */
 #endif
 #define AST_SD_PLL_CLOCK                (384 * 1000000) /* 384 Mhz */
