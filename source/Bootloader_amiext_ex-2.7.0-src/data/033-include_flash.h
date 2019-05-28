--- u-boot-2013.07/include/flash.h	2013-12-13 13:49:25.800958498 -0500
+++ uboot.new/include/flash.h	2013-12-05 12:17:55.247503948 -0500
@@ -105,8 +105,10 @@
 /* common/flash.c */
 extern void flash_protect (int flag, ulong from, ulong to, flash_info_t *info);
 extern int flash_write (char *, ulong, ulong);
+extern int flash_read (ulong, ulong ,char *);
 extern flash_info_t *addr2info (ulong);
 extern int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt);
+extern int read_buff(flash_info_t *info, uchar *dest, ulong addr, ulong cnt);
 
 /* drivers/mtd/cfi_mtd.c */
 #ifdef CONFIG_FLASH_CFI_MTD
