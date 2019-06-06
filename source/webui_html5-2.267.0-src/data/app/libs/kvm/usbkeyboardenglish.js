var KeyBoardType = {
	MOD_LEFT_CTRL : 0x01,
	MOD_RIGHT_CTRL : 0x10,
	MOD_LEFT_SHIFT : 0x02,
	MOD_RIGHT_SHIFT : 0x20,
	MOD_LEFT_ALT : 0x04,
	MOD_RIGHT_ALT : 0x40,
	MOD_LEFT_WIN : 0x08,
	MOD_RIGHT_WIN : 0x80
}


var USBKeyBoardEnglish = function(){}

var standardMap = {
	kc_65 : 4, //a
	kc_66 : 5,
	kc_67 : 6,
	kc_68 : 7,
	kc_69 : 8,
	kc_70 : 9,
	kc_71 : 10,
	kc_72 : 11,
	kc_73 : 12,
	kc_74 : 13,
	kc_75 : 14,
	kc_76 : 15,
	kc_77 : 16,
	kc_78 : 17,
	kc_79 : 18,
	kc_80 : 19,
	kc_81 : 20,
	kc_82 : 21,
	kc_83 : 22,
	kc_84 : 23,
	kc_85 : 24,
	kc_86 : 25,
	kc_87 : 26,
	kc_88 : 27,
	kc_89 : 28,
	kc_90 : 29, //z
	kc_49 : 30, //1
	kc_50 : 31, //
	kc_51 : 32,
	kc_52 : 33,
	kc_53 : 34,
	kc_54 : 35,
	kc_55 : 36,
	kc_56 : 37,
	kc_57 : 38, //9
	kc_48 : 39, //0
	kc_13 : 40, //enter
	kc_27 : 41, //esc
	kc_8 : 42, //bkspace
	kc_9 : 43, //tab
	kc_32 : 44, //space
	kc_189 : 45, //-
	kc_187 : 46, //=
	kc_219 : 47, //[
	kc_221 : 48, //]
	kc_220 : 49, //\
	kc_186 : 51, //,
	kc_222 : 52, //'
	kc_192 : 53, //`
	kc_188 : 54, //,
	kc_190 : 55, //.
	kc_191 : 56, ///
	kc_20 : 57, // CAPS LOCK
	kc_112 : 58, // F1
	kc_113 : 59, // F2
	kc_114 : 60, // F3
	kc_115 : 61, // F4
	kc_116 : 62, // F5
	kc_117 : 63, // F6
	kc_118 : 64, // F7
	kc_119 : 65, // F8
	kc_120 : 66, // F9
	kc_121 : 67, // F10
	kc_122 : 68, // F11
	kc_123 : 69, // F12
	kc_44 : 70, // Print Screen
	kc_145 : 71, // Scroll Lock
	kc_19 : 72, // Pause
	kc_45 : 73, // Insert
	kc_36 : 74, // Home
	kc_33 : 75, // Page up
	kc_46 : 76, // Delete
	kc_35 : 77, // End
	kc_34 : 78, // Page down
	kc_39 : 79, // Right
	kc_37 : 80, // Left
	kc_40 : 81, // Down
	kc_38 : 82, // Up
	kc_93 : 101 // right click
};

var keypadMap = {
	kc_144: 83, //Num Lock
	kc_111: 84, //Divide
	kc_106: 85, //Multiply
	kc_109: 86, //Subtract
	kc_107: 87, //Add
	kc_13: 88, //Enter
	kc_97: 30, //89, //Numpad 1
	kc_98: 31, //90, //Numpad 2
	kc_99: 32, //91, //Numpad 3
	kc_100: 33, //92, //Numpad 4
	kc_101: 34, //93, //Numpad 5
	kc_102: 35, //94, //Numpad 6
	kc_103: 36, //95, //Numpad 7
	kc_104: 37, //96, //Numpad 8
	kc_105: 38, //97, //Numpad 9
	kc_96: 39, //98, //Numpad 0
	kc_110: 55, //Decimal
	kc_45: 73, // Insert
	kc_36: 74, // Home
	kc_33: 75, // Page up
	kc_46: 76, // Delete
	kc_35: 77, // End
	kc_34: 78, // Page down
	kc_39: 79, // Right
	kc_37: 80, // Left
	kc_40: 81, // Down
	kc_38: 82 // Up
};

var leftMap = {
	kc_17 : 224, //left control
	kc_16 : 225, //left shift
	kc_18 : 226, //left alt
	kc_91 : 227 //left windows
};

var rightMap = {
	kc_17 : 228, //right control
	kc_16 : 229, //right shift
	kc_18 : 230, //right alt
	kc_91 : 231 //right windows
};

