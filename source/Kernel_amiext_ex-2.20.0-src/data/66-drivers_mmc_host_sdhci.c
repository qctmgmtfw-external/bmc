--- linux-3.14.17/drivers/mmc/host/sdhci.c	2014-08-13 21:38:34.000000000 -0400
+++ linux/drivers/mmc/host/sdhci.c	2015-07-28 15:24:20.046712357 -0400
@@ -197,28 +197,36 @@
 	if (host->ops->platform_reset_enter)
 		host->ops->platform_reset_enter(host, mask);
 
-	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
-
-	if (mask & SDHCI_RESET_ALL) {
-		host->clock = 0;
-		/* Reset-all turns off SD Bus Power */
-		if (host->quirks2 & SDHCI_QUIRK2_CARD_ON_NEEDS_BUS_ON)
-			sdhci_runtime_pm_bus_off(host);
+	if(host->ami_quirks & SDHCI_AMI_QUIRK_NO_SOFTWARE_RESET)
+	{
+		if (mask & SDHCI_RESET_ALL)
+			host->clock = 0;
 	}
+	else
+	{
+		sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
 
-	/* Wait max 100 ms */
-	timeout = 100;
-
-	/* hw clears the bit when it's done */
-	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
-		if (timeout == 0) {
-			pr_err("%s: Reset 0x%x never completed.\n",
-				mmc_hostname(host->mmc), (int)mask);
-			sdhci_dumpregs(host);
-			return;
+		if (mask & SDHCI_RESET_ALL) {
+			host->clock = 0;
+			/* Reset-all turns off SD Bus Power */
+			if (host->quirks2 & SDHCI_QUIRK2_CARD_ON_NEEDS_BUS_ON)
+				sdhci_runtime_pm_bus_off(host);
+		}
+
+		/* Wait max 100 ms */
+		timeout = 100;
+
+		/* hw clears the bit when it's done */
+		while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
+			if (timeout == 0) {
+				pr_err("%s: Reset 0x%x never completed.\n",
+					mmc_hostname(host->mmc), (int)mask);
+				sdhci_dumpregs(host);
+				return;
+			}
+			timeout--;
+			mdelay(1);
 		}
-		timeout--;
-		mdelay(1);
 	}
 
 	if (host->ops->platform_reset_exit)
@@ -462,6 +470,7 @@
 	local_irq_restore(*flags);
 }
 
