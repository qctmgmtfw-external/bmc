--- u-boot-2013.07/common/image.c	2013-12-13 13:49:25.704958498 -0500
+++ uboot.new/common/image.c	2013-12-05 12:17:55.227503948 -0500
@@ -35,6 +35,10 @@
 #include <dataflash.h>
 #endif
 
+#ifdef CONFIG_HAS_SPI
+#include <spiflash.h>
+#endif
+
 #ifdef CONFIG_LOGBUFFER
 #include <logbuff.h>
 #endif
@@ -341,6 +345,11 @@
 
 
 #ifndef USE_HOSTCC
+
+#ifdef CONFIG_YAFU_SUPPORT
+extern int fwupdate(void);
+#endif
+
 /**
  * image_get_ramdisk - get and verify ramdisk image
  * @rd_addr: ramdisk image start address
@@ -367,12 +376,24 @@
 	if (!image_check_magic(rd_hdr)) {
 		puts("Bad Magic Number\n");
 		bootstage_error(BOOTSTAGE_ID_RD_MAGIC);
+#ifndef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+#ifdef CONFIG_YAFU_SUPPORT
+		printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+		fwupdate();
+#endif
+#endif
 		return NULL;
 	}
 
 	if (!image_check_hcrc(rd_hdr)) {
 		puts("Bad Header Checksum\n");
 		bootstage_error(BOOTSTAGE_ID_RD_HDR_CHECKSUM);
+#ifndef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+#ifdef CONFIG_YAFU_SUPPORT
+		printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+		fwupdate();
+#endif
+#endif
 		return NULL;
 	}
 
@@ -384,6 +405,12 @@
 		if (!image_check_dcrc(rd_hdr)) {
 			puts("Bad Data CRC\n");
 			bootstage_error(BOOTSTAGE_ID_RD_CHECKSUM);
+#ifndef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+#ifdef CONFIG_YAFU_SUPPORT
+			printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+			fwupdate();
+#endif
+#endif
 			return NULL;
 		}
 		puts("OK\n");
@@ -397,6 +424,12 @@
 		printf("No Linux %s Ramdisk Image\n",
 				genimg_get_arch_name(arch));
 		bootstage_error(BOOTSTAGE_ID_RAMDISK);
+#ifndef CONFIG_SPX_FEATURE_FAIL_SAFE_BOOTING
+#ifdef CONFIG_YAFU_SUPPORT
+		printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+		fwupdate();
+#endif
+#endif
 		return NULL;
 	}
 
@@ -698,6 +731,61 @@
 ulong genimg_get_image(ulong img_addr)
 {
 	ulong ram_addr = img_addr;
+#ifdef CONFIG_HAS_SPI
+	ulong h_size, d_size;
+
+	if (addr_spi(img_addr)) {
+
+		
+		/* get RAM address */
+		ram_addr = CONFIG_SYS_LOAD_ADDR;
+
+		/* get header size */
+		h_size = image_get_header_size();
+
+#if defined(CONFIG_FIT)
+		if (sizeof(struct fdt_header) > h_size)
+			h_size = sizeof(struct fdt_header);
+#endif
+
+		/* read in header */
+		debug("   Reading image header from spi address "
+			"%08lx to RAM address %08lx\n", img_addr, ram_addr);
+
+		flash_read(img_addr, h_size, (char *)ram_addr);
+
+		/* get data size */
+		switch (genimg_get_format((void *)ram_addr)) {
+		case IMAGE_FORMAT_LEGACY:
+			d_size = image_get_data_size(
+					(const image_header_t *)ram_addr);
+			debug("   Legacy format image found at 0x%08lx, "
+					"size 0x%08lx\n",
+					ram_addr, d_size);
+			break;
+#if defined(CONFIG_FIT)
+		case IMAGE_FORMAT_FIT:
+			d_size = fit_get_size((const void *)ram_addr) - h_size;
+			debug("   FIT/FDT format image found at 0x%08lx, "
+					"size 0x%08lx\n",
+					ram_addr, d_size);
+			break;
+#endif
+		default:
+			printf("   No valid image found at 0x%08lx\n",
+				img_addr);
+			return ram_addr;
+		}
+
+		/* read in image data */
+		debug("   Reading image remaining data from spi address "
+			"%08lx to RAM address %08lx\n", img_addr + h_size,
+			ram_addr + h_size);
+
+		flash_read(img_addr + h_size, d_size,
+				(char *)(ram_addr + h_size));
+	}
+#endif 
 
 #ifdef CONFIG_HAS_DATAFLASH
 	ulong h_size, d_size;
