--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Tue Jul 10 13:50:40 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Wed Jul 11 11:37:42 2012
@@ -165,9 +165,9 @@
 
 
 	public void setkeyprocessor(){
-		KeyProcessor m_keyprocesor = JViewerApp.getInstance().getKeyprocesssor();
-		JViewerApp.getInstance().getKeyprocesssor().setAutoKeybreakMode(true);
-		JViewerApp.getInstance().getM_USBKeyRep().setM_USBKeyProcessor(m_keyprocesor);
+		KeyProcessor m_keyprocesor = JViewerApp.getInstance().getKeyProcesssor();
+		JViewerApp.getInstance().getKeyProcesssor().setAutoKeybreakMode(true);
+		m_USBKeyRep.setM_USBKeyProcessor(m_keyprocesor);
 	}
 
 	/**
@@ -504,8 +504,8 @@
 	 */
 	class RCKeyListener extends KeyAdapter {
 		public void keyTyped(KeyEvent e) {
-			if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null)
-				JViewerApp.getInstance().getJVMenu().getAutokeylayout().OnkeyTyped(e);
+			if(JViewerApp.getInstance().getAutokeylayout() != null)
+				JViewerApp.getInstance().getAutokeylayout().OnkeyTyped(e);
 		}
 
 		/**
@@ -588,8 +588,8 @@
 				JViewerApp.getInstance().getM_wndFrame().getM_status().setKeyboardLEDStatus(led_status);
 				JViewerApp.getInstance().setLed_status(led_status);
 
-				if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null)
-					if(JViewerApp.getInstance().getJVMenu().getAutokeylayout().OnkeyPressed(e))
+				if(JViewerApp.getInstance().getAutokeylayout() != null)
+					if(JViewerApp.getInstance().getAutokeylayout().OnkeyPressed(e))
 						return;
 			}
 			if(bdata != null && bdata[14] == 115 )// to slove mac auto keyboard issue
@@ -676,8 +676,8 @@
 				else
 					m_bRightCtrlDown = false;
 			}
-			if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null)
-				if(JViewerApp.getInstance().getJVMenu().getAutokeylayout().OnkeyReleased(e))
+			if(JViewerApp.getInstance().getAutokeylayout() != null)
+				if(JViewerApp.getInstance().getAutokeylayout().OnkeyReleased(e))
 					return;
 
 			m_USBKeyRep.set(e.getKeyCode(), e.getKeyLocation(), false);
