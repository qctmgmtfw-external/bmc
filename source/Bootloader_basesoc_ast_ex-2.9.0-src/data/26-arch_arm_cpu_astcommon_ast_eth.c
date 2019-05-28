--- uboot/arch/arm/cpu/astcommon/ast_eth.c	2015-11-06 20:09:12.318094471 +0800
+++ uboot.new/arch/arm/cpu/astcommon/ast_eth.c	2015-11-06 20:10:01.003718363 +0800
@@ -223,15 +223,67 @@
 }
 
 #ifdef CONFIG_MACADDR_IN_EEPROM
+// Quanta ------ 
+#define CFG_I2C_CHANNEL_ID	CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENABLE_I2C_BUS /* channel number in PRJ */
+#define AST_I2C_CHANNEL_BASE ((CFG_I2C_CHANNEL_ID < 7)? 0x1E78A040 + (0x040 * (CFG_I2C_CHANNEL_ID)) : 0x1E78A300 + (0x040 * (CFG_I2C_CHANNEL_ID - 7)))
+#define I2C_FCR		(AST_I2C_CHANNEL_BASE + 0x00) /* function control register */
+#define I2C_CSR		(AST_I2C_CHANNEL_BASE + 0x14) /* command/status register */
+void i2c_recovery(void)
+{
+	int i;
+   printf("\ni2c_recovery\n");	
+	*((volatile unsigned long *) I2C_FCR) = 0;  //disable bus
+	
+	for (i=0;i<8;i++)  //clear status
+	{
+		//clock low
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<13);
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)&~(1<<12);
+		udelay(5);
+		//clock high
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<13);
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<12);
+		udelay(5);
+		
+		if((*((volatile unsigned long *) I2C_CSR)&(1<<17))!=0)
+			break;
+	}
+	
+	if(((*((volatile unsigned long *) I2C_CSR)&(1<<17))==0)||((*((volatile unsigned long *) I2C_CSR)&(1<<18))==0))
+	{
+		//force stop
+		//data low
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<15);
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)&~(1<<14);
+		//clock high
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<13);
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<12);
+		//data high
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<15);
+		*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR)|(1<<14);
+		udelay(5);
+	}
+	
+	//clock/data stop
+	*((volatile unsigned long *) I2C_CSR) = *((volatile unsigned long *) I2C_CSR) &~((1<<15)|(1<<13));
+	
+	*((volatile unsigned long *) I2C_FCR) = 3;  //enable bus
+	
+}
+// ------ Quanta
+
 static void ast_eth_read_addr_from_eeprom(int id, bd_t *bis)
 {
 	unsigned char mac_address[ETH_ALEN];
 	char ethaddr_env[24];
 	int valid_mac_addr_in_eeprom;
+	char *tmp = NULL, ethaddr_tmp[24];  // Quanta
 	int i;
 
+	memset ((void *)mac_address, 0xFF, ETH_ALEN);	// Quanta
 	eeprom_init();
-	if (id == 0) {
+	// Quanta ------
+	/*if (id == 0) {
 		eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MACADDR_OFFSET, mac_address, ETH_ALEN);
 	} else if (id == 1) {
 		eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MAC1ADDR_OFFSET, mac_address, ETH_ALEN);
@@ -248,7 +300,34 @@
         eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MAC3ADDR_OFFSET, mac_address, ETH_ALEN);    
     }
 #endif    
+#endif*/
+	for(i=0;i<3;i++) {
+		if (id == 0) {
+			if(eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MACADDR_OFFSET, mac_address, ETH_ALEN)==0)
+				break;
+		} else if (id == 1){ 
+			if(eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MAC1ADDR_OFFSET, mac_address, ETH_ALEN)==0)
+				break;
+		}
+#if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT >= 3)
+
+#ifdef CONFIG_EEPROM_MAC2ADDR_OFFSET    
+		else if (id == 2) {
+			if(eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MAC2ADDR_OFFSET, mac_address, ETH_ALEN)==0)
+				break;
+		}
+#endif
+#ifdef CONFIG_EEPROM_MAC3ADDR_OFFSET     
+		else if (id == 3) {
+			if(eeprom_read(CONFIG_SYS_EEPROM_ADDR, CONFIG_EEPROM_MAC3ADDR_OFFSET, mac_address, ETH_ALEN)==0)
+				break;
+		}
+#endif    
 #endif
