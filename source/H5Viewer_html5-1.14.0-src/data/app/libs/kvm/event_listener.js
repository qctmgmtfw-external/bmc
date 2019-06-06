//usbkbdmsg.setAutoKeybreakMode(true);
var isAltKeyPressed = false;
usbkbd = null;
usbkbdmsg = null;
usbengkbd = null;
usbgerkbd = null;
kbddialogticker = null;

DIALOG_TIMER = 1000; // The time interval (in milliseconds) on how often to execute the code for showing modal dialog
		
/* Supported keyboard layout languages
** Note: when adding support for new keyboard layout language,
**       the entry should be present here in order to work. */
KEYBOARD_LAYOUTS = {
/* English US */EN_US: "US",
/* German */DE: "DE",
/* Japanese */JP: "JP"
};

/* Function for handling keyboard layout check and shows pop-up to save configuration or not*/
function keyboardLayoutCheck(kbd_layout) {

	// Check with already set layout, if same then no need to start layout dialog.
	if (window.keyboard_language != kbd_layout) {
		/* This following piece of code is used to invoke the keyboard layout language Dialog */
		$("#kbd_layout_dialog").show(); // For making Keyboard Layout language dialog visible
		// Function to be callled recursivley for countdown
		kbddialogticker = setInterval(function() {
			var secs = parseInt($("#kbd_time_left").html());
			if (secs > 0) {
				// starts the countdown
				$("#kbd_time_left").html(secs - 1);
			}
			else {
				// if timer expires close the dialog
				$("#kbd_layout_dialog").trigger("closeDialog");
			}
		},DIALOG_TIMER);
	}
}

/* Function for handling keyboard layout language menu item click event and update corresponding processor
 ** Note: This single function is sufficient for handling click events for all the keyboard layout language
 **       menu items. In case of adding new language support in future, the 'id' property value of the
 **       menu item should be mentioned inside the switch case of this method in order to work properly 
 */
function updateKeyBoardLang(kbd_layout_code){
	//On Launch "window.keyboard_language" will be null which will be used to set on launch
	if(window.keyboard_language != null)
		keyboardLayoutCheck(kbd_layout_code);

	/* Currently English(US) and German(DE) keyboard layout languages are only supported.
	** if unsupported value is found then English(US) will be selected by default.
	** in that case changes will not be reflected to Remote Session Settings page. */
	switch (kbd_layout_code) {
		case KEYBOARD_LAYOUTS.DE: // German
			$("#kl-select").appendTo($("#kbd_layout_ger"));
			window.keyboard_language = KEYBOARD_LAYOUTS.DE;
			break;
		case KEYBOARD_LAYOUTS.JP: // Japanese is currently unsupported
		case KEYBOARD_LAYOUTS.EN_US: // English US
		default: // Default keyboard layout language will be English US
			$("#kl-select").appendTo($("#kbd_layout_eng"));
			window.keyboard_language = KEYBOARD_LAYOUTS.EN_US;
			break;
	}	

	if(kbd_layout_code == KEYBOARD_LAYOUTS.DE){
		if(usbgerkbd == null)
			usbgerkbd = new USBKeyBoardGerman();
		usbkbd = usbgerkbd;		
	}else{
		if(usbengkbd == null)
			usbengkbd = new USBKeyBoardEnglish();
		usbkbd = usbengkbd;
	}

	if(usbkbdmsg == null)
		usbkbdmsg = new USBKeyBoardMessage();
	usbkbdmsg.setUSBKeyProcessor(usbkbd);
}

