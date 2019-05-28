--- u-boot-2013.07/common/cmd_mem.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/cmd_mem.c	2013-12-05 12:17:55.223503948 -0500
@@ -36,6 +36,9 @@
 #include <watchdog.h>
 #include <asm/io.h>
 #include <linux/compiler.h>
+#ifdef CONFIG_HAS_SPI
+#include <spiflash.h>
+#endif
 
 DECLARE_GLOBAL_DATA_PTR;
 
@@ -54,6 +57,33 @@
 
 static	ulong	base_address = 0;
 
+#ifdef CONFIG_HAS_SPI
+void spi_perror (int err)
+{
+	switch (err) {
+	case ERR_OK:
+		break;
+	case ERR_TIMOUT:
+		printf ("Timeout writing to DataFlash\n");
+		break;
+	case ERR_PROTECTED:
+		printf ("Can't write to protected DataFlash sectors\n");
+		break;
+	case ERR_INVAL:
+		printf ("Outside available DataFlash\n");
+		break;
+	case ERR_UNKNOWN_FLASH_TYPE:
+		printf ("Unknown Type of DataFlash\n");
+		break;
+	case ERR_PROG_ERROR:
+		printf ("General DataFlash Programming Error\n");
+		break;
+	default:
+		printf ("%s[%d] FIXME: rc=%d\n", __FILE__, __LINE__, err);
+		break;
+	}
+}
+#endif
 /* Memory Display
  *
  * Syntax:
@@ -63,7 +93,7 @@
 static int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
 {
 	ulong	addr, length;
-#if defined(CONFIG_HAS_DATAFLASH)
+#if defined(CONFIG_HAS_DATAFLASH) || defined(CONFIG_HAS_SPI)
 	ulong	nbytes, linebytes;
 #endif
 	int	size;
@@ -98,7 +128,7 @@
 			length = simple_strtoul(argv[2], NULL, 16);
 	}
 
-#if defined(CONFIG_HAS_DATAFLASH)
+#if defined(CONFIG_HAS_DATAFLASH) || defined(CONFIG_HAS_SPI)
 	/* Print the lines.
 	 *
 	 * We buffer all read data, so we can make sure data is read only
@@ -110,8 +140,13 @@
 		void* p;
 		linebytes = (nbytes>DISP_LINE_LEN)?DISP_LINE_LEN:nbytes;
 
+#ifdef CONFIG_HAS_DATAFLASH
 		rc = read_dataflash(addr, (linebytes/size)*size, linebuf);
 		p = (rc == DATAFLASH_OK) ? linebuf : (void*)addr;
+#else
+		rc = flash_read(addr, (linebytes/size)*size, linebuf);
+		p = (rc == ERR_OK) ? linebuf : (void*)addr;
+#endif
 		print_buffer(addr, p, size, linebytes/size, DISP_LINE_LEN/size);
 
 		nbytes -= linebytes;
@@ -303,6 +338,13 @@
 	}
 #endif
 
+#ifdef CONFIG_HAS_SPI
+	if (addr_spi(addr1) | addr_spi(addr2)){
+		puts ("Comparison with SPI space not supported.\n\r");
+		return 0;
+	}
+#endif
+
 #ifdef CONFIG_BLACKFIN
 	if (addr_bfin_on_chip_mem(addr1) || addr_bfin_on_chip_mem(addr2)) {
 		puts ("Comparison with L1 instruction memory not supported.\n\r");
@@ -383,6 +425,9 @@
 #ifdef CONFIG_HAS_DATAFLASH
 	   && (!addr_dataflash(dest))
 #endif
+#ifdef CONFIG_HAS_SPI
+	   && (!addr_spi(addr))
+#endif
 	   ) {
 		int rc;
 
@@ -436,6 +481,44 @@
 	}
 #endif
 
+#ifdef CONFIG_HAS_SPI
+	/* Check if we are copying from RAM or Flash to SPI */
+	if (addr_spi(dest) && !addr_spi(addr)){
+		int rc;
+
+		puts ("Copy to spi... ");
+
+		rc = flash_write ((char *)addr, dest, count * size);
+
+		if (rc != 1) {
+			spi_perror (rc);
+			return (1);
+		}
+		puts ("done\n");
+		return 0;
+	}
+
+	/* Check if we are copying from DataFlash to RAM */
+	if (addr_spi(addr) && !addr_spi(dest)
+#ifndef CONFIG_SYS_NO_FLASH
+				 && (addr2info(dest) == NULL)
+#endif
+	){
+		int rc;
+		rc = flash_read(addr, count * size, (char *) dest);
+		if (rc != 1) {
+			spi_perror (rc);
+			return (1);
+		}
+		return 0;
+	}
+
+	if (addr_spi(addr) && addr_spi(dest)){
+		puts ("Unsupported combination of source/destination.\n\r");
+		return 1;
+	}
+#endif
+
 #ifdef CONFIG_BLACKFIN
 	/* See if we're copying to/from L1 inst */
 	if (addr_bfin_on_chip_mem(dest) || addr_bfin_on_chip_mem(addr)) {
@@ -656,6 +739,14 @@
 		0x00000015,	/* three non-adjacent bits */
 		0x00000055,	/* four non-adjacent bits */
 		0xaaaaaaaa,	/* alternating 1/0 */
+		0xffffffff,	/* all 1s */
+		0xdeadbeef,	/* misc pattern */
+		0x00000000,	/* all 0s */
+		0xbaadf00d,	/* misc pattern */
+		0xff00ff00,	/* nibble check */
+		0x00ff00ff,	/* nibble check */
+		0x0f0f0f0f,	/* nibble check */
+		0xf0f0f0f0,	/* nibble check */
 	};
 
 	num_words = (end_addr - start_addr) / sizeof(vu_long);
@@ -1051,6 +1142,13 @@
 		return 0;
 	}
 #endif
+
+#ifdef CONFIG_HAS_SPI
+	if (addr_spi(addr)){
+		puts ("Can't modify SPI in place. Use cp instead.\n\r");
+		return 0;
+	}
+#endif
 
 #ifdef CONFIG_BLACKFIN
 	if (addr_bfin_on_chip_mem(addr)) {
