--- u-boot-2013.07/drivers/mtd/cfi_flash.c	2013-12-13 13:49:25.692958498 -0500
+++ uboot.new/drivers/mtd/cfi_flash.c	2013-12-05 12:17:55.211503948 -0500
@@ -1305,6 +1305,16 @@
 #define FLASH_SHOW_PROGRESS(scale, dots, digit, dots_sub)
 #endif
 
+int read_buff(flash_info_t *info, uchar *dest, ulong addr, ulong cnt)
+{
+	while (cnt--)
+	{
+		*dest = *(uchar *)addr;
+		dest++,addr++;
+	}
+	return 0;
+}
+
 /*-----------------------------------------------------------------------
  * Copy memory to flash, returns:
  * 0 - OK
