--- uboot.org/drivers/spiflash/generic.c Wed May 20 18:17:42 2015
+++ uboot/drivers/spiflash/generic.c Mon May 25 11:39:00 2015
@@ -57,6 +57,7 @@
 #define ADDRESS_LO3_HI4_BYTE 0x02
 
 
+extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
 static int wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv);
 
 static
@@ -67,6 +68,21 @@
 	return retval;
 }
 
+static int
+spi_generic_read_flag_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = 0x70;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1,0);
+
+	if (retval < 0)
+		return spi_error(retval);
+
+	return 0;
+}
+
 
 
 int
@@ -76,7 +92,7 @@
 	u8 code = OPCODE_RDSR;
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1,0);
 
 	if (retval < 0)
 		return spi_error(retval);
@@ -94,7 +110,7 @@
 	spi_generic_write_enable(bank,ctrl_drv);
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_WRITE,&status, 1);
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_WRITE,&status, 1,0);
 	if (retval < 0)
 		return spi_error(retval);
 
@@ -109,7 +125,7 @@
 	int retval;
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0,0);
 	if (retval < 0)
 		return spi_error(retval);
 	return 0;
@@ -122,7 +138,7 @@
 	int retval;
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0,0);
 	if (retval < 0)
 		return spi_error(retval);
 	return 0;
@@ -143,7 +159,9 @@
 
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((flash_info[bank].flash_id == 0x002019BA) || (flash_info[bank].flash_id == 0x002020BA)) spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0,0);
+	if ((flash_info[bank].flash_id == 0x002019BA) || (flash_info[bank].flash_id == 0x002020BA)) spi_generic_write_disable(bank,ctrl_drv);
 	if (retval < 0)
 	{
 		printk ("Could not Enter into 4-byte address mode\n");
@@ -167,7 +185,9 @@
 
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0);
+	if ((flash_info[bank].flash_id == 0x002019BA) || (flash_info[bank].flash_id == 0x002020BA)) spi_generic_write_enable(bank,ctrl_drv);
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_NONE, NULL, 0,0);
+	if ((flash_info[bank].flash_id == 0x002019BA) || (flash_info[bank].flash_id == 0x002020BA)) spi_generic_write_disable(bank,ctrl_drv);
 	if (retval < 0)
 	{
 		printk ("Could not Exit from 4-byte address mode\n");
@@ -176,6 +196,27 @@
 	return 0;
 }
 
+// the function just for WINBOND W25Q256 only
+int w25q256_force_exrended_address(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	int retval;
+	u8 code;
+	u8 reg_data;
+	u8 command[5];
+
+	code = 0xC8; // "Read Extended Address Register"
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1, 0);
+	if (reg_data == 0x01)
+	{
+		spi_generic_write_enable(bank,ctrl_drv);
+		command[0] = 0xC5; // "Write Extended Address Register" with the force address 0x00
+		command[1] = command[2] = command[3] = command[4] = 0x00;
+		retval = ctrl_drv->spi_transfer(bank, command, 5, SPI_NONE, NULL, 0, 0);
+		spi_generic_write_disable(bank,ctrl_drv);
+	}
+	return 0;
+}
+
 /* Define max times to check status register before we give up. */
 #define	MAX_READY_WAIT_COUNT	4000000
 
@@ -199,10 +240,33 @@
 		}
 	}
 
-	printk("spi_generic: Waiting for Ready Failed\n");
+	printk("spi_generic %s() : Waiting for Ready Failed\n", __func__);
 	return 1;
 }
 
+static int
+require_read_flag_status(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++)
+	{
+		if (spi_generic_read_flag_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else
+		{
+			if (sr & SR_SRWD)
+				return 0;
+		}
+	}
+
+	printk("spi_generic %s() : Waiting for Ready Failed\n", __func__);
+	return 1;
+}
 
 
 
@@ -216,6 +280,7 @@
 	unsigned char command[5];
 	int cmd_size;	
 	u8 address32 = priv->address32;
+	unsigned long flash_size = priv->size;
 	
 	down(&priv->chip_drv->lock);
 	
@@ -239,13 +304,13 @@
 		}
 	}
 
-	/* Send write enable */
-	spi_generic_write_enable(bank,ctrl_drv);
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 
 	if ( ((sect_addr >= ADDR_16MB) && (address32 == ADDRESS_LO3_HI4_BYTE)) || (address32 == ADDRESS_4BYTE))
 	{
 		/* Set up command buffer. */
 		command[0] = OPCODE_SE;
+		if (flash_info[bank].flash_id == 0x00011902) command[0] = 0xDC; // ERASE command in 4byte mode [spansion only]
 		command[1] = sect_addr >> 24;
 		command[2] = sect_addr >> 16;
 		command[3] = sect_addr >> 8;
@@ -264,7 +329,20 @@
 	}
 
 	/* Issue Controller Transfer Routine */
-	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_NONE, NULL, 0);
+	spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
+	retval = ctrl_drv->spi_transfer(bank,command, cmd_size ,SPI_NONE, NULL, 0,flash_size);
+	spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+	if (flash_info[bank].flash_id == 0x002020BA)
+	{
+		/* requires the read flag status with at latest one byte. */
+		if (require_read_flag_status(bank,ctrl_drv))
+		{
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+	}
+
 	if (retval < 0)
 	{
 		//if 4 byte mode exit
@@ -277,6 +355,8 @@
 			}
 		}
 
+		if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 		up(&priv->chip_drv->lock);
 		return spi_error(retval);
 	}