+
 static void sdhci_set_adma_desc(u8 *desc, u32 addr, int len, unsigned cmd)
 {
 	__le32 *dataddr = (__le32 __force *)(desc + 4);
@@ -732,16 +741,22 @@
 	u8 ctrl;
 	struct mmc_data *data = cmd->data;
 	int ret;
+	uint32_t reg; //winston
 
 	WARN_ON(host->data);
 
-	if (data || (cmd->flags & MMC_RSP_BUSY)) {
-		count = sdhci_calc_timeout(host, cmd);
-		sdhci_writeb(host, count, SDHCI_TIMEOUT_CONTROL);
+	if(!(host->ami_quirks & SDHCI_AMI_QUIRK_NO_TIMEOUT_CNTL_REG))
+	{
+		if (data || (cmd->flags & MMC_RSP_BUSY)) {
+			count = sdhci_calc_timeout(host, cmd);
+			sdhci_writeb(host, count, SDHCI_TIMEOUT_CONTROL);
+		}
 	}
 
 	if (!data)
+       {   
 		return;
+       }
 
 	/* Sanity checks */
 	BUG_ON(data->blksz * data->blocks > 524288);
@@ -886,10 +901,23 @@
 
 	sdhci_set_transfer_irqs(host);
 
-	/* Set the DMA boundary value and block size */
-	sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
+	if(host->ami_quirks & SDHCI_AMI_QUIRK_BROKEN_BLK_CNTL_REG)
+	{
+		/* We do not handle DMA boundaries, so set it to max (512 KiB)*/ 
+		reg = ioread32(host->ioaddr + SDHCI_BLOCK_SIZE);
+		reg &= ~(0x1FFFFF); /* clear bits[20:0] */
+		reg |= (data->blksz & 0xFFF);
+		if (data->blocks != 512)
+			reg |= (data->blocks << 12);
+		iowrite32(reg, host->ioaddr + SDHCI_BLOCK_SIZE);
+	}
+	else
+	{
+		/* Set the DMA boundary value and block size */
+		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
 		data->blksz), SDHCI_BLOCK_SIZE);
-	sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
+		sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
+	}
 }
 
 static void sdhci_set_transfer_mode(struct sdhci_host *host,
@@ -996,14 +1024,21 @@
 	/* Wait max 10 ms */
 	timeout = 10;
 
-	mask = SDHCI_CMD_INHIBIT;
-	if ((cmd->data != NULL) || (cmd->flags & MMC_RSP_BUSY))
-		mask |= SDHCI_DATA_INHIBIT;
-
-	/* We shouldn't wait for data inihibit for stop commands, even
-	   though they might use busy signaling */
-	if (host->mrq->data && (cmd == host->mrq->data->stop))
-		mask &= ~SDHCI_DATA_INHIBIT;
+	if(host->ami_quirks & SDHCI_AMI_QUIRK_INHIBIT_ABSENT)
+	{
+		mask = SDHCI_DOING_WRITE | SDHCI_DOING_READ;
+	}
+	else
+	{
+		mask = SDHCI_CMD_INHIBIT;
+		if ((cmd->data != NULL) || (cmd->flags & MMC_RSP_BUSY))
+			mask |= SDHCI_DATA_INHIBIT;
+
+		/* We shouldn't wait for data inihibit for stop commands, even
+		   though they might use busy signaling */
+		if (host->mrq->data && (cmd == host->mrq->data->stop))
+			mask &= ~SDHCI_DATA_INHIBIT;
+	}
 
 	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
 		if (timeout == 0) {
@@ -1026,6 +1061,7 @@
 	mod_timer(&host->timer, timeout);
 
 	host->cmd = cmd;
+	host->busy_handle = 0;
 
 	sdhci_prepare_data(host, cmd);
 
@@ -1050,8 +1086,17 @@
 	else
 		flags = SDHCI_CMD_RESP_SHORT;
 
-	if (cmd->flags & MMC_RSP_CRC)
-		flags |= SDHCI_CMD_CRC;
+	if(host->ami_quirks & SDHCI_AMI_QUIRK_RSP_136_IGN_CRC_ERR)
+	{
+		/* Some SoC SDHC generates CRC error with 136-bit response */
+		if ((cmd->flags & MMC_RSP_CRC) && !(cmd->flags & MMC_RSP_136)) 
+			flags |= SDHCI_CMD_CRC;
+	}
+	else
+	{
+		if (cmd->flags & MMC_RSP_CRC) 
+			flags |= SDHCI_CMD_CRC;
+	}
 	if (cmd->flags & MMC_RSP_OPCODE)
 		flags |= SDHCI_CMD_INDEX;
 
@@ -1060,7 +1105,15 @@
 	    cmd->opcode == MMC_SEND_TUNING_BLOCK_HS200)
 		flags |= SDHCI_CMD_DATA;
 
-	sdhci_writew(host, SDHCI_MAKE_CMD(cmd->opcode, flags), SDHCI_COMMAND);
+	if(host->ami_quirks & SDHCI_AMI_QUIRK_RSP_136_ONLY)
+	{
+		sdhci_writew(host, SDHCI_MAKE_CMD(cmd->opcode, flags), SDHCI_COMMAND);
+		sdhci_writew(host, SDHCI_MAKE_CMD(cmd->opcode, flags) | 0x8000, SDHCI_COMMAND);
+	}
+	else
+	{
+		sdhci_writew(host, SDHCI_MAKE_CMD(cmd->opcode, flags), SDHCI_COMMAND);
+	}
 }
 EXPORT_SYMBOL_GPL(sdhci_send_command);
 
@@ -1070,22 +1123,40 @@
 
 	BUG_ON(host->cmd == NULL);
 
