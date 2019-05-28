--- u-boot-2013.07/common/env_spi.c	1970-01-01 08:00:00.000000000 +0800
+++ u-boot-2013.07-ami/common/env_spi.c	2015-07-03 16:20:21.080777625 +0800
@@ -0,0 +1,167 @@
+/* 
+ * LowLevel function for SPI environment support
+ * Author : American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+/* #define DEBUG */
+
+#include <common.h>
+#include <command.h>
+#include <environment.h>
+#include <linux/stddef.h>
+#include <malloc.h>
+#include <search.h>
+#include <errno.h>
+#include <asm/sections.h>
+
+extern	void spi_silent_init(void);
+
+
+DECLARE_GLOBAL_DATA_PTR;
+
+
+#if defined(CONFIG_CMD_SAVEENV) && defined(CONFIG_CMD_FLASH)
+#define CMD_SAVEENV
+#endif
+
+char *env_name_spec = "SpiFlash";
+static ulong flash_addr = 0;
+static ulong end_addr = 0;
+
+
+#ifdef CMD_SAVEENV
+int saveenv(void)
+{
+	env_t	env_new;
+	ssize_t	len;
+	int	rc = 1;
+	char	*res, *saved_data = NULL;
+
+
+	debug("Protect off %08lX ... %08lX\n", (ulong)flash_addr, end_addr);
+
+	if (flash_sect_protect(0, (long)flash_addr, end_addr))
+		goto done;
+
+	res = (char *)&env_new.data;
+	len = hexport_r(&env_htab, '\0', 0, &res, ENV_SIZE, 0, NULL);
+	if (len < 0) {
+		error("Cannot export environment: errno = %d\n", errno);
+		goto done;
+	}
+	env_new.crc = crc32(0, env_new.data, ENV_SIZE);
+
+	puts("Erasing Flash...");
+	if (flash_sect_erase((long)flash_addr, end_addr))
+		goto done;
+
+	puts("Writing to Flash... ");
+	rc = flash_write((char *)&env_new, (long)flash_addr, CONFIG_ENV_SIZE);
+	if (rc != 0)
+		goto perror;
+
+	puts("done\n");
+	rc = 0;
+	goto done;
+perror:
+	flash_perror(rc);
+done:
+	if (saved_data)
+		free(saved_data);
+	/* try to re-protect */
+	flash_sect_protect(1, (long)flash_addr, end_addr);
+	return rc;
+}
+#endif /* CMD_SAVEENV */
+
+
+
+
+void env_relocate_spec(void)
+{
+	char buf[CONFIG_ENV_SIZE];
+	int ret;
+
+#if defined(CONFIG_PILOT4) || defined(CONFIG_TWO_STAGES_BOOTLOADER)
+	flash_addr = CONFIG_SPX_FEATURE_GLOBAL_UBOOT_ENV_START;
+#else
+	flash_addr = CONFIG_SYS_FLASH_BASE+
+		(((_end_ofs + CONFIG_ENV_SECT_SIZE-1)/ CONFIG_ENV_SECT_SIZE) * CONFIG_ENV_SECT_SIZE);
+#endif
+	end_addr =  flash_addr + CONFIG_ENV_SECT_SIZE - 1;
+
+	flash_read(flash_addr, CONFIG_ENV_SIZE, buf);
+
+	ret =env_import(buf, 1);
+	if (ret)
+		gd->env_valid = 1;
+	return; 
+}
+
+
+int env_init(void)
+{
+	/* SPI flash isn't usable before relocation */
+	gd->env_addr = (ulong)&default_environment[0];
+	gd->env_valid = 1;
+
+	return 0;
+}
+
+/* SPI flash isn't usable before relocation */
+#if 0
+int env_init(void)
+{
+	ulong crc, len = ENV_SIZE, new = 0;
+	unsigned off;
+	uchar buf[64];
+
+	if (gd->env_valid)
+		return 0;
+
+	gd->env_addr	= (ulong)&default_environment[0];
+	gd->env_valid	= 0;
+
+	spi_silent_init();
+
+	/* read old CRC */
+	flash_read((unsigned long)(flash_addr)+offsetof(env_t,crc),sizeof(ulong), (char *)&crc);
+
+	/* Compute actual Crc */
+	off = offsetof(env_t, data);
+	while (len > 0) 
+	{
+		int n = (len > sizeof(buf)) ? sizeof(buf) : len;
+
+		flash_read ((unsigned long)(flash_addr)+off, n, (char *)buf);
+
+		new = crc32(new, buf, n);
+		len -= n;
+		off += n;
+	}
+
+	/* Compare Crc matches */
+	if (crc == new) 
+	{
+		gd->env_addr	= (unsigned long)(flash_addr)+offsetof(env_t, data);
+		gd->env_valid	= 1;
+		return 0;
+	}
+	return 0;
+}
+#endif
