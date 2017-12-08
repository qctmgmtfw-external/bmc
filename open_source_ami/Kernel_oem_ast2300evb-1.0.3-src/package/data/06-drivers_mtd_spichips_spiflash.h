--- linux.org/drivers/mtd/spichips/spiflash.h Wed May 20 18:18:14 2015
+++ linux/drivers/mtd/spichips/spiflash.h Fri May 22 17:07:11 2015
@@ -165,5 +165,10 @@
 int spi_generic_probe(int bank, struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info,
 			char *spi_name,struct spi_flash_info *spi_list, int spi_list_len);
 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS
+#define MAX_SPI_BANKS	(CONFIG_SPX_FEATURE_GLOBAL_FLASH_BANKS + CONFIG_SPX_FEATURE_GLOBAL_BKUP_FLASH_BANKS)
+#else
+#define MAX_SPI_BANKS	CONFIG_SPX_FEATURE_GLOBAL_FLASH_BANKS
+#endif
 
 #endif
