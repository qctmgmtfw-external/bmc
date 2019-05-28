--- linux.pristine/drivers/mtd/spichips/winbond.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-amiext/drivers/mtd/spichips/winbond.c	2014-02-03 15:10:59.999029095 -0500
@@ -0,0 +1,146 @@
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
+#define W25Q25x_MFR_ID 0xEF
+#define W25Q25x_DEV_ID 0x1940
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode,{ Offset, Erase Size, Erase Block Count } */
+/* address mode: 0x00 -3 byte address
+			      0x01 - 4 byte address	
+			      0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info winbond_data [] = 
+{
+	/* Winbond 64 K Sectors */
+	{ "Winbond W25X64" , 0xEF, 0x1730, 0x00010007, 0x800000 , 75  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Winbond W25Q32" , 0xEF, 0x1640, 0x0001000F, 0x400000 , 75  * 1000000 , 1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Winbond W25Q64" , 0xEF, 0x1740, 0x0001000F, 0x800000 , 80  * 1000000,  1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Winbond W25Q128", 0xEF, 0x1840, 0x0001000F, 0x1000000, 104 * 1000000,  1, 0x00, {{ 0, 64  * 1024, 256 },} },
+	{ "Winbond W25Q256", 0xEF, 0x1940, 0x0001000F, 0x2000000, 104 * 1000000,  1, 0x02, {{ 0, 64  * 1024, 512 },} },
+	{ "Winbond W25Q257", 0xEF, 0x1940, 0x0001000F, 0x2000000, 104 * 1000000,  1, 0x01, {{ 0, 64  * 1024, 512 },} },
+	{ "Winbond W25M512", 0xEF, 0x1971, 0x0001000f, 0x4000000, 104 * 1000000,  2, 0x06, {{ 0, 64  * 1024, 512 },{ 0x2000000, 64  * 1024, 512 },} },
+};
+
+static
+int read_status_register(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 reg_data;
+	u8 pwr_up_mode = 0;
+	unsigned char command[5]={0};
+	int cmd_size = 0;
+	
+	command[0] = 0x15; // Read Status Register S23 ~ S16
+	cmd_size = 1;
+	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_READ, &reg_data, 1);
+	if (retval < 0)
+	{
+		printk ("Could not read status register\n");
+		return -1;
+	}
+	
+	pwr_up_mode = (reg_data & 0x2)>>1; // S17(ADP) field
+	
+	return pwr_up_mode;
+}
+
+static
+int 
+winbond_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	u8 pwr_up_mode = 0;
+	int i = 0;
+	
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"winbond",
+						winbond_data,ARRAY_SIZE(winbond_data));	
+
+	if (retval == -1)
+		return retval;
+	
+	// W25Q256 and W25Q257 have same ID code, distinguish them by reading status register
+	if((chip_info->mfr_id == W25Q25x_MFR_ID) && (chip_info->dev_id == W25Q25x_DEV_ID))
+	{
+		pwr_up_mode = read_status_register(bank, ctrl_drv);
+		if (pwr_up_mode >= 0)
+		{
+			memset(chip_info,0,sizeof(struct spi_flash_info));
+			for (i = 0; i < (ARRAY_SIZE(winbond_data)); i++)
+			{
+				if ((strncmp(winbond_data[i].name, "Winbond W25Q256", sizeof("Winbond W25Q256")) == 0) && (pwr_up_mode == 0))
+					break;
+				if ((strncmp(winbond_data[i].name, "Winbond W25Q257", sizeof("Winbond W25Q257")) == 0) && (pwr_up_mode == 1))
+					break;
+			}
+			memcpy(chip_info,&winbond_data[i],sizeof(struct spi_flash_info));
+		}
+	}
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
+int 
+winbond_init(void)
+{
+	sema_init(&winbond_driver.lock, 1);
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
+void 
+winbond_exit(void)
+{
+	sema_init(&winbond_driver.lock, 1);
+	unregister_spi_chip_driver(&winbond_driver);
+	return;
+}
+
+module_init(winbond_init);
+module_exit(winbond_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Winbond flash chips");
+
+#endif
