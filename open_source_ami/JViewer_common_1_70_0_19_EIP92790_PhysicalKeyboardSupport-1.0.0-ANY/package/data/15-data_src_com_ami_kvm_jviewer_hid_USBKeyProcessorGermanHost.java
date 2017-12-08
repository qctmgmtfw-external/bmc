--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGermanHost.java	Thu Jan  1 08:00:00 1970
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorGermanHost.java	Tue Jul 10 10:55:14 2012
@@ -0,0 +1,1923 @@
+package com.ami.kvm.jviewer.hid;
+
+import java.awt.event.KeyEvent;
+import java.util.HashMap;
+
+import com.ami.kvm.jviewer.Debug;
+import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
+import com.ami.kvm.jviewer.gui.JViewerApp;
+//import com.ami.kvm.jviewer.gui.LocaleStrings;
+
+public class USBKeyProcessorGermanHost extends USBKeyProcessorGerman{
+	private AutoKeyboardLayout autoKeyboradLayout;
+	private boolean shiftModified = false;
+	private boolean shiftHeldDown = false;
+	private boolean altGrModified = false;
+	private boolean autoKeyBreakMode = false;
+	private byte[] usbKeyRepPkt = null;
+	
+	public USBKeyProcessorGermanHost(){
+		super();
+	}
+	public byte[] convertKeyCode( int keyCode, int keyLocation, boolean keyPress )
+	{
+		Integer usbKeyCode = new Integer(0);
+		byte[] outputKeycodes = new byte[ 6 ];
+		boolean sendAutoKeybreak = false;
+		autoKeyBreakMode = autoKeybreakModeOn;
+		autoKeyboradLayout = JViewerApp.getInstance().getAutokeylayout();
+		if(autoKeyboradLayout != null) {
+			switch(autoKeyboradLayout.getKeyboardType()) {
+			case AutoKeyboardLayout.KBD_TYPE_ENGLISH : 
+				keyCode = convertEnglishToGerman(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_FRENCH:
+				keyCode = convertFrenchToGerman(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_GERMAN:
+				try{
+					keyCode = GermanMap.get(keyCode);
+				}
+				catch(Exception e){
+					//e.printStackTrace();
+				}
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_SPANISH:
+				keyCode = convertSpanishToGerman(keyCode, keyPress);
+				break;
+			}
+		}
+		switch( keyLocation )
+		{
+		case KeyEvent.KEY_LOCATION_LEFT:
+			usbKeyCode = leftMap.get( keyCode );
+			break;
+
+		case KeyEvent.KEY_LOCATION_NUMPAD:
+			usbKeyCode = keypadMap.get( keyCode );
+			break;
+
+		case KeyEvent.KEY_LOCATION_RIGHT:
+			usbKeyCode = rightMap.get( keyCode );
+			break;
+
+		case KeyEvent.KEY_LOCATION_STANDARD:
+			usbKeyCode = standardMap.get( keyCode );
+			break;
+
+		case KeyEvent.KEY_LOCATION_UNKNOWN:
+			System.err.println("Key located in unknown position");
+			break;
+
+		default:
+			System.err.println("Unknown key position value");
+			break;
+		}
+
+		if( usbKeyCode != null ) {
+			switch( keyCode )
+			{
+			case KeyEvent.VK_CONTROL:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= MOD_LEFT_CTRL;
+					else
+						modifiers &= ~MOD_LEFT_CTRL;
+				}
+				else {
+					if( keyPress )
+						modifiers |= MOD_RIGHT_CTRL;
+					else
+						modifiers &= ~MOD_RIGHT_CTRL;
+				}
+				break;
+
+			case KeyEvent.VK_SHIFT:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress ){
+						modifiers |= MOD_LEFT_SHIFT;
+						shiftHeldDown = true;
+					}
+					else {
+						shiftHeldDown = false;
+						modifiers &= ~MOD_LEFT_SHIFT;
+						modifiers &= ~MOD_RIGHT_SHIFT;
+					}
+				} else {
+					if( keyPress ){
+						modifiers |= MOD_RIGHT_SHIFT;
+						shiftHeldDown = true;
+					}
+					else {
+						shiftHeldDown = false;
+						modifiers &= ~MOD_RIGHT_SHIFT;
+						modifiers &= ~MOD_LEFT_SHIFT;
+					}
+				}
+				break;
+
+			case KeyEvent.VK_ALT:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= MOD_LEFT_ALT;
+					else
+						modifiers &= ~MOD_LEFT_ALT;
+				} else {
+					if( keyPress )
+						modifiers |= MOD_RIGHT_ALT;
+					else
+						modifiers &= ~MOD_RIGHT_ALT;
+				}
+				break;
+
+			case KeyEvent.VK_WINDOWS:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= MOD_LEFT_WIN;
+					else
+						modifiers &= ~MOD_LEFT_WIN;
+				} else {
+					if( keyPress )
+						modifiers |= MOD_RIGHT_WIN;
+					else
+						modifiers &= ~MOD_RIGHT_WIN;
+				}
+				break;
+
+			default:
+				if( keyPress || (keyCode == KeyEvent.VK_PRINTSCREEN)) {
+					outputKeycodes[ 0 ] = usbKeyCode.byteValue();
+					if( autoKeybreakModeOn )
+						sendAutoKeybreak = true;
+				} else {
+					if( !autoKeybreakModeOn ||
+							( keyCode == KeyEvent.VK_NUM_LOCK ) ||
+							( keyCode == KeyEvent.VK_CAPS_LOCK ) ||
+							( keyCode == KeyEvent.VK_SCROLL_LOCK ) )
+						outputKeycodes[ 0 ] = 0;
+					else{
+						autoKeybreakModeOn = autoKeyBreakMode;
+						return( null );
+					}
+				}
+				break;
+			}
+
+			//Set the current modifier key status for the default key processor also.
+
+			USBKeyProcessorEnglish.setModifiers(modifiers);
+			usbKeyRepPkt = USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak );
+			autoKeybreakModeOn = autoKeyBreakMode;
+			return( usbKeyRepPkt);
+		} else {
+			Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+			autoKeybreakModeOn = autoKeyBreakMode;
+			return( null );
+		}
+	}
+
+	/**
+	 * Converts the key codes send from a client with English keyboard layout to the corresponding key code in 
+	 * German keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertEnglishToGerman(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_Y:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_Y;
+			break;
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						modifiers &= ~MOD_RIGHT_ALT;
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+
+				}
+
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_EQUALS;
+				if( keyPress )
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_MINUS:
+			convertedKeyCode = KeyEvent.VK_SLASH;
+			break;
+		case KeyEvent.VK_EQUALS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_0;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+							autoKeybreakModeOn = false;
+						}
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					convertedKeyCode = KeyEvent.VK_0;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_7;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else {
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_8;
+				if(keyPress){
+					modifiers |= MOD_RIGHT_ALT;
+				}
+				else{
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_CLOSE_BRACKET:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_0;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else {
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_9;
+				if(keyPress){
+					modifiers |= MOD_RIGHT_ALT;
+				}
+				else{
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_BACK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = 226;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else {
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_MINUS;
+				if(keyPress){
+					modifiers |= MOD_RIGHT_ALT;
+				}
+				else{
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+
+		case KeyEvent.VK_SEMICOLON:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_PERIOD;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_COMMA;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_COMMA;
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_2;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_2;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+					if( keyPress )
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_COMMA:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT  || shiftModified){
+				convertedKeyCode = 226;
+				if(keyPress){
+					enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_COMMA;
+
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_COMMA;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_PERIOD:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = 226;
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_PERIOD;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_PERIOD;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+						autoKeybreakModeOn = false;
+					}
+				}
+				else
+				{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_MINUS;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_7;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_7;
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+						shiftModified = true;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_1:
+		case KeyEvent.VK_4:
+		case KeyEvent.VK_5:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_2:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_Q;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						enableShift(false);
+					}
+					modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+				if(keyPress){
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_6;	
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK) {
+
+					if (keyPress) {
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if (shiftHeldDown) {
+							enableShift(true);
+						}
+						else{
+							if(!shiftModified)
+								convertedKeyCode = keyCode;
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK) {
+					if (keyPress) {
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if (shiftHeldDown) {
+							enableShift(true);
+						}
+						else{
+							if(!shiftModified)
+								convertedKeyCode = keyCode;
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_8;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK) {
+
+					if (keyPress) {
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if (shiftHeldDown) {
+							enableShift(true);
+						}
+						else{
+							if(!shiftModified)
+								convertedKeyCode = keyCode;
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_9;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK) {
+
+					if (keyPress) {
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if (shiftHeldDown) {
+							enableShift(true);
+						}
+						else{
+							if(!shiftModified)
+								convertedKeyCode = keyCode;
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+
+		}
+
+		return convertedKeyCode;
+	}
+
+
+	/**
+	 * Converts the key codes send from a client with French keyboard layout to the corresponding key code in 
+	 * German keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertFrenchToGerman(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_Q:
+		case KeyEvent.VK_M:	
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_Y:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_Y;
+			break;
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			else{
+				convertedKeyCode = KeyEvent.VK_2;
+				if(keyPress){
+					modifiers |= MOD_RIGHT_ALT;
+				}
+				else{
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_1:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_6;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_6;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_2:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				else{
+					if(keyPress){
+						enableShift(true);
+					}
+				}
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_2;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified ){
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if(keyPress){
+					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = true;
+				}
+				else{
+					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_2;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_4:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_7;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_5:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_8;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_8;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_8;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_SLASH;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = 226;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_SLASH;
+				else{
+					if(keyPress){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified ){
+				convertedKeyCode = KeyEvent.VK_EQUALS;
+				if(keyPress){
+					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = true;
+					enableShift(true);
+				}
+				else{
+					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = false;
+					enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				else{
+					if(keyPress){
+						enableShift(true);
+					}
+				}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_SLASH;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_MINUS;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_SLASH;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified ){
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+				if(keyPress){
+					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = true;
+				}
+				else{
+					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				else{
+					if(keyPress){
+						enableShift(true);
+					}
+				}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_Q;
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				else{
+					if(keyPress){
+						enableShift(true);
+					}
+				}
+			break;
+		case KeyEvent.VK_MINUS:
+		case KeyEvent.VK_RIGHT_PARENTHESIS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_9;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_9;
+			}
+			else{
+				if(keyPress)
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_9;
+				enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_EQUALS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(false);
+					}
+					else 
+						convertedKeyCode = KeyEvent.VK_0;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_0;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+							autoKeybreakModeOn = false;
+						}
+					}
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_0;
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+						convertedKeyCode = KeyEvent.VK_0;
+					else
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+		case KeyEvent.VK_DEAD_CIRCUMFLEX:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+			}
+			break;
+		case KeyEvent.VK_CLOSE_BRACKET:
+		case KeyEvent.VK_DOLLAR:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_4;
+					else
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_4;
+						enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						convertedKeyCode = KeyEvent.VK_4;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_BACK_SLASH:
+		case KeyEvent.VK_ASTERISK:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_M;
+						modifiers |= MOD_RIGHT_ALT;
+						enableShift(false);
+						shiftModified = true;
+						altGrModified = true;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					if(altGrModified){
+						convertedKeyCode = KeyEvent.VK_M;
+						modifiers &= ~MOD_RIGHT_ALT;
+						altGrModified = false;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_M;
+						modifiers |= MOD_RIGHT_ALT;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(true);
+					}
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_M;
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_5;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					convertedKeyCode = KeyEvent.VK_5;
+			}
+			break;
+		case KeyEvent.VK_COMMA:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_MINUS;
+				else{
+					if(keyPress){
+						convertedKeyCode = keyCode;
+						enableShift(true);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else
+							enableShift(false);
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else {
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_MINUS;
+			}
+			break;
+		case KeyEvent.VK_SEMICOLON:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					else
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else
+							enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					else
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_COLON:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_7;
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_7;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else
+							enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_7;
+					else{
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_EXCLAMATION_MARK:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_1;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_3;
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_3;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_1;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else
+							enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_3;
+					else{
+						convertedKeyCode = KeyEvent.VK_1;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_LESS:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+		}
+		return convertedKeyCode;
+	}
+	/**
+	 * Converts the key codes send from a client with Spanish keyboard layout to the corresponding key code in 
+	 * German keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertSpanishToGerman(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_Y:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_Y;
+			break;
+		case KeyEvent.VK_Q:
+		case KeyEvent.VK_E:
+		case KeyEvent.VK_M:	
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified ){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+				else{
+					if(!shiftModified){
+						if(shiftHeldDown)
+							enableShift(true);
+						else
+							enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_MINUS;
+			}
+			else{
+				convertedKeyCode = keyCode;
+				enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_1:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = 226;
+			}
+			break;
+		case KeyEvent.VK_2:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_Q;
+			}
+			break;
+		case KeyEvent.VK_3:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else {
+				if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+						(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+						shiftModified){
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+			}
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified){
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if(keyPress){
+					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = true;
+				}
+				else{
+					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_4:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+			}
+			break;
+		case KeyEvent.VK_5:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_E;
+			}
+			break;
+		case KeyEvent.VK_6:
+		case KeyEvent.VK_7:
+		case KeyEvent.VK_8:
+		case KeyEvent.VK_9:
+		case KeyEvent.VK_0:
+			if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+				if(keyPress)
+				{
+					if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+							(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+							shiftModified){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						enableShift(true);
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else {
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+					shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_MINUS;
+						if(shiftHeldDown)
+							enableShift(true);
+						shiftModified = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+						if(shiftHeldDown)
+							enableShift(true);
+						else 
+							enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_INVERTED_EXCLAMATION_MARK:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_DEAD_GRAVE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||
+					shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					convertedKeyCode = KeyEvent.VK_EQUALS;
+					if(shiftHeldDown)
+						enableShift(true);
+					else 
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if ((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ) {
+				convertedKeyCode = KeyEvent.VK_8;
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_EQUALS;
+				if(keyPress)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_PLUS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||	shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						shiftModified = false;
+					}
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if(shiftHeldDown)
+						enableShift(true);
+					else 
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			if ((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ) {
+				convertedKeyCode = KeyEvent.VK_9;
+			}
+			break;
+		case KeyEvent.VK_BACK_SLASH:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			if ((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ) {
+				convertedKeyCode = KeyEvent.VK_0;
+			}
+			break;
+		case KeyEvent.VK_SEMICOLON:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_DEAD_ACUTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT ||	shiftModified){
+				if(keyPress)
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				else{
+					convertedKeyCode = KeyEvent.VK_EQUALS;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_EQUALS;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(false);
+			}
+			if ((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ) {
+				convertedKeyCode = KeyEvent.VK_7;
+			}
+			break;
+		case KeyEvent.VK_MINUS:
+			convertedKeyCode = KeyEvent.VK_SLASH;
+			break;
+		case KeyEvent.VK_LESS:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+		}
+		return convertedKeyCode;
+	}
+	/**
+	 * Enable Shift key modifier
+	 * @param state - true if the Shift modifier needs to be enabled; false otherwise.
+	 */
+	private void enableShift(boolean state){
+		if(state){
+			modifiers |= MOD_LEFT_SHIFT;
+			modifiers |= MOD_RIGHT_SHIFT;
+		}
+		else{
+			modifiers &= ~MOD_LEFT_SHIFT;
+			modifiers &= ~MOD_RIGHT_SHIFT;
+		}
+	}
+}
