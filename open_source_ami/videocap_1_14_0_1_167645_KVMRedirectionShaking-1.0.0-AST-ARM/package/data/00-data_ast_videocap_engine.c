--- .pristine/videocap-1.14.0-ARM-AST-src/data/ast_videocap_engine.c Fri Mar 28 17:22:23 2014
+++ source/videocap-1.14.0-ARM-AST-src/data/ast_videocap_engine.c Mon May 26 14:40:35 2014
@@ -42,6 +42,7 @@
 unsigned int CaptureTimeouts = 0;
 unsigned int CompressionTimeouts = 0;
 unsigned int SpuriousTimeouts = 0;
+unsigned int AutoModeTimeouts = 0;
 
 struct ast_videocap_engine_info_t ast_videocap_engine_info;
 
@@ -50,11 +51,13 @@
 static int ModeDetectionIntrRecd;
 static int CaptureIntrRecd;
 static int CompressionIntrRecd;
+static int AutoModeIntrRecd;
 
 /* Status variables for help in debugging  */
 int WaitingForModeDetection;
 int WaitingForCapture;
 int WaitingForCompression;
+int WaitingForAutoMode = 0;
 
 static void *vga_mem_virt_addr;
 
@@ -64,6 +67,7 @@
 wait_queue_head_t mdwq;
 wait_queue_head_t capwq;
 wait_queue_head_t compwq;
+wait_queue_head_t autowq;
 
 /* Defines for Video Interrupt Control Register */
 #define IRQ_WATCHDOG_OUT_OF_LOCK     (1<<0)
@@ -231,6 +235,7 @@
 	ModeDetectionIntrRecd = 0;
 	CaptureIntrRecd = 0;
 	CompressionIntrRecd = 0;
+	AutoModeIntrRecd = 0;
 
 	WaitingForModeDetection = 0;
 	WaitingForCapture = 0;
@@ -249,6 +254,8 @@
 	ast_videocap_engine_info.FrameHeader.CompressionMode = COMPRESSION_MODE;
 	ast_videocap_engine_info.FrameHeader.JPEGTableSelector = JPEG_TABLE_SELECTOR;
 	ast_videocap_engine_info.FrameHeader.AdvanceTableSelector = ADVANCE_TABLE_SELECTOR;
+
+	init_waitqueue_head(&autowq);
 }
 
 irqreturn_t ast_videocap_irq_handler(int irq, void *dev_id)
@@ -272,13 +279,26 @@
 	/* Capture Engine Idle */
 	if (status & STATUS_CAPTURE_COMPLETE) {
 		CaptureIntrRecd = 1;
-		wake_up_interruptible(&capwq);
+		if (!AUTO_MODE) {
+			wake_up_interruptible(&capwq);
+		}
 	}
 
 	/* Compression Engine Idle */
 	if (status & STATUS_COMPRESSION_COMPLETE) {
 		CompressionIntrRecd = 1;
-		wake_up_interruptible(&compwq);
+		if (!AUTO_MODE) {
+			wake_up_interruptible(&compwq);
+		}
+	}
+
+	if ((ISRDetectedModeOutOfLock == 1) || ((CaptureIntrRecd == 1) && (CompressionIntrRecd == 1))) {
+		CaptureIntrRecd = 0;
+		CompressionIntrRecd = 0;
+		AutoModeIntrRecd = 1;
+		 if (AUTO_MODE) {
+			wake_up_interruptible (&autowq);
+		 }
 	}
 
 	return IRQ_HANDLED;
@@ -415,6 +435,12 @@
 
 	if (Type == COMPRESSION_TIMEOUT) {
 		CompressionTimeouts ++;
+		ReInitVideoEngine = 1;
+		return;
+	}
+
+	if (Type == AUTOMODE_TIMEOUT) {
+		AutoModeTimeouts ++;
 		ReInitVideoEngine = 1;
 		return;
 	}
