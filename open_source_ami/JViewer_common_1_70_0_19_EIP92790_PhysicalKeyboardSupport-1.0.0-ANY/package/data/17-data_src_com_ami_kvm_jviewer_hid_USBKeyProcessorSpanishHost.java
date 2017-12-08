--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanishHost.java	Thu Jan  1 08:00:00 1970
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorSpanishHost.java	Tue Jul 10 10:55:28 2012
@@ -0,0 +1,1575 @@
+package com.ami.kvm.jviewer.hid;
+
+import java.awt.event.KeyEvent;
+
+import com.ami.kvm.jviewer.Debug;
+import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
+import com.ami.kvm.jviewer.gui.JViewerApp;
+//import com.ami.kvm.jviewer.gui.LocaleStrings;
+
+public class USBKeyProcessorSpanishHost extends USBKeyProcessorSpanish{
+	private AutoKeyboardLayout autoKeyboradLayout;
+	private boolean shiftModified = false;
+	private boolean shiftHeldDown = false;
+	private boolean altGrModified = false;
+	private boolean autoKeyBreakMode = false;
+	private byte[] usbKeyRepPkt = null;
+	public USBKeyProcessorSpanishHost(){
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
+				keyCode = convertEnglishToSpanish(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_FRENCH:
+				keyCode = convertFrenchToSpanish(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_GERMAN:
+				keyCode = convertGermanToSpanish(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_SPANISH:
+				try{
+					keyCode = SpanishMap.get(keyCode);
+				}
+				catch(Exception e){
+					//e.printStackTrace();
+				}
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
+		if( usbKeyCode != null )
+		{
+			switch( keyCode )
+			{
+			case KeyEvent.VK_CONTROL:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= 0x01;
+					else
+						modifiers &= ~0x01;
+				} else {
+					if( keyPress )
+						modifiers |= 0x10;
+					else
+						modifiers &= ~0x10;
+				}
+				break;
+
+			case KeyEvent.VK_SHIFT:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress ){
+						modifiers |= 0x02;
+						shiftHeldDown = true;
+					}
+					else {
+						modifiers &= ~0x02;
+						modifiers &= ~0x20;
+						shiftHeldDown = false;
+					}
+				} else {
+					if( keyPress ){
+						modifiers |= 0x20;
+						shiftHeldDown = true;
+					}
+					else {
+						modifiers &= ~0x20;
+						modifiers &= ~0x02;
+						shiftHeldDown = false;
+					}
+				}
+				break;
+
+			case KeyEvent.VK_ALT:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= 0x04;
+					else
+						modifiers &= ~0x04;
+				} else {
+					if( keyPress )
+						modifiers |= 0x40;
+					else
+						modifiers &= ~0x40;
+				}
+				break;
+
+			case KeyEvent.VK_WINDOWS:
+				if( keyLocation == KeyEvent.KEY_LOCATION_LEFT ) {
+					if( keyPress )
+						modifiers |= 0x08;
+					else
+						modifiers &= ~0x08;
+				} else {
+					if( keyPress )
+						modifiers |= 0x80;
+					else
+						modifiers &= ~0x80;
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
+					else
+						return( null );
+				}
+				break;
+			}
+			//Set the current modifier key status for the default key processor also.
+
+			USBKeyProcessorEnglish.setModifiers(modifiers);
+			usbKeyRepPkt = USBKeyboardRepPkt( outputKeycodes, modifiers, sendAutoKeybreak );
+			autoKeybreakModeOn = autoKeyBreakMode;
+			return( usbKeyRepPkt);
+		} else {
+			Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+			return( null );
+		}
+	}
+	/**
+	 * Converts the key codes send from a client with English keyboard layout to the corresponding key code in 
+	 * Spanish keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertEnglishToSpanish(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_4;
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_4;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						modifiers &= ~MOD_RIGHT_ALT;
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+			}
+			break;
+		case KeyEvent.VK_2:
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else{
+
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					shiftModified = false;
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+			break;
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_6;
+			}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_8;
+			}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_9;
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
+					enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						shiftModified = false;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_0;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if( keyPress ){
+					convertedKeyCode = KeyEvent.VK_0;
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_QUOTE;
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
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
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
+				convertedKeyCode = KeyEvent.VK_1;
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
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
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
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_COMMA;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_COMMA;
+				if( keyPress ){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_2;
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_MINUS;
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
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_COMMA;
+			}
+			break;
+		case KeyEvent.VK_PERIOD:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = 226;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_PERIOD;
+			}
+			break;
+		case KeyEvent.VK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+				}
+				else
+				{
+					convertedKeyCode = KeyEvent.VK_7;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_7;
+				if(keyPress){
+					enableShift(true);
+				}
+			}
+			break;
+		}
+		return convertedKeyCode;
+	}
+
+	/**
+	 * Converts the key codes send from a client with French keyboard layout to the corresponding key code in 
+	 * Spanish keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertFrenchToSpanish(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_Q:
+		case KeyEvent.VK_M:	
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_BACK_QUOTE:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_1:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_6;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_6;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_2:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_4;
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_2;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified ){
+				convertedKeyCode = KeyEvent.VK_3;
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_2;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_4:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_MINUS;
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
+				convertedKeyCode = KeyEvent.VK_QUOTE;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_MINUS;
+			}
+			break;
+		case KeyEvent.VK_5:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_8;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						shiftModified = false;
+					}
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_8;
+						enableShift(true);
+					}
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
+				convertedKeyCode = KeyEvent.VK_1;
+			}
+			else{
+				if(keyPress)
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_SLASH;
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
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+				if(keyPress){
+					modifiers &= ~MOD_RIGHT_ALT;
+					altGrModified = true;
+				}
+				else{
+					modifiers |= MOD_RIGHT_ALT;
+					altGrModified = false;
+					//enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
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
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_SLASH;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					else{
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_BACK_SLASH;
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
+				convertedKeyCode = KeyEvent.VK_2;
+			}
+			else{
+				if(keyPress)
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			break;
+		case KeyEvent.VK_RIGHT_PARENTHESIS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_9;
+					else{
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_9;
+						shiftModified = false;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_9;
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_EQUALS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_0;
+					}
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						shiftModified = false;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_0;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_0;
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_0;
+						enableShift(true);
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+				}
+			}
+			break;
+		case KeyEvent.VK_DEAD_CIRCUMFLEX:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+						convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+					else 
+						convertedKeyCode = KeyEvent.VK_QUOTE;
+				}
+				else {
+					if(shiftHeldDown){
+						convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+						enableShift(true);
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_QUOTE;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					convertedKeyCode = KeyEvent.VK_QUOTE;
+				else
+					convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+				enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_DOLLAR:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_4;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_4;
+					if(shiftModified)
+						shiftModified = false;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
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
+		case KeyEvent.VK_ASTERISK:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
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
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_5;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_5;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+					convertedKeyCode = KeyEvent.VK_5;
+					enableShift(true);
+				}
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
+					else{
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						shiftModified = false;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					else{
+						convertedKeyCode = KeyEvent.VK_COMMA;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_COLON:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					else
+						convertedKeyCode = KeyEvent.VK_7;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_PERIOD;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+				}
+				autoKeybreakModeOn = false;
+			}
+			else{
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_7;
+					else
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_EXCLAMATION_MARK:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_1;
+					else
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+				else{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
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
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+						convertedKeyCode = KeyEvent.VK_1;
+						enableShift(true);
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+			}
+			break;
+		case KeyEvent.VK_LESS:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			break;
+		}
+		return convertedKeyCode;
+	}
+	/**
+	 * Converts the key codes send from a client with German keyboard layout to the corresponding key code in 
+	 * Spanish keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertGermanToSpanish(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+		switch(keyCode){
+		case KeyEvent.VK_Q:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_2;
+			break;
+		case KeyEvent.VK_E:	
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT )
+				convertedKeyCode = KeyEvent.VK_5;
+			break;
+		case KeyEvent.VK_DEAD_CIRCUMFLEX:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+						enableShift(false);
+						shiftModified = true;
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+					if (shiftModified) {
+						convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+						shiftModified = false;
+					}
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+				enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_1:
+		case KeyEvent.VK_2:
+		case KeyEvent.VK_4:
+		case KeyEvent.VK_5:
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				convertedKeyCode = keyCode;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK) {
+						convertedKeyCode = keyCode;
+						enableShift(false);
+						shiftModified = true;
+					}
+					else
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT || altGrModified )
+				convertedKeyCode = KeyEvent.VK_QUOTE;
+			else{
+				convertedKeyCode = keyCode;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+			}
+			else{
+				convertedKeyCode = keyCode;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+			}
+			else{
+				convertedKeyCode = keyCode;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
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
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+			}
+			else{
+				convertedKeyCode = keyCode;
+				if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK )
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+					else
+						convertedKeyCode = KeyEvent.VK_MINUS;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_BACK_QUOTE;
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK ){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					enableShift(true);
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				}
+			}
+			break;
+		case KeyEvent.VK_DEAD_ACUTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+					enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+					if(shiftModified){
+						shiftModified = false;
+					}
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_QUOTE;
+			}
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_PLUS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					else
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT ){
+				convertedKeyCode  = KeyEvent.VK_4;
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					enableShift(true);
+			}
+			break;
+		case KeyEvent.VK_NUMBER_SIGN:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers |= MOD_RIGHT_ALT;
+						altGrModified = true;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_MINUS;
+					}
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					convertedKeyCode = KeyEvent.VK_3;
+					if(shiftModified)
+						shiftModified = false;
+					if(altGrModified){
+						modifiers &= ~MOD_RIGHT_ALT;
+						altGrModified = false;
+					}
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = false;
+				}
+			}
+			else if(((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT) && !altGrModified) {
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			}
+			else{
+				if (keyPress) {
+					if ((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+						convertedKeyCode = KeyEvent.VK_MINUS;
+					else {
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers |= MOD_RIGHT_ALT;
+						altGrModified = true;
+					}
+				}
+				else{
+					if(altGrModified){
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers &= ~MOD_RIGHT_ALT;
+						altGrModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_SEMICOLON:
+		case KeyEvent.VK_QUOTE:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_COMMA:
+		case KeyEvent.VK_PERIOD:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+					}
+				}
+				else{
+					if(shiftModified)
+						shiftModified = false;
+					if(shiftHeldDown)
+						enableShift(true);
+					else
+						enableShift(false);
+					autoKeybreakModeOn = true;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+					if(keyPress)
+						enableShift(true);
+					
+			}
+			break;
+		case KeyEvent.VK_MINUS:
+			convertedKeyCode = KeyEvent.VK_SLASH;
+			break;
+		case KeyEvent.VK_LESS:
+			if((modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT){
+				convertedKeyCode = KeyEvent.VK_1;
+			}
+			break;
+		}
+		return convertedKeyCode;
+	}
+
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
+
+}
