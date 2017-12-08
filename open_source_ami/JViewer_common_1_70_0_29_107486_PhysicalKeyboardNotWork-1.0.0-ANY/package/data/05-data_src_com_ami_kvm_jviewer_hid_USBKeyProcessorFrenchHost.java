--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrenchHost.java Tue Dec  4 15:51:20 2012
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrenchHost.java Fri Dec 14 17:09:56 2012
@@ -55,7 +55,10 @@
 			break;
 
 		case KeyEvent.KEY_LOCATION_NUMPAD:
-			usbKeyCode = keypadMap.get( keyCode );
+			if(keyCode == KeyEvent.VK_M)
+				usbKeyCode = standardMap.get(keyCode);
+			else				
+				usbKeyCode = keypadMap.get( keyCode );
 			break;
 
 		case KeyEvent.KEY_LOCATION_RIGHT:
@@ -574,25 +577,25 @@
 		case KeyEvent.VK_EQUALS:
 			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
 					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
-				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
-					if(keyPress){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
 						enableShift(false);
 						shiftModified = true;
 					}
-					else{
-						if(shiftHeldDown){
-							enableShift(true);
-						}
-						else{
-							enableShift(false);
-						}
-						shiftModified = false;
-						autoKeybreakModeOn = false;
-					}
-				}
-			}
-			else{
-				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
 					if(keyPress)
 						enableShift(true);
 				}
@@ -2169,7 +2172,10 @@
 			}
 			break;
 			
-		default :
+		case KeyEvent.VK_DECIMAL:
+			convertedKeyCode = KeyEvent.VK_M;
+			break;
+		default:
 			break;
 		}
 		return convertedKeyCode;
