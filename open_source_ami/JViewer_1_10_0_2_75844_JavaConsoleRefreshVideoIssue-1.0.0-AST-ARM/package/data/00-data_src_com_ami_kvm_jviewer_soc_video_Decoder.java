--- .pristine/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/video/Decoder.java Tue Nov 22 15:05:47 2011
+++ source/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/video/Decoder.java Thu Nov 24 14:31:38 2011
@@ -1720,7 +1720,7 @@
 
 				m_view.repaint((txb - 1) * 16, tyb * 16, 16, 16);
 				MBProcessed ++;
-			} while (_index <= CompressBufferSize);
+			} while (_index < CompressBufferSize);
 	 	} catch (Exception e) {
 			e.printStackTrace();
 		}