-	if (host->cmd->flags & MMC_RSP_PRESENT) {
-		if (host->cmd->flags & MMC_RSP_136) {
-			/* CRC is stripped so we need to do some shifting. */
-			for (i = 0;i < 4;i++) {
-				host->cmd->resp[i] = sdhci_readl(host,
-					SDHCI_RESPONSE + (3-i)*4) << 8;
-				if (i != 3)
-					host->cmd->resp[i] |=
-						sdhci_readb(host,
-						SDHCI_RESPONSE + (3-i)*4-1);
+	if(!(host->ami_quirks & SDHCI_AMI_QUIRK_RSP_136_ONLY))
+	{
+		if (host->cmd->flags & MMC_RSP_PRESENT) {
+			if (host->cmd->flags & MMC_RSP_136) {
+				/* CRC is stripped so we need to do some shifting. */
+				for (i = 0;i < 4;i++) {
+					host->cmd->resp[i] = sdhci_readl(host,
+						SDHCI_RESPONSE + (3-i)*4) << 8;
+					if (i != 3)
+						host->cmd->resp[i] |=
+							sdhci_readb(host,
+							SDHCI_RESPONSE + (3-i)*4-1);
+				}
+			} else {
+				host->cmd->resp[0] = sdhci_readl(host, SDHCI_RESPONSE);
+			}
+		}
+	}
+	else
+	{
+		if (host->cmd->flags & MMC_RSP_PRESENT) {
+			if (host->cmd->flags & MMC_RSP_136) {
+				/* CRC is stripped so we need to do some shifting. */
+				for (i = 0;i < 4;i++) {
+					host->cmd->resp[i] = readl(host->ioaddr + SDHCI_RESPONSE + (3-i)*4);
+				}
+				host->cmd->resp[3] &= 0xFFFFFF00;
+			} else {
+				host->cmd->resp[0] = readl(host->ioaddr + SDHCI_RESPONSE + 4);
 			}
-		} else {
-			host->cmd->resp[0] = sdhci_readl(host, SDHCI_RESPONSE);
 		}
 	}
 
+
 	host->cmd->error = 0;
 
 	/* Finished CMD23, now send actual command. */
@@ -2282,8 +2353,13 @@
 		if (host->cmd->data)
 			DBG("Cannot wait for busy signal when also "
 				"doing a data transfer");
-		else if (!(host->quirks & SDHCI_QUIRK_NO_BUSY_IRQ))
+		else if (!(host->quirks & SDHCI_QUIRK_NO_BUSY_IRQ)
+			&& !host->busy_handle) 
+               	{
+			/* Mark that command complete before busy is ended */
+			host->busy_handle = 1;
 			return;
+		}
 
 		/* The controller does not support the end-of-busy IRQ,
 		 * fall through and take the SDHCI_INT_RESPONSE */
@@ -2339,6 +2415,7 @@
 	}
 
 	if (!host->data) {
+
 		/*
 		 * The "data complete" interrupt is also used to
 		 * indicate that a busy state has ended. See comment
@@ -2346,7 +2423,15 @@
 		 */
 		if (host->cmd && (host->cmd->flags & MMC_RSP_BUSY)) {
 			if (intmask & SDHCI_INT_DATA_END) {
-				sdhci_finish_command(host);
+				/*
+				 * Some cards handle busy-end interrupt
+				 * before the command completed, so make
+				 * sure we do things in the proper order.
+				 */
+				if (host->busy_handle)
+					sdhci_finish_command(host);
+				else
+					host->busy_handle = 1;
 				return;
 			}
 		}
@@ -2782,8 +2867,9 @@
 
 	WARN_ON(host == NULL);
 	if (host == NULL)
+        {   
 		return -EINVAL;
-
+        }
 	mmc = host->mmc;
 
 	if (debug_quirks)
@@ -2800,6 +2886,8 @@
 		pr_err("%s: Unknown controller version (%d). "
 			"You may experience problems.\n", mmc_hostname(mmc),
 			host->version);
+
+        
 	}
 
 	caps[0] = (host->quirks & SDHCI_QUIRK_MISSING_CAPS) ? host->caps :
@@ -2833,6 +2921,10 @@
 		host->flags &= ~SDHCI_USE_ADMA;
 	}
 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_DISABLE_SDHCI_DMA
+	host->flags &= ~(SDHCI_USE_ADMA | SDHCI_USE_SDMA);
+#endif
+
 	if (host->flags & (SDHCI_USE_SDMA | SDHCI_USE_ADMA)) {
 		if (host->ops->enable_dma) {
 			if (host->ops->enable_dma(host)) {
