--- .pristine/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/reader/HardwareCursorReader.java Tue Nov 22 13:20:30 2011
+++ source/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/reader/HardwareCursorReader.java Tue Nov 22 13:53:23 2011
@@ -67,14 +67,21 @@
 			return -1;
 		}
 
+                if (hwCursorIndex == 0)
+                {
+                        KVMReader reader = client.getHdrReader();
+                        reader.initialize();
+                        client.setState(reader);
+                        return 0;
+                }
+
 		if (hwCursorIndex == (client.m_pktHdr.pktSize)) { // data received fully
-			((SOCApp) JViewerApp.getInstance().getSoc_App()).onHardwareCursor(hwCursorByteBuf);
-			hwCursorByteBuf.position(0);
-
 			// back to header reader
 			KVMReader reader = client.getHdrReader();
 			reader.initialize();
 			client.setState(reader);
+
+                        ((SOCApp) JViewerApp.getInstance().getSoc_App()).onHardwareCursor(hwCursorByteBuf);
 		}
 		return 0;
 	}
