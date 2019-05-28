--- linux.pristine/drivers/mtd/nand/amimap.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-amiext/drivers/mtd/nand/amimap.c	2014-02-03 15:10:59.939029096 -0500
@@ -0,0 +1,54 @@
+/*
+ * AMI (RACTRENDS) Parition Map for NAND
+ *
+ * Copyright (C) 2008 American Megatrends Inc
+ *
+ */
+
+#include <linux/init.h>
+#include <linux/types.h>
+#include <linux/module.h>
+#include <linux/moduleparam.h>
+#include <linux/vmalloc.h>
+#include <linux/slab.h>
+#include <linux/errno.h>
+#include <linux/string.h>
+#include <linux/mtd/mtd.h>
+#include <linux/mtd/nand.h>
+#include <linux/mtd/partitions.h>
+#include <linux/delay.h>
+
+
+int
+CreateNandPartitions(struct mtd_partition *part,unsigned long nandsize)
+{
+	/* Minimum Required is 32M */
+	if (nandsize < 32*1024*1024)
+		return 0;
+		
+	part[1].name 	= "Kernel";
+	part[1].offset 	= 0;
+	part[1].size 	= 8*1024*1024;
+
+	part[2].name 	= "Config";
+	part[2].offset 	= 8*1024*1024;
+	part[2].size 	= 8*1024*1024;
+	
+	part[3].name 	= "Root Fs";
+	part[3].offset 	= 16*1024*1024;
+	part[3].size 	= 12*1024*1024;
+
+	part[4].name 	= "Web Fs";
+	part[4].offset 	= 28*1024*1024;
+	part[4].size 	= 4 *1024 *1024;
+
+	if (nandsize > (32 *1024 *1024))
+	{
+		part[5].name 	= "User Area";
+		part[5].offset 	= 32*1024*1024;
+		part[5].size 	= nandsize - (32*1024*1024);
+		return 5;
+	}
+
+	return 4;
+}