USBKeyBoardEnglish.prototype = _.extend({
	standardMap : standardMap,
	keypadMap : keypadMap,
	leftMap : leftMap,
	rightMap : rightMap,

    KEY_LOCATION_LEFT: 1,
    KEY_LOCATION_RIGHT: 2,
    KEY_LOCATION_NUMPAD: 3,
    KEY_LOCATION_STANDARD: 0,

    modifiers: 0,

    setAutoKeybreakMode: function(state) {
        this.autoKeybreakMode = state;
    },

    getAutoKeybreakMode: function() {
        return this.autoKeybreakMode;
    },

    convertKeyCode: function(keyCode, keyLocation, keyPress) {
        var usbKeyCode = 0;
        var outputKeycodes = new DataStream(6);
        var sendAutoKeyBreak = false;

        switch (keyLocation) {
            case this.KEY_LOCATION_LEFT:
                usbKeyCode = this.leftMap['kc_' + keyCode];
                break;
            case this.KEY_LOCATION_NUMPAD:
                usbKeyCode = this.keypadMap['kc_' + keyCode];
                break;
            case this.KEY_LOCATION_RIGHT:
                usbKeyCode = this.rightMap['kc_' + keyCode];
                break;
            case this.KEY_LOCATION_STANDARD:
                usbKeyCode = this.standardMap['kc_' + keyCode];
                break;
            default:
                //console.log("Unknown Key Code");
                break;
        }

        //console.log("usbkeycode", usbKeyCode, keyCode);

		if(usbKeyCode!==null) {
            switch (keyCode) {
                case 17: //ctrl
                    if (keyLocation == this.KEY_LOCATION_LEFT) {
                        if (keyPress)
                            this.modifiers |= this.MOD_LEFT_CTRL;
                        else
                            this.modifiers &= ~this.MOD_LEFT_CTRL;
                    } else {
                        if (keyPress)
                            this.modifiers |= this.MOD_RIGHT_CTRL;
                        else
                            this.modifiers &= ~this.MOD_RIGHT_CTRL;
                    }
                    break;
                case 16: //shift
                    if (keyLocation == this.KEY_LOCATION_LEFT) {
                        if (keyPress)
                            this.modifiers |= this.MOD_LEFT_SHIFT;
                        else {
                            this.modifiers &= ~this.MOD_LEFT_SHIFT;
                            this.modifiers &= ~this.MOD_RIGHT_SHIFT;
                        }
                    } else {
                        if (keyPress)
                            this.modifiers |= this.MOD_RIGHT_SHIFT;
                        else {
                            this.modifiers &= ~this.MOD_RIGHT_SHIFT;
                            this.modifiers &= ~this.MOD_LEFT_SHIFT;
                        }
                    }
                    break;
                case 18: //alt
                    if (keyLocation == this.KEY_LOCATION_LEFT) {
                        if (keyPress)
                            this.modifiers |= this.MOD_LEFT_ALT;
                        else
                            this.modifiers &= ~this.MOD_LEFT_ALT;
                    } else {
                        if (keyPress)
                            this.modifiers |= this.MOD_RIGHT_ALT;
                        else
                            this.modifiers &= ~this.MOD_RIGHT_ALT;
                    }
                    break;
                case 91: //windows
                    if (keyLocation == this.KEY_LOCATION_LEFT) {
                        if (keyPress)
                            this.modifiers |= this.MOD_LEFT_WIN;
                        else
                            this.modifiers &= ~this.MOD_LEFT_WIN;
                    } else {
                        if (keyPress)
                            this.modifiers |= this.MOD_RIGHT_WIN;
                        else
                            this.modifiers &= ~this.MOD_RIGHT_WIN;
                    }
                    break;
                default:
                    if (keyPress) { //||print screen
                        outputKeycodes.writeInt8(usbKeyCode);
                        if (this.autoKeybreakMode) sendAutoKeyBreak = true;
                    } else {
                        if (!this.autoKeybreakMode) // NUM_LOCK, SCROLL_LOCK, CAPS_LOCK
                            outputKeycodes.writeInt8(0);
                        else
                            return null;
                    }
                    break;
            }
            outputKeycodes.seek(0);
            //console.log(this.modifiers);
            return this.USBKeyboardPacket(outputKeycodes.readInt8Array(), this.modifiers, sendAutoKeyBreak);
        } else {
            //console.log("Unknown Key Code " + keyCode);
            return null;
        }
    },

    USBKeyboardPacket: function(keys, modifiers, autoKeyBreak) {
        var USBReport = new DataStream(USBMessage.USB_PKT_KEYBDREP_SIZE);
        USBReport.writeInt8(modifiers);
        USBReport.writeInt8(autoKeyBreak ? 1 : 0);
        USBReport.writeInt8Array(keys);
        USBReport.seek(0);
        return USBReport;
    },

    getModifiers: function() {
        return this.modifiers;
    },

    setModifiers: function(modifiers) {
        this.modifiers = modifiers;
    }
}, KeyBoardType);
if (typeof define == "function" && define.amd) {
    define(function() {
        return USBKeyBoardEnglish;

    });
}