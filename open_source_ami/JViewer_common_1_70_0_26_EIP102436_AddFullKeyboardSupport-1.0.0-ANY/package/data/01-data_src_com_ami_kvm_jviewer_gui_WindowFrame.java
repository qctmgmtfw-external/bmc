--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/WindowFrame.java	Wed Oct  3 10:32:50 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/WindowFrame.java	Wed Oct  3 10:43:56 2012
@@ -68,6 +68,7 @@
 								 JViewerApp.getInstance().getRCView().USBsyncCursor(true);
 							 if(JViewerApp.getInstance().getRCView().GetUSBMouseMode() == USBMouseRep.RELATIVE_MOUSE_MODE)
 								 Mousecaliberation.resetCursor();
+								 JViewerApp.getInstance().resetModifiers();
 			           	}
 					 }
 					 public void windowLostFocus(WindowEvent e)