+		i2c_recovery();
+ 	}
+	printf("\nast_eth_read_addr_from_eeprom id=%d i=%d mac= %02X:%02X:%02X:%02X:%02X:%02X\n",id,i,mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
+	// ------ Quanta
 
 	valid_mac_addr_in_eeprom = 0;
 	for (i = 0; i < ETH_ALEN; i ++) {
@@ -261,27 +340,39 @@
 	if (valid_mac_addr_in_eeprom) {
 		sprintf(ethaddr_env, "%02X:%02X:%02X:%02X:%02X:%02X", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);
 		if (id == 0) {
+			if ((tmp = getenv("ethaddr")))     // Quanta
+				memcpy(ethaddr_tmp, tmp, 17);  // Quanta
 			memcpy(bis->bi_enetaddr, mac_address,6);
 			setenv("ethaddr", ethaddr_env);
 		}
 #if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT >= 2) 
 		else if (id == 1) {
+			if ((tmp = getenv("eth1addr")))     // Quanta
+				memcpy(ethaddr_tmp, tmp, 17);  // Quanta
 			memcpy(bis->bi_enet1addr, mac_address,6);
 			setenv("eth1addr", ethaddr_env);
 		} 
 #if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT >= 3)
 		else if (id == 2) {
+			if ((tmp = getenv("eth2addr")))     // Quanta
+				memcpy(ethaddr_tmp, tmp, 17);  // Quanta
 			memcpy(bis->bi_enet2addr, mac_address,6);
 			setenv("eth2addr", ethaddr_env);
 		}
 #if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT >= 4) 
 		else if (id == 3) {
+			if ((tmp = getenv("eth3addr")))     // Quanta
+				memcpy(ethaddr_tmp, tmp, 17);  // Quanta
 			memcpy(bis->bi_enet3addr, mac_address,6);
 			setenv("eth3addr", ethaddr_env);
 		}
 #endif
 #endif
 #endif		
+		// Quanta +++
+		if (memcmp(ethaddr_env, ethaddr_tmp, 17) == 0)
+			return;
+		// +++ Quanta
 		saveenv();
 	}
 }
@@ -305,18 +396,24 @@
 static void ast_eth_set_multipin(struct eth_device* dev)
 {
 	struct ast_eth_priv *priv;
+#if (!defined(CONFIG_NCSI_ETH0) || !defined(CONFIG_NCSI_ETH1))  // Quanta
 	unsigned int scu_reg;
+#endif                                                          // Quanta
 	
 	*((volatile ulong *) SCU_KEY_CONTROL_REG) = 0x1688A8A8; /* unlock SCU */
 	priv = (struct ast_eth_priv *) dev->priv;
 	if (priv->id == 0) {
+#ifndef CONFIG_NCSI_ETH0  // Quanta
 		scu_reg = *((volatile ulong *) (AST_SCU_VA_BASE + 0x88));	/* set MAC1 multi-function pin */
 		scu_reg |= 0xC0000000;	
 		*((volatile ulong *) (AST_SCU_VA_BASE + 0x88)) = scu_reg;
+#endif
 	} else if (priv->id == 1) {
+#ifndef CONFIG_NCSI_ETH1  // Quanta
 		scu_reg = *((volatile ulong *) (AST_SCU_VA_BASE + 0x90));	/* set MAC2 multi-function pin */
 		scu_reg |= 0x00000004;	
 		*((volatile ulong *) (AST_SCU_VA_BASE + 0x90)) = scu_reg;
+#endif
 	}
 	*((volatile ulong *) SCU_KEY_CONTROL_REG) = 0; /* lock SCU */
 }
@@ -774,7 +871,7 @@
 		} while (reg & MACCR_SW_RST);
 
 		/* Enable MII Pins */
-		ast_eth_set_mac_ctrl(dev);
+		//ast_eth_set_mac_ctrl(dev);
 
 #if defined (CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
 		/* register mii command access routines */
