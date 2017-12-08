--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanishHost.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanishHost.java Fri Dec 14 17:55:58 2012
@@ -52,7 +52,10 @@
 			break;
 
 		case KeyEvent.KEY_LOCATION_NUMPAD:
-			usbKeyCode = keypadMap.get( keyCode );
+			if(keyCode == KeyEvent.VK_COMMA)
+				usbKeyCode = standardMap.get(keyCode);
+			else
+				usbKeyCode = keypadMap.get( keyCode );
 			break;
 
 		case KeyEvent.KEY_LOCATION_RIGHT:
@@ -79,36 +82,36 @@
 			case KeyEvent.VK_CONTROL:
 				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
 					if( keyPress )
-						modifiers |= 0x01;
-					else
-						modifiers &= ~0x01;
+						modifiers |= MOD_LEFT_CTRL;
+					else
+						modifiers &= ~MOD_LEFT_CTRL;
 				} else {
 					if( keyPress )
-						modifiers |= 0x10;
-					else
-						modifiers &= ~0x10;
+						modifiers |= MOD_RIGHT_CTRL;
+					else
+						modifiers &= ~MOD_RIGHT_CTRL;
 				}
 				break;
 
 			case KeyEvent.VK_SHIFT:
 				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
 					if( keyPress ){
-						modifiers |= 0x02;
+						modifiers |= MOD_LEFT_SHIFT;
 						shiftHeldDown = true;
 					}
 					else {
-						modifiers &= ~0x02;
-						modifiers &= ~0x20;
+						modifiers &= ~MOD_LEFT_SHIFT;
+						modifiers &= ~MOD_RIGHT_SHIFT;
 						shiftHeldDown = false;
 					}
 				} else {
 					if( keyPress ){
-						modifiers |= 0x20;
+						modifiers |= MOD_RIGHT_SHIFT;
 						shiftHeldDown = true;
 					}
 					else {
-						modifiers &= ~0x20;
-						modifiers &= ~0x02;
+						modifiers &= ~MOD_RIGHT_SHIFT;
+						modifiers &= ~MOD_LEFT_SHIFT;
 						shiftHeldDown = false;
 					}
 				}
@@ -117,28 +120,28 @@
 			case KeyEvent.VK_ALT:
 				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
 					if( keyPress )
-						modifiers |= 0x04;
-					else
-						modifiers &= ~0x04;
+						modifiers |= MOD_LEFT_ALT;
+					else
+						modifiers &= ~MOD_LEFT_ALT;
 				} else {
 					if( keyPress )
-						modifiers |= 0x40;
-					else
-						modifiers &= ~0x40;
+						modifiers |= MOD_RIGHT_ALT;
+					else
+						modifiers &= ~MOD_RIGHT_ALT;
 				}
 				break;
 
 			case KeyEvent.VK_WINDOWS:
 				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
 					if( keyPress )
-						modifiers |= 0x08;
-					else
-						modifiers &= ~0x08;
+						modifiers |= MOD_LEFT_WIN;
+					else
+						modifiers &= ~MOD_LEFT_WIN;
 				} else {
 					if( keyPress )
-						modifiers |= 0x80;
-					else
-						modifiers &= ~0x80;
+						modifiers |= MOD_RIGHT_WIN;
+					else
+						modifiers &= ~MOD_RIGHT_WIN;
 				}
 				break;
 
@@ -1030,8 +1033,10 @@
 				}
 			}
 			else {
-				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
 					convertedKeyCode = KeyEvent.VK_MINUS;
+					enableShift(true);
+				}
 			}
 			break;
 		case KeyEvent.VK_SEMICOLON:
@@ -1553,6 +1558,9 @@
 				convertedKeyCode = KeyEvent.VK_1;
 			}
 			break;
+		case KeyEvent.VK_DECIMAL:
+			convertedKeyCode = KeyEvent.VK_COMMA;
+			break;
 		}
 		return convertedKeyCode;
 	}
