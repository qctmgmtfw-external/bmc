--- uboot/drivers/mmc/sdhci.c	2015-08-13 14:06:45.085958300 +0800
+++ uboot.new/drivers/mmc/sdhci.c	2015-08-24 11:56:24.846725000 +0800
@@ -136,7 +136,9 @@
 	u32 mask, flags, mode;
 	unsigned int timeout, start_addr = 0;
 	unsigned int retry = 10000;
-
+#ifdef CONFIG_AST2500
+	unsigned int make_sure = 0;
+#endif
 	/* Wait max 10 ms */
 	timeout = 10;
 
@@ -210,11 +212,27 @@
 	}
 
 	sdhci_writel(host, cmd->cmdarg, SDHCI_ARGUMENT);
+#ifdef CONFIG_AST2500
+	if (host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD){
+		do{
+			make_sure = sdhci_readl(host, SDHCI_PRESENT_STATE);
+		}while((make_sure & 0x00F00000) != 0x00F00000);
+		
+		sdhci_writel(host, 0x1ffffff, SDHCI_INT_STATUS);
+
+		udelay(1000);
+	}
+#endif
+	
 #ifdef CONFIG_MMC_SDMA
 	flush_cache(start_addr, trans_bytes);
 #endif
 	sdhci_writew(host, SDHCI_MAKE_CMD(cmd->cmdidx, flags), SDHCI_COMMAND);
 	do {
+#ifdef CONFIG_AST2500
+		if ((host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD) && (host->mmc->op_cond_pending == 1))
+			udelay(1000);
+#endif
 		stat = sdhci_readl(host, SDHCI_INT_STATUS);
 		if (stat & SDHCI_INT_ERROR)
 			break;
