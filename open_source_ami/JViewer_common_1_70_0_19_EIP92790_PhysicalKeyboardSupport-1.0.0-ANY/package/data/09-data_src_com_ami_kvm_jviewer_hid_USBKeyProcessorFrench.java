--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrench.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrench.java	Wed Jul 11 11:40:22 2012
@@ -23,8 +23,10 @@
 import java.nio.ByteOrder;
 import java.util.HashMap;
 
+import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
 import com.ami.kvm.jviewer.gui.JViewerApp;
+import com.ami.kvm.jviewer.gui.SoftKeyboard;
 
 public class USBKeyProcessorFrench implements KeyProcessor
 {
@@ -35,8 +37,8 @@
     protected HashMap<Integer, Integer> FrenchMap;
 
 
-    static private byte modifiers = 0;
-    private boolean autoKeybreakModeOn = false;
+    static protected byte modifiers = 0;
+    protected boolean autoKeybreakModeOn = false;
     static final int	USB_KEYBOARD_REPORT_SIZE = 8;
 
     /**
@@ -172,6 +174,7 @@
         //map.put( KeyEvent.VK_???, 102 );
         standardMap.put(153, 100);
         keypadMap.put( KeyEvent.VK_EQUALS, 103 );
+        standardMap.put(SoftKeyboard.VK_102KEY, 100);
 
         standardMap.put( 525, 101 ); //keycode for rightclick event in the keyboard(Apps key)adjacent to right alt
         standardMap.put( KeyEvent.VK_F13, 104 );
@@ -258,8 +261,8 @@
         Integer usbKeyCode = new Integer(0);
         byte[] outputKeycodes = new byte[ 6 ];
         boolean sendAutoKeybreak = false;
-        if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null) {
-	        switch(JViewerApp.getInstance().getJVMenu().getAutokeylayout().getKeyboardlayout()) {
+        if(JViewerApp.getInstance().getAutokeylayout() != null) {
+	        switch(JViewerApp.getInstance().getAutokeylayout().getKeyboardType()) {
 	        case AutoKeyboardLayout.KBD_TYPE_FRENCH:
 	        	try{
 	        		if(keyCode != KeyEvent.VK_LESS)
@@ -383,7 +386,7 @@
             USBKeyProcessorEnglish.setModifiers(modifiers);
             return( USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak ) );
         } else {
-            System.err.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+            Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
             return( null );
         }
     }
