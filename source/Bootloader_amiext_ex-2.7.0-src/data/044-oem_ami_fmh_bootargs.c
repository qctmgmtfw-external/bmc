--- uboot/oem/ami/fmh/bootargs.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.good/oem/ami/fmh/bootargs.c	2014-07-21 17:13:02.078826632 -0400
@@ -0,0 +1,23 @@
+# include <common.h>
+# include <config.h>
+
+int Get_bootargs(char *bootargs,int rootisinitrd,int RootMtdNo,int mmc)
+{
+
+#ifdef  CONFIG_SPX_FEATURE_MMC_BOOT 
+	if (mmc)
+	{
+		sprintf(bootargs,"root=/dev/%s ro ip=none ramdisk_blocksize=4096 mem=%dM ",CONFIG_SPX_FEATURE_MMC_BOOT_PARTITION,
+																				(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+		return 0;
+
+	}
+#endif
+
+
+	if(rootisinitrd == 1)
+		sprintf(bootargs,"root=/dev/ram0 ro ip=none ramdisk_blocksize=4096 mem=%dM ",(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+	else
+		sprintf(bootargs,"root=/dev/mtdblock%d ro ip=none mem=%dM ",RootMtdNo,(CONFIG_SYS_SDRAM_SYS_USED) >> 20);
+	return 0;
+}
