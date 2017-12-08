--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java	Wed Oct  3 10:32:51 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java	Wed Oct  3 10:43:31 2012
@@ -27,6 +27,7 @@
 import java.util.Hashtable;
 import java.util.Iterator;
 import java.util.Map;
+import java.util.Map.Entry;
 import java.util.Set;
 
 import javax.swing.ButtonGroup;
@@ -71,6 +72,7 @@
 	public static final String KEYBOARD_RIGHT_WINKEY_PRESSRELEASE = "KeyboardRightWindowsKeyPressRelease";
 	public static final String KEYBOARD_CTRL_ALT_DEL = "KeyboardCtrlAltDel";
 	public static final String KEYBOARD_CONTEXT_MENU = "KeyboardContextMenu";
+	public static final String KEYBOARD_FULL_KEYBOARD = "Full Keyborad Support";
 
 	public static final String AUTOMAIC_LANGUAGE="Auto detect";
 	public static final String KEYBOARD_LAYOUT = "Keyboard Layout";
@@ -173,6 +175,8 @@
     protected static Hashtable<String, Boolean> m_menuItems_setselected = new Hashtable<String, Boolean>();
     protected static Hashtable<String, Boolean> m_menuItems_setenabled = new Hashtable<String, Boolean>();
     protected static Hashtable<String, String> m_menustatusbar_text = new Hashtable<String, String>();
+    protected static Hashtable<String, Character> menuMnemonics = new Hashtable<String, Character>();
+    protected static Hashtable<String, KeyStroke> menuAccelerator = new Hashtable<String, KeyStroke>();
     public static String previous_bandwidth = JVMenu.OPTIONS_BANDWIDTH_100MBPS;
 
     // menu states
@@ -460,6 +464,13 @@
 			}
 			else if (cmdStr.equals(KEYBOARD_CONTEXT_MENU)) {
 				  RCApp.OnKeyboardContextMenu();
+			}
+			else if(cmdStr.equals(KEYBOARD_FULL_KEYBOARD)){
+				JCheckBoxMenuItem evtSrc = (JCheckBoxMenuItem) e.getSource();
+				SetMenuSelected(KEYBOARD_FULL_KEYBOARD, evtSrc.getState());
+				JViewerApp.getInstance().setFullKeyboardEnabled(evtSrc.getState());
+				enableMenuMnemonics(evtSrc.getState());
+				enableMenuAccelerator(evtSrc.getState());
 			}
 			else if (cmdStr.equals(MOUSE_CLIENTCURSOR_CONTROL)) {
 				JCheckBoxMenuItem evtSrc = (JCheckBoxMenuItem) e.getSource();
@@ -706,9 +717,14 @@
 		JMenuItem menuItem = new JMenuItem(menuName);
 		menuItem.addActionListener(m_menuListener);
 		menuItem.addMouseListener(m_menuStatus);
-		if(mnemonic != ' ')
+		if(mnemonic != ' '){
 			menuItem.setMnemonic(mnemonic);
-		menuItem.setAccelerator(KeyStroke.getKeyStroke(keyCode,modifiers));
+			menuMnemonics.put(actionCommand, mnemonic);
+		}
+		if(keyCode !=0 && modifiers != 0){
+			menuItem.setAccelerator(KeyStroke.getKeyStroke(keyCode,modifiers));
+			menuAccelerator.put(actionCommand, KeyStroke.getKeyStroke(keyCode,modifiers));
+		}
 		menuItem.setActionCommand(actionCommand);
 		m_menuItems.put(actionCommand, menuItem);
 		m_menustatusbar_text.put(actionCommand, status);
@@ -722,8 +738,10 @@
 		JMenuItem menuItem = new JMenuItem(menuName);
 		menuItem.addActionListener(m_menuListener);
 		menuItem.addMouseListener(m_menuStatus);
-		if(mnemonic != ' ')
+		if(mnemonic != ' '){
 			menuItem.setMnemonic(mnemonic);
+			menuMnemonics.put(actionCommand, mnemonic);
+		}
 		menuItem.setActionCommand(actionCommand);
 		m_menuItems.put(actionCommand, menuItem);
 		m_menustatusbar_text.put(actionCommand, status);
@@ -737,8 +755,10 @@
 		JMenuItem menuItem = new JMenuItem(menuName);
 		menuItem.addActionListener(m_menuListener);
 		menuItem.addMouseListener(m_menuStatus);
-		if(mnemonic != ' ')
+		if(mnemonic != ' '){
 			menuItem.setMnemonic(mnemonic);
+			menuMnemonics.put(actionCommand, mnemonic);
+		}
 		menuItem.setActionCommand(actionCommand);
 		menuItem.setEnabled(state);
 		m_menuItems.put(actionCommand, menuItem);
@@ -746,16 +766,21 @@
 		return menuItem;
 	}
 	
