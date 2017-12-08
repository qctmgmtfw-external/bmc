--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Aug 24 10:49:12 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Aug 24 11:09:10 2012
@@ -1090,6 +1090,10 @@
 		// update menu option
 		m_frame.getMenu().notifyMenuStateSelected(
 				JVMenu.KEYBOARD_LEFT_WINKEY_PRESSHOLD, state);
+		
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}	
 	}
 
 	/**
@@ -1120,6 +1124,10 @@
 		m_KVMClnt.sendKMMessage(m_USBKeyRep);
 		// update menu option
 		m_frame.getMenu().notifyMenuStateSelected(JVMenu.KEYBOARD_RIGHT_WINKEY_PRESSHOLD, state);
+		
+		if(getSoftKeyboard() != null) {
+			getSoftKeyboard().syncHoldKey();
+		}
 	}
 
 	/**
