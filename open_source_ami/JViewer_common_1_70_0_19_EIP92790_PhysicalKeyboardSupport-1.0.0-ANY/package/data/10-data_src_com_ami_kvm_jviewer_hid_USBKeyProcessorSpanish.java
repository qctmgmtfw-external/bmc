--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanish.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanish.java	Wed Jul 11 11:42:06 2012
@@ -23,8 +23,10 @@
 import java.nio.ByteOrder;
 import java.util.HashMap;
 
+import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
 import com.ami.kvm.jviewer.gui.JViewerApp;
+import com.ami.kvm.jviewer.gui.SoftKeyboard;
 
 public class USBKeyProcessorSpanish implements KeyProcessor
 {
@@ -35,8 +37,8 @@
 
     protected HashMap<Integer, Integer> SpanishMap;
 
-    private static byte modifiers = 0;
-    private boolean autoKeybreakModeOn = false;
+    protected static byte modifiers = 0;
+    protected boolean autoKeybreakModeOn = false;
     final int	USB_KEYBOARD_REPORT_SIZE = 8;
 
     /**
@@ -171,7 +173,7 @@
         //map.put( KeyEvent.VK_???, 100 );
         //map.put( KeyEvent.VK_???, 101 );
         //map.put( KeyEvent.VK_???, 102 );
-        standardMap.put(153, 100);
+        standardMap.put(KeyEvent.VK_LESS, 100);
         keypadMap.put( KeyEvent.VK_EQUALS, 103 );
 
         standardMap.put( 525, 101 ); //keycode for rightclick event in the keyboard(Apps key)adjacent to right alt
@@ -187,6 +189,7 @@
         standardMap.put( KeyEvent.VK_F22, 113 );
         standardMap.put( KeyEvent.VK_F23, 114 );
         standardMap.put( KeyEvent.VK_F24, 115 );
+        standardMap.put(SoftKeyboard.VK_102KEY, 100);
 //        map.put( KeyEvent.VK_???, 116 );
         standardMap.put( KeyEvent.VK_HELP, 117 );
 //        map.put( KeyEvent.VK_???, 118 );
@@ -245,9 +248,8 @@
         Integer usbKeyCode = new Integer(0);
         byte[] outputKeycodes = new byte[ 6 ];
         boolean sendAutoKeybreak = false;
-        if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null) {
-	        switch(JViewerApp.getInstance().getJVMenu().getAutokeylayout().getKeyboardlayout())
-	        {
+        if(JViewerApp.getInstance().getAutokeylayout() != null) {
+	        switch(JViewerApp.getInstance().getAutokeylayout().getKeyboardType()){
 
 	           case AutoKeyboardLayout.KBD_TYPE_SPANISH:
 	           	try{
@@ -373,7 +375,7 @@
             USBKeyProcessorEnglish.setModifiers(modifiers);
             return( USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak ) );
         } else {
-            System.err.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+            Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
             return( null );
         }
     }
