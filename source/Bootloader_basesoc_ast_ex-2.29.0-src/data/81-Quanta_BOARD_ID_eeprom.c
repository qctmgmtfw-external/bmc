--- uboot/common/board_r.c	2013-07-23 19:58:13.000000000 +0800
+++ uboot/common/board_r.c.new	2016-03-11 14:17:12.000000000 +0800
@@ -711,6 +711,107 @@
 	return 0;
 }
 
+#ifdef CONFIG_BOARDID_IN_EEPROM
+static int boardid_init (void)
+{
+	int    i, offs, retry, ret = 0;
+	uchar  fru_buf[256], chksum;
+	char   boardid_env[8], boardid_tmp[8];
+	memset(boardid_env,0,8); memset(boardid_tmp,0,8); 
+	eeprom_init();
+
+	sprintf(boardid_env, "%02X", 0xFF);
+	do {
+		// read Common Header
+		retry = 0;
+		while (++retry <= 3) {
+			if ((ret = eeprom_read(CONFIG_SYS_EEPROM_ADDR, CFG_EEPROM_FRU_OFFSET, fru_buf, 8)) == 0)
+				break;
+		}
+		if (ret != 0)
+			break;
+
+		chksum = 0;
+		for (i = 0; i < 8; i++) {
+			chksum += fru_buf[i];
+		}
+		if (chksum != 0)
+			break;
+
+		offs = fru_buf[3] * 8;
+		if ((fru_buf[3] == 0) || (offs >= sizeof(fru_buf)))
+			break;
+
+		// read Board Info Area
+		retry = 0;
+		while (++retry <= 3) {
+			if ((ret = eeprom_read(CONFIG_SYS_EEPROM_ADDR, CFG_EEPROM_FRU_OFFSET+offs, fru_buf, 8)) == 0)
+				break;
+		}
+		if (ret != 0)
+			break;
+
+		if ((offs + fru_buf[1]*8) >= sizeof(fru_buf))
+			break;
+
+		offs += CFG_EEPROM_FRU_OFFSET;
+		for (i = 1; i < fru_buf[1]; i++) {
+			retry = 0;
+			while (++retry <= 3) {
+				if ((ret = eeprom_read(CONFIG_SYS_EEPROM_ADDR, offs+i*8, fru_buf+i*8, 8)) == 0)
+					break;
+			}
+			if (ret != 0)
+				break;
+		}
+		if (i != fru_buf[1])
+			break;
+
+		offs = fru_buf[1] * 8;
+		chksum = 0;
+		for (i = 0; i < offs; i++) {
+			chksum += fru_buf[i];
+		}
+		if (chksum != 0)
+			break;
+
+		offs = 7 + (fru_buf[6] & 0x3F);  // Board Product Name
+		for (i = 0; i < 4; i++) {
+			offs += 1 + (fru_buf[offs] & 0x3F);
+		}
+
+		// Custom Field 1
+		if (fru_buf[offs] == 0xC1)
+			break;
+		offs += 1 + (fru_buf[offs] & 0x3F);
+
+		// Custom Field 2
+		if (fru_buf[offs] == 0xC1)
+			break;
+		offs += 1 + (fru_buf[offs] & 0x3F);
+
+		// Custom Field 3
+		if ((fru_buf[offs] == 0xC1) || ((fru_buf[offs] & 0x3F) != 2))
+			break;
+		offs += 1;
+		memcpy(boardid_env, &fru_buf[offs], 2);
+	} while (0);
+
+	memcpy(boardid_tmp, getenv("boardid"), 2);
+	
+
+	if (memcmp(boardid_env, boardid_tmp, 2) != 0)
+	{
+		setenv("boardid", boardid_env);
+		saveenv();
+		memcpy(boardid_tmp, getenv("boardid"), 2);
+	}
+		
+	puts("  BOARDID");	
+	printf (" :%s(Hex)\n",boardid_tmp);
+	return 0;
+}
+#endif
 /*
  * Over time we hope to remove these functions with code fragments and
  * stub funtcions, and instead call the relevant function directly.
@@ -914,6 +1015,9 @@
 #ifdef CONFIG_MODEM_SUPPORT
 	initr_modem,
 #endif
+#ifdef CONFIG_BOARDID_IN_EEPROM
+	boardid_init,
+#endif
 	run_main_loop,
 };
 
