--- .pristine/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/video/HardwareCursor.java Thu Jul 26 15:17:24 2012
+++ source/JViewer-1.10.0-ARM-AST-src/data/src/com/ami/kvm/jviewer/soc/video/HardwareCursor.java Mon Jul 30 10:51:05 2012
@@ -139,6 +139,9 @@
 				}
 			}
 		}
-		viewerView.repaint(m_currCursorRect);
+		if (JViewerApp.getInstance().getJVMenu().m_scale != 1.0)
+			viewerView.repaint();
+		else 
+			viewerView.repaint(m_currCursorRect);
 	}
 }
