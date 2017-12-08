--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGerman.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGerman.java Fri Dec 14 17:10:13 2012
@@ -297,32 +297,32 @@
                 case KeyEvent.VK_CONTROL:
                     if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
                         if( keyPress )
-                            modifiers |= 0x01;
-                        else
-                            modifiers &= ~0x01;
+                            modifiers |= MOD_LEFT_CTRL;
+                        else
+                            modifiers &= ~MOD_LEFT_CTRL;
                     }
                     else {
                         if( keyPress )
-                            modifiers |= 0x10;
-                        else
-                            modifiers &= ~0x10;
+                            modifiers |= MOD_RIGHT_CTRL;
+                        else
+                            modifiers &= ~MOD_RIGHT_CTRL;
                     }
                     break;
 
                 case KeyEvent.VK_SHIFT:
                     if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
                         if( keyPress )
-                            modifiers |= 0x02;
+                            modifiers |= MOD_LEFT_SHIFT;
                         else {
-                            modifiers &= ~0x02;
-                            modifiers &= ~0x20;
+                            modifiers &= ~MOD_LEFT_SHIFT;
+                            modifiers &= ~MOD_RIGHT_SHIFT;
                          }
                     } else {
                         if( keyPress )
-                            modifiers |= 0x20;
+                            modifiers |= MOD_RIGHT_SHIFT;
                         else {
-                            modifiers &= ~0x20;
-                            modifiers &= ~0x02;
+                            modifiers &= ~MOD_RIGHT_SHIFT;
+                            modifiers &= ~MOD_LEFT_SHIFT;
                         }
                     }
                     break;
@@ -330,28 +330,28 @@
                 case KeyEvent.VK_ALT:
                     if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
                         if( keyPress )
-                            modifiers |= 0x04;
-                        else
-                            modifiers &= ~0x04;
+                            modifiers |= MOD_LEFT_ALT;
+                        else
+                            modifiers &= ~MOD_LEFT_ALT;
                     } else {
                         if( keyPress )
-                            modifiers |= 0x40;
-                        else
-                            modifiers &= ~0x40;
+                            modifiers |= MOD_RIGHT_ALT;
+                        else
+                            modifiers &= ~MOD_RIGHT_ALT;
                     }
                     break;
 
                 case KeyEvent.VK_WINDOWS:
                     if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
                         if( keyPress )
-                            modifiers |= 0x08;
-                        else
-                            modifiers &= ~0x08;
+                            modifiers |= MOD_LEFT_WIN;
+                        else
+                            modifiers &= ~MOD_LEFT_WIN;
                     } else {
                         if( keyPress )
-                            modifiers |= 0x80;
-                        else
-                            modifiers &= ~0x80;
+                            modifiers |= MOD_RIGHT_WIN;
+                        else
+                            modifiers &= ~MOD_RIGHT_WIN;
                     }
                     break;
 
