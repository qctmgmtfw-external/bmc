--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Wed Oct  3 10:32:51 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java	Wed Oct  3 10:40:44 2012
@@ -565,25 +565,20 @@
 			KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
 			byte led_status=0;
 			
-			// Windows Key
 			if(e.getKeyCode() == KeyEvent.VK_WINDOWS)
 				return;
-
-			//Alt key modifier will  be allowed to pass, if its pressed along with Control of Shift modifier
 			if ((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) == KeyEvent.ALT_DOWN_MASK) {
-				if(((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != KeyEvent.CTRL_DOWN_MASK)&&
-						((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != KeyEvent.SHIFT_DOWN_MASK)&&
-						(e.getKeyCode() != KeyEvent.VK_ALT)){
+				if(!JViewerApp.getInstance().isFullKeyboardEnabled()){
 					if (JViewerApp.getInstance().isFullScreenMode()) {
-							/*
-							 * In full screen mode, top menu is not associated with
-							 * the view and so the regular shortcuts will not work.
-							 * So the following keyboard shortcuts are required only
-							 * in full screen mode. All the follwoing keyboard
-							 * shortcuts are combined with ALT Key
-							 */
-							OnInvokeMenuShortCutFullscreen(e);
-							e.consume();
+						/*
+						 * In full screen mode, top menu is not associated with
+						 * the view and so the regular shortcuts will not work.
+						 * So the following keyboard shortcuts are required only
+						 * in full screen mode. All the follwoing keyboard
+						 * shortcuts are combined with ALT Key
+						 */
+						OnInvokeMenuShortCutFullscreen(e);
+						e.consume();
 					}
 					return;
 				}
@@ -595,7 +590,7 @@
 					m_bLeftCtrlDown = true;
 				else
 					m_bRightCtrlDown = true;
-				if (e.getKeyCode() == KeyEvent.VK_F1) {
+				if (e.getKeyCode() == KeyEvent.VK_F1 && !JViewerApp.getInstance().isFullKeyboardEnabled()) {
 					JViewerApp.getInstance().OnHelpAboutJViewer();
 					e.consume();
 					return;
@@ -699,9 +694,6 @@
 				keyCode = 226;
 			m_USBKeyRep.set(keyCode, keyLocation, false);
 			KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
-			
-			if ((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) == KeyEvent.ALT_DOWN_MASK)
-				return;
 
 			/* Need to remember Ctrl key state to avoid CTRL+ESC condition */
 			if ((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) == KeyEvent.CTRL_DOWN_MASK) {
