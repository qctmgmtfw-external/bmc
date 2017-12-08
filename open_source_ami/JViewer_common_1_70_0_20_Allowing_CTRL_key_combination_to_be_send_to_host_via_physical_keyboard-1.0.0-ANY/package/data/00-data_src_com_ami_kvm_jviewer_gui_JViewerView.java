--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Wed Jul 25 10:35:46 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerView.java Wed Jul 25 11:41:39 2012
@@ -53,6 +53,7 @@
 import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.JViewer;
 import com.ami.kvm.jviewer.hid.KeyProcessor;
+import com.ami.kvm.jviewer.hid.USBKeyProcessorEnglish;
 import com.ami.kvm.jviewer.hid.USBKeyboardRep;
 import com.ami.kvm.jviewer.hid.USBMouseRep;
 import com.ami.kvm.jviewer.kvmpkts.KVMClient;
@@ -516,6 +517,39 @@
 		 */
 		public void keyPressed(KeyEvent e) {
 		
+			// A modifier which is set without the corresponding key being pressed, or menu item being selected
+			//(Control key menus or satusbar toggle buttons), should be reset before a key event is being sent.
+			byte modifiers = USBKeyProcessorEnglish.getModifiers();
+			if(((modifiers & KeyProcessor.MOD_LEFT_CTRL) == KeyProcessor.MOD_LEFT_CTRL)&&
+					((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != KeyEvent.CTRL_DOWN_MASK)&&
+					!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_LEFT_CTRL_KEY)){
+				modifiers &= ~KeyProcessor.MOD_LEFT_CTRL;
+			}
+			if(((modifiers & KeyProcessor.MOD_RIGHT_CTRL) == KeyProcessor.MOD_RIGHT_CTRL)&&
+					((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != KeyEvent.CTRL_DOWN_MASK)&&
+					!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_RIGHT_CTRL_KEY)){
+				modifiers &= ~KeyProcessor.MOD_RIGHT_CTRL;
+			}
+			if(((modifiers & KeyProcessor.MOD_LEFT_ALT) == KeyProcessor.MOD_LEFT_ALT)&&
+					((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) != KeyEvent.ALT_DOWN_MASK)&&
+					!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_LEFT_ALT_KEY)){
+				modifiers &= ~KeyProcessor.MOD_LEFT_ALT;
+			}
+			if(((modifiers & KeyProcessor.MOD_RIGHT_ALT) == KeyProcessor.MOD_RIGHT_ALT)&&
+					((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) != KeyEvent.ALT_DOWN_MASK)&&
+					!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_RIGHT_ALT_KEY)){
+				modifiers &= ~KeyProcessor.MOD_RIGHT_ALT;
+			}
+			if(((modifiers & KeyProcessor.MOD_LEFT_SHIFT) == KeyProcessor.MOD_LEFT_SHIFT)&&
+					((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != KeyEvent.SHIFT_DOWN_MASK)){
+				modifiers &= ~KeyProcessor.MOD_LEFT_SHIFT;
+			}
+			if(((modifiers & KeyProcessor.MOD_RIGHT_SHIFT) == KeyProcessor.MOD_RIGHT_SHIFT)&&
+					((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != KeyEvent.SHIFT_DOWN_MASK)){
+				modifiers &= ~KeyProcessor.MOD_RIGHT_SHIFT;
+			}
+			USBKeyProcessorEnglish.setModifiers(modifiers);
+
 			Class cl;
 			try {
 				cl = Class.forName("java.awt.AWTEvent");
@@ -531,25 +565,28 @@
 			KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
 			byte led_status=0;
 			
-			if(e.getKeyCode()==17)
-				return;
-			
 			// Windows Key
 			if(e.getKeyCode() == KeyEvent.VK_WINDOWS)
 				return;
+
+			//Alt key modifier will  be allowed to pass, if its pressed along with Control of Shift modifier
 			if ((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) == KeyEvent.ALT_DOWN_MASK) {
-				if (JViewerApp.getInstance().isFullScreenMode()) {
-						/*
-						 * In full screen mode, top menu is not associated with
-						 * the view and so the regular shortcuts will not work.
-						 * So the following keyboard shortcuts are required only
-						 * in full screen mode. All the follwoing keyboard
-						 * shortcuts are combined with ALT Key
-						 */
-						OnInvokeMenuShortCutFullscreen(e);
-						e.consume();
+				if(((e.getModifiersEx() & KeyEvent.CTRL_DOWN_MASK) != KeyEvent.CTRL_DOWN_MASK)&&
+						((e.getModifiersEx() & KeyEvent.SHIFT_DOWN_MASK) != KeyEvent.SHIFT_DOWN_MASK)&&
+						(e.getKeyCode() != KeyEvent.VK_ALT)){
+					if (JViewerApp.getInstance().isFullScreenMode()) {
+							/*
+							 * In full screen mode, top menu is not associated with
+							 * the view and so the regular shortcuts will not work.
+							 * So the following keyboard shortcuts are required only
+							 * in full screen mode. All the follwoing keyboard
+							 * shortcuts are combined with ALT Key
+							 */
+							OnInvokeMenuShortCutFullscreen(e);
+							e.consume();
+					}
+					return;
 				}
-				return;
 			}
 
 				/* Need to remember Ctrl key state to avoid CTRL+ESC condition */
@@ -560,9 +597,9 @@
 					m_bRightCtrlDown = true;
 				if (e.getKeyCode() == KeyEvent.VK_F1) {
 					JViewerApp.getInstance().OnHelpAboutJViewer();
+					e.consume();
+					return;
 				}
-				e.consume();
-				return;
 			}
 
 			//Avoid Windows Security Dialog to pop out
@@ -663,9 +700,6 @@
 			m_USBKeyRep.set(keyCode, keyLocation, false);
 			KVMClient kvmClnt = JViewerApp.getInstance().getKVMClient();
 			
-			if(e.getKeyCode()==17)
-				return;
-
 			if ((e.getModifiersEx() & KeyEvent.ALT_DOWN_MASK) == KeyEvent.ALT_DOWN_MASK)
 				return;
 
