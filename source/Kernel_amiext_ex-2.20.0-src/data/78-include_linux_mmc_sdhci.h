--- linux/include/linux/mmc/sdhci.h	2014-08-14 01:38:34.000000000 +0000
+++ linux.new/include/linux/mmc/sdhci.h	2014-09-12 15:35:00.433024723 +0000
@@ -194,5 +194,29 @@
 	struct timer_list	tuning_timer;	/* Timer for tuning */
 
 	unsigned long private[0] ____cacheline_aligned;
+	
+#if defined(CONFIG_SOC_SE_PILOT4)
+	u32 expected_irqs;
+	u32 sdhc_id;
+	u32 id;
+	u32 bus_width;
+	u32 power_mode;
+
+	struct mmc_command *stop;
+
+	struct 
+	{
+		int len;
+		int dir;
+	} dma;
+
+	unsigned long timer_val;
+	unsigned long timed_out;
+	unsigned long timer_added;
+	//LMEM registers for SD
+	unsigned char * lmem_v;
+	unsigned long lmem_p;
+	unsigned long lmem_length;	
+#endif	
 };
 #endif /* LINUX_MMC_SDHCI_H */
