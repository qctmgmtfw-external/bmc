--- linux_old/drivers/mtd/spichips/micron.c	2015-07-30 17:19:18.247776890 +0800
+++ linux/drivers/mtd/spichips/micron.c	2015-07-30 17:18:55.095776920 +0800
@@ -0,0 +1,104 @@
+/*
+ * Copyright (C) 2007-2013 American Megatrends Inc
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
+/* address mode:  0x00 -3 byte address
+				0x01 - 4 byte address	
+				0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+
+static struct spi_flash_info micron_data [] = 
+{
+	/* Micron 64 K Sectors */
+	{ "Micron/Numonyx N25Q00A"  , 0x20, 0x21BA, 0x00010001, 0x8000000  , 108 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 2048 },} },
+	{ "Micron/Numonyx N25Q512A"  , 0x20, 0x20BA, 0x00010001, 0x4000000 , 108 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 1024 },} },
+	{ "Micron/Numonyx N25Q256A"  , 0x20, 0x19BA, 0x00010001, 0x2000000 , 108 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 512 },} },
+    { "Micron/Numonyx n25q128" , 0x20, 0x18BA, 0x00010001, 0x1000000, 50 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 256 },} },
+};
+
+
+static
+int 
+micron_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"micron",
+						micron_data,ARRAY_SIZE(micron_data));	
+
+	if (retval == -1)
+		return retval;
+	
+	/* UnProctect all sectors */
+ 	/* SRWD=0 (Bit 7)  BP0,BP1,BP2 = 0 (Bit 2,3,4) */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("micron: Unable to Unprotect all sectors\n");
+
+	return retval;
+}
+
+struct spi_chip_driver micron_driver =
+{
+	.name 		= "micron",
+	.module 	= THIS_MODULE,
+	.probe	 	= micron_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+micron_init(void)
+{
+	sema_init(&micron_driver.lock,1);
+#ifdef __UBOOT__	/* MIPS */
+	micron_driver.probe	 		= micron_probe;
+	micron_driver.erase_sector 	= spi_generic_erase;
+	micron_driver.read_bytes	= spi_generic_read;
+	micron_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&micron_driver);
+	return 0;
+}
+
+
+void 
+micron_exit(void)
+{
+	sema_init(&micron_driver.lock,1);
+	unregister_spi_chip_driver(&micron_driver);
+	return;
+}
+
+
+module_init(micron_init);
+module_exit(micron_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for micron flash chips");
+
+#endif