-	private JCheckBoxMenuItem  createCheckBocMenuItem( String menuName, char mnemonic, int keyCode, int modifiers, 
+	private JCheckBoxMenuItem  createCheckBoxMenuItem( String menuName, char mnemonic, int keyCode, int modifiers, 
 			String actionCommand, String status)
 	{
 		JCheckBoxMenuItem checkBoxMenuItem = new JCheckBoxMenuItem(menuName);
 		checkBoxMenuItem.addActionListener(m_menuListener);
 		checkBoxMenuItem.addMouseListener(m_menuStatus);
-		if(mnemonic != ' ')
+		if(mnemonic != ' '){
 			checkBoxMenuItem.setMnemonic(mnemonic);
+			menuMnemonics.put(actionCommand, mnemonic);
+		}
 		checkBoxMenuItem.setActionCommand(actionCommand);
-		checkBoxMenuItem.setAccelerator(KeyStroke.getKeyStroke(keyCode,modifiers));
+		if(keyCode != 0 && modifiers != 0){
+			checkBoxMenuItem.setAccelerator(KeyStroke.getKeyStroke(keyCode,modifiers));
+			menuAccelerator.put(actionCommand, KeyStroke.getKeyStroke(keyCode,modifiers));
+		}
 		m_menuItems.put(actionCommand, checkBoxMenuItem);
 		m_menustatusbar_text.put(actionCommand, status);
 		if(INITIAL_MENU_STATUS)
@@ -772,8 +797,10 @@
 		JCheckBoxMenuItem checkBoxMenuItem = new JCheckBoxMenuItem(menuName);
 		checkBoxMenuItem.addActionListener(m_menuListener);
 		checkBoxMenuItem.addMouseListener(m_menuStatus);
-		if(mnemonic != ' ')
+		if(mnemonic != ' '){
 			checkBoxMenuItem.setMnemonic(mnemonic);
+			menuMnemonics.put(actionCommand, mnemonic);
+		}
 		checkBoxMenuItem.setActionCommand(actionCommand);
 		m_menuItems.put(actionCommand, checkBoxMenuItem);
 		m_menustatusbar_text.put(actionCommand, status);
@@ -825,7 +852,7 @@
 		menu.addSeparator();
 
 		// construct full screen menu item
-		menuItem = createCheckBocMenuItem( "Full Screen", 'F', KeyEvent.VK_F, Event.ALT_MASK, VIDEO_FULL_SCREEN, "Change to full screen or regular mode");
+		menuItem = createCheckBoxMenuItem( "Full Screen", 'F', KeyEvent.VK_F, Event.ALT_MASK, VIDEO_FULL_SCREEN, "Change to full screen or regular mode");
 		if( JViewer.isStandalone() )
 			menu.add(menuItem);
 		menuItem.setEnabled(false);
@@ -907,6 +934,10 @@
 
 		menuItem = createMenuItem("Context Menu", ' ',KEYBOARD_CONTEXT_MENU, "Press Context Menu key Event");
 		menu.add(menuItem);
+		menu.addSeparator();
+		//full keyboard support menuitem
+		menuItem = createCheckBoxMenuItem("Full Keyboard Support", ' ', KEYBOARD_FULL_KEYBOARD,  "Enable/Disable Full Keyboard Support");
+		menu.add(menuItem);
 
 		return menu;
 	}
@@ -930,10 +961,10 @@
 		menu.setMnemonic('u');
 
 		//construct sync cursor key menu item
-		menuItem = createCheckBocMenuItem( "Show Cursor", ' ', KeyEvent.VK_C, Event.ALT_MASK, MOUSE_CLIENTCURSOR_CONTROL,  "Show mouse cursor");
-		menu.add(menuItem);
-		
-		menuItem = createCheckBocMenuItem( "Mouse Calibration", ' ', KeyEvent.VK_T, Event.ALT_MASK, CALIBRATEMOUSETHRESHOLD, "Caliberating the Mouse");
+		menuItem = createCheckBoxMenuItem( "Show Cursor", ' ', KeyEvent.VK_C, Event.ALT_MASK, MOUSE_CLIENTCURSOR_CONTROL,  "Show mouse cursor");
+		menu.add(menuItem);
+		
+		menuItem = createCheckBoxMenuItem( "Mouse Calibration", ' ', KeyEvent.VK_T, Event.ALT_MASK, CALIBRATEMOUSETHRESHOLD, "Caliberating the Mouse");
 		menu.add(menuItem);
 		
 		//Construct mouse mode menu to change the mouse mode from the JViewer 
@@ -1345,6 +1376,70 @@
 			menu.add(new JMenuItem(userData[i],menuIcon));
 		}
     }
