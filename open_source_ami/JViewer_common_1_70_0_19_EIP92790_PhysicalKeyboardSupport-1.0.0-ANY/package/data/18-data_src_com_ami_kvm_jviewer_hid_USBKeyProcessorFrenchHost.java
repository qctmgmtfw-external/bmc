--- .pristine/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrenchHost.java	Thu Jan  1 08:00:00 1970
+++ source/JViewer_common-1.70.0-src/data/src/com/ami/kvm/jviewer/hid/USBKeyProcessorFrenchHost.java	Tue Jul 10 10:55:03 2012
@@ -0,0 +1,2192 @@
+package com.ami.kvm.jviewer.hid;
+
+import java.awt.event.KeyEvent;
+
+import com.ami.kvm.jviewer.Debug;
+import com.ami.kvm.jviewer.gui.AutoKeyboardLayout;
+import com.ami.kvm.jviewer.gui.JViewerApp;
+//import com.ami.kvm.jviewer.gui.LocaleStrings;
+
+public class USBKeyProcessorFrenchHost extends USBKeyProcessorFrench{
+
+	private AutoKeyboardLayout autoKeyboradLayout;
+	boolean shiftModified = false;
+	boolean shiftHeldDown = false;
+	boolean autoKeyBreakMode = false;
+	byte[] usbKeyRepPkt = null;
+	
+	public USBKeyProcessorFrenchHost(){
+		super();
+	}
+	
+	public byte[] convertKeyCode( int keyCode, int keyLocation, boolean keyPress )
+	{		
+		Integer usbKeyCode = new Integer(0);
+		byte[] outputKeycodes = new byte[ 6 ];
+		boolean sendAutoKeybreak = false;
+		autoKeyBreakMode = autoKeybreakModeOn;
+		
+		autoKeyboradLayout = JViewerApp.getInstance().getAutokeylayout();
+		if(autoKeyboradLayout != null) {
+			switch(autoKeyboradLayout.getKeyboardType()) {
+			case AutoKeyboardLayout.KBD_TYPE_ENGLISH : 
+				keyCode = convertEnglishToFrench(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_SPANISH : 
+				keyCode = convertSpanishToFrench(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_GERMAN : 
+				keyCode = convertGermanToFrench(keyCode, keyPress);
+				break;
+			case AutoKeyboardLayout.KBD_TYPE_FRENCH:
+				try{
+					keyCode = FrenchMap.get(keyCode);
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
+			
+			usbKeyCode = standardMap.get( keyCode );
+			
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
+					
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
+					
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
+
+			Debug.out.println("Unknown keycode for key: " + KeyEvent.getKeyText( keyCode ) );
+			autoKeybreakModeOn = autoKeyBreakMode;
+			return( null );
+		}
+	}
+	/**
+	 * Converts the key codes send from a client with English keyboard layout to the corresponding key code in 
+	 * French keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertEnglishToFrench(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+
+		switch(keyCode){
+		//first line
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_2;
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
+				convertedKeyCode = KeyEvent.VK_7;
+				if(keyPress){
+					modifiers |= MOD_RIGHT_ALT;
+				}
+				else{
+					modifiers &= ~MOD_RIGHT_ALT;
+					autoKeybreakModeOn = false;
+				}
+			}
+
+			break;
+		case KeyEvent.VK_1:	
+
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				convertedKeyCode = KeyEvent.VK_SLASH;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+
+		case KeyEvent.VK_2:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_0;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					if((modifiers& MOD_RIGHT_ALT) == MOD_RIGHT_ALT)
+						modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_3;
+				if(keyPress){
+					enableShift(false);
+					modifiers |= MOD_RIGHT_ALT;
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						enableShift(false);
+					}
+					if((modifiers& MOD_RIGHT_ALT) == MOD_RIGHT_ALT)
+						modifiers &= ~MOD_RIGHT_ALT;
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+
+		case KeyEvent.VK_4:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_5:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_QUOTE;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+						autoKeybreakModeOn = false;
+					}
+				}
+				else
+				{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_QUOTE;
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
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+						shiftModified = true;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_1;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_8:
+
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				convertedKeyCode = KeyEvent.VK_5;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_MINUS;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+
+		case KeyEvent.VK_MINUS:
+
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_8;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_8;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_6;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_6;
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+
+			break;
+		case KeyEvent.VK_EQUALS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress){
+						enableShift(false);
+						shiftModified = true;
+					}
+					else{
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						else{
+							enableShift(false);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK ){
+					if(keyPress)
+						enableShift(true);
+				}
+			}
+			break;
+			//line 2
+		case KeyEvent.VK_Q:
+			convertedKeyCode = KeyEvent.VK_A;
+			break;
+		case KeyEvent.VK_W:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_4;
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
+				convertedKeyCode = KeyEvent.VK_5;
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
+				convertedKeyCode = KeyEvent.VK_EQUALS;
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
+		case KeyEvent.VK_BACK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_6;
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
+			//line 3
+		case KeyEvent.VK_A:
+			convertedKeyCode = KeyEvent.VK_Q;
+			break;
+		case KeyEvent.VK_SEMICOLON:	
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_PERIOD;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
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
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_3;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_3;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_4;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_4;
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+			//line 4
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_W;
+			break;
+		case KeyEvent.VK_M:
+			convertedKeyCode = KeyEvent.VK_SEMICOLON;
+			break;
+		case KeyEvent.VK_COMMA:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT  || shiftModified){
+				convertedKeyCode = KeyEvent.VK_LESS;
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
+						convertedKeyCode = KeyEvent.VK_M;
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
+				convertedKeyCode = KeyEvent.VK_M;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_PERIOD:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_LESS;
+				}
+				else {
+					convertedKeyCode = KeyEvent.VK_COMMA;
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
+				convertedKeyCode = KeyEvent.VK_COMMA;
+				if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_M;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+						shiftModified = true;
+						autoKeybreakModeOn = false;
+					}
+				}
+				else
+				{
+					if(shiftModified){
+						convertedKeyCode = KeyEvent.VK_M;
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
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_PERIOD;
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(true);
+						shiftModified = true;
+					}
+				}
+			}
+			break;
+		}
+		return convertedKeyCode;
+	}
+	/**
+	 * Converts the key codes send from a client with Spanish keyboard layout to the corresponding key code in 
+	 * French keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertSpanishToFrench(int keyCode, boolean keyPress){
+
+		int convertedKeyCode = keyCode;
+
+		switch(keyCode){
+		//first line
+		case KeyEvent.VK_BACK_QUOTE:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_UNDEFINED;
+				if(keyPress){
+					enableShift(false);
+					shiftModified = true;
+				}
+				else{
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+						convertedKeyCode = KeyEvent.VK_MINUS;
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
+				if( keyPress ){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					else
+					enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_1:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_SLASH;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+
+		case KeyEvent.VK_2: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+
+				convertedKeyCode = KeyEvent.VK_3;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_3: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_M;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_4:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){	
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_5: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_QUOTE;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_6: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_1;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}				
+			}
+			break;
+		case KeyEvent.VK_7: 			
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){	
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_PERIOD;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_5;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_0: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_EQUALS;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_QUOTE: 
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_M;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+					shiftModified = true;
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
+				convertedKeyCode = KeyEvent.VK_4;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+				}
+			}
+			break;
+			//line 2
+		case KeyEvent.VK_Q:
+			convertedKeyCode = KeyEvent.VK_A;
+			break;
+		case KeyEvent.VK_W:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_DEAD_GRAVE: //ALTGR
+			break;
+		case KeyEvent.VK_PLUS:   
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_EQUALS;
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_EQUALS;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_BACK_SLASH:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;		
+
+			//line 3	
+		case KeyEvent.VK_A:
+			convertedKeyCode = KeyEvent.VK_Q;
+			break;		
+
+		case KeyEvent.VK_SEMICOLON:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_DEAD_ACUTE:
+			break;	
+			//LINE 4
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_W;
+			break;
+		case KeyEvent.VK_M:
+			convertedKeyCode =KeyEvent.VK_SEMICOLON;
+			break;
+		case KeyEvent.VK_COMMA:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_COMMA;
+
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_M;
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_M;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_PERIOD :
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
+				}
+				else{
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_COMMA;
+						enableShift(false);
+					}
+					shiftModified = false;
+					autoKeybreakModeOn = false;
+				}
+			}
+			else{
+				convertedKeyCode = KeyEvent.VK_COMMA;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(true);
+					else
+						enableShift(false);
+				}
+			}
+			break;
+		case KeyEvent.VK_MINUS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				convertedKeyCode = KeyEvent.VK_8;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+					shiftModified = true;
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
+				convertedKeyCode = KeyEvent.VK_6;
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK )
+						enableShift(false);
+					else
+						enableShift(true);
+				}
+			}
+			break;
+		default :
+			break;
+		}
+		return convertedKeyCode;
+	}
+	/**
+	 * Converts the key codes send from a client with German keyboard layout to the corresponding key code in 
+	 * French keyboard layout. 
+	 * @param keyCode - the key code to be converted
+	 * @param keyPress - true if key is pressed; false otherwise. 
+	 * @return
+	 */
+	private int convertGermanToFrench(int keyCode, boolean keyPress){
+		int convertedKeyCode = keyCode;
+
+		switch(keyCode){
+		//first line
+		case KeyEvent.VK_DEAD_CIRCUMFLEX:
+
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_MINUS;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_MINUS;
+						else
+							convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+					if(shiftHeldDown){
+						enableShift(true);
+					}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					convertedKeyCode = KeyEvent.VK_OPEN_BRACKET;
+
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_1:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_SLASH;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_SLASH;
+						
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_SLASH;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_SLASH;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_2:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_3;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_3;
+						
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_3;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_3;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_3:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_SLASH;
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_SLASH;
+						
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_SLASH;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_SLASH;
+					}
+					else
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_4:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_CLOSE_BRACKET;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_5:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_QUOTE;
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_QUOTE;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_QUOTE;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_QUOTE;
+					}
+					else
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_6:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_1;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_1;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_1;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_1;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_7:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						enableShift(true);
+					}
+					else
+						enableShift(false);
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_PERIOD;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_PERIOD;
+					}
+					else
+						enableShift(true);
+				}
+			}
+			break;
+		case KeyEvent.VK_8:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_5;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_5;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_5;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_5;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_9:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_MINUS;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_MINUS;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_MINUS;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_MINUS;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_0:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_EQUALS;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_EQUALS;
+						
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_EQUALS;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_EQUALS;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_SLASH:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_M;
+						enableShift(true);
+					}
+					else
+					{	convertedKeyCode = KeyEvent.VK_UNDEFINED;
+						enableShift(false);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_M;
+						
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_M;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_M;
+					}
+					else
+					{
+						convertedKeyCode = KeyEvent.VK_UNDEFINED;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+//line 2
+		case KeyEvent.VK_Q:
+			convertedKeyCode = KeyEvent.VK_A;
+			break;
+		case KeyEvent.VK_W:
+			convertedKeyCode = KeyEvent.VK_Z;
+			break;
+		case KeyEvent.VK_Z:
+			convertedKeyCode = KeyEvent.VK_W;
+			break;
+		case KeyEvent.VK_OPEN_BRACKET:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_PLUS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					}
+					else
+					{	convertedKeyCode = KeyEvent.VK_EQUALS;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+						else
+							convertedKeyCode = KeyEvent.VK_EQUALS;
+						
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
+				if( keyPress ){
+					enableShift(true);
+					
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_BACK_SLASH;
+					}
+					else
+					{
+						convertedKeyCode = KeyEvent.VK_EQUALS;
+						enableShift(true);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_NUMBER_SIGN:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_4;
+					}
+					else
+					{
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers |= MOD_RIGHT_ALT;
+
+					}
+					shiftModified = true;
+				}
+				else{
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_4;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						if((modifiers & MOD_LEFT_ALT) == MOD_LEFT_ALT || 
+								(modifiers & MOD_RIGHT_ALT) == MOD_RIGHT_ALT)
+						{
+							convertedKeyCode = KeyEvent.VK_3;
+							modifiers &= ~MOD_RIGHT_ALT;
+						}
+							
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_4;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}	
+					
+				}
+			}else{
+				if( keyPress ){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+						convertedKeyCode = KeyEvent.VK_4;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers |= MOD_RIGHT_ALT;
+					}
+				}
+				else{
+						convertedKeyCode = KeyEvent.VK_3;
+						modifiers &= ~MOD_RIGHT_ALT;
+						autoKeybreakModeOn = false;
+					}
+			}
+			break;
+//line 3		
+		case KeyEvent.VK_A:
+			convertedKeyCode = KeyEvent.VK_Q;
+			break;
+		case KeyEvent.VK_SEMICOLON:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+		case KeyEvent.VK_QUOTE:
+			convertedKeyCode = KeyEvent.VK_UNDEFINED;
+			break;
+//line 4
+		case KeyEvent.VK_M:
+			convertedKeyCode =KeyEvent.VK_SEMICOLON;
+			break;
+		case KeyEvent.VK_COMMA:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(false);
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					}
+					else
+					{	enableShift(true);
+						convertedKeyCode = KeyEvent.VK_M;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_COMMA;
+						else
+							convertedKeyCode = KeyEvent.VK_M;
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
+				if( keyPress ){
+
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+					else
+					{
+						convertedKeyCode = KeyEvent.VK_M;
+						enableShift(false);
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_PERIOD:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT || 
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					enableShift(false);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+							convertedKeyCode = KeyEvent.VK_COMMA;
+						}
+						
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
+				if( keyPress ){
+					enableShift(true);
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						convertedKeyCode = KeyEvent.VK_COMMA;
+					}
+				}
+			}
+			break;
+		case KeyEvent.VK_MINUS:
+			if((modifiers & MOD_LEFT_SHIFT) == MOD_LEFT_SHIFT ||
+					(modifiers & MOD_RIGHT_SHIFT) == MOD_RIGHT_SHIFT || shiftModified){
+				if(keyPress){
+					convertedKeyCode = KeyEvent.VK_8;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) != JViewerApp.CAPSLOCK){
+						enableShift(false);
+					}
+					else
+					{	enableShift(true);
+					}
+					shiftModified = true;
+				}
+				else {
+					if(shiftModified){
+						if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK)
+							convertedKeyCode = KeyEvent.VK_8;
+						else
+							convertedKeyCode = KeyEvent.VK_6;
+						if(shiftHeldDown){
+							enableShift(true);
+						}
+						shiftModified = false;
+						autoKeybreakModeOn = false;
+					}
+					else{
+						convertedKeyCode = KeyEvent.VK_6;
+						enableShift(false);
+						autoKeybreakModeOn = false;
+					}
+				}
+			}
+			else{
+				if( keyPress ){
+					convertedKeyCode = KeyEvent.VK_6;
+					if((JViewerApp.getInstance().getLed_status() & JViewerApp.CAPSLOCK) == JViewerApp.CAPSLOCK){
+						enableShift(true);
+					}
+					else
+					{
+						enableShift(false);
+					}
+				}
+			}
+			break;
+			
+		default :
+			break;
+		}
+		return convertedKeyCode;
+
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
