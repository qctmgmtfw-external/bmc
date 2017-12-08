--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/JViewer.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/JViewer.java	Wed Jul 11 11:22:40 2012
@@ -58,9 +58,10 @@
 	private static boolean VMUseSSL = false;
 	private static String webSessionToken = null;
 	private static String kvmToken= null;
-	private static String lang= null;
+	private static String lang= "EN";// English-US
 	private static int webPort = 0;
 	private static int webSecure = 0;
+	private static String keyboardLayout = "EN";// English-US 
 	private static String videoFile = null;
 
 	/*apptype - first argument
@@ -435,6 +436,18 @@
 					return;
 				}
 			}
+			if(arg.equals("-keyboardlayout"))
+			{
+				try {
+					keyboardLayout = args[i++];
+					continue;
+				}
+				catch (NumberFormatException e) {
+					printUsage();
+					return;
+				} 
+			}
+
 			//videofile
 			if(arg.equals("-videofile"))
 			{
@@ -534,4 +547,17 @@
 	public static boolean isWebPreviewer() {
 		return webPreviewer;
 	}
+	/**
+	 * @return the keyboardLayout
+	 */
+	public static String getKeyboardLayout() {
+	        return keyboardLayout;
+	}
+	
+	/**
+	 * @param keyboardLayout the keyboardLayout to set
+	 */
+	public static void setKeyboardLayout(String keyboardLayout) {
+	        JViewer.keyboardLayout = keyboardLayout;
+	}
 }
