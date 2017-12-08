--- linux.org/drivers/mtd/spichips/winbond.c Wed May 20 18:18:14 2015
+++ linux/drivers/mtd/spichips/winbond.c Fri May 22 16:38:44 2015
@@ -33,6 +33,7 @@
 	{ "Winbond W25X64" , 0xEF, 0x1730, 0x800000 , 75  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
 	{ "Winbond W25Q64" , 0xEF, 0x1740, 0x800000 , 80  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
 	{ "Winbond W25Q128", 0xEF, 0x1840, 0x1000000, 104 * 1000000,  1, 0x00, {{ 0, 64  * 1024, 256 },} },
+	{ "Winbond W25Q256", 0xEF, 0x1940, 0x2000000, 104 * 1000000,  1, 0x02, {{ 0, 64  * 1024, 512 },} },
 
 };
 
