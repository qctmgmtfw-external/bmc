--- linux.pristine/drivers/mtd/spichips/atmel.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-amiext/drivers/mtd/spichips/atmel.c	2014-02-03 15:10:59.975029095 -0500
@@ -0,0 +1,133 @@
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
+#ifdef __UBOOT__	
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode,{ Offset, Erase Size, Erase Block Count } */
+/* address mode:  0x00 -3 byte address
+			 	0x01 - 4 byte address	
+			 	0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info atmel_data [] = 
+{
+	/* Atmel 26F 64K Sectors */
+	{ "Atmel at26f004"   , 0x1F, 0x0004, 0x00010001, 0x100000 , 25 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 8   },} },
+
+	/* Atmel 25DF 64K Sectors */
+	{ "Atmel at25df041a" , 0x1F, 0x0144, 0x00010001, 0x100000 , 70 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 8   },} },
+
+	/* Atmel 26DF 64K Sectors */
+	{ "Atmel at26df081a" , 0x1F, 0x0145, 0x00010001, 0x100000 , 70 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 16  },} },
+	{ "Atmel at26df161a" , 0x1F, 0x0146, 0x00010001, 0x200000 , 25 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 32  },} },
+	{ "Atmel at26df161"  , 0x1F, 0x0046, 0x00010001, 0x200000 , 25 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 32  },} },
+	{ "Atmel at26df321"  , 0x1F, 0x0047, 0x00010001, 0x400000 , 25 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 64  },} },
+	{ "Atmel at25df321"  , 0x1F, 0x0147, 0x00010001, 0x400000 , 85 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 64  },} },
+};
+
+
+static
+int 
+atmel_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	unsigned char status;
+
+	retval =  spi_generic_probe(bank, ctrl_drv,chip_info,"atmel",atmel_data,ARRAY_SIZE(atmel_data));
+	if (retval == -1)
+		return retval;
+
+	if (spi_generic_read_status(bank,ctrl_drv,&status) < 0)
+	{
+		printk("atmel: Read SR Failed.Cannot Unprotect all sectors\n");
+		return retval;
+	}
+
+	/* If SRPL = 1 (Bit 7)and WP/ = 0 (Bit 4), then it is hardware locked */
+	if ((status & 0x80) && (!(status & 0x10)))
+	{
+		printk("atmel: Hardware Locked\n");
+		return retval;
+	}
+
+	/* Check if already unprotected */
+	if ((status & 0xC) == 0)
+		return retval;
+
+	/* if SRPL is set, we have to disable SRPL before unprotect */
+	if (status & 0x80)
+	{
+		if (spi_generic_write_status(bank,ctrl_drv,status& 0x7F) < 0)
+		{
+			printk("atmel: Clearing SRPL failed .Cannot Unprotect all sectors\n");
+			return retval;
+		}
+	}
+	
+	/* Unprotect all sectors */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("atmel: Unable to Unprotect all sectors\n");
+
+
+	return retval;
+}
+
+struct spi_chip_driver atmel_driver =
+{
+	.name 		= "atmel",
+	.module 	= THIS_MODULE,
+	.probe	 	= atmel_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+	/* Atmel supports individual protect and unprotect of sectors */
+	/* if needed implement the functions and add here */
+};
+
+int 
+atmel_init(void)
+{
+	sema_init(&atmel_driver.lock, 1);
+#ifdef __UBOOT__	/* MIPS */
+	atmel_driver.probe	 		= atmel_probe;
+	atmel_driver.erase_sector 	= spi_generic_erase;
+	atmel_driver.read_bytes		= spi_generic_read;
+	atmel_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&atmel_driver);
+	return 0;
+}
+
+void 
+atmel_exit(void)
+{
+	sema_init(&atmel_driver.lock, 1);
+	unregister_spi_chip_driver(&atmel_driver);
+	return;
+}
+
+module_init(atmel_init);
+module_exit(atmel_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Atmel flash chips");
+
+#endif
