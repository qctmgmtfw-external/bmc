--- u-boot-2013.07/oem/ami/bootlogo/cmd_bootlogo.c	1970-01-01 08:00:00.000000000 +0800
+++ uboot.new/oem/ami/bootlogo/cmd_bootlogo.c	2015-01-16 16:06:13.000000000 +0800
@@ -0,0 +1,80 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <flash.h>
+# include <malloc.h>
+# include <linux/string.h>
+# include "bootlogo.h"
+
+#define ACTION_ADDR	"addr"
+extern unsigned int enable_framebuffer(void);
+int ShowBootLogoByAddress(unsigned int logo_bin_addr, int idx)
+{
+	unsigned int ptr_addr ;
+	struct boot_logos_t *boot_logos = NULL;
+	struct logo_info_t *logo_info = NULL;
+	struct color_t *clut_ptr = NULL;
+	unsigned char *logo_data_ptr = NULL;
+	unsigned int fb_addr = 0;
+	int i;
+	struct color_t clut;
+
+	// Logo binary start address
+	ptr_addr = logo_bin_addr;
+
+	// Get Boot logos header
+	boot_logos = ((struct boot_logos_t *) logo_bin_addr);
+
+	// Get Logo info header
+	ptr_addr += sizeof(struct boot_logos_t);
+
+	for(i=0;i<boot_logos->count;i++){
+		logo_info = ((struct logo_info_t *)ptr_addr);
+		printf("(%d)idx %d, type %x, w*h (%d x %d), clut[%d: %d], data[%d: %d]\n", boot_logos->count, 
+		logo_info->index, logo_info->type, logo_info->width, logo_info->height, logo_info->clut.offset, logo_info->clut.size, logo_info->data.offset, logo_info->data.size);
+
+		// Get Logo palette
+		ptr_addr += sizeof(struct logo_info_t);
+
+		// Get Logo Data
+		ptr_addr += logo_info->clut.size * sizeof(struct color_t);
+		ptr_addr += logo_info->data.size;
+
+		if(i == idx)
+			break;
+	}
+
+	// Enable Framebuffer device
+	fb_addr = enable_framebuffer();
+	clut_ptr = (struct color_t *)(logo_bin_addr + logo_info->clut.offset);
+	logo_data_ptr = (unsigned char*)(logo_bin_addr + logo_info->data.offset);
+
+	// show Logo 
+	for(i=0;i<logo_info->data.size;i++){
+		clut = clut_ptr[logo_data_ptr[i]];
+		*( (unsigned int *) (fb_addr + (i << 2)) ) = (clut.alpha << 24) | (clut.red << 16) | (clut.green << 8) | (clut.blue << 0);
+	}
+
+	return 0;
+}
+
+/* U-boot's cmd function to show boot logo */
+int do_bootlogo (cmd_tbl_t *cmdtp, int flag , int argc, char * const argv[])
+{
+	int ret = -1;
+
+	if(!strcasecmp(argv[1], ACTION_ADDR)) {
+		ret = ShowBootLogoByAddress(simple_strtoul(argv[2], NULL, 16), simple_strtoul(argv[3], NULL, 10));
+	}
+
+	return ret;
+}
+
+static char help_text[] = 
+	"addr [Address of bootlogo binary] [image idx of bootlogo] - show logo from address.\n";
+
+U_BOOT_CMD(
+	bootlogo,	4,	0,	do_bootlogo,
+	"Show boot logo from address", help_text
+);
+
