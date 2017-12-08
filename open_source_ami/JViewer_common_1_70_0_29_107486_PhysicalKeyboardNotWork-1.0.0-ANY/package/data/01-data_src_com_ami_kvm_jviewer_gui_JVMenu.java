--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java Tue Dec  4 15:51:21 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java Fri Dec 14 17:44:43 2012
@@ -523,10 +523,11 @@
 				JCheckBoxMenuItem evtSrc = (JCheckBoxMenuItem) e.getSource();
 
 				if(evtSrc.getState()) {
+					notifyMenuStateSelected(AUTOMAIC_LANGUAGE, true);
 					keyBoardLayout *= -1;
 					JViewerApp.getInstance().setAutokeylayout(new AutoKeyboardLayout());
 					getMenu(JVMenu.SOFTKEYBOARD).setEnabled(false);
-					notifyMenuStateSelected(AUTOMAIC_LANGUAGE, true);
+
 					SetMenuEnable(SOFTKEYBOARD, false);
 					m_menuItems_setenabled.put(SOFTKEYBOARD, false);
 					RCApp.OnSkbrdDisplay(-1);
@@ -543,18 +544,33 @@
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_ENGLISH_US)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_ENGLISH_US);
+				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
+				SetMenuEnable(SOFTKEYBOARD, true);
+				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_FRENCH)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_FRENCH);
+				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
+				SetMenuEnable(SOFTKEYBOARD, true);
+				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_GERMAN_GER)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_GERMAN_GER);
+				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
+				SetMenuEnable(SOFTKEYBOARD, true);
+				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_JAPANESE)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_JAPANESE);
+				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
+				SetMenuEnable(SOFTKEYBOARD, true);
+				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_SPANISH)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_SPANISH);
+				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
+				SetMenuEnable(SOFTKEYBOARD, true);
+				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if (cmdStr.equals(SKBRD_LANGUAGE_ENGLISH_US)) {
 				RCApp.OnSkbrdDisplay(LANGUAGE_ENGLISH_US);