@@ -289,6 +369,8 @@
 			printk ("Unable to exit 4 byte address mode\n");
 		}
 	}
+
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 
 	up(&priv->chip_drv->lock);
 	return retval;
@@ -305,10 +387,10 @@
 	size_t transfer;
 	unsigned char command[6];
 	int cmd_size;
-	int  (*readfn)(int bank,unsigned char *,int , SPI_DIR, unsigned char *, unsigned long);
+	int  (*readfn)(int bank,unsigned char *,int , SPI_DIR, unsigned char *, unsigned long, unsigned long);
 	int end_addr = (addr+bytes-1);	
 	u8 address32 = priv->address32;
-	
+	unsigned long flash_size = priv->size;
 	
 	/* Some time zero bytes length are sent */
 	if (bytes==0)
@@ -346,6 +428,8 @@
 		}
 	}
 
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 	while (bytes)
 	{
 		if (ctrl_drv->spi_burst_read)
@@ -363,6 +447,7 @@
 			{
 				/* Set up command buffer. */	/* Normal Read */
 				command[0] = OPCODE_READ;
+				if (flash_info[bank].flash_id == 0x00011902) command[0] = 0x13; // READ command in 4byte mode [spansion only]
 				command[1] = addr >> 24;
 				command[2] = addr >> 16;
 				command[3] = addr >> 8;
@@ -381,7 +466,7 @@
 				cmd_size = 4;
 			}
 			/* Issue Controller Transfer Routine */
-			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer, flash_size);
 		}
 		else // Need to check Fast Read in 4 byte address mode
 		{
@@ -389,6 +474,7 @@
 			{ 
 				/* Set up command buffer. */   /* Fast Read */
 				command[0] = OPCODE_FAST_READ;
+				if (flash_info[bank].flash_id == 0x00011902) command[0] = 0x0C; // FAST_READ command in 4byte mode [spansion only]
 				command[1] = addr >> 24;
 				command[2] = addr >> 16;
 				command[3] = addr >> 8;
@@ -409,7 +495,7 @@
 				cmd_size = 5;
 			}
 			/* Issue Controller Transfer Routine */
-			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer);
+			retval = (*readfn)(bank,command, cmd_size ,SPI_READ, buff, (unsigned long)transfer,flash_size);
 		}
 
 		if (retval < 0)
@@ -424,6 +510,8 @@
 				}
 			}
 
+			if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 			up(&priv->chip_drv->lock);
 			return spi_error(retval);
 		}
@@ -444,6 +532,7 @@
 		}
 	}
 
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 
 	up(&priv->chip_drv->lock);
 	return 0;
@@ -463,6 +552,7 @@
 	int cmd_size;
 	int end_addr = (addr+bytes-1);
 	u8 address32 = priv->address32;
+	unsigned long flash_size = priv->size;
 	
 	/* Some time zero bytes length are sent */
 	if (bytes==0)
@@ -482,6 +572,7 @@
 		}
 	}
 
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 
 	while (bytes)
 	{
@@ -491,9 +582,6 @@
 			up(&priv->chip_drv->lock);
 			return -1;
 		}
-
-		/* Send write enable */
-		spi_generic_write_enable(bank,ctrl_drv);
 
 		transfer = PROGRAM_PAGE_SIZE;
 		if (bytes <  transfer)
@@ -503,6 +591,7 @@
 		{
 			/* Set up command buffer. */
 			command[0] = OPCODE_PP;
+			if (flash_info[bank].flash_id == 0x00011902) command[0] = 0x12; // PROGRAM command in 4byte mode [spansion only]
 			command[1] = addr >> 24;
 			command[2] = addr >> 16;
 			command[3] = addr >> 8;
@@ -519,8 +608,21 @@
 		}
 
 		/* Issue Controller Transfer Routine */
+		spi_generic_write_enable(bank,ctrl_drv); /* Send write enable */
 		retval = ctrl_drv->spi_transfer(bank,command,cmd_size ,SPI_WRITE,
-						(unsigned char *)buff, transfer);
+						(unsigned char *)buff, transfer,flash_size);
+		spi_generic_write_disable(bank,ctrl_drv); /* Send write disable */
+
+		if (flash_info[bank].flash_id == 0x002020BA)
+		{
+			/* requires the read flag status with at latest one byte. */
+			if (require_read_flag_status(bank,ctrl_drv))
+			{
+				up(&priv->chip_drv->lock);
+				return -1;
+			}
+		}
+
 		if (retval < 0)
 		{
 			//if 4 byte mode exit
@@ -533,6 +635,8 @@
 				}
 			}
 
+			if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 			up(&priv->chip_drv->lock);
 			return spi_error(retval);
 		}
@@ -550,6 +654,8 @@
 			printk ("Unable to exit 4 byte address mode\n");
 		}
 	}
+
+	if (flash_info[bank].flash_id == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 
 	up(&priv->chip_drv->lock);
 	return 0;
@@ -577,7 +683,7 @@
 	
 	/* Issue Controller Transfer Routine */
 	val = 0;
-	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,(unsigned char *)&val, 3);
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,(unsigned char *)&val, 3,0);
 	val &= 0x00FFFFFF;
 
 	if (retval < 0)
@@ -585,6 +691,11 @@
 		spi_error(retval);
 		return -1;
 	}
+
+	/* Send write disable */
+	retval = spi_generic_write_disable(bank,ctrl_drv);
+	if (retval < 0)
+		return -1;
 
 	/* Match the ID against the table entries */
 	for (i = 0; i < spi_list_len; i++)
