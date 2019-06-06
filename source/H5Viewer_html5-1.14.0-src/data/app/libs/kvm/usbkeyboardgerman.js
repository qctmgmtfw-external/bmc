var USBKeyBoardGerman = function(){};

USBKeyBoardGerman.prototype = _.extend({}, USBKeyBoardEnglish.prototype);

//deep clone any object that is overridden
USBKeyBoardGerman.prototype.standardMap = _.extend({}, USBKeyBoardEnglish.prototype.standardMap);
USBKeyBoardGerman.prototype.keypadMap = _.extend({}, USBKeyBoardEnglish.prototype.keypadMap);

USBKeyBoardGerman.prototype.standardMap['kc_189'] = 56; //minus
USBKeyBoardGerman.prototype.standardMap['kc_219'] = 45; //beta
USBKeyBoardGerman.prototype.standardMap['kc_186'] = 47; //u
USBKeyBoardGerman.prototype.standardMap['kc_187'] = 48; //`plus
USBKeyBoardGerman.prototype.standardMap['kc_191'] = 49; //#
USBKeyBoardGerman.prototype.standardMap['kc_220'] = (navigator.userAgent.match("Linux.*Chrome") ? 100 : 53); //deg (or < in Linux Chrome)
USBKeyBoardGerman.prototype.standardMap['kc_221'] = 46; //acute
USBKeyBoardGerman.prototype.standardMap['kc_192'] = 51; //o with dots
USBKeyBoardGerman.prototype.standardMap['kc_226'] = (navigator.userAgent.match("Linux.*Chrome") ? 53 : 100); //< (or deg in Linux Chrome)
USBKeyBoardGerman.prototype.standardMap['kc_89'] = 29; //y
USBKeyBoardGerman.prototype.standardMap['kc_90'] = 28; //z
USBKeyBoardGerman.prototype.keypadMap['kc_110'] = 54; //comma

if (typeof define == "function" && define.amd) {
    define(function() {
        return USBKeyBoardGerman;

    });
}