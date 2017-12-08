--- uboot.org/cpu/arm926ejs/AST2300/ast_spiflash.c Wed May 20 18:17:43 2015
+++ uboot/cpu/arm926ejs/AST2300/ast_spiflash.c Mon May 25 15:20:02 2015
@@ -182,7 +182,7 @@
 	return 0;
 }
 
-static int ast_spiflash_transfer(int bank, unsigned char *cmd, int cmdlen, SPI_DIR dir, unsigned char *data, unsigned long datalen)
+static int ast_spiflash_transfer(int bank, unsigned char *cmd, int cmdlen, SPI_DIR dir, unsigned char *data, unsigned long datalen,unsigned long flashsize)
 {
 	ulong base;
 	int i;
