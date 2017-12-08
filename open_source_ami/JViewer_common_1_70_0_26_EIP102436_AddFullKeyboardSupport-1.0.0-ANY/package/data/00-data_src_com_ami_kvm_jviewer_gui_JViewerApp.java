--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java	Wed Oct  3 10:32:51 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java	Wed Oct  3 10:39:15 2012
@@ -361,6 +361,8 @@
 
 	private String currentVersion = "1.70.0";	
 	
+	private boolean fullKeyboardEnabled = false;
+	
 	private Object createObject(String className) {
 		Object object = null;
 		try {
@@ -971,6 +973,10 @@
 		m_frame.getMenu().notifyMenuStateSelected(JVMenu.VIDEO_FULL_SCREEN, state);
 		m_frame.getMenu().notifyMenuStateEnable(JVMenu.VIDEO_FULL_SCREEN, true);
 		m_frame.getMenu().refreshMenu();
+		//Maintain menu mneumonics and accelerartor status when switching between fullscreen and normal mode.  
+		JViewerApp.getInstance().getJVMenu().getMenuItem(JVMenu.KEYBOARD_FULL_KEYBOARD).setSelected(isFullKeyboardEnabled());
+		JViewerApp.getInstance().getJVMenu().enableMenuAccelerator(isFullKeyboardEnabled());
+		JViewerApp.getInstance().getJVMenu().enableMenuMnemonics(isFullKeyboardEnabled());
 	}
 
 	/**
@@ -1024,6 +1030,32 @@
 		}
 	}
 
+	/**
+	 * Hold left shift key request handler
+	 */
+	public void OnKeyboardHoldLeftShiftKey(boolean state) {
+		Debug.out.println("OnKeyboardHoldLeftShiftKey");
+
+		if (!m_KVMClnt.redirection())
+			return;
+
+		m_USBKeyRep.set(KeyEvent.VK_SHIFT, KeyEvent.KEY_LOCATION_LEFT, state);
+		m_KVMClnt.sendKMMessage(m_USBKeyRep);
+	}
+
+	/**
+	 *  Hold right shift key request handler
+	 */
+	public void OnKeyboardHoldRightShiftKey(boolean state) {
+		Debug.out.println("OnKeyboardHoldRightShiftKey");
+
+		if (!m_KVMClnt.redirection())
+			return;
+
+		m_USBKeyRep.set(KeyEvent.VK_SHIFT, KeyEvent.KEY_LOCATION_RIGHT, state);
+		m_KVMClnt.sendKMMessage(m_USBKeyRep);
+	}
+	
 	/**
 	 * Hold left control key request handler
 	 */
@@ -1181,6 +1213,36 @@
 		m_KVMClnt.sendKMMessage(m_USBKeyRep);
 	}
 
+	/**
+	 * Reset the Modifiers
+	 */
+	public void resetModifiers(){
+		byte modifiers = USBKeyProcessorEnglish.getModifiers();
+		if((modifiers & KeyProcessor.MOD_LEFT_CTRL) == KeyProcessor.MOD_LEFT_CTRL &&
+		!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_LEFT_CTRL_KEY)){
+			JViewerApp.getInstance().OnKeyboardHoldLeftCtrlKey(false);
+		}
+		if((modifiers & KeyProcessor.MOD_RIGHT_CTRL) == KeyProcessor.MOD_RIGHT_CTRL &&
+				!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_RIGHT_CTRL_KEY)){
+			JViewerApp.getInstance().OnKeyboardHoldRightCtrlKey(false);
+		}
+		if(((modifiers & KeyProcessor.MOD_LEFT_ALT) == KeyProcessor.MOD_LEFT_ALT)&&
+				!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_LEFT_ALT_KEY)){
+			JViewerApp.getInstance().OnKeyboardHoldLeftAltKey(false);
+		}
+		if(((modifiers & KeyProcessor.MOD_RIGHT_ALT) == KeyProcessor.MOD_RIGHT_ALT)&&
+				!JViewerApp.getInstance().getJVMenu().getMenuSelected(JVMenu.KEYBOARD_RIGHT_ALT_KEY)){
+			JViewerApp.getInstance().OnKeyboardHoldRightAltKey(false);
+		}
+		if(((modifiers & KeyProcessor.MOD_LEFT_SHIFT) == KeyProcessor.MOD_LEFT_SHIFT)){
+			JViewerApp.getInstance().OnKeyboardHoldLeftShiftKey(false);
+		}
+		if(((modifiers & KeyProcessor.MOD_RIGHT_SHIFT) == KeyProcessor.MOD_RIGHT_SHIFT)){
+			JViewerApp.getInstance().OnKeyboardHoldRightShiftKey(false);
+		}
+		USBKeyProcessorEnglish.setModifiers(modifiers);
+	}
+	
 	/**
 	 * soft keyboard request handler
 	 */
@@ -2601,6 +2663,21 @@
 	public void setM_bVMUseSSL(boolean useSSL) {
 		m_bVMUseSSL = useSSL;
 	}
+
+	/**
+	 * @return the fullKeyboardEnabled
+	 */
+	public boolean isFullKeyboardEnabled() {
+		return fullKeyboardEnabled;
+	}
+
+	/**
+	 * @param fullKeyboardEnabled the fullKeyboardEnabled to set
+	 */
+	public void setFullKeyboardEnabled(boolean fullKeyboardEnabled) {
+		this.fullKeyboardEnabled = fullKeyboardEnabled;
+	}
+
 	public void syncVMediaRedirection(){
 		synchronized (CDROMRedir.getSyncObj()) {
 			for(int count = 0; count< JViewerApp.getInstance().getM_cdNum();count++)
