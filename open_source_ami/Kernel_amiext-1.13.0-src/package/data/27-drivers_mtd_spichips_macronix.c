--- linux-2.6.28.10/drivers/mtd/spichips/macronix.c	1970-01-01 08:00:00.000000000 +0800
+++ linux/drivers/mtd/spichips/macronix.c	2011-07-01 16:26:21.000000000 +0800
@@ -0,0 +1,174 @@
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
+#define CMD_MX25XX_RDSCUR		0x2B	/* Read security register */
+
+/* Security register*/ 
+#define SCUR_BIT2			0x04
+
+#define ADDRESS_3BYTE	0x00
+#define ADDRESS_4BYTE	0x01
+#define ADDRESS_LO3_HI4_BYTE 0x02
+#define MX25L25x35E_MFR_ID 0xC2 
+#define MX25L25x35E_DEV_ID 0x1920
+
+
+/* Name, ID1, ID2 , Size, Clock, Erase regions, address mode,{ Offset, Erase Size, Erase Block Count } */
+/* address mode:  0x00 -3 byte address
+			 	0x01 - 4 byte address	
+			 	0x02 - Low byte: 3 byte address, High byte: 4 byte address*/
+static struct spi_flash_info macronix_data [] = 
+{
+	/* Macronix 64 K Sectors */
+	{ "Macronix MX25L6405D" , 0xC2, 0x1720, 0x800000 , 66 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 128 },} },
+	{ "Macronix MX25L12805D", 0xC2, 0x1820, 0x1000000, 50 * 1000000, 1, 0x00, {{ 0, 64  * 1024, 256 },} },
+   	{ "Macronix MX25L25635E", 0xC2, 0x1920, 0x2000000, 50 * 1000000, 1, 0x02, {{ 0, 64  * 1024, 512 },} },
+    	{ "Macronix MX25L25735E", 0xC2, 0x1920, 0x2000000, 50 * 1000000, 1, 0x01, {{ 0, 64  * 1024, 512 },} },      
+
+};
+
+/*to dinstinguish between MX25L25635E and MX25L25735E*/
+static int read_security_register(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = CMD_MX25XX_RDSCUR;
+	int retval;
+	unsigned char scur_reg;
+	
+	/* Issue Controller Transfer Routine*/ 
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,&scur_reg, 1);
+	if (retval < 0)
+	{
+		printk ("Could not read security register\n");
+		return -1;
+		
+	}
+
+	/*0x00 - 3 byte mode	 
+	   0x04 - 4 byte mode*/
+	scur_reg &= SCUR_BIT2;
+	
+	if(scur_reg == 0x04)
+		return ADDRESS_4BYTE;
+	else
+		return ADDRESS_LO3_HI4_BYTE;
+
+	return 0;
+}	
+
+static
+int 
+macronix_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	int address_mode = 0;
+	int i=0;
+	
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"macronix",
+						macronix_data,ARRAY_SIZE(macronix_data));	
+
+	if (retval == -1)
+		return retval;
+
+	/*  MX25L25635E or MX25L25735E */
+	if((chip_info->mfr_id == MX25L25x35E_MFR_ID) && (chip_info->dev_id == MX25L25x35E_DEV_ID))
+	{
+		address_mode = read_security_register(bank, ctrl_drv);
+
+		if (address_mode == -1)
+			return address_mode;
+			
+		if(chip_info->address32 != address_mode)
+		{
+			memset(chip_info,0,sizeof(struct spi_flash_info));
+			for (i = 0; i < (ARRAY_SIZE(macronix_data)); i++)
+			{
+				if((macronix_data[i].mfr_id == MX25L25x35E_MFR_ID) && (macronix_data[i].dev_id == MX25L25x35E_DEV_ID))
+				{
+					if(macronix_data[i].address32 == address_mode)
+					{
+							
+					   break;
+					}
+				}
+	
+			}
+			memcpy(chip_info,&macronix_data[i],sizeof(struct spi_flash_info));
+		
+		}
+	
+	}
+
+	/* UnProctect all sectors */
+ 	/* SRWD=0 (Bit 7)  BP0,BP1,BP2 = 0 (Bit 2,3,4) */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("macronix: Unable to Unprotect all sectors\n");
+
+	return retval;
+}
+
+struct spi_chip_driver macronix_driver =
+{
+	.name 		= "macronix",
+	.module 	= THIS_MODULE,
+	.probe	 	= macronix_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+macronix_init(void)
+{
+	init_MUTEX(&macronix_driver.lock);
+#ifdef __UBOOT__	/* MIPS */
+	macronix_driver.probe	 		= macronix_probe;
+	macronix_driver.erase_sector 	= spi_generic_erase;
+	macronix_driver.read_bytes	= spi_generic_read;
+	macronix_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&macronix_driver);
+	return 0;
+}
+
+
+void 
+macronix_exit(void)
+{
+	init_MUTEX(&macronix_driver.lock);
+	unregister_spi_chip_driver(&macronix_driver);
+	return;
+}
+
+
+module_init(macronix_init);
+module_exit(macronix_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Macronix flash chips");
+
+#endif
