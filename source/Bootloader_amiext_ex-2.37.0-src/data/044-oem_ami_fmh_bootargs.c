--- u-boot-2013.07/oem/ami/fmh/bootargs.c	2015-09-21 01:16:07.510031306 -0400
+++ uboot/oem/ami/fmh/bootargs.c	2015-09-18 16:06:16.036566991 -0400
@@ -0,0 +1,81 @@
+# include <common.h>
+# include <config.h>
+
+int Get_bootargs(char *bootargs,int rootisinitrd,int RootMtdNo,int mmc)
+{
+
+#ifdef  CONFIG_SPX_FEATURE_MMC_BOOT 
+	char *bootselect = NULL;
+	char bootpart[256] = { 0 };
+	int len = 0;
+
+	len = strlen(CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION);
+	bootselect = getenv("sdbootselector");
+	if(bootselect != NULL)
+	{
+		strncpy(bootpart, CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION, (len-1));
+		if(strncmp(bootselect, "2", sizeof("2")) == 0)
+			strncat(bootpart, "2", sizeof("2"));
+		else
+			strncat(bootpart, "1", sizeof("1"));
+	}
+	else
+	{
+		strncpy(bootpart, CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION, len);
+	}
+
+	if (strspn(bootpart,"mmcblk0") == 7)
+	{
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530) || defined(CONFIG_PILOT4)
+		if ((CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT == 0) && (CONFIG_SPX_FEATURE_GLOBAL_EMMC_FLASH_COUNT == 1))
+			sprintf(bootargs,"root=/dev/%s rootdelay=2 ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart,(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+		else
+#endif
+		{
+			sprintf(bootargs,"root=/dev/%s ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart,(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+		}
+	}
+	else
+		sprintf(bootargs,"root=/dev/%s rootdelay=2 ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart,(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+
+	return 0;
+
+#if 0
+	char *bootpart = CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION;
+	char *bootargsstring = NULL;
+	if (mmc)
+	{
+		if ((bootargsstring=getenv("bootargs")) != NULL)
+		{
+			sprintf(bootargs,"%s ", bootargsstring);
+		}
+		else
+		{
+			if (strspn(bootpart,"mmcblk0") == 7)
+			{
+#if defined(CONFIG_AST2500) || defined(CONFIG_AST2530) || defined(CONFIG_PILOT4)
+				if ((CONFIG_SPX_FEATURE_GLOBAL_SD_SLOT_COUNT == 0) && (CONFIG_SPX_FEATURE_GLOBAL_EMMC_FLASH_COUNT == 1))
+					sprintf(bootargs,"root=/dev/%s rootdelay=2 ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart, (CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+				else
+#endif
+				{
+					sprintf(bootargs,"root=/dev/%s ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart, (CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+				}
+			}
+			else
+				sprintf(bootargs,"root=/dev/%s rootdelay=2 ro ip=none ramdisk_blocksize=4096 mem=%dM ",bootpart, (CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+		}
+			
+		return 0;
+
+	}
+#endif
+#endif
+
+
+	if(rootisinitrd == 1)
+		sprintf(bootargs,"root=/dev/ram0 ro ip=none ramdisk_blocksize=4096 mem=%dM ",(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+	else
+		sprintf(bootargs,"root=/dev/mtdblock%d ro ip=none mem=%dM ",RootMtdNo,(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+	return 0;
+}
