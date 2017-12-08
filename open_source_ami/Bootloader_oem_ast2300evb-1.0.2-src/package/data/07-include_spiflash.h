--- uboot.org/include/spiflash.h Wed May 20 18:17:42 2015
+++ uboot/include/spiflash.h Mon May 25 15:28:21 2015
@@ -112,11 +112,11 @@
 
 	/* spi_transfer can be used for all type of spi access */
 	int  (*spi_transfer)(int bank,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
-				unsigned char *data, unsigned long datalen);
+				unsigned char *data, unsigned long datalen, unsigned long flash_size);
 
 	/* spi_burst_read is not NULL, if the ctrl supports read large data continuosly */
 	int  (*spi_burst_read)(int bank,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
-				unsigned char *data, unsigned long  datalen);
+				unsigned char *data, unsigned long  datalen, unsigned long flashsize);
 	
 #ifdef __UBOOT__   
 	int  (*spi_init)(void);
@@ -149,6 +149,7 @@
 	int numchips;
 	unsigned long chipshift;
 	__u8 address32;
+	u_long size;
 	struct flchip chips;
 	struct spi_chip_driver *chip_drv;
 	struct spi_ctrl_driver *ctrl_drv;
