--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Mon Sep  5 17:06:11 2011
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Thu Sep  8 16:51:38 2011
@@ -92,6 +92,8 @@
 	private USBKeyboardRep m_USBKeyRep;
 	private USBMouseRep m_USBMouseRep;
 	byte m_USBMouseMode = USBMouseRep.INVALID_MOUSE_MODE;
+	private InfoDialog infoDialog = null;
+	
 	public static final int NUMLOCK		 = 0x01;
 	public static final int CAPSLOCK	 = 0x02;
 	public static final int SCROLLLOCK 	 = 0x04;
@@ -368,17 +370,17 @@
 	 * @param mode Absolute or Relative
 	 */
 	public void SetUSBMouseMode(byte mode) {
-		if(!firsttime){
-			InfoDialog dlg = new InfoDialog();
-		}
-		// if absolute mode is set then the firsttime flag will bw set false, so
-		// that next time if the relative mode comes next time it will 
-		// show the information dialog. 
+		/*
+		 * Show the information dialog if the information Dialog object is not equal to null. Otherwise create the dialog
+		 * object. 
+		 */
+		if(infoDialog != null){
+			infoDialog.showDialog();
+		}
+		else{
+			infoDialog = new InfoDialog();
+		}
 		
-		if(mode == USBMouseRep.ABSOLUTE_MOUSE_MODE)
-			firsttime = false;
-		else
-			firsttime = true;
 		m_USBMouseMode = mode;
 		JVMenu.m_mouseMode = mode;
 	}
