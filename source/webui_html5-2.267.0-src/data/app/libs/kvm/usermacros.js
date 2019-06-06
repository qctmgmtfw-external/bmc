/*
** usermacros.js - For processing User Defined Macros ( Hot Keys )
*/

var SINGLE_MACRO_LENGTH = 6; // Maximum number of key combinations for a single macro
var MACRO_COUNT = 20; // Maximum number of macros supported
var SINGLE_KEY_SIZE = 2; // single key size
var INT_SIZE = 4; // size of int
var _256BIT = 256; // 32 bytes = 256 bits
var _256BIT_HEX = 0xff; // Hex equivalent of 256 (bits) [ 32 bytes ]
var SIZE_OF_MACRO = ( MACRO_COUNT * SINGLE_MACRO_LENGTH * SINGLE_KEY_SIZE * INT_SIZE ); // total size of macro

var MACRO_MSG_DURATION = 100; // macro dialog warning message show / hide delay duration
var ID_INDEX = 4; // Index value used for parsing delete button's 'id' value

var VK_ALT = 18; // key code constant for Alt key
var VK_F4 = 115; // key code constant for F4 key
var VK_PRINTSCREEN = 44; // key code constant for Print Screen key
var VK_WINDOWS = 91; // key code constant for Windows key

var hotkey_code = ""; // A string representing, set of keycodes and keylocations for the current macro
var macroMap = {}; // A map containing formated keycodes and keylocations for hotkey combinations

var isFirefox = ( navigator.userAgent.indexOf('Firefox') !== -1 ? true : false );

var macroMsg = new USBKeyBoardMessage(); // for sending key events
var macroKBD = new USBKeyBoardEnglish(); // For KEY_LOCATION_* constants

var keyTextMap = { // A map containing key texts for browser key codes
/* Key texts for standard browser keycodes */
  8 : "Backspace",
  9 : "Tab",
  13 : "Enter",
  16 : "Shift",
  17 : "Ctrl",
  18 : "Alt",
  19 : "Pause",
  20 : "Caps Lock",
  27 : "Escape",
  32 : "Space",
  33 : "Page Up",
  34 : "Page Down",
  35 : "End",
  36 : "Home",
  37 : "Left",
  38 : "Up",
  39 : "Right",
  40 : "Down",
  44 : "Print Screen",
  45 : "Insert",
  46 : "Delete",
  48 : "0",
  49 : "1",
  50 : "2",
  51 : "3",
  52 : "4",
  53 : "5",
  54 : "6",
  55 : "7",
  56 : "8",
  57 : "9",
  65 : "A",
  66 : "B",
  67 : "C",
  68 : "D",
  69 : "E",
  70 : "F",
  71 : "G",
  72 : "H",
  73 : "I",
  74 : "J",
  75 : "K",
  76 : "L",
  77 : "M",
  78 : "N",
  79 : "O",
  80 : "P",
  81 : "Q",
  82 : "R",
  83 : "S",
  84 : "T",
  85 : "U",
  86 : "V",
  87 : "W",
  88 : "X",
  89 : "Y",
  90 : "Z",
  91 : "Windows",
  93 : "Context Menu",
  96 : "NumPad 0",
  97 : "NumPad 1",
  98 : "NumPad 2",
  99 : "NumPad 3",
  100 : "NumPad 4",
  101 : "NumPad 5",
  102 : "NumPad 6",
  103 : "NumPad 7",
  104 : "NumPad 8",
  105 : "NumPad 9",
  106 : "NumPad *",
  107 : "NumPad Plus",
  109 : "NumPad -",
  110 : "NumPad .",
  111 : "NumPad /",
  112 : "F1",
  113 : "F2",
  114 : "F3",
  115 : "F4",
  116 : "F5",
  117 : "F6",
  118 : "F7",
  119 : "F8",
  120 : "F9",
  121 : "F10",
  122 : "F11",
  123 : "F12",
  144 : "Num Lock",
  145 : "Scroll Lock",
  188 : "Comma",
  190 : "Period",
  191 : "Slash",
  192 : "Back Quote",
  219 : "Open Bracket",
  220 : "Back Slash",
  221 : "Close Bracket",
  222 : "Quote",
/* Keytext for Firefox specific keycodes */
  59 : "Semicolon",
  61 : "Equals",
  173 : "Minus",
/* Keytext for keyCodes [ browsers other than Firefox ] */
  186 : "Semicolon",
  187 : "Equals",
  189 : "Minus"
};

/* Firefox specific keyCodes cross mapped according to
** the Standard web browser specifications.*/
var keyCodes_Firefox = {
  59 : 186,  // Semicolon
  61 : 187,  // Equals
  173 : 189, // Minus
};

