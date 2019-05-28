--- u-boot-2013.07/common/flash.c	2013-12-13 13:49:25.700958498 -0500
+++ uboot.new/common/flash.c	2013-12-05 12:17:55.223503948 -0500
@@ -125,6 +125,43 @@
 	return (NULL);
 }
 
+int
+flash_read (ulong addr, ulong cnt,char* dest)
+{
+	ulong         end        = addr + cnt - 1;
+	flash_info_t *info_first = addr2info (addr);
+	flash_info_t *info_last  = addr2info (end );
+	flash_info_t *info;
+	ulong len;
+
+	if (cnt == 0) {
+		return (ERR_OK);
+	}
+
+	if (!info_first || !info_last) {
+		return (ERR_INVAL);
+	}
+
+	while (cnt)
+	{
+		info = addr2info(addr);
+		/* Get remaining size in the bank */
+		len =  info->size - (addr - info->start[0]);
+
+		/* Copy till the end of bank or less (cnt) */
+		len = (cnt > len) ? len:cnt;
+
+		/* finally read data from flash */
+		read_buff(info, (uchar *)dest, addr, len);
+		dest+=len;
+		addr+=len;
+		cnt-=len;
+	}
+
+	return (ERR_OK);
+}
+
+
 /*-----------------------------------------------------------------------
  * Copy memory to flash.
  * Make sure all target addresses are within Flash bounds,
