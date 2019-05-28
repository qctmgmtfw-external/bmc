--- linux.org/drivers/video/ast/ast_fb.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/video/ast/ast_fb.h	2014-01-31 15:42:24.563921267 -0500
@@ -0,0 +1,95 @@
+#ifndef __AST_FB_H__
+#define __AST_FB_H__
+
+#define Enable2D			0 /* Enable 2D Acceleration */
+#define EnableCMDQ			0 /* 0/1 : MMIO/CMDQ */
+
+#define GET_DINFO(info)		(struct astfb_info *)(info->par)
+#define GET_DISP(info, con)	((con) < 0) ? (info)->disp : &fb_display[con]
+
+#define AST_FRAME_BUF_SZ					0x00800000 /* 8 MB */
+#define AST_GRAPHICS_CMD_Q_SZ				0x00100000 /* 1 MB */
+
+#define AST_GRAPHICS_REG_BASE				0x1E6E6000
+#define AST_GRAPHICS_REG_SZ					SZ_4K
+#define AST_GRAPHICS_IRQ					25
+
+#define AST_2D_ENGINE_REG_BASE				0x1E760000
+#define AST_2D_ENGINE_REG_SZ				SZ_128K
+#define AST_2D_ENGINE_IRQ					29
+
+/* registers of graphics display controller */
+#define AST_GRAPHICS_CTRL					0x60
+#define AST_GRAPHICS_CTRL2					0x64
+#define AST_GRAPHICS_STS					0x68
+#define AST_GRAPHICS_PLL					0x6C
+#define AST_GRAPHICS_H_TOTAL_END			0x70
+#define AST_GRAPHICS_H_RETRACE				0x74
+#define AST_GRAPHICS_V_TOTAL_END			0x78
+#define AST_GRAPHICS_V_RETRACE				0x7C
+#define AST_GRAPHICS_START_ADDR				0x80
+#define AST_GRAPHICS_OFFSET					0x84
+#define AST_GRAPHICS_THRES					0x88
+
+/* bits of control register */
+#define AST_GRAPHICS_CTRL_ENABLE			0x00000001 /* bit 0 */
+#define AST_GRAPHICS_CTRL_FORMAT			0x00000380 /* bits[9:7] */
+#define AST_GRAPHICS_CTRL_H_SYNC_POLAR		0x00010000 /* bit 16 */
+#define AST_GRAPHICS_CTRL_V_SYNC_POLAR		0x00020000 /* bit 17 */
+#define AST_GRAPHICS_CTRL_H_SYNC_OFF		0x00040000 /* bit 18 */
+#define AST_GRAPHICS_CTRL_V_SYNC_OFF		0x00080000 /* bit 19 */
+
+#define AST_GRAPHICS_CTRL_FORMAT_RGB555			0x00000000
+#define AST_GRAPHICS_CTRL_FORMAT_XRGB8888		0x00000100
+#define AST_GRAPHICS_CTRL_FORMAT_YUV444_RGB		0x00000270
+#define AST_GRAPHICS_CTRL_FORMAT_YUV444_YUV		0x00000070
+
+/* bits of control register 2 */
+#define AST_GRAPHICS_CTRL2_DAC					0x00000001 /* bit 0 */
+#define AST_GRAPHICS_CTRL2_DVO					0x00000080 /* bit 7 */
+
+struct astfb_info {
+	/* fb info */
+	struct fb_info *info;
+	struct device *dev;
+
+	struct fb_var_screeninfo var;
+	struct fb_fix_screeninfo fix;
+	u32 pseudo_palette[17];
+	
+	/* driver registered */
+	int registered;
+	
+	/* chip info */
+	char name[16];
+
+	/* resource stuff */
+	unsigned long frame_buf_phys;
+	unsigned long frame_buf_sz;
+	void *frame_buf;
+
+	unsigned long ulMMIOPhys;
+	unsigned long ulMMIOSize;
+
+	void __iomem *io;
+	void __iomem *io_2d;
+
+	/* Options */
+
+	/* command queue */
+	unsigned long cmd_q_sz;
+	unsigned long cmd_q_offset;
+	int use_2d_engine;
+
+	/* mode stuff */
+	int xres;
+	int yres;
+	int xres_virtual;
+	int yres_virtual;
+	int bpp;
+	int pixclock;
+	int pitch;
+	int refreshrate;
+};
+
+#endif /* !__AST_FB_H__ */
