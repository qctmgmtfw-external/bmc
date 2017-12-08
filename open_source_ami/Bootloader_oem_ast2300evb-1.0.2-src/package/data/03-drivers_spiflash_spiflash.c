--- uboot.org/drivers/spiflash/spiflash.c Wed May 20 18:17:42 2015
+++ uboot/drivers/spiflash/spiflash.c Mon May 25 15:30:22 2015
@@ -223,7 +223,9 @@
 extern int winbond_init(void);
 extern int s33_init(void);
 extern int default_init(void);
-
+#ifdef POST_SPI_INIT
+extern void post_spi_init(int bank, struct spi_flash_info *spi_info);
+#endif
 
 /* Called by env_spi before console is opened */
 void
@@ -298,7 +300,7 @@
 
 		flash_info[bank].start[0] = CFG_FLASH_BASE+totalsize;
 	    	flash_info[bank].size = spi_info.size;
-		flash_info[bank].flash_id = 0;	/* TODO : Fill proper value */
+		flash_info[bank].flash_id = ((spi_info.mfr_id << 16) | spi_info.dev_id);
 	
 		sector=0;
 		start = CFG_FLASH_BASE + totalsize;
@@ -323,9 +325,13 @@
 		private[bank].interleave = 1;
 		private[bank].device_type = DEVICE_TYPE_X8;
 		private[bank].address32 = spi_info.address32;
+		private[bank].size = spi_info.size;
 		dev_map[bank].fldrv_priv = &private[bank];
 		dev_map[bank].map_priv_1= bank;
 		configure_spi_clock(bank, spi_info.max_clock);
+#ifdef POST_SPI_INIT
+		post_spi_init(bank, &spi_info);
+#endif
 	}
 
 	
