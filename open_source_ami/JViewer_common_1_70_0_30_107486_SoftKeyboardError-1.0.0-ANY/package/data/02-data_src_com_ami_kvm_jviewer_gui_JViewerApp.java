--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Feb  1 13:54:40 2013
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Mon Feb  4 13:49:03 2013
@@ -1258,8 +1258,13 @@
 				softKeyboard =null;
 			}
 		}
-		else
+		else {
+			if(autokeylayout != null) {
+				autokeylayout = null;
+				autokeylayout = new AutoKeyboardLayout();
+			}
 			softKeyboard = new SoftKeyboard(langindex);
+		}
 	}
 
 	/**
@@ -2397,10 +2402,20 @@
 		else if(keyboardLayout.equalsIgnoreCase("es"))
 			keyBoardType = AutoKeyboardLayout.KBD_TYPE_SPANISH;
 
-		if(autokeylayout == null)
+		if(autokeylayout == null) {
 			autokeylayout = new AutoKeyboardLayout();
+		}
 
 		getJVMenu().notifyMenuStateSelected(JVMenu.AUTOMAIC_LANGUAGE, false);
+		//enable Softkeyboard menu		
+		if(getJVMenu().getMenuEnable(JVMenu.SOFTKEYBOARD) == false){			
+			getJVMenu().notifyMenuEnable(JVMenu.SOFTKEYBOARD, true);
+			JVMenu.keyBoardLayout *= -1;
+		}
+		if (softKeyboard != null) {
+			softKeyboard.m_skmouselistener.close();
+			softKeyboard.dispose();
+		}
 		autokeylayout.setHostKeyboardType(keyBoardType);
 		getM_USBKeyRep().setM_USBKeyProcessor(getKeyProcesssor());
 	}