@@ -515,6 +541,44 @@
 			return -1; /* Compression remains busy */
 		} else {
 			/* CompressionIntrRecd is set, but timeout triggered - should never happen!! */
+			ProcessTimeOuts(SPURIOUS_TIMEOUT);
+			/* Not much to do here, just continue on */
+		}
+	}
+
+	return 0;
+}
+
+int ast_videocap_automode_trigger(void)
+{
+	unsigned int status;
+
+	TimeToWait = VE_TIMEOUT*HZ;
+	AutoModeIntrRecd = 0;
+
+	if (!(ast_videocap_read_reg(AST_VIDEOCAP_SEQ_CTRL) & AST_VIDEOCAP_SEQ_CTRL_CAP_STATUS)) {
+		 return -1; /* capture engine remains busy */
+	}
+	if (!(ast_videocap_read_reg(AST_VIDEOCAP_SEQ_CTRL) & AST_VIDEOCAP_SEQ_CTRL_COMPRESS_STATUS)) {
+		 return -1; /* compression engine remains busy */
+	}
+
+	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS_IDLE);
+	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_CAP);
+	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS); 
+	barrier();
+
+	status = ast_videocap_read_reg(AST_VIDEOCAP_SEQ_CTRL);
+	ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, ((status & 0xFFFFFFED) | 0x12));
+
+	WaitingForAutoMode = 1;
+	if (wait_event_interruptible_timeout(autowq, AutoModeIntrRecd, TimeToWait) != 0) {
+		WaitingForAutoMode = 0;
+	} else {
+		 if (AutoModeIntrRecd == 0) {
+			ProcessTimeOuts(AUTOMODE_TIMEOUT);
+			return -1;
+		} else {                       
 			ProcessTimeOuts(SPURIOUS_TIMEOUT);
 			/* Not much to do here, just continue on */
 		}
@@ -765,7 +829,7 @@
 		ast_videocap_auto_position_adjust(info);
 	}
 
-	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_BIT_NUM); /* 18 bits for single edge only */
+	//ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_BIT_NUM); /* 18 bits for single edge only */
 
 	return 0;
 }
@@ -897,6 +961,37 @@
 	ast_videocap_write_reg(0x00080000, AST_VIDEOCAP_COMPRESS_CTRL);
 
 	DRAM_Data = *(volatile unsigned long *) SDRAM_CONFIG_REG;
+	
+#ifdef SOC_AST2300
+	switch (DRAM_Data & 0x03) {
+	case 0:
+		info->TotalMemory = 64;
+		break;
+	case 1:
+		info->TotalMemory = 128;
+		break;
+	case 2:
+		info->TotalMemory = 256;
+		break;
+	case 3:
+		info->TotalMemory = 512;
+		break;
+	}
+	switch ((DRAM_Data >> 2) & 0x03) {
+	case 0:
+		info->VGAMemory = 8;
+		break;
+	case 1:
+		info->VGAMemory = 16;
+		break;
+	case 2:
+		info->VGAMemory = 32;
+		break;
+	case 3:
+		info->VGAMemory = 64;
+		break;
+	}
+#else
 	switch ((DRAM_Data >> 2) & 0x03) {
 	case 0:
 		info->TotalMemory = 32;
@@ -927,6 +1022,7 @@
 		info->VGAMemory = 64;
 		break;
 	}
+#endif
 	//printk("VGA Memory = %ld MB\n", info->VGAMemory);
 
 	ast_videocap_get_mem_bandwidth(info);
@@ -999,8 +1095,13 @@
 	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_MASK);
 	info->FrameHeader.Mode420 = 0;
 
-	/* disable automatic compression */
-	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_AUTO_COMPRESS);
+	if (info->INFData.AutoMode) {
+		ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_AUTO_COMPRESS);
+	}
+	else {
+		/* disable automatic compression */
+		ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_AUTO_COMPRESS);
+	}
 
 	/* Internal always uses internal timing generation */
 	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_DE_SIGNAL);
@@ -1037,9 +1138,9 @@
 
 	/* set buffer offset based on detected mode */
 	buf_offset = info->src_mode.x;
-	remainder = buf_offset % 16;
+	remainder = buf_offset % 8;
 	if (remainder != 0) {
-		buf_offset += (16 - remainder);
+		buf_offset += (8 - remainder);
 	}
 	ast_videocap_write_reg(buf_offset * 4, AST_VIDEOCAP_SCAN_LINE_OFFSET);
 
@@ -1193,15 +1294,21 @@
 	ast_videocap_engine_init(info);
 	ast_videocap_horizontal_down_scaling(info);
 	ast_videocap_vertical_down_scaling(info);
-
-	if (ast_videocap_trigger_capture() < 0) {
-		return ASTCAP_IOCTL_BLANK_SCREEN;
-	}
-
-	// If RC4 is required, here is where we enable it
-
-	if (ast_videocap_trigger_compression() < 0) {
-		return ASTCAP_IOCTL_BLANK_SCREEN;
+	if (info->INFData.AutoMode) {
+		if (ast_videocap_automode_trigger() < 0) {
+			return ASTCAP_IOCTL_BLANK_SCREEN;
+		}
+	} else {
+
+		if (ast_videocap_trigger_capture() < 0) {
+			return ASTCAP_IOCTL_BLANK_SCREEN;
+		}
+
+		// If RC4 is required, here is where we enable it
+
+		if (ast_videocap_trigger_compression() < 0) {
+			return ASTCAP_IOCTL_BLANK_SCREEN;
+		}
 	}
 
 	ast_videocap_data_in_old_video_buf = 1;
