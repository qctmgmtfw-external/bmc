--- linux-3.14.17/drivers/mtd/mtdpart.c	2014-08-14 09:38:34.000000000 +0800
+++ linux.new/drivers/mtd/mtdpart.c	2014-10-14 14:14:32.162279000 +0800
@@ -338,6 +338,7 @@
 
 	return err;
 }
+EXPORT_SYMBOL_GPL(del_mtd_partitions);
 
 static struct mtd_part *allocate_partition(struct mtd_info *master,
 			const struct mtd_partition *part, int partno,
@@ -649,6 +650,7 @@
 
 	return 0;
 }
+EXPORT_SYMBOL_GPL(add_mtd_partitions);
 
 static DEFINE_SPINLOCK(part_parser_lock);
 static LIST_HEAD(part_parsers);
