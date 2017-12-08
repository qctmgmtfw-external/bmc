--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGermanHost.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGermanHost.java Fri Dec 14 17:10:51 2012
@@ -54,7 +54,10 @@
 			break;
 
 		case KeyEvent.KEY_LOCATION_NUMPAD:
-			usbKeyCode = keypadMap.get( keyCode );
+			if(keyCode == KeyEvent.VK_PERIOD)
+				usbKeyCode = standardMap.get(keyCode);
+			else
+				usbKeyCode = keypadMap.get( keyCode );
 			break;
 
 		case KeyEvent.KEY_LOCATION_RIGHT:
@@ -461,6 +464,7 @@
 			}
 			break;
 		case KeyEvent.VK_PERIOD:
+		case KeyEvent.VK_DECIMAL:
 			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
 					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
 				if(keyPress){
@@ -790,15 +794,17 @@
 			convertedKeyCode = KeyEvent.VK_Y;
 			break;
 		case KeyEvent.VK_BACK_QUOTE:
-			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT && !altGrModified)
 				convertedKeyCode = KeyEvent.VK_UNDEFINED;
 			else{
 				convertedKeyCode = KeyEvent.VK_2;
 				if(keyPress){
 					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = true;
 				}
 				else{
 					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = false;
 					autoKeybreakModeOn = false;
 				}
 			}
@@ -1061,10 +1067,10 @@
 				}
 				else{
 					if(shiftModified){
-						if(shiftHeldDown){
-							enableShift(true);
-						}
-						shiftModified = false;
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
 					}
 					else
 						enableShift(false);
@@ -1525,6 +1531,10 @@
 			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
 				convertedKeyCode = KeyEvent.VK_UNDEFINED;
 			}
+			break;
+		case KeyEvent.VK_DECIMAL:
+			convertedKeyCode = KeyEvent.VK_PERIOD;
+			break;
 		}
 		return convertedKeyCode;
 	}
@@ -1896,6 +1906,29 @@
 				convertedKeyCode = KeyEvent.VK_7;
 			}
 			break;
+		case KeyEvent.VK_COMMA:
+		case KeyEvent.VK_PERIOD:
+			if(keyPress){
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT)
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
+				}
+			}
+			else{
+				if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
 		case KeyEvent.VK_MINUS:
 			convertedKeyCode = KeyEvent.VK_SLASH;
 			break;
@@ -1903,6 +1936,10 @@
 			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
 				convertedKeyCode = KeyEvent.VK_UNDEFINED;
 			}
+			break;
+		case KeyEvent.VK_DECIMAL:
+			convertedKeyCode = KeyEvent.VK_PERIOD;
+			break;
 		}
 		return convertedKeyCode;
 	}
