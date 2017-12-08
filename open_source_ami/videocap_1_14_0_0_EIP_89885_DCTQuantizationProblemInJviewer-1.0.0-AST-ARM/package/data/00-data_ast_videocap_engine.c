--- .pristine/videocap-1.14.0-ARM-AST-src/data/ast_videocap_engine.c Wed May 23 10:40:21 2012
+++ source/videocap-1.14.0-ARM-AST-src/data/ast_videocap_engine.c Thu May 24 14:33:53 2012
@@ -827,13 +827,13 @@
 int ast_videocap_enable_video_dac(struct ast_videocap_engine_info_t *info, struct ast_videocap_engine_config_t *config)
 {
 	ast_videocap_data_in_old_video_buf = 0; /* configurations are changed, send a full screen next time */
-	info->INFData.DifferentialSetting = config->differential_setting;
-	info->FrameHeader.JPEGScaleFactor = config->dct_quant_quality;
-	info->FrameHeader.JPEGTableSelector = config->dct_quant_tbl_select;
-	info->FrameHeader.SharpModeSelection = config->sharp_mode_selection;
-	info->FrameHeader.AdvanceScaleFactor = config->sharp_quant_quality;
-	info->FrameHeader.AdvanceTableSelector = config->sharp_quant_tbl_select;
-	info->FrameHeader.CompressionMode = config->compression_mode;
+//	info->INFData.DifferentialSetting = config->differential_setting;
+//	info->FrameHeader.JPEGScaleFactor = config->dct_quant_quality;
+//	info->FrameHeader.JPEGTableSelector = config->dct_quant_tbl_select;
+//	info->FrameHeader.SharpModeSelection = config->sharp_mode_selection;
+//	info->FrameHeader.AdvanceScaleFactor = config->sharp_quant_quality;
+//	info->FrameHeader.AdvanceTableSelector = config->sharp_quant_tbl_select;
+//	info->FrameHeader.CompressionMode = config->compression_mode;
     config->vga_dac=1;
 	ast_videocap_vga_dac_ctrl(1);
 	return 0;
