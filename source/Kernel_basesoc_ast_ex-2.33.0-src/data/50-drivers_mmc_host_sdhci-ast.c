--- linux.old/drivers/mmc/host/sdhci-ast.c	2014-08-05 12:47:26.196514992 +0530
+++ linux-3.14.17-ami/drivers/mmc/host/sdhci-ast.c	2014-09-05 13:41:58.383659456 +0530
@@ -0,0 +1,411 @@
+#include <linux/version.h>
+#include <linux/delay.h>
+#include <linux/highmem.h>
+#include <linux/io.h>
+#include <linux/dma-mapping.h>
+#include <linux/scatterlist.h>
+#include <linux/platform_device.h>
+#include <linux/export.h>
+#include <linux/slab.h>
+#include <linux/module.h>
+#include <linux/leds.h>
+#include <linux/mmc/host.h>
+
+#include "sdhci-ast.h"
+#include "sdhci.h"
+
+static int  ast_sd_remove(struct platform_device *pdev);
+
+static unsigned int ast_scu_sdhc1_enabled = 0;
+static unsigned int ast_scu_sdhc2_enabled = 0;
+static unsigned int ast_sdhc_port1_enabled = 0;
+static unsigned int ast_sdhc_port2_enabled = 0;
+
+#ifdef CONFIG_PM
+static const struct dev_pm_ops ast_sd_pmops = {
+	/* Use standard power save option (if enabled) by using core
+	 * SD/MMC stack function call itself, as AST register set aligns
+	 * perfectly with the call */
+	.resume  = sdhci_resume_host,
+	.suspend = sdhci_suspend_host,
+};
+#endif
+
+static unsigned int ast_get_max_clk(struct sdhci_host *host)
+{
+	uint32_t reg;
+
+	/* Read clock divider from SCU to calculate MAX clock */
+	reg = ioread32((void __iomem*)SCU_CLK_SELECT_REG);
+	reg &= SD_CLOCK_DIVIDER_MASK;
+
+	return (AST_SD_PLL_CLOCK / ((( reg >> 12 ) + 1) * 2));
+}
+
+static unsigned int ast_get_min_clk(struct sdhci_host *host)
+{
+	return AST_SD_MIN_CLOCK;
+}
+
+static struct sdhci_ops ast_sd_ops = {
+	/* Define the max and min clock */
+	.get_max_clock = ast_get_max_clk,
+	.get_min_clock = ast_get_min_clk,
+	/* Remaining elements in this structure is intentionally left blank,
+	 * as core SD/MMC stack operation matches with AST register sets */
+};
+
+static struct platform_driver ast_sd_driver1 = {
+	.driver         = {
+		.name   = AST_SDHCI_SLOT1_NAME,
+		.owner  = THIS_MODULE,
+#ifdef CONFIG_PM
+		.pm     = &ast_sd_pmops,
+#endif
+	},
+	.remove         = ast_sd_remove,
+};
+
+static struct platform_driver ast_sd_driver2 = {
+	.driver         = {
+		.name   = AST_SDHCI_SLOT2_NAME,
+		.owner  = THIS_MODULE,
+#ifdef CONFIG_PM
+		.pm     = &ast_sd_pmops,
+#endif
+	},
+	.remove         = ast_sd_remove,
+};
+
+static struct resource ast_sd_resource1[] = {
+	[0]            = {
+		.start = AST_SDHC_SLOT1_ADDR,
+		.end   = AST_SDHC_SLOT1_ADDR + 0xFF,
+		.flags = IORESOURCE_MEM,
+	},
+	[1]            = {
+		.start = AST_SDHC_IRQ_NUM,
+		.end   = AST_SDHC_IRQ_NUM,
+		.flags = IORESOURCE_IRQ,
+	},
+};
+
+static struct resource ast_sd_resource2[] = {
+	[0]            = {
+		.start = AST_SDHC_SLOT2_ADDR,
+		.end   = AST_SDHC_SLOT2_ADDR + 0xFF,
+		.flags = IORESOURCE_MEM,
+	},
+	[1]            = {
+		.start = AST_SDHC_IRQ_NUM,
+		.end   = AST_SDHC_IRQ_NUM,
+		.flags = IORESOURCE_IRQ,
+	},
+};
+
+static void ast_sd_release(struct device * dev)
+{
+	/* Currently not doing any operation on release */
+	return;
+}
+
+static struct platform_device ast_sd_device1 = {
+	.name            = AST_SDHCI_SLOT1_NAME,
+	.resource        = ast_sd_resource1,
+	.dev = {
+		.release = ast_sd_release,
+	}
+};
+
+static struct platform_device ast_sd_device2 = {
+	.name            = AST_SDHCI_SLOT2_NAME,
+	.resource        = ast_sd_resource2,
+	.dev = {
+		.release = ast_sd_release,
+	}
+};
+
+#if 0
+static void ast_dumpregs(struct sdhci_host *host)
+{
+	printk(KERN_INFO ": =========== REGISTER DUMP (%s)===========\n",
+		mmc_hostname(host->mmc));
+
+	printk(KERN_INFO ": Sys addr: 0x%08x | Version:  0x%08x\n",
+		sdhci_readl(host, SDHCI_DMA_ADDRESS),
+		sdhci_readw(host, SDHCI_HOST_VERSION));
+	printk(KERN_INFO ": Blk size: 0x%08x | Blk cnt:  0x%08x\n",
+		sdhci_readw(host, SDHCI_BLOCK_SIZE),
+		sdhci_readw(host, SDHCI_BLOCK_COUNT));
+	printk(KERN_INFO ": Argument: 0x%08x | Trn mode: 0x%08x\n",
+		sdhci_readl(host, SDHCI_ARGUMENT),
+		sdhci_readw(host, SDHCI_TRANSFER_MODE));
+	printk(KERN_INFO ": Present:  0x%08x | Host ctl: 0x%08x\n",
+		sdhci_readl(host, SDHCI_PRESENT_STATE),
+		sdhci_readb(host, SDHCI_HOST_CONTROL));
+	printk(KERN_INFO ": Power:    0x%08x | Blk gap:  0x%08x\n",
+		sdhci_readb(host, SDHCI_POWER_CONTROL),
+		sdhci_readb(host, SDHCI_BLOCK_GAP_CONTROL));
+	printk(KERN_INFO ": Wake-up:  0x%08x | Clock:    0x%08x\n",
+		sdhci_readb(host, SDHCI_WAKE_UP_CONTROL),
+		sdhci_readw(host, SDHCI_CLOCK_CONTROL));
+	printk(KERN_INFO ": Timeout:  0x%08x | Int stat: 0x%08x\n",
+		sdhci_readb(host, SDHCI_TIMEOUT_CONTROL),
+		sdhci_readl(host, SDHCI_INT_STATUS));
+	printk(KERN_INFO ": Int enab: 0x%08x | Sig enab: 0x%08x\n",
+		sdhci_readl(host, SDHCI_INT_ENABLE),
+		sdhci_readl(host, SDHCI_SIGNAL_ENABLE));
+	printk(KERN_INFO ": AC12 err: 0x%08x | Slot int: 0x%08x\n",
+		sdhci_readw(host, SDHCI_ACMD12_ERR),
+		sdhci_readw(host, SDHCI_SLOT_INT_STATUS));
+	printk(KERN_INFO ": Caps:     0x%08x | Caps_1:   0x%08x\n",
+		sdhci_readl(host, SDHCI_CAPABILITIES),
+		sdhci_readl(host, SDHCI_CAPABILITIES_1));
+	printk(KERN_INFO ": Cmd:      0x%08x | Max curr: 0x%08x\n",
+		sdhci_readw(host, SDHCI_COMMAND),
+		sdhci_readl(host, SDHCI_MAX_CURRENT));
+	printk(KERN_INFO ": Host ctl2: 0x%08x\n",
+		sdhci_readw(host, SDHCI_HOST_CONTROL2));
+
+	if (host->flags & SDHCI_USE_ADMA)
+		printk(KERN_INFO ": ADMA Err: 0x%08x | ADMA Ptr: 0x%08x\n",
+		       readl(host->ioaddr + SDHCI_ADMA_ERROR),
+		       readl(host->ioaddr + SDHCI_ADMA_ADDRESS));
+
+	printk(KERN_INFO ": ===========================================\n");
+}
+#endif
+
+static int ast_hw_init(struct sdhci_host *host)
+{
+	/* Check slot(s) to be enabled */
+	if ((ast_scu_sdhc1_enabled == 1) && (ast_sdhc_port1_enabled == 0))
+	{
+		host->ioaddr = ioremap(ast_sd_device1.resource[0].start, SZ_4K);
+		host->irq = ast_sd_device1.resource[1].start;
+		host->hw_name = ast_sd_device1.name;
+		ast_sdhc_port1_enabled = 1;
+	}
+	else if ((ast_scu_sdhc2_enabled == 1) && (ast_sdhc_port2_enabled == 0))
+	{
+		host->ioaddr = ioremap(ast_sd_device2.resource[0].start, SZ_4K);
+		host->irq = ast_sd_device2.resource[1].start;
+		host->hw_name = ast_sd_device2.name;
+		ast_sdhc_port2_enabled = 1;
+	}
+	if ((ast_sdhc_port1_enabled == 0) && (ast_sdhc_port2_enabled == 0))
+	{
+		printk (KERN_ERR "Both hardware SD ports can not be enabled now\n");
+		return -ENODEV;
+	}
+
+	/* Assign the hooks required to get MAX and MIN
+	 * clock frequency as the frequency reported by AST
+	 * capability register is very low, which makes the
+	 * high speed(Class 10) cards to not function */
+	host->ops = &ast_sd_ops;
+
+	/* Added the following quirk to inform SD host controller
+	 * not to get clock info from capability registers */
+	host->quirks |= SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN;
+
+#if defined(CONFIG_SOC_AST3200)
+	/* Added following quirks to allow write operation
+	 * support on the SD card, with this the value
+	 * returned by "Write protection switch pin" will
+	 * getting inverted to correct value */
+	host->quirks |= SDHCI_QUIRK_INVERTED_WRITE_PROTECT;
+
+	/* Added following quirk to let sdhci know that timeout
+	 * value used is broken, without this the sdhci reports
+	 * error "Too large timeout requested for CMD25" once
+	 * every second when SD gets mounted as read-write */
+	host->quirks |= SDHCI_QUIRK_BROKEN_TIMEOUT_VAL;
+#endif
+
+#if defined(CONFIG_SOC_AST2500) || defined(CONFIG_SOC_AST2530) || defined(CONFIG_SOC_AST3200)
+	/* Timeout clock frequency is not reported in
+	 * capabilities register, so added following
+	 * quirk to get it from other means */
+	host->quirks |= SDHCI_QUIRK_DATA_TIMEOUT_USES_SDCLK;
+#endif
+
+	return 0;
+}
+
+static int  ast_sd_remove(struct platform_device *pdev)
+{
+	struct mmc_host *mmc = platform_get_drvdata(pdev);
+	struct sdhci_host *host;
+
+	if(mmc == NULL)
+	{
+		printk(KERN_ERR "Couldn't get the driver data\n");
+		return -EINVAL;
+	}
+
+	host = mmc_priv(mmc);
+	sdhci_remove_host(host, 1);
+	iounmap(host->ioaddr);
+	sdhci_free_host(host);
+	platform_set_drvdata(pdev, NULL);
+
+	return 0;
+}
+
+static int  ast_sd_probe(struct platform_device *pdev)
+{
+	struct sdhci_host *host = NULL;
+	int ret = -EINVAL;
+
+	host = sdhci_alloc_host(&pdev->dev, sizeof(struct sdhci_host));
+	if (IS_ERR(host))
+	{
+		printk(KERN_ERR "Cannot allocate host\n");
+		return ret;
+	}
+	platform_set_drvdata(pdev, host->mmc);
+
+	ret = ast_hw_init(host);
+	if(ret != 0)
+	{
+		printk(KERN_ERR "Failed to initialize AST SD/MMC Hardware\n");
+		goto ast_probe_exit;
+	}
+
+	ret = sdhci_add_host(host);
+	if(ret != 0)
+	{
+		printk(KERN_ERR "Failed to add host\n");
+		goto ast_probe_exit;
+	}
+
+ast_probe_exit:
+	if(ret != 0 && host != NULL)
+	{
+		sdhci_free_host(host);
+		platform_set_drvdata(pdev, NULL);
+	}
+
+	return ret;
+}
+
+static int __init ast_sd_init(void)
+{
+	uint32_t reg;
+	
+	/* Unlock SCU */
+	iowrite32(SCU_UNLOCK_MAGIC, (void __iomem*)SCU_KEY_CONTROL_REG);
+
+	/* Enable reset of SDHC */
+	reg = ioread32((void __iomem*)SCU_SYS_RESET_REG);
+	reg |= SD_RESET_BIT;
+	iowrite32(reg, (void __iomem*)SCU_SYS_RESET_REG);
+	mdelay(10);
+
+	/* Enable SDHC clock */
+	reg = ioread32((void __iomem*)SCU_CLK_STOP_REG);
+	reg &= ~SD_CLOCK_BIT;
+	iowrite32(reg, (void __iomem*)SCU_CLK_STOP_REG);
+	mdelay(10);
+
+	/* Set clock of SDHC */
+	reg = ioread32((void __iomem*)SCU_CLK_SELECT_REG);
+	reg |= SD_CLOCK_RUN;
+	iowrite32(reg, (void __iomem*)SCU_CLK_SELECT_REG);
+	mdelay(10);
+
+	/* Set clock divider of SDHC */
+	reg = ioread32((void __iomem*)SCU_CLK_SELECT_REG);
+	reg &= ~SD_CLOCK_DIVIDER_MASK;
+	reg |= (SD_CLOCK_DIVIDER_SELECTION << 12);
+	iowrite32(reg, (void __iomem*)SCU_CLK_SELECT_REG);
+	mdelay(10);
+
+	/* Disable reset of SDHC */
+	reg = ioread32((void __iomem*)SCU_SYS_RESET_REG);
+	reg &= ~SD_RESET_BIT;
+	iowrite32(reg, (void __iomem*)SCU_SYS_RESET_REG);
+
+	/* Lock SCU */
+	iowrite32(0, (void __iomem*)SCU_KEY_CONTROL_REG);
+
+	/* Initiate software reset */
+	reg = ioread32((void __iomem*)AST_SDHC_VA_BASE + AST_SDHC_INFO);
+	reg |= SDIO_ALL_SOFTWARE_RESET;
+	iowrite32(reg, (void __iomem*)AST_SDHC_VA_BASE + AST_SDHC_INFO);
+
+	barrier();
+
+	/* Wait for software reset to finish */
+	do
+	{
+		reg = ioread32((void __iomem*)AST_SDHC_VA_BASE + AST_SDHC_INFO);
+	} while (reg & SDIO_ALL_SOFTWARE_RESET);
+
+	/* Set debounce value */
+	iowrite32(0x1000, (void __iomem*)AST_SDHC_VA_BASE + AST_SDHC_DEBOUNCE);
+
+	/* Read SCU90 to check if SD is enabled */
+	if ((ast_scu_sdhc1_enabled == 0) && (ast_scu_sdhc2_enabled == 0))
+	{
+		reg = ioread32((void __iomem*)AST_SCU_VA_BASE + 0x90);
+		if (reg & AST_SCU_MULTI_PIN_SDHC1)
+			ast_scu_sdhc1_enabled = 1;
+		if (reg & AST_SCU_MULTI_PIN_SDHC2)
+			ast_scu_sdhc2_enabled = 1;
+	}
+
+	barrier();
+
+	/* Add probe function for enabled SD alone */	
+	if (ast_scu_sdhc1_enabled == 1)
+	{
+		platform_device_register(&ast_sd_device1);
+		platform_driver_probe(&ast_sd_driver1, ast_sd_probe);
+	}
+
+	if (ast_scu_sdhc2_enabled == 1)
+	{
+		platform_device_register(&ast_sd_device2);
+		platform_driver_probe(&ast_sd_driver2, ast_sd_probe);
+	}
+
+	/* Enable interrupts */
+	if (ast_scu_sdhc1_enabled == 1)
+	{	
+		iowrite32(0x2ff00cb, (void __iomem*)AST_SDHC_VA_BASE + AST_INT_STATUSEN_REG);
+		iowrite32(0x2ff00cb, (void __iomem*)AST_SDHC_VA_BASE + AST_INT_ENABLE_REG);
+	}
+	
+	if (ast_scu_sdhc2_enabled == 1)
+	{
+		iowrite32(0x2ff00cb, (void __iomem*)AST_SDHC_VA_BASE + AST_INT_STATUSEN2_REG);
+		iowrite32(0x2ff00cb, (void __iomem*)AST_SDHC_VA_BASE + AST_INT_ENABLE2_REG);
+	}
+	
+	printk(KERN_INFO "AST SoC SD/MMC Driver Init Success\n");	
+	return 0;
+}
+
+static void __exit ast_sd_exit(void)
+{
+	if (ast_scu_sdhc1_enabled == 1)
+	{
+		platform_device_unregister(&ast_sd_device1);
+		platform_driver_unregister(&ast_sd_driver1);
+	}
+	if (ast_scu_sdhc2_enabled == 1)
+	{
+		platform_device_unregister(&ast_sd_device2);
+		platform_driver_unregister(&ast_sd_driver2);
+	}
+	printk(KERN_INFO "AST SoC SD/MMC Driver Exit Success\n");	
+}
+
+module_init(ast_sd_init);
+module_exit(ast_sd_exit);
+
+MODULE_AUTHOR("American Megatrends Inc.");
+MODULE_LICENSE("GPL");
+MODULE_DESCRIPTION("SD/MMC driver for AST SoC Board");
