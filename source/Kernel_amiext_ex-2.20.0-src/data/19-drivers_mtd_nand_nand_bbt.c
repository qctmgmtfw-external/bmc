--- linux.pristine/drivers/mtd/nand/nand_bbt.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/drivers/mtd/nand/nand_bbt.c	2014-02-03 15:11:17.175028950 -0500
@@ -1249,6 +1249,7 @@
 #define BADBLOCK_SCAN_MASK (~NAND_BBT_NO_OOB)
 /**
  * nand_create_badblock_pattern - [INTERN] Creates a BBT descriptor structure
+ * @mtd : MTD device structure
  * @this: NAND chip to create descriptor for
  *
  * This function allocates and initializes a nand_bbt_descr for BBM detection
@@ -1256,7 +1257,7 @@
  * this->badblock_pattern. Thus, this->badblock_pattern should be NULL when
  * passed to this function.
  */
-static int nand_create_badblock_pattern(struct nand_chip *this)
+static int nand_create_badblock_pattern(struct mtd_info *mtd, struct nand_chip *this)
 {
 	struct nand_bbt_descr *bd;
 	if (this->badblock_pattern) {
@@ -1268,7 +1269,13 @@
 		return -ENOMEM;
 	bd->options = this->bbt_options & BADBLOCK_SCAN_MASK;
 	bd->offs = this->badblockpos;
-	bd->len = (this->options & NAND_BUSWIDTH_16) ? 2 : 1;
+	if (mtd->writesize > 512) {
+		bd->options |= NAND_BBT_SCAN2NDPAGE;
+		bd->len = 1;
+	}
+	else {
+		bd->len = (this->options & NAND_BUSWIDTH_16) ? 2 : 1;
+	}
 	bd->pattern = scan_ff_pattern;
 	bd->options |= NAND_BBT_DYNAMICSTRUCT;
 	this->badblock_pattern = bd;
@@ -1304,7 +1311,7 @@
 	}
 
 	if (!this->badblock_pattern)
-		nand_create_badblock_pattern(this);
+		nand_create_badblock_pattern(mtd, this);
 
 	return nand_scan_bbt(mtd, this->badblock_pattern);
 }
