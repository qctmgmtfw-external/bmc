--- linux.org/drivers/video/ast/ast_fb_mode.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/video/ast/ast_fb_mode.c	2014-01-31 15:42:24.563921267 -0500
@@ -0,0 +1,379 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2006-2009, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        5555 Oakbrook Pkwy Suite 200, Norcross              **
+ **                                                            **
+ **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+
+#include <linux/version.h>
+#include <linux/module.h>
+#include <linux/moduleparam.h>
+#include <linux/kernel.h>
+//#include <linux/smp_lock.h>
+#include <linux/spinlock.h>
+#include <linux/errno.h>
+#include <linux/string.h>
+#include <linux/mm.h>
+#include <linux/tty.h>
+#include <linux/slab.h>
+#include <linux/fb.h>
+#include <linux/selection.h>
+#include <linux/ioport.h>
+#include <linux/init.h>
+#include <linux/pci.h>
+#include <linux/vmalloc.h>
+#include <linux/capability.h>
+#include <linux/fs.h>
+#include <linux/types.h>
+#include <asm/uaccess.h>
+#include <asm/io.h>
+
+#ifdef CONFIG_MTRR
+#include <asm/mtrr.h>
+#endif
+
+#include <mach/platform.h>
+//#include <mach/memory.h>
+
+#include "ast_fb.h"
+#include "ast_fb_mode.h"
+
+struct ast_fb_mode_info_t mode_tab_640x480[] = {
+	{
+		800, 640, 8, 96, /* horizontal */
+		525, 480, 2, 2, /* vertical */
+		60, VCLK25_175, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN | MODE_FLAG_H_BORDER | MODE_FLAG_V_BORDER,
+	}, {
+		832, 640, 16, 40, /* horizontal */
+		520, 480, 1, 3, /* vertical */
+		72, VCLK31_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN | MODE_FLAG_H_BORDER | MODE_FLAG_V_BORDER,
+	}, {
+		840, 640, 16, 64, /* horizontal */
+		500, 480, 1, 3, /* vertical */
+		75, VCLK31_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN,
+	}, {
+		832, 640, 56, 56, /* horizontal */
+		509, 480, 1, 3, /* vertical */
+		85, VCLK36, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN,
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+struct ast_fb_mode_info_t mode_tab_800x600[] = {
+	{
+		1024, 800, 24, 72, /* horizontal */
+		625, 600, 1, 2, /* vertical */
+		56, VCLK36, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, {
+		1056, 800, 40, 128, /* horizontal */
+		628, 600, 1, 4, /* vertical */
+		60, VCLK40, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, {
+		1040, 800, 56, 120, /* horizontal */
+		666, 600, 37, 6, /* vertical */
+		72, VCLK50, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, {
+		1056, 800, 16, 80, /* horizontal */
+		625, 600, 1, 3, /* vertical */
+		75, VCLK49_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, {
+		1048, 800, 32, 64, /* horizontal */
+		631, 600, 1, 3, /* vertical */
+		85, VCLK56_25, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+struct ast_fb_mode_info_t mode_tab_1024x768[] = {
+	{
+		1344, 1024, 24, 136, /* horizontal */
+		806, 768, 3, 6, /* vertical */
+		60, VCLK65, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN,
+	}, {
+		1328, 1024, 24, 136, /* horizontal */
+		806, 768, 3, 6, /* vertical */
+		70, VCLK75, /* refresh rate and clock */
+		MODE_FLAG_SYNC_NN,
+	}, {
+		1312, 1024, 16, 96, /* horizontal */
+		800, 768, 1, 3, /* vertical */
+		75, VCLK78_75, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, {
+		1376, 1024, 48, 96, /* horizontal */
+		808, 768, 1, 3, /* vertical */
+		85, VCLK94_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+struct ast_fb_mode_info_t mode_tab_1280x1024[] = {
+	{
+		1688, 1280, 48, 112, /* horizontal */
+		1066, 1024, 1, 3, /* vertical */
+		60, VCLK108, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP
+	}, {
+		1688, 1280, 16, 144, /* horizontal */
+		1066, 1024, 1, 3, /* vertical */
+		75, VCLK135, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP
+	}, {
+		1728, 1280, 64, 160, /* horizontal */
+		1072, 1024, 1, 3, /* vertical */
+		85, VCLK157_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+struct ast_fb_mode_info_t mode_tab_1600x1200[] = {
+	{
+		2160, 1600, 64, 192, /* horizontal */
+		1250, 1200, 1, 3, /* vertical */
+		60, VCLK162, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+struct ast_fb_mode_info_t mode_tab_1920x1080[] = {
+	{
+		2200, 1920, 88, 44, /* horizontal */
+		1125, 1080, 4, 5, /* vertical */
+		60, VCLK148_5, /* refresh rate and clock */
+		MODE_FLAG_SYNC_PP,
+	}, { /* end */
+		0, 0, 0, 0,
+		0, 0, 0, 0,
+		-1, 0,
+		0
+	}
+};
+
+uint32_t ast_fb_clk_table[] = {
+    0x00046515, /* 00: VCLK25_175 */
+    0x00047255, /* 01: VCLK28_322 */
+    0x0004682a, /* 02: VCLK31_5 */
+    0x0004672a, /* 03: VCLK36 */
+    0x00046c50, /* 04: VCLK40 */
+    0x00046842, /* 05: VCLK49_5 */
+    0x00006c32, /* 06: VCLK50 */
+    0x00006a2f, /* 07: VCLK56_25 */
+    0x00006c41, /* 08: VCLK65 */
+    0x00006832, /* 09: VCLK75 */
+    0x0000672e, /* 10: VCLK78_75 */
+    0x0000683f, /* 11: VCLK94_5 */
+    0x00004824, /* 12: VCLK108 */
+    0x0000482d, /* 13: VCLK135 */
+    0x0000472e, /* 14: VCLK157_5 */
+    0x00004836, /* 15: VCLK162 */
+    0x00004b44, /* 16: VCLK148_5 */
+};
+
+extern int ast_fb_enable_2d_engine(struct astfb_info *dinfo);
+extern void ast_fb_disable_2d_engine(struct astfb_info *dinfo);
+
+int ast_fb_get_mode_info(struct fb_var_screeninfo *var, struct astfb_info *dinfo, struct ast_fb_mode_info_t *mode_info)
+{
+	struct ast_fb_mode_info_t *mode_walker;
+	uint32_t ulHT, ulVT;
+	uint32_t ulHBorder, ulVBorder;
+
+	ulHT = var->xres + var->left_margin + var->right_margin + var->hsync_len;
+	ulVT = var->yres + var->upper_margin + var->lower_margin + var->vsync_len;
+
+	switch (var->xres) {
+	case 640:
+		mode_walker = mode_tab_640x480;
+		break;
+	case 800:
+		mode_walker = mode_tab_800x600;
+		break;
+	case 1024:
+		mode_walker = mode_tab_1024x768;
+		break;
+	case 1280:
+		mode_walker = mode_tab_1280x1024;
+		break;
+	case 1600:
+		mode_walker = mode_tab_1600x1200;
+		break;
+	case 1920:
+		mode_walker = mode_tab_1920x1080;
+		break;
+	default:
+		return 1;
+	}
+
+	/* Get Proper Mode Index */
+	while (mode_walker->refresh_rate < dinfo->refreshrate) {
+		mode_walker++;
+		if (mode_walker->clk_tab_index == -1) {
+			printk("AST FB NO Match mode\n");
+			mode_walker--;
+			break;
+		}
+	}
+
+	memcpy(mode_info, mode_walker, sizeof(struct ast_fb_mode_info_t));
+
+	/* Update var info */
+	ulHBorder = (mode_info->flags & MODE_FLAG_H_BORDER) ? 1 : 0;
+	ulVBorder = (mode_info->flags & MODE_FLAG_V_BORDER) ? 1 : 0;
+
+	var->left_margin = ulHBorder ? (u32) (mode_info->horiz_front_porch + 8) : (u32) (mode_info->horiz_front_porch);
+	var->hsync_len = (u32) mode_info->horiz_sync;
+	var->right_margin = (u32) (mode_info->horiz_total - var->xres - var->left_margin - var->hsync_len);
+
+	var->upper_margin = ulVBorder ? (u32) (mode_info->verti_front_porch + 1) : (u32) (mode_info->verti_front_porch);
+	var->vsync_len = (u32) mode_info->verti_sync;
+	var->lower_margin = (u32) (mode_info->verti_total - var->yres - var->upper_margin - var->vsync_len);
+
+	/* update dinfo */
+	dinfo->xres = var->xres;
+	dinfo->yres = var->yres;
+	dinfo->xres_virtual = var->xres_virtual;
+	dinfo->yres_virtual = var->yres_virtual;
+	dinfo->bpp = var->bits_per_pixel;
+	dinfo->pitch = var->xres_virtual * ((dinfo->bpp + 1) / 8);
+	dinfo->pixclock = var->pixclock;
+
+	return 0;
+}
+
+void astfb_set_mode_reg(struct astfb_info *dinfo, struct ast_fb_mode_info_t *mode_info)
+{
+	uint32_t horiz_total;
+	uint32_t horiz_display_end;
+	uint32_t horiz_border_start;
+	uint32_t horiz_border_end;
+	uint32_t horiz_retrace_start;
+	uint32_t horiz_retrace_end;
+
+	uint32_t verti_total;
+	uint32_t verti_display_end;
+	uint32_t verti_border_start;
+	uint32_t verti_border_end;
+	uint32_t verti_retrace_start;
+	uint32_t verti_retrace_end;
+
+	horiz_total = mode_info->horiz_total;
+	horiz_display_end = mode_info->horiz_display_end;
+	horiz_border_start = (mode_info->flags & MODE_FLAG_H_BORDER) ? (horiz_display_end + 1) : horiz_display_end;
+	horiz_border_end = (mode_info->flags & MODE_FLAG_H_BORDER) ? (horiz_total - 1) : horiz_total;
+	horiz_retrace_start = horiz_border_start + mode_info->horiz_front_porch;
+	horiz_retrace_end = horiz_border_start + mode_info->horiz_front_porch + mode_info->horiz_sync;
+
+	verti_total = mode_info->verti_total;
+	verti_display_end = mode_info->verti_display_end;
+	verti_border_start = (mode_info->flags & MODE_FLAG_V_BORDER) ? (verti_display_end + 1) : verti_display_end;
+	verti_border_end = (mode_info->flags & MODE_FLAG_V_BORDER) ? (verti_total - 1) : verti_total;
+	verti_retrace_start = verti_border_start + mode_info->verti_front_porch;
+	verti_retrace_end = verti_border_start + mode_info->verti_front_porch + mode_info->verti_sync;
+
+	iowrite32(((horiz_display_end - 1) << 16) | (horiz_total - 1), dinfo->io + AST_GRAPHICS_H_TOTAL_END);
+	iowrite32(((horiz_retrace_end - 1) << 16) | (horiz_retrace_start - 1), dinfo->io + AST_GRAPHICS_H_RETRACE);
+	iowrite32(((verti_display_end - 1) << 16) | (verti_total - 1), dinfo->io + AST_GRAPHICS_V_TOTAL_END);
+	iowrite32(((verti_retrace_end - 1) << 16) | (verti_retrace_start - 1), dinfo->io + AST_GRAPHICS_V_RETRACE);
+}
+
+void astfb_set_offset_reg(struct astfb_info *dinfo, struct ast_fb_mode_info_t *mode_info)
+{
+	uint32_t offset, terminal_count;
+
+	offset = ((uint32_t) dinfo->xres * dinfo->bpp) >> 3; /* Unit: 8 bits */
+	terminal_count = ((uint32_t) dinfo->xres * dinfo->bpp + 63) >> 6; /* Unit: 64 bits, 63 is for alignment */
+
+	iowrite32((terminal_count << 16) | (offset), dinfo->io + AST_GRAPHICS_OFFSET);
+}
+
+void astfb_set_ctrl_reg(struct astfb_info *dinfo, struct ast_fb_mode_info_t *mode_info)
+{
+	uint32_t reg;
+
+	reg = AST_GRAPHICS_CTRL_ENABLE;
+
+	/* Mode Type Setting */
+	switch (dinfo->bpp) {
+	case 16:
+		reg |= AST_GRAPHICS_CTRL_FORMAT_RGB555;
+		break;
+	case 32:
+		reg |= AST_GRAPHICS_CTRL_FORMAT_XRGB8888;
+		break;
+	}
+
+	/* Polarity */
+	reg |= ((mode_info->flags & MODE_FLAG_SYNC_NN) << 16);
+
+	iowrite32(dinfo->frame_buf_phys, dinfo->io + AST_GRAPHICS_START_ADDR);
+	iowrite32(((uint32_t) CRT_HIGH_THRESHOLD_VALUE << 8) | (uint32_t) (CRT_LOW_THRESHOLD_VALUE), dinfo->io + AST_GRAPHICS_THRES);
+	iowrite32(AST_GRAPHICS_CTRL2_DAC | AST_GRAPHICS_CTRL2_DVO, dinfo->io + AST_GRAPHICS_CTRL2);
+	iowrite32(reg, dinfo->io + AST_GRAPHICS_CTRL);
+}
+
+int ast_fb_set_mode(struct fb_var_screeninfo *var, struct astfb_info *dinfo)
+{
+	struct ast_fb_mode_info_t mode_info;
+
+	if (ast_fb_get_mode_info(var, dinfo, &mode_info)) {
+		printk("AST FB invalid mode\n");
+		return -1;
+	}
+
+	/* pre set mode */
+	#if Enable2D
+	ast_fb_disable_2d_engine(dinfo);
+	#endif
+	
+	iowrite32(ast_fb_clk_table[mode_info.clk_tab_index], dinfo->io + AST_GRAPHICS_PLL);
+	astfb_set_mode_reg(dinfo, &mode_info);
+	astfb_set_offset_reg(dinfo, &mode_info);
+	astfb_set_ctrl_reg(dinfo, &mode_info);
+	
+	/* post set mode */
+	#if Enable2D
+	ast_fb_enable_2d_engine(dinfo);
+	#endif
+
+	return 0;
+}