/* Standard keyCodes cross mapped according to
** the web browser specifications.*/
var BrowserKeyCodeMap = {
  10 : 13,  // Enter
  44 : 188, // Comma
  45 : 189, // Minus
  46 : 190, // Period
  47 : 191, // Slash
  59 : 186, // Semicolon
  61 : 187, // Equals
  91 : 219, // Open Bracket
  92 : 220, // Backslash
  93 : 221, // Close Bracket
  127 : 46, // Delete
  154 : 44, // Print Screen
  155 : 45, // Insert
  524 : 91, // Windows
  525 : 93  // Context Menu
};

/* web browser keyCodes cross mapped according to
** the Standard key code format.*/
var StandardKeyCodeMap = {
  13 : 10,  // Enter
  44 : 154, // Print Screen
  45 : 155, // Insert
  46 : 127, // Delete
  91 : 524, // Windows
  93 : 525, // Context Menu
  186 : 59, // Semicolon
  187 : 61, // Equals
  188 : 44, // Comma
  189 : 45, // Minus
  190 : 46, // Period
  191 : 47, // Slash
  219 : 91, // Open Bracket
  220 : 92, // Backslash
  221 : 93  // Close Bracket
};

/* =============================================================================
** Function used to update macroMap values
** =============================================================================
*/
function setMacroMap(_macroMap) {
  macroMap = _macroMap;
}

/* =============================================================================
** Function used to retrive macroMap values
** =============================================================================
*/
function getMacroMap() {
  return macroMap;
}

/* =============================================================================
** Function used to display the key text in the input dialog
** =============================================================================
*/
function displayHotkey(keyCode, keyLocation) {
  var macro_input = document.getElementById("macro_dialog_input"); // macro input box

  $('#err_DuplicateMacro').hide(MACRO_MSG_DURATION); // hide the error msg. (if any)
  /* if the key code length is not exceeding the single macro length limit then proceed */
  if( macro_input.value.split("+").length < SINGLE_MACRO_LENGTH ) {
    if( macro_input.value.length > 0 ) {
      // If the input dialog contains a value already, then append '+' symbol
      // before displaying the key text
      macro_input.value = macro_input.value.concat("+");
      hotkey_code = hotkey_code.concat("+");
    }

    // Display the key text
    macro_input.value = macro_input.value.concat( keyTextMap[keyCode] );
    // A string representing, set of keycodes and keylocations for the current macro
    hotkey_code = hotkey_code.concat(keyCode, "+", keyLocation);

    // Appending left / Right to key text based on key location
    if ( keyLocation == macroKBD.KEY_LOCATION_LEFT ) {
      macro_input.value = macro_input.value.concat("(Left)");
    } else if ( keyLocation == macroKBD.KEY_LOCATION_RIGHT ) {
      macro_input.value = macro_input.value.concat("(Right)");
    }

  } else {
    // Show the warning message if single macro length limit is reached
    $('#err_SingleMacroLimit').show(MACRO_MSG_DURATION);
  }
}

/* =============================================================================
** Function used to convert int value to 4-bytes array representation
** =============================================================================
*/
function intToByteArray(intValue) {
    // we want to represent the input as a 4-bytes array
    // EX: 524 = [0, 0, 2, 12]
    var keyCodes = [0];

    // Since the byte order is BIGINDIAN we're going in reverse
    for ( var index = INT_SIZE-1 ; index >= 0; index -- ) {
        var byte = intValue & _256BIT_HEX;
        keyCodes [ index ] = byte;
        intValue = (intValue - byte) / _256BIT ;
    }
    return keyCodes;
}

/* =============================================================================
** Function used to convert 4-bytes array value to int representation
** =============================================================================
*/
function byteArrayToInt(keyCodes) {
    // we want to represent the input value [4-bytes array] to it's int equivalent.
    // EX: [0, 0, 2, 12] = 524
    var intValue = 0;
    // Since the byte order is BIGINDIAN we're going in stright manner ( not reverse )
    for ( var index = 0; index < keyCodes.length; index++) {
        intValue = (intValue * _256BIT) + keyCodes[ index ];
    }
    return intValue;
}