var eventListener = function(_video) {

	/* The following line is disabled to avoid triggering click event for keyboard layout language
	 ** upon pressing startKVM button. If enabled it will show the pop-up for keyboard layout language on
	 ** every single time. (When pressing startKVM button)
	 */
	//$("#kbd_layout_eng").click();
	//if usbkbd is null set default EN
	if (usbkbd == null){
		if(usbengkbd == null)
			usbengkbd = new USBKeyBoardEnglish();
		usbkbd = usbengkbd;
	}
	if(usbkbdmsg == null){
		usbkbdmsg = new USBKeyBoardMessage();
		usbkbdmsg.setUSBKeyProcessor(usbkbd);
	}

	var usbmouse = new USBMouseMessage();

	$("#kvm_textbox").css("z-index", "0");

	var listener = this;

	this.send = function(){	
		//sends data if video is not paused 
		if(_video.isVideoPaused!=true){

			if(_video.sock.validated) {
				var rpt = usbkbdmsg.report();
				_video.sock.send(rpt.buffer);
				delete rpt;
				rpt = null;
			}
		}
	}

	this.sendMouseData=function(){

		if(_video.isVideoPaused!=true)	{
			var rpt = usbmouse.report();
			_video.sock.send(rpt.buffer);
			delete rpt;
			rpt = null;
		}

	}

	$("#kbd_layout_eng").click(function(ev){
		ev.preventDefault();
		if(_video.running) {
			updateKeyBoardLang(KEYBOARD_LAYOUTS.EN_US);
		}
	});

	$("#kbd_layout_ger").click(function(ev){
		ev.preventDefault();
		if(_video.running) {
			updateKeyBoardLang(KEYBOARD_LAYOUTS.DE);
		}
	});

	$('#cad').click(function(ev){
		ev.preventDefault();
		if(_video.running) {
			usbkbdmsg.set(17, usbkbd.KEY_LOCATION_LEFT, true);
			listener.send();
			usbkbdmsg.set(18, usbkbd.KEY_LOCATION_LEFT, true);
			listener.send();
			usbkbdmsg.set(46, usbkbd.KEY_LOCATION_STANDARD, true);
			listener.send();


			usbkbdmsg.set(46, usbkbd.KEY_LOCATION_STANDARD, false);
			listener.send();
			usbkbdmsg.set(18, usbkbd.KEY_LOCATION_LEFT, false);
			listener.send();
			usbkbdmsg.set(17, usbkbd.KEY_LOCATION_LEFT, false);
			listener.send();
		}
	});

	/*
	 * This function will send the key press and key release event for a key.
	 * The keyCode argument is the key code of the key to be pressed and relesed.
	 * The keyCode argument is the key location of the key to be pressed and relesed.
	 */
	function keyPressRelease(keyCode, keyLocation){
		usbkbdmsg.set(keyCode, keyLocation, true);// Key Press
		listener.send();

		usbkbdmsg.set(keyCode, keyLocation, false);// Key Release
		listener.send();
	}

	function handleHoldDownKey(id, key, key_loc, mask) {
		if(_video.running && _video.sock.validated) {
			if($(id).children("i").length == 0) {
				usbkbdmsg.set(key, key_loc, true);
				listener.send();
				$(id).append("<i class=\"fa fa-check\"></i>");
				//manipulate id(menu) from l_ctrl,l_alt,r_ctrl and r_alt to 
				//	to id(buttons) l_ctrl_btn,l_alt_btn,r_ctrl_btn and r_alt_btn
				if( _video.kbdLEDBtns.indexOf(id+"_btn") != -1) {
					window._video.sock.updateColor(id+"_btn",COLOR.green,true);//green
				}
				window.keyboard_mask |= mask;
			}
			else {
				usbkbdmsg.set(key, key_loc, false);
				listener.send();
				$(id).children("i").remove()
				//manipulate id(menu) from l_ctrl,l_alt,r_ctrl and r_alt to 
				//	to id(buttons) l_ctrl_btn,l_alt_btn,r_ctrl_btn and r_alt_btn
				if( _video.kbdLEDBtns.indexOf(id+"_btn") != -1){
					window._video.sock.updateColor(id+"_btn",COLOR.white,false);//default
				}
				window.keyboard_mask &= ~mask;
			}
		}
	}

	//buttons

	$("#l_win_hold_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_win_hold", 91, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_WIN);
	});

	$("#r_win_hold_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_win_hold", 91, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_WIN);
	});

	$("#r_ctrl_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_ctrl", 17, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_CTRL);
	});

	$("#r_alt_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_alt", 18, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_ALT);
	});

	$("#l_ctrl_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_ctrl", 17, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_CTRL);
	});

	$("#l_alt_btn").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_alt", 18, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_ALT);
	});

	//menu
	$("#r_ctrl").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_ctrl", 17, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_CTRL);
	});

	$("#r_alt").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_alt", 18, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_ALT);
	});

	$("#l_ctrl").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_ctrl", 17, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_CTRL);
	});

	$("#l_alt").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_alt", 18, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_ALT);
	});

	$("#l_win_hold").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#l_win_hold", 91, usbkbd.KEY_LOCATION_LEFT, usbkbd.MOD_LEFT_WIN);
	});

	$("#r_win_hold").click(function(ev) {
		ev.preventDefault();
		handleHoldDownKey("#r_win_hold", 91, usbkbd.KEY_LOCATION_RIGHT, usbkbd.MOD_RIGHT_WIN);
	});

	$("#l_win_press").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(91, usbkbd.KEY_LOCATION_LEFT);
	});

	$("#r_win_press").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(91, usbkbd.KEY_LOCATION_RIGHT);
	});

	$("#context").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(93, usbkbd.KEY_LOCATION_STANDARD);
	});

	$("#prtscn").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(44, usbkbd.KEY_LOCATION_STANDARD);
	});

	//NUM Lock status button event handler
	$("#num_lock").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(144, usbkbd.KEY_LOCATION_NUMPAD);//NUM LOCK PRESS & REALSE
	});

	//CAPS Lock status button event handler
	$("#caps_lock").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(20, usbkbd.KEY_LOCATION_STANDARD);//CAPS LOCK PRESS & REALSE
	});

	//SCR Lock status button event handler
	$("#scr_lock").click(function(ev) {
		ev.preventDefault();
		keyPressRelease(145, usbkbd.KEY_LOCATION_STANDARD);//SCR LOCK PRESS & REALSE
	});

	_video.textbox.onkeyup = function(e) {
		if(_video.running && _video.sock.validated) {
			var keyCode = listener.keyCodeAssertion(e.keyCode, e),
			keyLocation = (e.keyLocation!==undefined? e.keyLocation: e.location),
			keyPressed = false;
			// set proper key location for num lock key
			if(keyCode == 144) // keycode for num lock
				keyLocation = 3; // keylocation for num lock
			e.stopPropagation();
			e.preventDefault();

			if(keyCode == 18) { // set the flag to false if alt key is released
				isAltKeyPressed = false;
			}
			// Handling AltGr key release in Linux Firefox and Chrome
			if((navigator.userAgent.indexOf('Firefox')!=-1 && e.code == "AltRight") || (navigator.userAgent.match("Linux.*Chrome") && e.keyCode == 225) && usbkbd instanceof USBKeyBoardGerman ) {
				_video.textbox.altgr_down = false;
			}

			if(e.altKey && e.ctrlKey || _video.textbox.altgr_down) {
				//altgr
				return;
			}
			if(navigator.userAgent.indexOf('Firefox')!=-1 && usbkbd instanceof USBKeyBoardEnglish) {
				if(keyCode == 173) // - key
					keyCode = 189;
				else if(keyCode == 61) // = key
					keyCode = 187;
				else if(keyCode == 59) // ; key
					keyCode = 186;
			}
			if(_video.isVideoPaused!=true){
				// In Linux, the dead keys on a German keyboard only register a keydown event every other time and a keyup event every time.
				// The <key>_down_sent flags monitor if the keydown event has already been sent for that key. The keyup event will send the
				// keydown event if the keydown event was not triggered.

				// Handling dead accent and caret keys in Firefox
				if(navigator.userAgent.indexOf('Firefox')!=-1 && usbkbd === usbgerkbd && (e.code == "Equal" && !_video.textbox.accent_down_sent) || (e.code == "Backquote" && !_video.textbox.caret_down_sent)) {
					usbkbdmsg.set(keyCode, keyLocation, true);
					var rpt = usbkbdmsg.report();
					if(_video.sock.wsmode=="binary") {
						_video.sock.send(rpt.buffer);
					} else {
						_video.sock.send(base64ArrayBuffer(rpt.buffer));
					}
					delete rpt;
					rpt = null;
				}
				// Handling dead accent key in Linux Chrome
				else if(navigator.userAgent.match("Linux.*Chrome") && usbkbd === usbgerkbd && keyCode == 187 && !_video.textbox.plus_down_sent) {
					// Because the dead accent and + keys have the same key code and the dead accent key does not always trigger both the keydown and
					// keyup events, it is necessary to have flags about the + key to track when the keyup is for the dead accent key and when it is
					// for the + key.
					keyCode = 221;

					if(!_video.textbox.accent_down_sent) {
						usbkbdmsg.set(keyCode, keyLocation, true);
						var rpt = usbkbdmsg.report();

						if(_video.sock.wsmode=="binary") {
							_video.sock.send(rpt.buffer);
						} else {
							_video.sock.send(base64ArrayBuffer(rpt.buffer));
						}
						delete rpt;
						rpt = null;
					}

				}
				else if(navigator.userAgent.match("Linux.*Chrome") && usbkbd === usbgerkbd && keyCode == 192 && !_video.textbox.o_down_sent) {
					// Because the caret and ö keys have the same key code and the caret key does not always trigger both the keydown and
					// keyup events, it is necessary to have flags about the ö key to track when the keyup is for the caret key and when it is
					// for the ö key.
					keyCode = 226;

					if(!_video.textbox.caret_down_sent) {
						usbkbdmsg.set(keyCode, keyLocation, true);
						var rpt = usbkbdmsg.report();
						if(_video.sock.wsmode=="binary") {
							_video.sock.send(rpt.buffer);
						} else {
							_video.sock.send(base64ArrayBuffer(rpt.buffer));
						}
						delete rpt;
						rpt = null;
					}
				}

				// Resetting flags
				_video.textbox.accent_down_sent = false;
				_video.textbox.caret_down_sent = false;
				_video.textbox.plus_down_sent = false;
				_video.textbox.o_down_sent = false;

				// Sending keyup
				usbkbdmsg.set(keyCode, keyLocation, keyPressed);
				listener.send();

				//TODO:need to set modifiers correctly
				if(!e.altKey && !e.shiftKey && !e.ctrlKey){
					usbkbd.setModifiers(0 | window.keyboard_mask);
				}
			}
		}
	}

	// onkeypress is only called for the dead accent, +/*/~, caret, and � keys on Linux when using Chrome with the German keyboard
	_video.textbox.onkeypress = function(e){
		// Object that converts character codes to correct key codes
		var german = {};
		german[42] = 187; // * character
		german[43] = 187; // + character
		german[126] = 187; // ~ character
		german[96] = 221; // ` accent character
		german[180] = 221; // ´ accent character
		german[94] = 226; // ^ character
		german[176] = 226; // ° character
		german[246] = 192; // ö character
		german[214] = 192; // Ö character

		if(_video.running && _video.sock.validated && german[e.keyCode] && navigator.userAgent.match("Linux.*Chrome") && usbkbd instanceof USBKeyBoardGerman) {
			var keyCode = german[e.keyCode],
			keyLocation = (e.keyLocation!==undefined? e.keyLocation: e.location);

			e.preventDefault();
			e.stopPropagation();

			if(e.keyCode == 180 || e.keyCode == 96) {
				// Setting flag to indicate that the dead accent keydown is being sent
				_video.textbox.accent_down_sent = true;
			}
			else if(e.keyCode == 126) {
				// Handling the ~ key that requires the AltGr press
				usbkbdmsg.set(17, usbkbd.KEY_LOCATION_LEFT, false); //ctrl
				listener.send();
				usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, false); //alt
				listener.send();
				usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, true); //alt
				listener.send();

				usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, true);
				listener.send();
				usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, false);
				listener.send();
				usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, false); // alt
				listener.send();

				return;
			}
			else if(keyCode == 187) {
				// Setting flag to indicate that the plus keydown is being sent
				_video.textbox.plus_down_sent = true;
			}
			else if(e.keyCode == 94) {
				// Setting flag to indicate that the caret keydown is being sent
				_video.textbox.caret_down_sent = true;
			}
			else if(keyCode == 192) {
				// Setting flag to indicate that the ö keydown is being sent
				_video.textbox.o_down_sent = true;
			}

			// Sending keydown
			usbkbdmsg.set(keyCode, keyLocation, true);
			var rpt = usbkbdmsg.report();
			if(_video.sock.wsmode=="binary") {
				_video.sock.send(rpt.buffer);
			} else {
				_video.sock.send(base64ArrayBuffer(rpt.buffer));
			}
			delete rpt;
			rpt = null;
		}
	}

	this.keyCodeAssertion = function(keyCode, e) {
		if(navigator.userAgent.indexOf('Firefox')!=-1 && usbkbd === usbgerkbd) {
			//1st pass neturalize
			var german = {};
			german['BracketLeft'] = 186; // u with 2 dots
			german['BracketRight'] = 187; // + * ~
			german['Semicolon'] = 192; //o - o with 2 dots!!
			german['Quote'] = 222; //a - alpha
			german['Backslash'] = 191; //#
			german['IntlBackslash'] = 226; //< next to left shift
			german['Minus'] = 219; //Beta
			german['Equal'] = 221; //dead
			german['Slash'] = 189; //-_
			german['Backquote'] = 220; //^ degree

			keyCode = (german[e.code] ?german[e.code] :keyCode);
			//console.log(keyCode);
		}

		return keyCode; 

	},

	_video.textbox.onkeydown = function(e) {
		if(_video.running && _video.sock.validated) {
			var keyCode = listener.keyCodeAssertion(e.keyCode, e),
			keyLocation = (e.keyLocation!==undefined? e.keyLocation: e.location),
			keyPressed = true;

			e.stopPropagation();
			if(keyCode == 18) { // set the flag to true if alt key is pressed
				isAltKeyPressed = true;
			}
			
			// set proper key location for num lock key
			if(keyCode == 144) // keycode for num lock
				keyLocation = 3; // keylocation for num lock
			// In Linux Chrome with the German keyboard, there are two sets of two keys that have the same key code (the dead accent and +/*/~
			// both use key code 187; the ^ and ö keys both use keycode 192). Because of this, these keys have to handled using the onkeypress
			// event, which is triggered when the onkeydown event does not call preventDefault()
			if(navigator.userAgent.match("Linux.*Chrome") && (keyCode == 187 || keyCode == 192) && usbkbd instanceof USBKeyBoardGerman) {
				return;
			}

			e.preventDefault();

			if(e.altKey && e.ctrlKey || _video.textbox.altgr_down) {
				//Block other events and just send the special keys
				//altgr
				if(usbkbd instanceof USBKeyBoardGerman) {

					usbkbdmsg.set(17, usbkbd.KEY_LOCATION_LEFT, false); //ctrl
					listener.send();
					usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, false); //alt
					listener.send();
					usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, true); //alt
					listener.send();
					//~ more special handling, send it twice to register an escape
					/*
					if(e.keyCode == 187) {
						usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, true);
						listener.send();
						usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, false);
						listener.send();
					}*/


					usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, true);
					listener.send();
					usbkbdmsg.set(keyCode, usbkbd.KEY_LOCATION_STANDARD, false);
					listener.send();
					usbkbdmsg.set(18, usbkbd.KEY_LOCATION_RIGHT, false); // alt
					listener.send();
				}

				return;
			}

			// Handling dead keys and AltGr in Firefox
			if(navigator.userAgent.indexOf('Firefox')!=-1 && usbkbd === usbgerkbd) {
				if(e.code == "Equal") {
					// Setting flag to indicate that the dead accent keydown is being sent
					_video.textbox.accent_down_sent = true;
				} else if(e.code == "AltRight") {
					// Setting flag to indicate that AltGr is down
					_video.textbox.altgr_down = true;
				}
				else if(e.code == "Backquote") {
					// Setting flag to indicate that the caret keydown is being sent
					_video.textbox.caret_down_sent = true;
				}
			}
			// Handling AltGr in Linux Chrome
			else if(navigator.userAgent.match("Linux.*Chrome") && usbkbd === usbgerkbd && e.keyCode == 225) {
				// Setting flag to indicate that AltGr is down
				_video.textbox.altgr_down = true;
			}

			if(navigator.userAgent.indexOf('Firefox')!=-1 && usbkbd instanceof USBKeyBoardEnglish) {
				if(keyCode == 173) // - key
					keyCode = 189;
				else if(keyCode == 61) // = key
					keyCode = 187;
				else if(keyCode == 59) // ; key
					keyCode = 186;
			}
			// Sending keydown 
			usbkbdmsg.set(keyCode, keyLocation, keyPressed);
			listener.send();

		}	
	}

	MOUSE = {
			LBUTTON_DOWN : 0x01,
			MBUTTON_DOWN : 0x04,
			RBUTTON_DOWN : 0x02
	};

	var browser = "";
	if(navigator.userAgent.indexOf('Firefox')!==-1) {
		browser = 'Firefox';
	} 
	else if(navigator.userAgent.indexOf('Chrome')!==-1) {
			browser = 'Chrome';
	}
	var btnStatus = 0;

	_video.canvas.onmousedown = function(e) {
		if(_video.sock.validated) {
			//console.log("down",e);
			e.stopPropagation();
			e.preventDefault();
			//(btnStatus, x, y, mode, width, height, wheelrotation)

			var x = e.offsetX || e.layerX,
			y = e.offsetY || e.layerY;

			switch(e.which){
			case 1:
				btnStatus |= MOUSE.LBUTTON_DOWN;
				break;
			case 2:
				btnStatus |= MOUSE.MBUTTON_DOWN;
				break;
			case 3:
				btnStatus |= MOUSE.RBUTTON_DOWN;
				break;
			}
			usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  0);
			listener.sendMouseData();
		}
	};

	_video.canvas.onmouseup = function(e) {
		if(_video.sock.validated) {
			//console.log("up",e);
			e.stopPropagation();
			e.preventDefault();

			var x = e.offsetX || e.layerX,
			y = e.offsetY || e.layerY;

			//$(_video.textbox).css('left', e.pageX+'px').css('top', e.pageY+'px');

			switch(e.which){
			case 1:
				btnStatus &= ~MOUSE.LBUTTON_DOWN;
				break;
			case 2:
				btnStatus &= ~MOUSE.MBUTTON_DOWN;
				break;
			case 3:
				btnStatus &= ~MOUSE.RBUTTON_DOWN;
				break;
			}

			usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  0);
			listener.sendMouseData();
		}
	}

	_video.canvas.onmousemove = function(e) {
		if(_video.sock.validated) {
			e.stopPropagation();
			e.preventDefault();

			var x = e.offsetX || e.layerX,
			y = e.offsetY || e.layerY;

			usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  0);
			listener.sendMouseData();
		}
	}
	/*
	document.getElementById("kvm").onmouseover = function(e) {
		//console.log("over",e);
	}

	document.getElementById("kvm").onmouseout = function(e) {
		//console.log("out",e);
	}*/

	_video.canvas.onmousewheel = function(e) {
		if(_video.sock.validated) {
			//console.log("wheel", e);
			e.stopPropagation();
			e.preventDefault();

			var x = e.offsetX || e.layerX,
			y = e.offsetY || e.layerY;

			usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  e.wheelDelta/120);
			listener.sendMouseData();
		}
	}

	_video.canvas.addEventListener("DOMMouseScroll", function(e) {
		if(_video.sock.validated) {
			//console.log("wheel", e);
			e.stopPropagation();
			e.preventDefault();

			var x = e.offsetX || e.layerX,
			y = e.offsetY || e.layerY;

			usbmouse.set(btnStatus, x, y, window.mousemode, _video.$canvas.width(), _video.$canvas.height(),  e.detail*-1);
			listener.sendMouseData();
		}
	}, false);

};
if (typeof define == "function" && define.amd) {
	define(function() {
		return eventListener;
	});
}