--- linux.org/drivers/mmc/host/sdhci-ast.h	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/mmc/host/sdhci-ast.h	2014-03-07 10:15:01.457242877 -0500
@@ -0,0 +1,37 @@
+
+/* AST SoC SD/MMC related register address and values */
+
+#define AST_SDHC_VA_BASE                IO_ADDRESS(0x1E740000)
+
+#define AST_SDHC_SLOT1_ADDR             0x1E740100
+#define AST_SDHC_SLOT2_ADDR             0x1E740200
+
+#define SCU_UNLOCK_MAGIC                0x1688A8A8
+
+#define SD_CLOCK_BIT                    (1 << 27)
+#define SD_CLOCK_RUN                    (1 << 15)
+#define SD_RESET_BIT                    (1 << 16)
+
+#define SD_CLOCK_DIVIDER_MASK           (7 << 12) /* bits[14:12] */
+#define SD_CLOCK_DIVIDER_SELECTION      1 /* SDCLK = H-PLL / 4 */
+#define SDIO_ALL_SOFTWARE_RESET         0x01
+
+#define AST_SDHC_INFO                   0x00
+#define AST_SDHC_DEBOUNCE               0x04
+#define AST_SDHC1_CAP                   0x140
+#define AST_SDHC2_CAP                   0x240
+
+#define AST_INT_STATUS_REG              0x130
+#define AST_INT_STATUSEN_REG            0x134
+#define AST_INT_ENABLE_REG              0x138
+
+#define AST_SCU_MULTI_PIN_SDHC1         0x01
+#define AST_SCU_MULTI_PIN_SDHC2         0x02
+
+#define AST_SDHC_IRQ_NUM                26
+
+#define AST_SDHCI_SLOT1_NAME            "ast_sdhci1"
+#define AST_SDHCI_SLOT2_NAME            "ast_sdhci2"
+
+#define AST_SD_MIN_CLOCK                (20 * 1000000) /* 20 Mhz */
+#define AST_SD_PLL_CLOCK                (384 * 1000000) /* 384 Mhz */
