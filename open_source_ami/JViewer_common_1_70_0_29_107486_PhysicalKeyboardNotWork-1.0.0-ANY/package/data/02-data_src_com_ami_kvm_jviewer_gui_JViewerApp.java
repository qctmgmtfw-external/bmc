--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Tue Dec  4 15:51:21 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java Fri Dec 14 17:07:13 2012
@@ -2400,11 +2400,9 @@
 		if(autokeylayout == null)
 			autokeylayout = new AutoKeyboardLayout();
 
-		if(autokeylayout.getKeyboardType() != keyBoardType){
-			getJVMenu().notifyMenuStateSelected(JVMenu.AUTOMAIC_LANGUAGE, false);
-			autokeylayout.setHostKeyboardType(keyBoardType);
-			getM_USBKeyRep().setM_USBKeyProcessor(getKeyProcesssor());
-		}
+		getJVMenu().notifyMenuStateSelected(JVMenu.AUTOMAIC_LANGUAGE, false);
+		autokeylayout.setHostKeyboardType(keyBoardType);
+		getM_USBKeyRep().setM_USBKeyProcessor(getKeyProcesssor());
 	}
 
 	public USBKeyboardRep getM_USBKeyRep() {
