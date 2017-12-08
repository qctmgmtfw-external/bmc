--- .pristine/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/SOCFrameHdr.java	Wed Jun 13 14:18:17 2012
+++ source/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/SOCFrameHdr.java	Thu Jun 14 18:23:44 2012
@@ -12,6 +12,7 @@
 
 package com.ami.kvm.jviewer.soc;
 
+import java.nio.ByteOrder;
 import java.nio.ByteBuffer;
 import java.util.Arrays;
 
@@ -33,6 +34,7 @@
 	
 	private VideoHeader videoHdr;
 	private VideoEngineInfo videoEngInfo;
+	public int frameSize;
 
 	/* constructor */
 	public SOCFrameHdr() {
@@ -46,6 +48,12 @@
 	 */
 	public void setHeader(ByteBuffer buf) {
 		m_compressedBuf = videoHdr.set(buf);
+
+		//to solved the auto video recording can not play video - get frame size
+		int bufsize = buf.limit();
+		if (bufsize == 86) {
+			setFrameSize(buf);
+		}
 	}
 
 	public byte[] getCompressedBuf() {
@@ -124,6 +132,70 @@
 	}
 
 	public int getFrameSize() {
-		return 0;///Add the Frame size Video data size
-	}
+		return frameSize;///Add the Frame size Video data size
+	}
+	private void setFrameSize(ByteBuffer buf) {
+		int [] FrameHdr_Reserved = new int[2];
+
+		buf.rewind();
+		buf.order(ByteOrder.LITTLE_ENDIAN);
+
+		short iEngVersion = buf.getShort();
+		short wHeaderLen = buf.getShort();
+
+		// SourceModeInfo
+		short SourceMode_X = buf.getShort();
+		short SourceMode_Y = buf.getShort();
+		short SourceMode_ColorDepth = buf.getShort();
+		short SourceMode_RefreshRate = buf.getShort();
+		byte SourceMode_ModeIndex = buf.get();
+
+		// DestinationModeInfo
+		short DestinationMode_X = buf.getShort();
+		short DestinationMode_Y = buf.getShort();
+		short DestinationMode_ColorDepth = buf.getShort();
+		short DestinationMode_RefreshRate = buf.getShort();
+		byte DestinationMode_ModeIndex = buf.get();
+
+		// FRAME_HEADER
+		int FrameHdr_StartCode = buf.getInt();
+		int FrameHdr_FrameNumber = buf.getInt();
+		short FrameHdr_HSize = buf.getShort();
+		short FrameHdr_VSize = buf.getShort();
+		FrameHdr_Reserved[0] = buf.getInt();
+		FrameHdr_Reserved[1] = buf.getInt();
+		byte FrameHdr_CompressionMode = buf.get();
+		byte FrameHdr_JPEGScaleFactor = buf.get();
+		byte FrameHdr_JPEGTableSelector = buf.get();
+		byte FrameHdr_JPEGYUVTableMapping = buf.get();
+		byte FrameHdr_SharpModeSelection = buf.get();
+		byte FrameHdr_AdvanceTableSelector = buf.get();
+		byte FrameHdr_AdvanceScaleFactor = buf.get();
+		int FrameHdr_NumberOfMB = buf.getInt();
+		byte FrameHdr_RC4Enable = buf.get();
+		byte FrameHdr_RC4Reset = buf.get();
+		byte Mode420 = buf.get();
+
+		// INF_DATA
+		byte InfData_DownScalingMethod = buf.get();
+		byte InfData_DifferentialSetting = buf.get();
+		short InfData_AnalogDifferentialThreshold = buf.getShort();
+		short InfData_DigitalDifferentialThreshold = buf.getShort();
+		byte InfData_ExternalSignalEnable = buf.get();
+		byte InfData_AutoMode = buf.get();
+		byte InfData_VQMode = buf.get();
+
+		// COMPRESS_DATA
+		int CompressData_SourceFrameSize = buf.getInt();
+		int CompressData_CompressSize = buf.getInt();
+		int CompressData_HDebug = buf.getInt();
+		int CompressData_VDebug = buf.getInt();
+		
+		byte InputSignal = buf.get();
+		short Cursor_XPos = buf.getShort();
+		short Cursor_YPos = buf.getShort();
+
+		this.frameSize = CompressData_CompressSize;
+	}
+	
 }
