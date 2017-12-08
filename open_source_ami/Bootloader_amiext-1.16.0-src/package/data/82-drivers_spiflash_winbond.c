--- u-boot-1.1.6/drivers/spiflash/winbond.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot/drivers/spiflash/winbond.c	2011-06-20 17:40:38.000000000 +0800
@@ -0,0 +1,102 @@
+/*
+ * Copyright (C) 2007 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ */
+
+
+#ifdef __UBOOT__	
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode, { Offset, Erase Size, Erase Block Count } */
+/* address mode: 0x00 -3 byte address
+			      0x01 - 4 byte address	
+			      0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info winbond_data [] = 
+{
+	/* Winbond 64 K Sectors */
+	{ "Winbond W25X64" , 0xEF, 0x1730, 0x800000 , 75  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Winbond W25Q64" , 0xEF, 0x1740, 0x800000 , 80  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Winbond W25Q128", 0xEF, 0x1840, 0x1000000, 104 * 1000000,  1, 0x00, {{ 0, 64  * 1024, 256 },} },
+
+};
+
+
+static
+int 
+winbond_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"winbond",
+						winbond_data,ARRAY_SIZE(winbond_data));	
+
+	if (retval == -1)
+		return retval;
+
+	/* UnProctect all sectors */
+ 	/* SRWD=0 (Bit 7)  BP0,BP1,BP2 = 0 (Bit 2,3,4) */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("winbond: Unable to Unprotect all sectors\n");
+
+	return retval;
+}
+
+struct spi_chip_driver winbond_driver =
+{
+	.name 		= "winbond",
+	.module 	= THIS_MODULE,
+	.probe	 	= winbond_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+winbond_init(void)
+{
+	init_MUTEX(&winbond_driver.lock);
+#ifdef __UBOOT__	/* MIPS */
+	winbond_driver.probe	 		= winbond_probe;
+	winbond_driver.erase_sector 	= spi_generic_erase;
+	winbond_driver.read_bytes	= spi_generic_read;
+	winbond_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&winbond_driver);
+	return 0;
+}
+
+
+void 
+winbond_exit(void)
+{
+	init_MUTEX(&winbond_driver.lock);
+	unregister_spi_chip_driver(&winbond_driver);
+	return;
+}
+
+
+module_init(winbond_init);
+module_exit(winbond_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Winbond flash chips");
+
+#endif
