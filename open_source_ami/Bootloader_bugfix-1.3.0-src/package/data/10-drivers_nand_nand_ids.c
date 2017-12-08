--- uboot/drivers/nand/nand_ids.c	2006-11-02 09:15:01.000000000 -0500
+++ uboot.new/drivers/nand/nand_ids.c	2008-10-07 11:18:59.000000000 -0400
@@ -67,7 +67,7 @@
 
 	{"NAND 256MiB 3,3V 8-bit", 	0x71, 512, 256, 0x4000, 0},
 
-	{"NAND 512MiB 3,3V 8-bit", 	0xDC, 512, 512, 0x4000, 0},
+//	{"NAND 512MiB 3,3V 8-bit", 	0xDC, 512, 512, 0x4000, 0},
 
 	/* These are the new chips with large page size. The pagesize
 	* and the erasesize is determined from the extended id bytes
