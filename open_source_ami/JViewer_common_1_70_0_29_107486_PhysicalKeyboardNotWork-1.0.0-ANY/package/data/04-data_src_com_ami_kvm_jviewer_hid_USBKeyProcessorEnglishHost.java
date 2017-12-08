--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorEnglishHost.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorEnglishHost.java Fri Dec 14 17:09:15 2012
@@ -45,6 +45,9 @@
 			break;
 
 		case KeyEvent.KEY_LOCATION_NUMPAD:
+			if(keyCode == KeyEvent.VK_COMMA)
+				usbKeyCode = standardMap.get(keyCode);
+			else
 			usbKeyCode = keypadMap.get( keyCode );
 			break;
 
@@ -72,36 +75,36 @@
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
@@ -110,28 +113,28 @@
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
 
@@ -172,12 +175,6 @@
 		int convertedKeyCode = keyCode;
 
 		switch(keyCode){
-		case KeyEvent.VK_Y:
-			convertedKeyCode = KeyEvent.VK_Z;
-			break;
-		case KeyEvent.VK_Z:
-			convertedKeyCode = KeyEvent.VK_Y;
-			break;
 		case KeyEvent.VK_DEAD_CIRCUMFLEX:
 			convertedKeyCode = KeyEvent.VK_UNDEFINED;
 			break;
@@ -637,6 +634,8 @@
 				enableShift(true);
 			}
 			break;
+		case KeyEvent.VK_DECIMAL:
+			convertedKeyCode = KeyEvent.VK_COMMA;
 		}
 
 		return convertedKeyCode;
@@ -1684,6 +1683,8 @@
 					if(shiftModified){
 						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
 							convertedKeyCode = KeyEvent.VK_SLASH;
+						if(shiftHeldDown)
+							enableShift(true);
 						shiftModified = false;
 						autoKeybreakModeOn = false;
 					}
@@ -1727,6 +1728,8 @@
 					if(shiftModified){
 						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
 							convertedKeyCode = KeyEvent.VK_PERIOD;
+						if(shiftHeldDown)
+							enableShift(true);
 						shiftModified = false;
 						autoKeybreakModeOn = false;
 					}
@@ -1775,6 +1778,8 @@
 						else
 							convertedKeyCode = KeyEvent.VK_SEMICOLON;
 						
+						if(shiftHeldDown)
+							enableShift(true);						
 						shiftModified = false;
 						autoKeybreakModeOn = false;
 					}
@@ -1832,8 +1837,7 @@
 						else
 							convertedKeyCode = KeyEvent.VK_1;
 						if(shiftHeldDown){
-							
-							//enableShift(true);
+							enableShift(true);
 						}
 						shiftModified = false;
 						autoKeybreakModeOn = false;