/* =============================================================================
** Function used to parse the user macro data from the adviser
** =============================================================================
*/
function parseUserMacroData(macroBuffer) {
  var localMacroMap = {}; // map in which the parsed data to be stored
  var index = 0; // buffer index
  var keys = []; // Array of key texts
  var keyCode, keyLocation; // key code and key location values
  var macroString = "", macroCode = ""; // formatted key text and key code + key location values for a single macro
  for (var i = 0; i < SIZE_OF_MACRO; i++) { // loop untill max macro limit
      // reset the values for single macro
      macroString = "";
      macroCode = "";
      for (var j = 0; j < SINGLE_MACRO_LENGTH; j++) { // loop untill single macro limit
        // process the values as 4-bytes arrays for key code
        keyCode = byteArrayToInt( macroBuffer.subarray(index, index + INT_SIZE) );
        index += INT_SIZE; // update the seek position
        // process the values as 4-bytes arrays for key location
        keyLocation = byteArrayToInt( macroBuffer.subarray(index, index + INT_SIZE) );
        index += INT_SIZE; // update the seek position
        if ((keyCode != 0) && (keyLocation != 0)) { // if the valid value is found
          // concat '+' with display text if the text contains values already
          if (macroString.length != 0) {
            macroString = macroString.concat("+");
          }
          // concat '+' with formatted string (key code and key location) if the string contains values already
          if (macroCode.length != 0) {
            macroCode = macroCode.concat("+");
          }
          // N-1 value difference between standard key location and browser key location
          // Ex: for key location standard -> standard format = 1; browser format = 0;
          keyLocation = parseInt(keyLocation) - 1;
          // Some key code values will be different from existing browser key code implementation.
          // Hence cross map the key codes to browser key code format before further proceeding.
          keyCode = ( BrowserKeyCodeMap[keyCode] ? BrowserKeyCodeMap[keyCode] : keyCode );
          // concat key text for the key code value
          macroString = macroString.concat( keyTextMap[keyCode] );
          // concat formatted string ( key codes and key locations )
          macroCode = macroCode.concat(keyCode.toString(), "+", keyLocation.toString());
          // append left / right to key text based on key location
          if( keyLocation == macroKBD.KEY_LOCATION_LEFT ) {
            macroString = macroString.concat("(Left)");
          } else if ( keyLocation == macroKBD.KEY_LOCATION_RIGHT ) {
            macroString = macroString.concat("(Right)");
          }
        }
      }
      if(macroString.length != 0 && macroCode.length != 0) {
        // if the entry is not found already then add it to the map.
        if( typeof localMacroMap[macroString] == 'undefined' ) {
          // add the entry to map
          localMacroMap[macroString] = macroCode;
          // push the values to key text array which will be used for constructing menu items
          keys.push(macroString);
        }
      }
  }
  setMacroMap(localMacroMap); // update the existing map with current values
  // remove the existing menu items
  $('#usr_macros_list').empty();
  $('#usr_macros_menu').empty();
  $('#usr_macros_menu').append('<li><a href="#" id="addHotKey">'
  + window.CommonStrings.add + '&nbsp;' + window.CommonStrings.hotkeys
  + '</a></li><li role="separator" class="divider" id="macroDivider" hidden></li>');
  // iterating through array of key texts for constructing menu items
  keys.forEach(function(keytext) {
    constructMacroMenu(keytext);
  });
}

/* =============================================================================
** Function used to send key events to host
** =============================================================================
*/
function sendKeyEvent(keycode, keylocation, keyPressedStatus) {
  // Set the key processor value before sending
  macroMsg.setUSBKeyProcessor(macroKBD);
  // if the browser is firefox the convert the key code to standard browser format
  // before sending to host
  if (isFirefox) {
    keycode = ( keyCodes_Firefox[keycode] ? keyCodes_Firefox[keycode] : keycode );
  }
  // set the appropriate values for the usb packet
  macroMsg.set(keycode, keylocation, keyPressedStatus);
  //sends data if video is not paused
  if(window._video.isVideoPaused != true){
    if(window._video.sock.validated) {
      var rpt = macroMsg.report(); // Generate the usb report
      window._video.sock.send(rpt.buffer); // Send the data
    }
  }
}

/* =============================================================================
** Function used to properly send key pressed / released events to the host
** =============================================================================
*/
function OnsendMacrokeycode(hotkey) {
  /* In JViewer we follow FIFO approach
  ** i.e, First pressed key will be released first
  ** Ex: for Ctrl + A -> Ctrl (press) A (press) Ctrl (release) A (release)
  **
  ** For H5Viewer we follow LIFO approach
  ** i.e, last pressed key will be released first
  ** Ex: for Ctrl + A -> Ctrl (press) A (press) A (release) Ctrl (release)*/

  // hotkey will contains combination of key codes and key locations to be processed
  var keycodesplit = hotkey.split("+");
  var index = 0;
  var keycode, keylocation;
  // Sending key pressed event
  while(index < keycodesplit.length) {
    keycode = parseInt( keycodesplit[ index++ ] );
    keylocation = parseInt( keycodesplit[ index++ ] );
    //console.log(keyTextMap[keycode] + " + " + keylocation + " + keypressed");
    sendKeyEvent(keycode, keylocation, true);
  }

  index = keycodesplit.length-1;
  // Sending key released event
  while(index >= 0) {
    keylocation = parseInt( keycodesplit[ index-- ] );
    keycode = parseInt( keycodesplit[ index-- ] );
    //console.log(keyTextMap[keycode] + " + " + keylocation + " + keyreleased");
    sendKeyEvent(keycode, keylocation, false);
  }
}

