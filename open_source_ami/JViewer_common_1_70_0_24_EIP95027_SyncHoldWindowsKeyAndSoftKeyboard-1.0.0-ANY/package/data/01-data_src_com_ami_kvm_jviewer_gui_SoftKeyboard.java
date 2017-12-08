--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SoftKeyboard.java Fri Aug 24 10:49:12 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/SoftKeyboard.java Fri Aug 24 11:07:13 2012
@@ -1371,13 +1371,13 @@
 						switch (butid) {
 
 						case 68:// Left Windows
+							m_toggleKey[4].setSelected(true);
+							keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_LEFT, true);
+							m_KVMClnt.sendKMMessage(keyRep);
+							break;
+						case 72:// Right Windows
 							m_toggleKey[7].setSelected(true);
 							keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_RIGHT, true);
-							m_KVMClnt.sendKMMessage(keyRep);
-							break;
-						case 72:// Right Windows
-							m_toggleKey[4].setSelected(true);
-							keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_LEFT, true);
 							m_KVMClnt.sendKMMessage(keyRep);
 							break;
 						case 65:// Caps Lock
@@ -1683,14 +1683,28 @@
 						switch (butid) {
 
 						case 68:// Left Windows
-							m_toggleKey[7].setSelected(false);
-							keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_RIGHT, false);
-							m_KVMClnt.sendKMMessage(keyRep);
+							if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.KEYBOARD_LEFT_WINKEY_PRESSHOLD).isSelected()) {
+								m_toggleKey[4].setSelected(true);
+								keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_LEFT, true);
+								m_KVMClnt.sendKMMessage(keyRep);
+							}	 
+							else {
+								m_toggleKey[4].setSelected(false);
+								keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_LEFT, false);
+								m_KVMClnt.sendKMMessage(keyRep);	
+							}			
 							break;
 						case 72:// Right Windows
-							m_toggleKey[4].setSelected(false);
-							keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_LEFT, false);
-							m_KVMClnt.sendKMMessage(keyRep);
+							if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.KEYBOARD_RIGHT_WINKEY_PRESSHOLD).isSelected()) {
+								m_toggleKey[7].setSelected(true);
+								keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_RIGHT, true);
+								m_KVMClnt.sendKMMessage(keyRep);	
+							}
+							else {
+								m_toggleKey[7].setSelected(false);
+								keyRep.set(KeyEvent.VK_WINDOWS, KeyEvent.KEY_LOCATION_RIGHT, false);
+								m_KVMClnt.sendKMMessage(keyRep);
+							}
 							break;
 						case 65:// Caps Lock
 							if(lngindex != 16) {
@@ -2194,7 +2208,23 @@
 			m_skmouselistener.OnModifier(KeyEvent.VK_ALT,
 					KeyEvent.KEY_LOCATION_RIGHT,
 					KeyEvent.KEY_PRESSED);
-		}			
+		}
+
+		// Left Windows
+		if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.KEYBOARD_LEFT_WINKEY_PRESSHOLD).isSelected()) {
+			m_toggleKey[4].setSelected(true);			
+		}
+		else {
+			m_toggleKey[4].setSelected(false);
+		}
+
+		// Right Windows
+		if(JViewerApp.getInstance().getMainWindow().getMenu().getMenuItem(JVMenu.KEYBOARD_RIGHT_WINKEY_PRESSHOLD).isSelected()) {
+			m_toggleKey[7].setSelected(true);
+		}
+		else {
+			m_toggleKey[7].setSelected(false);				
+		}
 	}
 		
 	/**
