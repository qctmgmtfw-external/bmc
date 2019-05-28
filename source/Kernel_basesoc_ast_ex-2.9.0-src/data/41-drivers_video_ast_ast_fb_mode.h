--- linux.org/drivers/video/ast/ast_fb_mode.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/video/ast/ast_fb_mode.h	2014-01-31 15:42:24.563921267 -0500
@@ -0,0 +1,68 @@
+#ifndef __AST_FB_MODE_H__
+#define __AST_FB_MODE_H__
+
+/* Mode Limitation */
+#define MAX_HResolution		1600
+#define MAX_VResolution		1200
+
+/* Default Seting */
+#define CRT_LOW_THRESHOLD_VALUE         0x40
+#define CRT_HIGH_THRESHOLD_VALUE        0x70
+
+/* Std. Table Index Definition */
+#define TextModeIndex 		0
+#define EGAModeIndex 		1
+#define VGAModeIndex 		2
+#define HiCModeIndex 		3
+#define TrueCModeIndex 		4
+
+/* DCLK Index */
+#define VCLK25_175			0
+#define VCLK28_322			1
+#define VCLK31_5			2
+#define VCLK36				3
+#define VCLK40				4
+#define VCLK49_5			5
+#define VCLK50				6
+#define VCLK56_25			7
+#define VCLK65				8
+#define VCLK75				9
+#define VCLK78_75			10
+#define VCLK94_5			11
+#define VCLK108				12
+#define VCLK135				13
+#define VCLK157_5			14
+#define VCLK162				15
+#define VCLK148_5			16
+
+/* Mode flags definition */
+#define MODE_FLAG_SYNC_PP       0x00000000
+#define MODE_FLAG_SYNC_PN       0x00000001
+#define MODE_FLAG_SYNC_NP       0x00000002
+#define MODE_FLAG_SYNC_NN       0x00000003
+#define MODE_FLAG_V_BORDER      0x00000004
+#define MODE_FLAG_H_BORDER      0x00000008
+
+/* DAC Definition */
+#define DAC_NUM_TEXT	64
+#define DAC_NUM_EGA		64
+#define DAC_NUM_VGA		256
+
+struct ast_fb_mode_info_t {
+	unsigned long horiz_total;
+	unsigned long horiz_display_end;
+	unsigned long horiz_front_porch;
+	unsigned long horiz_sync;
+
+	unsigned long verti_total;
+	unsigned long verti_display_end;
+	unsigned long verti_front_porch;
+	unsigned long verti_sync;
+
+	int refresh_rate;
+	int clk_tab_index;
+
+	unsigned int flags;
+};
+
+#endif /* !__AST_FB_MODE_H__ */
