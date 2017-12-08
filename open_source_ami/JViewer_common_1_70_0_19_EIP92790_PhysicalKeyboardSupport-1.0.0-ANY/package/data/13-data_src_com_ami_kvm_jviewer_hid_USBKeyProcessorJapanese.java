--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorJapanese.java	Tue Jul 10 13:50:39 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorJapanese.java	Wed Jul 11 11:41:36 2012
@@ -23,6 +23,7 @@
 import java.nio.ByteOrder;
 import java.util.HashMap;
 
+import com.ami.kvm.jviewer.Debug;
 import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
 import com.ami.kvm.jviewer.gui.JViewerApp;
 
@@ -265,8 +266,8 @@
         Integer usbKeyCode = new Integer(0);
         byte[] outputKeycodes = new byte[ 6 ];
         boolean sendAutoKeybreak = false;
-        if(JViewerApp.getInstance().getJVMenu().getAutokeylayout() != null) {
-	        switch(JViewerApp.getInstance().getJVMenu().getAutokeylayout().getKeyboardlayout())
+        if(JViewerApp.getInstance().getAutokeylayout() != null) {
+	        switch(JViewerApp.getInstance().getAutokeylayout().getKeyboardType())
 	        {
 
 	           case AutoKeyboardLayout.KBD_TYPE_JAPANESE:
@@ -390,7 +391,7 @@
             USBKeyProcessorEnglish.setModifiers(modifiers);
             return( USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak ) );
         } else {
-            System.err.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+            Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
             return( null );
         }
     }
