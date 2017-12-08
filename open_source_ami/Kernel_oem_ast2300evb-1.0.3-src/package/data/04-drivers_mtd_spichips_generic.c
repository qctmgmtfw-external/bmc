--- linux.org/drivers/mtd/spichips/generic.c Wed May 20 18:18:14 2015
+++ linux/drivers/mtd/spichips/generic.c Mon May 25 16:47:21 2015
@@ -59,7 +59,7 @@
 
 
 
-
+extern unsigned long ractrends_spiflash_flash_id[MAX_SPI_BANKS];
 static int wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv);
 
 static 
@@ -179,6 +179,26 @@
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
+	retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &reg_data, 1);
+	if (reg_data == 0x01)
+	{
+		spi_generic_write_enable(bank,ctrl_drv);
+		command[0] = 0xC5; // "Write Extended Address Register" with the force address 0x00
+		command[1] = command[2] = command[3] = command[4] = 0x00;
+		retval = ctrl_drv->spi_transfer(bank, command, 5, SPI_NONE, NULL, 0);
+		spi_generic_write_disable(bank,ctrl_drv);
+	}
+	return 0;
+}
 
 /* Define max times to check status register before we give up. */
 #define	MAX_READY_WAIT_COUNT	4000000
@@ -241,10 +261,12 @@
 				return spi_error(retval);
 		}
 	}
-	
+
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 	/* Send write enable */
 	spi_generic_write_enable(bank,ctrl_drv);
-	
+
 	if ( ((sect_addr >= ADDR_16MB) && (address32 == ADDRESS_LO3_HI4_BYTE)) || (address32 == ADDRESS_4BYTE))
 	{
 		/* Set up command buffer. */
@@ -282,6 +304,7 @@
 			}
 		}
 		
+		if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 		up(&priv->chip_drv->lock);
 		return spi_error(retval);
 	}
@@ -296,6 +319,7 @@
 		}
 	}
 	
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 	up(&priv->chip_drv->lock);
 	return retval;
 }
@@ -349,6 +373,7 @@
 		}
 	}
 	
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
 	while (bytes)
 	{
 		if (ctrl_drv->spi_burst_read)
@@ -431,6 +456,8 @@
 				}
 			}
 			
+			if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 			up(&priv->chip_drv->lock);
 			return spi_error(retval);
 		}
@@ -452,6 +479,8 @@
 		}
 	}
 	
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 	up(&priv->chip_drv->lock);
 	//kfree(spi_info);
 	return 0;
@@ -491,6 +520,8 @@
 		}
 	}
 	
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 	while (bytes)
 	{
 		/* Wait until finished previous command. */
@@ -544,6 +575,8 @@
 				}
 			}
 			
+			if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 			up(&priv->chip_drv->lock);
 			return spi_error(retval);
 		}
@@ -563,6 +596,8 @@
 		}
 	}
 	
+	if (ractrends_spiflash_flash_id[bank] == 0x00EF1940) w25q256_force_exrended_address(bank, ctrl_drv);
+
 	up(&priv->chip_drv->lock);
 	return 0;
 }
