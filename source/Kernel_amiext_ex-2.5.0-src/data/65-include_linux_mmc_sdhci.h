--- linux-3.14.17/include/linux/mmc/sdhci.h	2014-08-14 01:38:34.000000000 +0000
+++ linux/include/linux/mmc/sdhci.h	2014-09-12 15:35:00.433024723 +0000
@@ -101,6 +101,15 @@
 /* Controller does not support HS200 */
 #define SDHCI_QUIRK2_BROKEN_HS200			(1<<6)
 
+       unsigned int ami_quirks; /* More deviations from spec*/
+
+#define SDHCI_AMI_QUIRK_NO_SOFTWARE_RESET (1<<0)
+#define SDHCI_AMI_QUIRK_INHIBIT_ABSENT (1<<1)
+#define SDHCI_AMI_QUIRK_RSP_136_IGN_CRC_ERR (1<<2)
+#define SDHCI_AMI_QUIRK_RSP_136_ONLY (1<<3)
+#define SDHCI_AMI_QUIRK_NO_TIMEOUT_CNTL_REG (1<<4)
+#define SDHCI_AMI_QUIRK_BROKEN_BLK_CNTL_REG (1<<5)
+
 	int irq;		/* Device IRQ */
 	void __iomem *ioaddr;	/* Mapped address */
 
@@ -150,6 +159,7 @@
 	struct mmc_command *cmd;	/* Current command */
 	struct mmc_data *data;	/* Current data request */
 	unsigned int data_early:1;	/* Data finished before cmd */
+	unsigned int busy_handle:1;	/* Handling the order of Busy-end */
 
 	struct sg_mapping_iter sg_miter;	/* SG state for PIO */
 	unsigned int blocks;	/* remaining PIO blocks */
