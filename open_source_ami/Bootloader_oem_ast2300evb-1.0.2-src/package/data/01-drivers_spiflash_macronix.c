--- uboot.org/drivers/spiflash/macronix.c	Mon Nov 10 16:07:00 2014
+++ uboot/drivers/spiflash/macronix.c	Mon Nov 10 17:18:21 2014
@@ -28,6 +28,11 @@
 /* Security register*/ 
 #define SCUR_BIT2			0x04
 
+#define CMD_MX25XX_RDCR                 0x15    /* Read configuration register */
+
+/* Configuration register */
+#define CR_BIT5                         0x20
+
 #define ADDRESS_3BYTE	0x00
 #define ADDRESS_4BYTE	0x01
 #define ADDRESS_LO3_HI4_BYTE 0x02
@@ -49,7 +54,7 @@
 
 };
 
-/*to dinstinguish between MX25L25635E and MX25L25735E*/
+/* to dinstinguish between MX25L25635/MX25L25735 E and F type */
 static
 int read_security_register(int bank, struct spi_ctrl_driver *ctrl_drv)
 {
@@ -70,10 +75,40 @@
 	   0x04 - 4 byte mode*/
 	scur_reg &= SCUR_BIT2;
 	
-	if(scur_reg == 0x04)
-		return ADDRESS_4BYTE;
+        if(scur_reg == SCUR_BIT2)
+                return ADDRESS_4BYTE;           // MX25L25735E
+        else
+                return ADDRESS_LO3_HI4_BYTE;    // MX25L25635E/F or MX25L25735F
+
+        return 0;
+}
+
+/* to dinstinguish between MX25L25635/MX25L25735 F type */
+static
+int read_configuration_register(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+        u8 code = CMD_MX25XX_RDCR;
+        int retval;
+        unsigned char conf_reg;
+
+        /* Issue Controller Transfer Routine*/ 
+        retval = ctrl_drv->spi_transfer(bank, &code, 1, SPI_READ, &conf_reg, 1);
+        if (retval < 0)
+        {
+                printk ("Could not read configuration register\n");
+                return -1;
+        }
+
+        if (conf_reg == 0xFF) conf_reg = 0x00;  // invalid value (maybe unsupported the RDCR command)
+
+        /* 0x00 - 3 byte mode    
+           0x04 - 4 byte mode */
+        conf_reg &= CR_BIT5;
+
+        if (conf_reg == CR_BIT5)
+                return ADDRESS_4BYTE;           // MX25L25735F
 	else
-		return ADDRESS_LO3_HI4_BYTE;
+		return ADDRESS_LO3_HI4_BYTE;    // MX25L25635E/F
 
 	return 0;
 }	
@@ -92,10 +127,11 @@
 	if (retval == -1)
 		return retval;
 
-	/* MX25L25635E or MX25L25735E */
+	/* MX25L25635E/F or MX25L25735E/E */
 	if((chip_info->mfr_id == MX25L25x35E_MFR_ID) && (chip_info->dev_id == MX25L25x35E_DEV_ID))
 	{
 		address_mode = read_security_register(bank, ctrl_drv);
+                if (address_mode == ADDRESS_LO3_HI4_BYTE) address_mode = read_configuration_register(bank, ctrl_drv);
 
 		if (address_mode == -1)
 			return address_mode;
