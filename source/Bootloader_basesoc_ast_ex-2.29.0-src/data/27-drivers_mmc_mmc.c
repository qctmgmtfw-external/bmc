--- uboot/drivers/mmc/mmc.c	2015-08-13 14:06:53.554973600 +0800
+++ uboot.new/drivers/mmc/mmc.c	2015-08-24 10:55:57.888446000 +0800
@@ -578,20 +578,34 @@
 {
 	struct mmc_cmd cmd;
 	int timeout = 1000;
+#ifndef CONFIG_AST2500
 	uint start;
+#endif
 	int err;
 
 	mmc->op_cond_pending = 0;
+#ifndef CONFIG_AST2500
 	start = get_timer(0);
+#endif
 	do {
 		err = mmc_send_op_cond_iter(mmc, &cmd, 1);
 		if (err)
 			return err;
+#ifdef CONFIG_AST2500
+		udelay(1000);
+	} while (!(mmc->op_cond_response & OCR_BUSY) && timeout--);
+#else
 		if (get_timer(start) > timeout)
 			return UNUSABLE_ERR;
 		udelay(100);
 	} while (!(mmc->op_cond_response & OCR_BUSY));
+#endif
 
+#ifdef CONFIG_AST2500
+	if (timeout <= 0)
+		return UNUSABLE_ERR;
+#endif
+		
 	if (mmc_host_is_spi(mmc)) { /* read OCR for spi */
 		cmd.cmdidx = MMC_CMD_SPI_READ_OCR;
 		cmd.resp_type = MMC_RSP_R3;
@@ -1356,6 +1370,9 @@
 int mmc_start_init(struct mmc *mmc)
 {
 	int err;
+#ifdef CONFIG_AST2500
+	int retries = 5;
+#endif
 
 	if (mmc_getcd(mmc) == 0) {
 		mmc->has_init = 0;
@@ -1391,8 +1408,16 @@
 
 	/* If the command timed out, we check for an MMC card */
 	if (err == TIMEOUT) {
+#ifdef CONFIG_AST2500	
+		// Force resend CMD1 to deal with Card busy 
+		do{
+			err = mmc_send_op_cond(mmc);
+
+			retries--;
+		}while(!(retries <= 0));
+#else
 		err = mmc_send_op_cond(mmc);
-
+#endif
 		if (err && err != IN_PROGRESS) {
 			printf("Card did not respond to voltage select!\n");
 			return UNUSABLE_ERR;
