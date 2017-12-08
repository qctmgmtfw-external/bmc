--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java	Wed Jul 11 11:32:56 2012
@@ -74,6 +74,14 @@
 
 	public static final String AUTOMAIC_LANGUAGE="Auto detect";
 	public static final String KEYBOARD_LAYOUT = "Keyboard Layout";
+	
+	public static final String PHYSICAL_KEYBOARD = "PhysicalKeyboard";
+	public static final String PKBRD_LANGUAGE_ENGLISH_US="EN";
+	public static final String PKBRD_LANGUAGE_FRENCH="FR";
+	public static final String PKBRD_LANGUAGE_GERMAN_GER="DE";
+	public static final String PKBRD_LANGUAGE_JAPANESE="JP";
+	public static final String PKBRD_LANGUAGE_SPANISH="ES";
+	
 	public static final String SOFTKEYBOARD = "SoftKeyboard";
 
 	public static final String SKBRD_LANGUAGE_ENGLISH_US="English(United States)";
@@ -169,7 +177,6 @@
 
     // menu states
     public static int m_mouseMode = USBMouseRep.ABSOLUTE_MOUSE_MODE;
-    public AutoKeyboardLayout  autokeylayout;
     protected static JComboBox combo;
     public JSlider slider;
     public JLabel label_size;
@@ -505,8 +512,8 @@
 				JCheckBoxMenuItem evtSrc = (JCheckBoxMenuItem) e.getSource();
 
 				if(evtSrc.getState()) {
-					keyBoardLayout *= -1;					
-					autokeylayout = new AutoKeyboardLayout();
+					keyBoardLayout *= -1;
+					JViewerApp.getInstance().setAutokeylayout(new AutoKeyboardLayout());
 					getMenu(JVMenu.SOFTKEYBOARD).setEnabled(false);
 					notifyMenuStateSelected(AUTOMAIC_LANGUAGE, true);
 					SetMenuEnable(SOFTKEYBOARD, false);
@@ -517,12 +524,26 @@
 					if(keyBoardLayout != -1)
 						keyBoardLayout *= -1;
 					getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-					autokeylayout= null;
 					notifyMenuStateSelected(AUTOMAIC_LANGUAGE, false);
 					SetMenuEnable(SOFTKEYBOARD, true);
 					m_menuItems_setenabled.put(SOFTKEYBOARD, true);
-					JViewerApp.getInstance().getM_USBKeyRep().setM_USBKeyProcessor(JViewerApp.getInstance().getKeyprocesssor());
-				}
+				}				
+				JViewerApp.getInstance().getM_USBKeyRep().setM_USBKeyProcessor(JViewerApp.getInstance().getKeyProcesssor());
+			}
+			else if(cmdStr.equals(PKBRD_LANGUAGE_ENGLISH_US)){
+				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_ENGLISH_US);
+			}
+			else if(cmdStr.equals(PKBRD_LANGUAGE_FRENCH)){
+				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_FRENCH);
+			}
+			else if(cmdStr.equals(PKBRD_LANGUAGE_GERMAN_GER)){
+				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_GERMAN_GER);
+			}
+			else if(cmdStr.equals(PKBRD_LANGUAGE_JAPANESE)){
+				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_JAPANESE);
+			}
+			else if(cmdStr.equals(PKBRD_LANGUAGE_SPANISH)){
+				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_SPANISH);
 			}
 			else if (cmdStr.equals(SKBRD_LANGUAGE_ENGLISH_US)) {
 				RCApp.OnSkbrdDisplay(LANGUAGE_ENGLISH_US);
@@ -1055,7 +1076,7 @@
 		return menu;
 	}
 
-	protected JMenu constructSoftKeyboardMenu()
+	protected JMenu constructKeyboardLayoutMenu()
 	{
 		JMenu menu,subMenu;
 		JMenuItem menuItem;
@@ -1067,9 +1088,47 @@
 		m_menu.put(KEYBOARD_LAYOUT,menu);
 		m_menustatusbar_text.put(KEYBOARD_LAYOUT,"Select required keyboard layouts");
 
+		// construct physical keyboard submenu
+		subMenu = new JMenu("Physical Keyboard");
+		subMenu.addActionListener(m_menuListener);
+		subMenu.addMouseListener(m_menuStatus);
+		m_menu.put(PHYSICAL_KEYBOARD,subMenu);
+		m_menustatusbar_text.put(PHYSICAL_KEYBOARD, "Physical Keyboard");
+		group = new ButtonGroup();
+		if(INITIAL_MENU_STATUS)
+		{
+			m_menuItems_setenabled.put(PHYSICAL_KEYBOARD, true);
+		}
+		//construct auto keyboard layout menu item
 		menuItem = createCheckBoxMenuItem("Auto Detect", ' ', AUTOMAIC_LANGUAGE, "Auto detect the client system keyboard ");
-		menu.add(menuItem);
-		
+		subMenu.add(menuItem);
+		
+		// construct English - US menu item
+		menuItem = createRadioButtonMenu("English(United States)", PKBRD_LANGUAGE_ENGLISH_US, "Invoke the English(US) physical keyboard layout");
+		group.add(menuItem);
+		subMenu.add(menuItem);
+		
+		// construct French menu item
+		menuItem = createRadioButtonMenu("French", PKBRD_LANGUAGE_FRENCH, "Invoke the French physical keyboard layout");
+		group.add(menuItem);
+		subMenu.add(menuItem);
+		
+		// construct German(Germany) menu item
+		menuItem = createRadioButtonMenu("German(Germany)", PKBRD_LANGUAGE_GERMAN_GER, "Invoke the German(Germany) physical keyboard layout");
+		group.add(menuItem);
+		subMenu.add(menuItem);
+		
+		// construct Japanese menu item
+		/*menuItem = createRadioButtonMenu(LocaleStrings.getString("F_93_JVM"), PKBRD_LANGUAGE_JAPANESE, LocaleStrings.getString("F_75_JVM")+LocaleStrings.getString("F_93_JVM")+LocaleStrings.getString("F_116_JVM"));
+		group.add(menuItem);
+		subMenu.add(menuItem);*/
+		
+		// construct Spanish menu item
+		menuItem = createRadioButtonMenu("Spanish", PKBRD_LANGUAGE_SPANISH, "Invoke the Spanish physical keyboard layout");
+		group.add(menuItem);
+		subMenu.add(menuItem);
+		
+		menu.add(subMenu);
 		// construct soft keyboard submenu
 		subMenu = new JMenu("SoftKeyboard");
 		subMenu.addActionListener(m_menuListener);
@@ -1312,14 +1371,6 @@
 		return 0;
 	}
 
-	public AutoKeyboardLayout getAutokeylayout() {
-		return autokeylayout;
-	}
-
-	public void setAutokeylayout(AutoKeyboardLayout autokeylayout) {
-		this.autokeylayout = autokeylayout;
-	}
-
 	public Hashtable<String, JMenu> getM_menu() {
 		return m_menu;
 	}
