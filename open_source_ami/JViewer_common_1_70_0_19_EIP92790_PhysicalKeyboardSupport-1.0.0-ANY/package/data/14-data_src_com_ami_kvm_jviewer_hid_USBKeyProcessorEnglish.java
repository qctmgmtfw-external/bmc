--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorEnglish.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorEnglish.java	Wed Jul 11 11:39:48 2012
@@ -23,6 +23,7 @@
 import java.nio.ByteOrder;
 import java.util.HashMap;
 
+import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.gui.SoftKeyboard;
 
 public class USBKeyProcessorEnglish implements KeyProcessor
@@ -33,8 +34,8 @@
     protected HashMap<Integer, Integer> rightMap;
     //The modifier member should be initialized only at the beginning, 
     //and this member should be common for all the instances of this class. 
-    private static byte modifiers = 0;
-    private boolean autoKeybreakModeOn = false;
+    protected static byte modifiers = 0;
+    protected boolean autoKeybreakModeOn = false;
     final int	USB_KEYBOARD_REPORT_SIZE = 8;
 
     /**
@@ -169,6 +170,7 @@
         //map.put( KeyEvent.VK_???, 100 );
         //map.put( KeyEvent.VK_???, 101 );
         //map.put( KeyEvent.VK_???, 102 );
+        standardMap.put(KeyEvent.VK_LESS, 100);
         keypadMap.put( KeyEvent.VK_EQUALS, 103 );
 
         standardMap.put( 525, 101 ); //keycode for rightclick event in the keyboard(Apps key)adjacent to right alt
@@ -353,7 +355,7 @@
 
             return( USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak ) );
         } else {
-            System.err.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+            Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
             return( null );
         }
     }
