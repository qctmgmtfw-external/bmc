--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java Fri Feb  1 13:54:40 2013
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JVMenu.java Mon Feb  4 13:44:40 2013
@@ -523,13 +523,18 @@
 				JCheckBoxMenuItem evtSrc = (JCheckBoxMenuItem) e.getSource();
 
 				if(evtSrc.getState()) {
+					keyBoardLayout *= -1;
+					getMenu(JVMenu.SOFTKEYBOARD).setEnabled(false);
 					notifyMenuStateSelected(AUTOMAIC_LANGUAGE, true);
-					keyBoardLayout *= -1;
-					JViewerApp.getInstance().setAutokeylayout(new AutoKeyboardLayout());
-					getMenu(JVMenu.SOFTKEYBOARD).setEnabled(false);
-
 					SetMenuEnable(SOFTKEYBOARD, false);
 					m_menuItems_setenabled.put(SOFTKEYBOARD, false);
+					JViewerApp.getInstance().getAutokeylayout().setKeyboardType(AutoKeyboardLayout.KBD_TYPE_AUTO);
+					if(JViewerApp.getInstance().getAutokeylayout() == null)
+						JViewerApp.getInstance().setAutokeylayout(new AutoKeyboardLayout());
+					else{
+						JViewerApp.getInstance().getAutokeylayout().getKeyboardType();
+						JViewerApp.getInstance().getAutokeylayout().ongetKeyprocessor();
+					}
 					RCApp.OnSkbrdDisplay(-1);
 				}
 				else {
@@ -544,33 +549,18 @@
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_ENGLISH_US)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_ENGLISH_US);
-				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-				SetMenuEnable(SOFTKEYBOARD, true);
-				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_FRENCH)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_FRENCH);
-				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-				SetMenuEnable(SOFTKEYBOARD, true);
-				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_GERMAN_GER)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_GERMAN_GER);
-				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-				SetMenuEnable(SOFTKEYBOARD, true);
-				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_JAPANESE)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_JAPANESE);
-				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-				SetMenuEnable(SOFTKEYBOARD, true);
-				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if(cmdStr.equals(PKBRD_LANGUAGE_SPANISH)){
 				JViewerApp.getInstance().setKeyProcessor(PKBRD_LANGUAGE_SPANISH);
-				getMenu(JVMenu.SOFTKEYBOARD).setEnabled(true);
-				SetMenuEnable(SOFTKEYBOARD, true);
-				m_menuItems_setenabled.put(SOFTKEYBOARD, true);
 			}
 			else if (cmdStr.equals(SKBRD_LANGUAGE_ENGLISH_US)) {
 				RCApp.OnSkbrdDisplay(LANGUAGE_ENGLISH_US);
