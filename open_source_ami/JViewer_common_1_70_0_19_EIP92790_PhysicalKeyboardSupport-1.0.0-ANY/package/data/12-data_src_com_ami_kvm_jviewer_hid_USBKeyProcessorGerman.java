--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGerman.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGerman.java	Wed Jul 11 11:41:12 2012
@@ -23,8 +23,10 @@
 import java.nio.ByteOrder;
 import java.util.HashMap;
 
+import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
 import com.ami.kvm.jviewer.gui.JViewerApp;
+import com.ami.kvm.jviewer.gui.SoftKeyboard;
 
 public class USBKeyProcessorGerman implements KeyProcessor
 {
@@ -35,9 +37,9 @@
     protected HashMap<Integer, Integer> GermanMap;
 
 
-    static private byte modifiers = 0;
-    private boolean autoKeybreakModeOn = false;
-    static final int	USB_KEYBOARD_REPORT_SIZE = 8;
+    static protected byte modifiers = 0;
+    protected boolean autoKeybreakModeOn = false;
+    protected static final int	USB_KEYBOARD_REPORT_SIZE = 8;
 
     /**
      * Creates a new instance of USBKeyProcessor
@@ -172,7 +174,7 @@
         //map.put( KeyEvent.VK_???, 101 );
         //map.put( KeyEvent.VK_???, 102 );
         
-        standardMap.put(153, 100);
+        standardMap.put(KeyEvent.VK_LESS, 100);
         
         keypadMap.put( KeyEvent.VK_EQUALS, 103 );
 
@@ -207,6 +209,7 @@
 //        map.put( KeyEvent.VK_???, 157 );
 //        map.put( KeyEvent.VK_???, 158 );
         standardMap.put( KeyEvent.VK_SEPARATOR, 159 );
+        standardMap.put(SoftKeyboard.VK_102KEY, 100);
         /* Skip some more... */
 
         leftMap.put( KeyEvent.VK_CONTROL, 224 );
@@ -248,8 +251,8 @@
         Integer usbKeyCode = new Integer(0);
         byte[] outputKeycodes = new byte[ 6 ];
         boolean sendAutoKeybreak = false;
-        if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null) {
-	        switch(JViewerApp.getInstance().getJVMenu().getAutokeylayout().getKeyboardlayout()) {
+        if(JViewerApp.getInstance().getAutokeylayout() != null) {
+	        switch(JViewerApp.getInstance().getAutokeylayout().getKeyboardType()) {
 	           case AutoKeyboardLayout.KBD_TYPE_GERMAN:
 	              	try{
 	              		keyCode = GermanMap.get(keyCode);
@@ -373,7 +376,7 @@
             USBKeyProcessorEnglish.setModifiers(modifiers);
             return( USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak ) );
         } else {
-            System.err.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+            Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
             return( null );
         }
     }
