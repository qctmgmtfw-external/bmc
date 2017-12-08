--- linux.org/drivers/mtd/spichips/spimtd.c Wed May 20 18:18:14 2015
+++ linux/drivers/mtd/spichips/spimtd.c Fri May 22 17:14:07 2015
@@ -28,6 +28,9 @@
 	.name = "spi_probe",
 	.module = THIS_MODULE
 };
+
+unsigned long ractrends_spiflash_flash_id[MAX_SPI_BANKS];
+unsigned long ractrends_spiflash_flash_size[MAX_SPI_BANKS];
 
 /*------------------------------------------------------------------------------*/
 /*                               Probe Function       			                */
@@ -133,6 +136,10 @@
 		return NULL;
 	}
 
+	/* Fill flash ID and size in public array */
+	ractrends_spiflash_flash_id[map->map_priv_1] = ((spi_info.mfr_id << 16) | spi_info.dev_id);
+	ractrends_spiflash_flash_size[map->map_priv_1] = spi_info.size;
+
 	configure_spi_clock(private->ctrl_drv, map->map_priv_1, spi_info.max_clock);
 
 	chips.start = 0;
