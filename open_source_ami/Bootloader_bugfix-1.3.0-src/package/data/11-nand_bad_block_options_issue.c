diff -Naur uboot/drivers/nand/nand_bbt.c uboot.new/drivers/nand/nand_bbt.c
--- uboot/drivers/nand/nand_bbt.c	2006-11-02 09:15:01.000000000 -0500
+++ uboot.new/drivers/nand/nand_bbt.c	2008-10-27 13:40:17.000000000 -0400
@@ -915,9 +915,9 @@
 };
 
 static struct nand_bbt_descr largepage_memorybased = {
-	.options = 0,
+	.options = NAND_BBT_SCAN2NDPAGE,
 	.offs = 0,
-	.len = 2,
+	.len = 1,
 	.pattern = scan_ff_pattern
 };
 
