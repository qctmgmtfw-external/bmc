--- .pristine/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/SOCKVMclient.java Fri Jun 29 17:44:59 2012
+++ source/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/SOCKVMclient.java Fri Jun 29 17:51:21 2012
@@ -69,37 +69,37 @@
 
 			// change selected menu item according to configs
 			if (configs.compression_mode == 0) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_COMPRESSION_YUV_420).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_COMPRESSION_YUV_420, true);
 			} else if (configs.compression_mode == 1) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_COMPRESSION_YUV_444).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_COMPRESSION_YUV_444, true);
 			} else if (configs.compression_mode == 2) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_COMPRESSION_YUV_444_2_COLORS_VQ).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_COMPRESSION_YUV_444_2_COLORS_VQ, true);
 			} else if (configs.compression_mode == 3) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_COMPRESSION_YUV_444_4_COLORS_VQ).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_COMPRESSION_YUV_444_4_COLORS_VQ, true);
 			}
 
 			if (configs.dct_quant_tbl_select == 0) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_0).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_0, true);
 			} else if (configs.dct_quant_tbl_select == 1) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_1).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_1, true);
 			} else if (configs.dct_quant_tbl_select == 2) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_2).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_2, true);
 			} else if (configs.dct_quant_tbl_select == 3) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_3).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_3, true);
 			} else if (configs.dct_quant_tbl_select == 4) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_4).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_4, true);
 			} else if (configs.dct_quant_tbl_select == 5) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_5).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_5, true);
 			} else if (configs.dct_quant_tbl_select == 6) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_6).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_6, true);
 			} else if (configs.dct_quant_tbl_select == 7) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_7).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_DCT_QUANTIZATION_TABLE_7, true);
 			}
 
 			if (configs.vga_dac == 0) {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_HOST_VIDEO_OUTPUT).setSelected(false);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_HOST_VIDEO_OUTPUT, false);
 			} else {
-				JViewerApp.getInstance().getJVMenu().getMenuItem(SOCMenu.VIDEO_HOST_VIDEO_OUTPUT).setSelected(true);
+				JViewerApp.getInstance().getJVMenu().notifyMenuStateSelected(SOCMenu.VIDEO_HOST_VIDEO_OUTPUT, true);
 			}
 			break;
 