+
+	/**
+	 * Enable/Disable the mnemonics associated with menus and menu items.
+	 * 
+	 * @param status
+	 *            - enable mnemonics if true, disable mnemonics if false.
+	 */
+	protected void enableMenuMnemonics(boolean status) {
+		Set menuSet = menuMnemonics.entrySet();
+		Iterator itr = menuSet.iterator();
+		Map.Entry menuMnemonicEntry;
+		String menuString = null;
+		Character mnemonic;
+
+		while (itr.hasNext()) {
+			menuMnemonicEntry = (Map.Entry) itr.next();
+			menuString = (String) menuMnemonicEntry.getKey();
+			mnemonic = (Character) menuMnemonicEntry.getValue();
+			if (status) {
+				if (getMenu(menuString) != null) {
+					getMenu(menuString).setMnemonic('\0');
+				} else if (getMenuItem(menuString) != null) {
+					getMenuItem(menuString).setMnemonic('\0');
+				}
+			} else {
+				if (getMenu(menuString) != null) {
+					getMenu(menuString).setMnemonic(mnemonic.charValue());
+				} else if (getMenuItem(menuString) != null) {
+					getMenuItem(menuString).setMnemonic(mnemonic.charValue());
+				}
+			}
+		}
+	}
+
+	/**
+	 * Enable/Disable the accelerators associated with menus and menu items.
+	 * 
+	 * @param status
+	 *            - enable menu accelerators if true, disable menu accelerators
+	 *            if false.
+	 */
+	protected void enableMenuAccelerator(boolean status) {
+		Set menuSet = menuAccelerator.entrySet();
+		Iterator itr = menuSet.iterator();
+		Map.Entry menuAccelerartorEntry;
+		String menuItemString;
+		KeyStroke accelerator;
+
+		while (itr.hasNext()) {
+			menuAccelerartorEntry = (Map.Entry) itr.next();
+			menuItemString = (String) menuAccelerartorEntry.getKey();
+			accelerator = (KeyStroke) menuAccelerartorEntry.getValue();
+			if (status) {
+				if (getMenuItem(menuItemString) != null) {
+					getMenuItem(menuItemString).setAccelerator(null);
+				}
+			} else {
+				if (getMenuItem(menuItemString) != null) {
+					getMenuItem(menuItemString).setAccelerator(accelerator);
+				}
+			}
+		}
+	}
+	
     protected JLabel constructZoomLabelText(){
         label_Text = new JLabel("Zoom Size : 100%");
         return label_Text;
