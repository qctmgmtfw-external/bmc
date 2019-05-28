--- linux.org/drivers/video/ast/ast_fb.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/video/ast/ast_fb.c	2014-01-31 16:36:04.363860021 -0500
@@ -0,0 +1,774 @@
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
+/****************************************************************
+ *
+ * ast_fb.c
+ * ASPEED AST frame buffer driver
+ *
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
+#include <linux/delay.h>
+#include <linux/selection.h>
+#include <linux/bigphysarea.h>
+#include <linux/ioport.h>
+#include <linux/init.h>
+#include <linux/vmalloc.h>
+#include <linux/vt_kern.h>
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
+#include <linux/platform_device.h>
+
+#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28)
+#include <mach/platform.h>
+#include <mach/hardware.h>
+#else
+#include <asm/arch/platform.h>
+#include <asm/arch/hardware.h>
+#endif
+
+#include "ast_fb.h"
+#include "ast_fb_mode.h"
+#include "ast_fb_ioctl.h"
+
+#define AST_FB_DRV_NAME "astfb"
+
+extern int ast_fb_set_mode(struct fb_var_screeninfo *var, struct astfb_info *dinfo);
+extern int ast_fb_get_mode_info(struct fb_var_screeninfo *var, struct astfb_info *dinfo, struct ast_fb_mode_info_t *mode_info);
+
+static int ypan = 0;
+
+static struct fb_var_screeninfo default_var = {
+	.xres            = 0,
+	.yres            = 0,
+	.xres_virtual    = 0,
+	.yres_virtual    = 0,
+	.xoffset         = 0,
+	.yoffset         = 0,
+	.bits_per_pixel  = 0,
+	.grayscale       = 0,
+	.red             = {0, 8, 0},
+	.green           = {0, 8, 0},
+	.blue            = {0, 8, 0},
+	.transp          = {0, 0, 0},
+	.nonstd          = 0,
+	.activate        = FB_ACTIVATE_NOW,
+	.height          = -1,
+	.width           = -1,
+	.accel_flags     = 0,
+	.pixclock        = 0,
+	.left_margin     = 0,
+	.right_margin    = 0,
+	.upper_margin    = 0,
+	.lower_margin    = 0,
+	.hsync_len       = 0,
+	.vsync_len       = 0,
+	.sync            = 0,
+	.vmode           = FB_VMODE_NONINTERLACED,
+};
+
+/*
+static struct fb_fix_screeninfo default_fix = {
+	.id	= "AST1000/2000",
+	.type	= FB_TYPE_PACKED_PIXELS,
+	.accel	= FB_ACCEL_NONE,
+};
+*/
+
+#define AST_SCU_LOCK_KEY				0x00
+#define AST_SCU_MISC_CTRL				0x2C
+
+#define AST_SCU_UNLOCK_MAGIC			0x1688A8A8
+
+#define AST_SCU_MISC_CTRL_DVO_SRC		0x00040000 /* bit 18 */
+#define AST_SCU_MISC_CTRL_DAC_SRC_CRT	0x00010000
+#define AST_SCU_MISC_CTRL_DAC_SRC		0x00030000 /* bits[17:16] */
+#define AST_SCU_MISC_CTRL_VGA_REG		0x00000010 /* bit 4 */
+#define AST_SCU_MISC_CTRL_D2_PLL		0x00000008 /* bit 3 */
+
+int astfb_init_hw(struct device *dev, struct astfb_info *dinfo)
+{
+	uint32_t reg;
+
+	iowrite32(AST_SCU_UNLOCK_MAGIC, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY)); /* unlock SCU */
+
+	reg = ioread32((void * __iomem)(AST_SCU_VA_BASE + AST_SCU_MISC_CTRL));
+	reg &= ~(AST_SCU_MISC_CTRL_DAC_SRC | AST_SCU_MISC_CTRL_VGA_REG | AST_SCU_MISC_CTRL_D2_PLL); /* enable D2 PLL */
+	reg |= (AST_SCU_MISC_CTRL_DAC_SRC_CRT | AST_SCU_MISC_CTRL_DVO_SRC); /* set source to Graphics CRT */
+	iowrite32(reg, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_MISC_CTRL));
+
+	/* enable clock of Graphics Display controller */
+	reg = ioread32((void * __iomem)(SCU_CLK_STOP_REG));
+	reg &= ~(0x00000400);
+	iowrite32(reg, (void * __iomem)(SCU_CLK_STOP_REG));
+
+	udelay(10);
+
+	/* reset Graphics Display controller */
+	reg = ioread32((void * __iomem)(SCU_SYS_RESET_REG));
+	reg &= ~(0x00002000);
+	iowrite32(reg, (void * __iomem)(SCU_SYS_RESET_REG));
+
+	/* reset 2D engine */
+	reg = ioread32((void * __iomem)(SCU_SYS_RESET_REG));
+	reg &= ~(0x00000080);
+	iowrite32(reg, (void * __iomem)(SCU_SYS_RESET_REG));
+
+	iowrite32(0, (void * __iomem)(AST_SCU_VA_BASE + AST_SCU_LOCK_KEY)); /* lock SCU */
+
+	return 0;
+}
+
+int astfb_crtc_to_var(struct fb_var_screeninfo *var, struct astfb_info *dinfo)
+{
+	/* crtc */
+	var->xoffset = var->yoffset = 0;
+
+	/* palette */
+	switch (var->bits_per_pixel) {
+	case 8:
+		var->red.offset = var->green.offset = var->blue.offset = 0;
+		var->red.length = var->green.length = var->blue.length = 6;
+		break;
+	case 16:
+		var->red.offset = 11;
+		var->red.length = 5;
+		var->green.offset = 5;
+		var->green.length = 6;
+		var->blue.offset = 0;
+		var->blue.length = 5;
+		var->transp.offset = 0;
+		var->transp.length = 0;
+		break;
+	case 32:
+		var->red.offset = 16;
+		var->red.length = 8;
+		var->green.offset = 8;
+		var->green.length = 8;
+		var->blue.offset = 0;
+		var->blue.length = 8;
+		var->transp.offset = 24;
+		var->transp.length = 8;
+		break;
+	}
+
+	var->red.msb_right = 0;
+	var->green.msb_right = 0;
+	var->blue.msb_right = 0;
+	var->transp.offset = 0;
+	var->transp.length = 0;
+	var->transp.msb_right = 0;
+
+	return 0;
+}
+
+/*****************************************************************************/
+
+void ast_fd_wait_2d_engine_idle(struct astfb_info *dinfo)
+{
+	uint32_t reg;
+	uint32_t reg2;
+	uint32_t check_setting;
+	
+	check_setting = (dinfo->use_2d_engine) ? 0x80000000 : 0x10000000;
+
+	/* Chk if disable 2D */
+	reg = ioread32(dinfo->io_2d + 0x40);
+	if (!(reg & 0x80000000)) {
+		return;
+	}
+
+	do {
+		reg  = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+		reg2 = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+		reg2 = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+		reg2 = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+		reg2 = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+		reg2 = ioread32(dinfo->io_2d + 0x4C) & 0xFFFC0000;
+	} while ((reg & check_setting) || (reg != reg2));
+}
+
+int astfb_enable_2d_cmd_q(struct astfb_info *dinfo)
+{
+	uint32_t ulVMCmdQBasePort = 0;
+
+	ast_fd_wait_2d_engine_idle(dinfo);
+
+	if (dinfo->use_2d_engine) { /* command */
+		ulVMCmdQBasePort = (dinfo->frame_buf_phys + dinfo->cmd_q_offset) >> 3;
+
+		/* set CMDQ Threshold */
+		ulVMCmdQBasePort |= 0xF0000000;
+	
+		/* set CMDQ Size */
+		switch (dinfo->cmd_q_sz) {
+		case SZ_256K:
+			ulVMCmdQBasePort |= 0x00000000;
+			break;
+		case SZ_512K:
+			ulVMCmdQBasePort |= 0x04000000;
+			break;
+		case SZ_1M:
+			ulVMCmdQBasePort |= 0x08000000;
+			break;
+		case SZ_2M:
+			ulVMCmdQBasePort |= 0x0C000000;
+			break;
+		default:
+			printk(KERN_WARNING "Unsupported command Q size\n");
+			return 0;
+		}
+	} else { /* memory-mapped */
+		/* set CMDQ Threshold */
+		ulVMCmdQBasePort |= 0xF0000000;
+		ulVMCmdQBasePort |= 0x02000000; /* MMIO mode */
+    }
+
+	iowrite32(ulVMCmdQBasePort, dinfo->io_2d + 0x44);
+	return 0;
+}
+
+int ast_fb_enable_2d_engine(struct astfb_info *dinfo)
+{
+	iowrite32(0x80000000, dinfo->io_2d + 0x40);
+
+	astfb_enable_2d_cmd_q(dinfo);
+
+	return 0;
+}
+
+void ast_fb_disable_2d_engine(struct astfb_info *dinfo)
+{
+	iowrite32(0x00000000, dinfo->io_2d + 0x40);
+
+	ast_fd_wait_2d_engine_idle(dinfo);
+}
+
+/*****************************************************************************/
+
+static int astfb_set_fix(struct fb_info *info)
+{
+	struct fb_fix_screeninfo *fix;
+	struct astfb_info *dinfo = GET_DINFO(info);
+
+	fix = &(info->fix);
+	memset(fix, 0, sizeof(struct fb_fix_screeninfo));
+	strcpy(fix->id, dinfo->name);
+	fix->smem_start = dinfo->frame_buf_phys;
+	fix->smem_len = dinfo->frame_buf_sz;
+	fix->type = FB_TYPE_PACKED_PIXELS;
+	fix->type_aux = 0;
+	fix->visual = (dinfo->bpp == 8) ? FB_VISUAL_PSEUDOCOLOR : FB_VISUAL_TRUECOLOR;
+	fix->xpanstep = 0;
+	fix->ypanstep = ypan ? 1 : 0;
+	fix->ywrapstep = 0;
+	fix->line_length = dinfo->pitch;
+	fix->mmio_start = dinfo->ulMMIOPhys;
+	fix->mmio_len = dinfo->ulMMIOSize;
+	fix->accel = FB_ACCEL_NONE;
+
+	return 0;
+}
+
+static int astfb_blank(int blank, struct fb_info *info)
+{
+	struct astfb_info *dinfo = GET_DINFO(info);
+	uint32_t reg;
+
+	switch (blank) {
+	case 0: /* on */
+		reg = 0x00000000;
+		break;
+	case 1: /* blank */
+		reg = 0x00100000;
+		break;
+	case 2: /* no vsync */
+		reg = 0x00180000;
+		break;
+	case 3: /* no hsync */
+		reg = 0x00140000;
+		break;
+	case 4: /* off */
+		reg = 0x001c0000;
+		break;
+	default:
+		return 1;
+	}
+
+	iowrite32(reg, dinfo->io + AST_GRAPHICS_CTRL);
+
+	return 0;
+}
+
+static int astfb_pan_display(struct fb_var_screeninfo *var, struct fb_info* info)
+{
+	struct astfb_info *dinfo = GET_DINFO(info);
+	uint32_t base;
+	u32 xoffset, yoffset;
+	
+	xoffset = (var->xoffset + 3) & ~3; /* DW alignment */
+	yoffset = var->yoffset;
+
+	if ((xoffset + var->xres) > var->xres_virtual) {
+		return -EINVAL;
+	}
+
+	if ((yoffset + var->yres) > var->yres_virtual) {
+		return -EINVAL;
+	}
+	
+	info->var.xoffset = xoffset;
+	info->var.yoffset = yoffset;
+
+	base = (var->yoffset * var->xres_virtual) + var->xoffset;
+
+	/* calculate base bpp depth */
+	switch(var->bits_per_pixel) {
+	case 32:
+		break;
+	case 16:
+		base >>= 1;
+		break;
+	case 8:
+	default:
+		base >>= 2;
+		break;
+	}
+
+	/* set starting address of frame buffer */
+	iowrite32(dinfo->frame_buf_phys + base, dinfo->io + AST_GRAPHICS_START_ADDR);
+
+	return 0;
+}
+
+static int astfb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
+{
+	struct astfb_info *dinfo = GET_DINFO(info);
+	struct astfb_dfbinfo dfbinfo;
+
+	switch(cmd) {
+	case ASTFB_GET_DFBINFO:
+		dfbinfo.ulFBSize = dinfo->frame_buf_sz;
+		dfbinfo.ulFBPhys = dinfo->frame_buf_phys;
+		dfbinfo.ulCMDQSize = dinfo->cmd_q_sz;
+		dfbinfo.ulCMDQOffset = dinfo->cmd_q_offset;
+		dfbinfo.ul2DMode = dinfo->use_2d_engine;
+		if (copy_to_user((void __user *)arg, &dfbinfo, sizeof(struct astfb_dfbinfo)))
+			return -EFAULT;
+		return 0;
+
+	default:
+		return -EINVAL;
+	}
+
+	return 0;
+}
+
+static int astfb_get_cmap_len(struct fb_var_screeninfo *var)
+{
+	return (var->bits_per_pixel == 8) ? 256 : 16;
+}
+
+static int astfb_open(struct fb_info *info, int user)
+{
+	return 0;
+}
+
+static int astfb_release(struct fb_info *info, int user)
+{
+	return 0;
+}
+
+static int astfb_setcolreg(unsigned regno, unsigned red, unsigned green, unsigned blue, unsigned transp, struct fb_info *info)
+{
+	if (regno >= astfb_get_cmap_len(&info->var))
+		return 1;
+
+	switch(info->var.bits_per_pixel) {
+	case 8:
+		return 1;
+		break;
+	case 16:
+		((u32 *) (info->pseudo_palette))[regno] = (red & 0xf800) | ((green & 0xfc00) >> 5) | ((blue & 0xf800) >> 11);
+		break;
+	case 32:
+		red >>= 8;
+		green >>= 8;
+		blue >>= 8;
+		((u32 *)(info->pseudo_palette))[regno] =
+				(red << 16) | (green << 8) | (blue);
+		break;
+	}
+
+	return 0;
+}
+
+static int astfb_set_par(struct fb_info *info)
+{
+	struct astfb_info *dinfo = GET_DINFO(info);
+
+	if (ast_fb_set_mode(&info->var, dinfo))
+	    return 1;
+
+	astfb_set_fix(info);
+	
+	return 0;
+}
+
+static int astfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
+{
+	struct astfb_info *dinfo = GET_DINFO(info);
+	struct ast_fb_mode_info_t mode_info;
+	
+	if (ast_fb_get_mode_info(var, dinfo, &mode_info)) {
+		//printk(KERN_ERR "astfb: check mode failed \n");
+		return 1;
+	}
+
+	/* Sanity check for offsets */
+	if (var->xoffset < 0)
+		var->xoffset = 0;
+	if (var->yoffset < 0)
+		var->yoffset = 0;
+
+	if (var->xres > var->xres_virtual)
+		var->xres_virtual = var->xres;
+
+	/* Truncate offsets to maximum if too high */
+	if (var->xoffset > var->xres_virtual - var->xres) {
+		var->xoffset = var->xres_virtual - var->xres - 1;
+	}
+
+	if (var->yoffset > var->yres_virtual - var->yres) {
+		var->yoffset = var->yres_virtual - var->yres - 1;
+	}
+
+	switch (var->bits_per_pixel) {
+	case 8:
+		var->red.offset = var->green.offset = var->blue.offset = 0;
+		var->red.length = var->green.length = var->blue.length = 6;
+		break;
+	case 16:
+		var->red.offset = 11;
+		var->red.length = 5;
+		var->green.offset = 5;
+		var->green.length = 6;
+		var->blue.offset = 0;
+		var->blue.length = 5;
+		var->transp.offset = 0;
+		var->transp.length = 0;
+		break;
+	case 32:
+		var->red.offset = 16;
+		var->red.length = 8;
+		var->green.offset = 8;
+		var->green.length = 8;
+		var->blue.offset = 0;
+		var->blue.length = 8;
+		var->transp.offset = 24;
+		var->transp.length = 8;
+		break;
+	}
+
+	/* Set everything else to 0 */
+	var->red.msb_right = 0;
+	var->green.msb_right = 0;
+	var->blue.msb_right = 0;
+	var->transp.offset = 0;
+	var->transp.length = 0;
+	var->transp.msb_right = 0;
+
+	return 0;
+}
+
+static struct fb_ops ast_fb_ops = {
+	.owner          = THIS_MODULE,
+	.fb_open        = astfb_open,
+	.fb_release     = astfb_release,
+	.fb_check_var   = astfb_check_var,
+	.fb_set_par     = astfb_set_par,
+	.fb_setcolreg   = astfb_setcolreg,
+	.fb_blank       = astfb_blank,
+	.fb_pan_display = astfb_pan_display,
+	.fb_fillrect    = cfb_fillrect,
+	.fb_copyarea    = cfb_copyarea,
+	.fb_imageblit   = cfb_imageblit,
+	.fb_ioctl       = astfb_ioctl
+};
+
+/*****************************************************************************/
+
+static int astfb_set_fbinfo(struct astfb_info *dinfo)
+{
+	struct fb_info *info;
+
+	info = dinfo->info;
+
+	info->flags = FBINFO_FLAG_DEFAULT;
+	info->fbops = &ast_fb_ops;
+	info->var = dinfo->var;
+	info->fix = dinfo->fix;
+	info->pseudo_palette = dinfo->pseudo_palette;
+	info->screen_base = dinfo->frame_buf;
+
+	/* update crtc to var */
+	if (astfb_check_var(&info->var, info)) {
+		printk(KERN_ERR "astfb: check default mode failed\n");
+		return 1;
+	}
+
+	if (ast_fb_set_mode(&info->var, dinfo)) {
+		printk(KERN_ERR "astfb: set default mode failed\n");
+		return 1;
+	}
+
+	astfb_set_fix(info);
+
+	return 0;
+}
+
+static int astfb_alloc_bigphysarea(struct astfb_info *dinfo)
+{
+	unsigned long addr;
+	unsigned long size;
+
+	dinfo->frame_buf_sz = AST_FRAME_BUF_SZ;
+
+	dinfo->frame_buf = bigphysarea_alloc_pages(dinfo->frame_buf_sz / PAGE_SIZE, 0, GFP_KERNEL);
+	if (dinfo->frame_buf == NULL) {
+		return -1;
+	}
+
+	addr = (unsigned long) dinfo->frame_buf;
+	size = dinfo->frame_buf_sz;
+	while (size > 0) {
+		SetPageReserved(virt_to_page(addr));
+		addr += PAGE_SIZE;
+		size -= PAGE_SIZE;
+	}
+
+	memset(dinfo->frame_buf, 0x00, dinfo->frame_buf_sz);
+	dinfo->frame_buf_phys = virt_to_phys(dinfo->frame_buf);
+
+	return 0;
+}
+
+static void cleanup(struct astfb_info *dinfo)
+{
+	if (!dinfo)
+		return;
+
+	#if Enable2D
+	ast_fb_disable_2d_engine(dinfo);
+	#endif
+
+	if (dinfo->frame_buf != NULL)
+		iounmap(dinfo->frame_buf);
+
+	if (dinfo->registered) {
+		unregister_framebuffer(dinfo->info);
+		framebuffer_release(dinfo->info);
+	}
+
+	dev_set_drvdata(dinfo->dev, NULL);
+}
+
+static int astfb_drv_probe(struct device *dev)
+{
+	struct fb_info *info;
+	struct astfb_info *dinfo;
+	int ret;
+
+	info = framebuffer_alloc(sizeof(struct astfb_info), dev);
+	if (!info) {
+		printk(KERN_ERR "Could not allocate memory for astfb_info.\n");
+		return -ENODEV;
+	}
+	
+	if (fb_alloc_cmap(&info->cmap, 256, 0) < 0) {
+		printk(KERN_ERR "Could not allocate cmap for astfb_info.\n");
+		framebuffer_release(info);
+		return -ENODEV;
+	}
+
+	dinfo = (struct astfb_info *) info->par;
+	memset(dinfo, 0, sizeof(struct astfb_info));
+	dinfo->info = info;
+	dinfo->dev = dev;
+	strcpy(dinfo->name, "astfb");
+	dev_set_drvdata(dev, (void *) dinfo);
+
+	/* frame buffer */
+	ret = astfb_alloc_bigphysarea(dinfo);
+	if (ret != 0) {
+		printk(KERN_WARNING "failed to allocate physical memory for frame buffer\n");
+		cleanup(dinfo);
+		return -ENOMEM;
+	}
+
+	/* I/O registers */
+	if (request_mem_region(AST_GRAPHICS_REG_BASE, AST_GRAPHICS_REG_SZ, AST_FB_DRV_NAME) == NULL) {
+		printk(KERN_WARNING "%s: request memory region failed\n", AST_FB_DRV_NAME);
+		cleanup(dinfo);
+		return -EBUSY;
+	}
+
+	dinfo->io = ioremap(AST_GRAPHICS_REG_BASE, AST_GRAPHICS_REG_SZ);
+	if (dinfo->io == NULL) {
+		printk(KERN_WARNING "%s: ioremap failed\n", AST_FB_DRV_NAME);
+		cleanup(dinfo);
+		return -ENOMEM;
+	}
+
+	/* I/O registers of 2D engine */
+	dinfo->ulMMIOSize = AST_2D_ENGINE_REG_SZ;
+	dinfo->ulMMIOPhys = AST_2D_ENGINE_REG_BASE;
+
+	if (request_mem_region(AST_2D_ENGINE_REG_BASE, AST_2D_ENGINE_REG_SZ, AST_FB_DRV_NAME) == NULL) {
+		printk(KERN_WARNING "%s: request memory region failed\n", AST_FB_DRV_NAME);
+		cleanup(dinfo);
+		return -EBUSY;
+	}
+
+	dinfo->io_2d = ioremap(AST_2D_ENGINE_REG_BASE, AST_2D_ENGINE_REG_SZ);
+	if (dinfo->io_2d == NULL) {
+		printk(KERN_WARNING "%s: ioremap failed\n", AST_FB_DRV_NAME);
+		cleanup(dinfo);
+		return -ENOMEM;
+	}
+
+	#if EnableCMDQ
+	dinfo->cmd_q_sz = AST_GRAPHICS_CMD_Q_SZ;
+	dinfo->cmd_q_offset = dinfo->frame_buf_sz - dinfo->cmd_q_sz;
+	dinfo->use_2d_engine = 1;
+	#else
+	dinfo->cmd_q_sz = 0;
+	dinfo->cmd_q_offset = 0;
+	dinfo->use_2d_engine = 0;
+	#endif
+
+	if (astfb_init_hw(dev, dinfo)) {
+		cleanup(dinfo);
+		return -ENODEV;
+	}
+
+	/* set fb interface */
+	memcpy(&dinfo->var, &default_var, sizeof(default_var));
+
+	/* set default var mode */
+	dinfo->var.xres = dinfo->var.xres_virtual = 1024;
+	dinfo->var.yres = dinfo->var.yres_virtual = 768;
+	dinfo->var.bits_per_pixel = 32;
+	dinfo->refreshrate = 60;
+
+	if (astfb_set_fbinfo(dinfo)) {
+		cleanup(dinfo);
+		return -ENODEV;
+	}
+
+	if (register_framebuffer(info) < 0) {
+		printk(KERN_ERR "Cannot register framebuffer.\n");
+		cleanup(dinfo);
+		return -ENODEV;
+	}
+
+	dinfo->registered = 1;
+
+	return 0;
+}
+
+static int astfb_drv_remove(struct device *dev)
+{
+	struct astfb_info *dinfo = dev_get_drvdata(dev);
+
+	if (!dinfo)
+		return 0;
+
+	cleanup(dinfo);
+
+	return 0;
+}
+
+int astfb_drv_suspend(struct device *dev, pm_message_t state)
+{
+	/* TODO */
+	return 0;
+}
+
+int astfb_drv_resume(struct device *dev)
+{
+	/* TODO */
+	return 0;
+}
+
+static struct device_driver astfb_driver = {
+	.name = "astfb",
+	.bus = &platform_bus_type,
+	.probe = astfb_drv_probe,
+	.remove = astfb_drv_remove,
+	.suspend = astfb_drv_suspend,
+	.resume = astfb_drv_resume,
+};
+
+static struct platform_device astfb_device = {
+	.name = "astfb",
+};
+
+int __init astfb_init(void)
+{
+	int ret;
+
+	ret = driver_register(&astfb_driver);
+
+	if (!ret) {
+		ret = platform_device_register(&astfb_device);
+		if (ret)
+			driver_unregister(&astfb_driver);
+	}
+
+	return ret;
+}
+
+static void __exit astfb_exit(void)
+{
+	platform_device_unregister(&astfb_device);
+	driver_unregister(&astfb_driver);
+}
+
+module_init(astfb_init);
+module_exit(astfb_exit);
+
+MODULE_AUTHOR("American Megatrends Inc.");
+MODULE_DESCRIPTION("AST frame buffer driver");
+MODULE_LICENSE("American Megatrends Inc.");