/* =============================================================================
** Function for constructing Hotkey menu items
** =============================================================================
*/
function constructMacroMenu(menuItem) {
  var macro_itemList = document.getElementById("usr_macros_list");  // list in user macro dialog box
  var macro_itemMenu = document.getElementById("usr_macros_menu");  // dropdown menu
  /* For creating string for 'id' property of each menu item, we'll get the key
  ** text and format the string by eliminating all the special characters.
  ** Ex: For Alt + F4 key combination, menuItem will contain string like
  ** 'Alt(left)+F4'. after formating the string will be like 'AltleftF4' Which
  ** will be suitable for using as 'id' value for the menu item created during
  ** runtime. This is implemented using regular expression written below. */
  var regex = new RegExp('[\*|\+|\-|\/|\||\)|\(|\ |\.]', 'g'); // 'g' for global replace [ equivalent to replaceAll ]
  var id_str = menuItem.replace(regex,''); // formatted string which is used as 'id' value

  $('#macroDivider').show(); // For making the divider in dropdown menu visible

  // creating list entry for the given key combination
  $(macro_itemList).append('<li class = \"list-group-item\"' + ' id = \"' + id_str + '\">' + menuItem
  + ' <button type=\"button\" class=\"close\" id=\"btn_' + id_str +'\"><i class="glyphicon glyphicon-trash" style="color:red"></i> </button></li>');

  // creating an entry in dropdown menu for the given key combination
  $(macro_itemMenu).append('<li id = \"HK_' + id_str + '\"><a href=\"#\">' + menuItem + '</a></li>');

  /* Note: For each menu item three components will be created at runtime,
  **       1. MenuList ( under user macro dialog )
  **       2. Dropdown Menu ( under Hot Keys menu )
  **       3. Delete button ( under user macro dialog, following MenuList created )
  **
  ** The 'id' values for the three items will be like
  **       1. MenuList -> 'formatted id string'
  **       2. Dropdown Menu -> 'HK_ + formatted id string'
  **       3. Delete button -> 'btn_ + formatted id string'
  **
  ** Ex: for Alt+F4 combination
  **       1. MenuList -> 'AltleftF4'
  **       2. Dropdown Menu -> 'HK_AltleftF4'
  **       3. Delete button -> 'btn_AltleftF4' */

  // event listner for delete button. upon clicking it'll remove the entry from
  // Macro map as well as removing the respective menu components
  $(macro_itemList).on('click', '#btn_' + id_str, function() {
    var size = 0; // size of current Macro map
    var id = ""; // For parsing the id value
    var localMacroMap = getMacroMap(); // current Macro map

    id = $(this).attr("id").substring(ID_INDEX); // Common 'id' value for menu items

    var list = document.getElementById(id); // menu list id
    var menu = document.getElementById('HK_' + id); // dropdown menu id
    var menuItemText = $(menu).text(); // dropdown menu item text

    for (var value in localMacroMap) {
      if (localMacroMap.hasOwnProperty(value)) {
          // if value found in macro map
          if ( value == menuItemText ) {
              delete localMacroMap[value]; // Deleting the entry in macroMap
              // remove event listner
              $(list).off("click");
              $(menu).off("click");
              $(list).remove(); // removing menu list
              $(menu).remove(); // removing dropdown menu item
          }
          size ++;
      }
    }
    setMacroMap(localMacroMap); // update the existing map with current values

    // hide the divider if macro map is empty, show otherwise
    if (size > 0) {
        $('#macroDivider').show();
    } else {
        $('#macroDivider').hide();
    }
  });

  // event listner for the dropdown menu item
  // upon clicking it'll send the corresponding key code for the menu item to adviser
  $(macro_itemMenu).on('click', '#HK_' + id_str, function(ev) {
    ev.preventDefault();
    var currentMacroMap = getMacroMap();
    // get menu item text
    var menuText = $(ev.target).text();
    // if the entry found in macroMap send the respective keycode values
    if ( currentMacroMap[ menuText ] !== 'undefined' ) {
      OnsendMacrokeycode( currentMacroMap[ menuText ] );
    }
  });
}
