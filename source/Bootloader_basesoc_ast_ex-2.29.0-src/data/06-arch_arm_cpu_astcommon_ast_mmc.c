--- uboot/arch/arm/cpu/astcommon/ast_mmc.c	1970-01-01 05:30:00.000000000 +0530
+++ uboot.new/arch/arm/cpu/astcommon/ast_mmc.c	2015-09-22 12:16:46.157665952 +0530
@@ -0,0 +1,168 @@
+#include <common.h>
+#include <command.h>
+#include <malloc.h>
+#include <sdhci.h>
+#include "soc_hw.h"
+#ifdef CONFIG_SPX_FEATURE_MMC_OEM_PLATFORM
+#include "../../../../../../../Build/include/projdef.h"
+#endif
+
+static char *SD_NAME = "ast_sd";
+
+
+#define iowrite32(value,reg) 	(*(volatile unsigned long *)(reg))=(value)
+#define ioread32(reg) 			(*(volatile unsigned long *)(reg))
+#define mdelay(n) 				({unsigned long msec=(n); while (msec--) udelay(1000);})
+
+#define AST_SD_BASE_ADDR 		0x1e740000	
+#define AST_SDHC_INFO           0x00
+#define AST_SDHC_DEBONUCE       0x04
+#define AST_SDHC1_CAP           0x140
+#define AST_SDHC2_CAP           0x240
+
+#define SCU_UNLOCK_MAGIC     	0x1688A8A8
+#define SD_CLOCK_BIT    	 	(1 << 27)
+#define SD_CLOCK_RUN    	 	(1 << 15)
+#define SD_RESET_BIT   		 	(1 << 16)
+#define AST_SCU_MULTI_PIN_SDHC2 0x00000002 /* bit 1 */
+#define AST_SCU_MULTI_PIN_SDHC1 0x00000001 /* bit 0 */
+
+#define SDIO_ALL_SOFTWARE_RESET         0x01
+
+
+/* Clock Calculation */
+#define SD_CLOCK_DIVIDER_MASK           (7 << 12) /* bits[14:12] */
+#define SD_CLOCK_DIVIDER_SELECTION      1 /* SDCLK = H-PLL /4  */
+#ifdef CONFIG_AST2500
+#define PLL_CLK 						(792*1000000)
+#define MAX_CLK 						(PLL_CLK / 8)
+#else
+#define PLL_CLK 						(384*1000000)
+#define MAX_CLK 						(PLL_CLK / (((SD_CLOCK_DIVIDER_SELECTION) +1 )*2))
+#endif
+#ifdef CONFIG_SPX_FEATURE_MMC_OEM_PLATFORM
+#define MIN_CLK 						(1 * 1000000)
+#else
+#define MIN_CLK							(20*1000000) /* Known slowest device - mmc cards */
+#endif
+
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530)
+#define SDHC					CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT
+#define EMMC					CONFIG_SPX_FEATURE_GLOBAL_EMMC_FLASH_COUNT
+#else
+#define SDHC					1
+#define EMMC					1
+#endif 
+
+int ast_register_sdhci(u32 regbase, u32 max_clk, u32 min_clk)
+{
+		struct sdhci_host *host = NULL;
+		host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
+		if (!host) 
+		{
+			printf("sdh_host malloc fail!\n");
+			return 1;
+		}
+		memset(host,0,sizeof(struct sdhci_host));
+
+		host->name = SD_NAME;
+		host->ioaddr = (void *)regbase;  
+#ifdef CONFIG_AST2500		
+		host->quirks =  SDHCI_QUIRK_BROKEN_R1B | SDHCI_QUIRK_WAIT_SEND_CMD;
+#endif
+//		host->quirks =  <If any for broken clock> 
+//		Controller returns max clock 24 MHz -  So cannot detect HS devices
+// 		so we override default by passing max_clk and min_clk
+
+		host->version = sdhci_readw(host, SDHCI_HOST_VERSION);
+//		printf("SD Controller version %d found at %lx\n",host->version,(ulong)host->ioaddr);
+
+		add_sdhci(host, max_clk, min_clk);
+		return 0;
+}
+
+int ast_enable_sdhci(void)
+{
+		u32 reg;
+
+        iowrite32(SCU_UNLOCK_MAGIC, SCU_KEY_CONTROL_REG); /* unlock SCU */
+
+		/* enable multifucntion for SDHC */	
+        reg = ioread32(SCU_MISC_MULTI_PIN5_REG);
+		if (SDHC != 0)
+			reg |= AST_SCU_MULTI_PIN_SDHC1;
+		if (EMMC != 0)
+			reg |= AST_SCU_MULTI_PIN_SDHC2;
+        iowrite32(reg, SCU_MISC_MULTI_PIN5_REG);
+        mdelay(10);
+
+        /* enable reset of SDHC */
+        reg = ioread32(SCU_SYS_RESET_REG);
+        reg |= SD_RESET_BIT;
+        iowrite32(reg, SCU_SYS_RESET_REG);
+        mdelay(10);
+
+        /* enable SDHC clock */
+        reg = ioread32(SCU_CLK_STOP_REG);
+        reg &= ~SD_CLOCK_BIT;
+        iowrite32(reg, SCU_CLK_STOP_REG);
+        mdelay(10);
+
+        /* set clock of SDHC */
+        reg = ioread32(SCU_CLK_SELECT_REG);
+        reg |= SD_CLOCK_RUN;
+        iowrite32(reg, SCU_CLK_SELECT_REG);
+        mdelay(10);
+
+        reg = ioread32(SCU_CLK_SELECT_REG);
+        reg &= ~SD_CLOCK_DIVIDER_MASK;
+        reg |= (SD_CLOCK_DIVIDER_SELECTION << 12);
+        iowrite32(reg, SCU_CLK_SELECT_REG);
+        mdelay(10);
+
+        /* disable reset of SDHC */
+        reg = ioread32(SCU_SYS_RESET_REG);
+        reg &= ~SD_RESET_BIT;
+        iowrite32(reg, SCU_SYS_RESET_REG);
+
+        iowrite32(0, SCU_KEY_CONTROL_REG); /* lock SCU */
+
+        /* Reset  SDIO */
+        reg = ioread32(AST_SD_BASE_ADDR + AST_SDHC_INFO);
+        reg |= SDIO_ALL_SOFTWARE_RESET;
+        iowrite32(reg, AST_SD_BASE_ADDR + AST_SDHC_INFO);
+
+        do 
+		{
+        	reg = ioread32(AST_SD_BASE_ADDR + AST_SDHC_INFO);
+        } while (reg & SDIO_ALL_SOFTWARE_RESET);
+
+		/* Debouce value */
+        iowrite32(0x1000, AST_SD_BASE_ADDR + AST_SDHC_DEBONUCE);
+
+		return 0;
+}
+
+void default_clock_timeout(void)
+{
+	if (SDHC != 0)
+	{
+		*(volatile unsigned char *)(AST_SD_BASE_ADDR + 0x12D) = 0x20;	/* SDCLK */
+		*(volatile unsigned char *)(AST_SD_BASE_ADDR + 0x12E) = 0x0F;	/* Timeout */
+	}
+	if (EMMC != 0)
+	{
+		*(volatile unsigned char *)(AST_SD_BASE_ADDR + 0x22D) = 0x20;	/* SDCLK */
+		*(volatile unsigned char *)(AST_SD_BASE_ADDR + 0x22E) = 0x0F;	/* Timeout */
+	}	
+}
+
+int cpu_mmc_init(void)
+{
+		ast_enable_sdhci();
+		if (SDHC != 0)
+			ast_register_sdhci (AST_SD_BASE_ADDR+0x100,MAX_CLK,MIN_CLK);
+		if (EMMC != 0)
+			ast_register_sdhci (AST_SD_BASE_ADDR+0x200,MAX_CLK,MIN_CLK);
+		return 0;
+}
