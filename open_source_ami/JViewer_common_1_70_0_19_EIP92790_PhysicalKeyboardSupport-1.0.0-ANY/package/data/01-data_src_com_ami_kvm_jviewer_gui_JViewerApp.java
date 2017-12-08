--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java	Tue Jul 10 13:50:40 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/gui/JViewerApp.java	Wed Jul 11 11:36:54 2012
@@ -357,6 +357,7 @@
 	public static final byte SERVER_POWER_OFF = 0;
 	private byte powerStatus;	
 
+	private AutoKeyboardLayout  autokeylayout = null;
 
 	private String currentVersion = "1.70.0";	
 	
@@ -1011,6 +1012,7 @@
 			this.getM_wndFrame().getM_status().getRightAlt().setForeground(Color.red);
 		else
 			this.getM_wndFrame().getM_status().getRightAlt().setForeground(Color.gray);
+		this.getM_wndFrame().getM_status().getRightAlt().setSelected(state);
 	}
 
 	/**
@@ -2272,23 +2274,48 @@
 		this.sockvmclient = sockvmclient;
 	}
 
-	public KeyProcessor getKeyprocesssor() {
-		//If keyprocessor is already created, return that keyprocessor. Otherwise create
-		//new keyprocessor.
-		if(keyprocessor != null)
-			return keyprocessor;
-		else{
-			if(getJVMenu().getAutokeylayout() == null) {
-				keyprocessor =  new USBKeyProcessorEnglish();
-			}
-			else {
-				keyprocessor = getJVMenu().getAutokeylayout().ongetKeyprocessor();
-			}
-
-			keyprocessor.setAutoKeybreakMode(true);
-			return keyprocessor;
-		}
+	public AutoKeyboardLayout getAutokeylayout() {
+		return autokeylayout;
+	}
+
+	public void setAutokeylayout(AutoKeyboardLayout autokeylayout) {
+		this.autokeylayout = autokeylayout;
+	}
+
+	public KeyProcessor getKeyProcesssor() {
+		if(autokeylayout == null) {
+			autokeylayout = new AutoKeyboardLayout();
+			keyprocessor =  new USBKeyProcessorEnglish();
+		}
+		else {
+			keyprocessor = autokeylayout.ongetKeyprocessor();
+		}
+		keyprocessor.setAutoKeybreakMode(true);
+		return keyprocessor;
+	}
+	
+	public void setKeyProcessor(String keyboardLayout){
+		int keyBoardType = AutoKeyboardLayout.KBD_TYPE_ENGLISH;
 		
+		if(keyboardLayout.equalsIgnoreCase("en"))
+			keyBoardType = AutoKeyboardLayout.KBD_TYPE_ENGLISH;
+		else if(keyboardLayout.equalsIgnoreCase("fr"))
+			keyBoardType = AutoKeyboardLayout.KBD_TYPE_FRENCH;
+		else if(keyboardLayout.equalsIgnoreCase("de"))
+			keyBoardType = AutoKeyboardLayout.KBD_TYPE_GERMAN;
+		else if(keyboardLayout.equalsIgnoreCase("jp"))
+			keyBoardType = AutoKeyboardLayout.KBD_TYPE_JAPANESE;
+		else if(keyboardLayout.equalsIgnoreCase("es"))
+			keyBoardType = AutoKeyboardLayout.KBD_TYPE_SPANISH;
+
+		if(autokeylayout == null)
+			autokeylayout = new AutoKeyboardLayout();
+
+		if(autokeylayout.getKeyboardType() != keyBoardType){
+			getJVMenu().notifyMenuStateSelected(JVMenu.AUTOMAIC_LANGUAGE, false);
+			autokeylayout.setHostKeyboardType(keyBoardType);
+			getM_USBKeyRep().setM_USBKeyProcessor(getKeyProcesssor());
+		}
 	}
 
 	public USBKeyboardRep getM_USBKeyRep() {
