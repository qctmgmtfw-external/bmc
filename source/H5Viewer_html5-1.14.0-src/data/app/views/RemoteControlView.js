define(['jquery', 'underscore', 'backbone', 'app',
        //data
        'models/kvm_token',
        'models/media/remotesession',
        //vmedia information
        'models/media/instance',
        'models//adviser',
        'models//oem_resource',
        //localize
        'i18n!strings/nls/remote_control',
        //template
        'text!templates/remote.html',
        //libs
        'video',
        'cdrom',
        'oem_app',
        //plugins
        // 'moment',
        // 'knob',
        // 'sparkline'
        'dropdown'
    ],

	function($, _, Backbone, app, KVMTokenModel, RemoteSessionModel,MediaSessionModel,AdviserModel,OEMResourceModel,CommonStrings, RemoteTemplate, Video, CDROM, OEM_APP) {
		var oem_app = new OEM_APP();
		/*Flag to keep track if oem has handled initial auth functionalities*/
		this.oemAuthHandled = false;
		// list of menu items to show/hide on startKVM and stopKVM
		kvmMenus = new Array("#button_video_menu","#button_mouse_menu","#button_options_menu", "#button_keyboard_menu","#button_send_keys","#button_HotKeys","#button_power_menu","#button_active_user","#button_video_recording", "#button_help", "#stop");

		var KBD_LAYOUT_DIALOG_COUNTDOWN = 5; // Maximum time(in seconds) for countdown in keyboard layout language dialog
		var SWITCH_MASTER_DIALOG_COUNTDOWN = 10; // Maximum time(in seconds) for countdown in switch master dialog

		var CAPTURE_SCREEN_FILE_NAME = "CaptureScreen.jpeg"; // File name to be used for screenshot captured.

		var OEMPATH = "/api/logs/help-data"; // remote path which contains resource files for help content
		var COPYRIGHT_FILE_NAME = "copyright_"; // generic name of the resource file
		var COPYRIGHT_FILE_LOCALE = "EN"; // localization code of the resource file
		var COPYRIGHT_FILE_EXTN = ".txt"; // resource file extension
		var LOGO_FILE_NAME = "jviewerlogo.jpg"; // image file name used as logo
		var DEFAULT_LOGO_PATH = "./images/" + LOGO_FILE_NAME; // local folder which contains logo file for help content

		var view = Backbone.View.extend({

			template: _.template(RemoteTemplate),
			initialize: function() {
				this.oemAuthHandled = oem_app.isOEMAuthHandled();
				if(this.oemAuthHandled == false)
				{
					this.model = new KVMTokenModel();
					this.adviserModel= new AdviserModel();	
				}
			},
					
			events: {
				"click #toggle": "startKVM",
				"click #record": "recordVideo",
				"click #stop": "stopRecording",
				"click #video_rec_setting": "videoRecordSettings",
				"click #css": "animateVideo",
				"click #media": "startMedia",
				"click #browse": "showFile",
				"change #files": "chooseISO",
				"requestKvmPermission #master_dialog": "handleKvmPermissionRequest",
				"requestMasterPermission #master_dialog": "handleMasterRequest",
				"resetDialog #master_dialog": "resetMasterDialog",
				"click #close_master": "cancelMasterRequest",
				"click #full_permission": "grantFullPermission",
				"click #part_permission": "grantPartPermission",
				"click #no_permission": "grantNoPermission",
				"click #block_priv_request_opt": "blockIncomingPrivRequest",
				"waitForPermission #slave_dialog": "handleSlaveRequest",
				"resetDialog #slave_dialog": "resetSlaveDialog",
				"click #close_slave": "cancelSlaveRequest",
				"click #close_status": "closeStatus",
				"click #close_master_switch": "closeMasterSwitch",
				"click #close_settings": "closeVideoRecSetting",
				"click #request_full": "requestFullAccess",
				"showStatus #status_dialog": "showStatus",
				"prepareSwitchMaster #switch_master_dialog" : "prepareSwitchMaster",
				"closeDialog #switch_master_dialog": "closeMasterSwitch",
				"click #switch_master": "handleSwitchMaster",
				"close #toggle": "stopKVM",
        			"click #kbd_layout_jap": "keyboardLayoutDisabledCheck",
				"click #power_on": "powerOn",
				"click #power_reset": "powerReset",
				"click #power_cycle": "powerCycle",
				"click #power_off_immediate": "powerOffImmediate",
				"click #power_off_orderly": "powerOffOrderly",
				"click #pause_video": "pauseVideo",
				"click #resume_video": "resumeVideo",
				"click #refresh_video": "refreshVideo",
				"click #zoom_in": "ZoomIn",
				"click #zoom_out": "ZoomOut",
				"click #show_cli_cursor": "ShowClientCursor",
				"click #show_host_cursor": "ShowHostCursor",
				"click #absolute_mm": "absoluteMouseMode",
				"click #relative_mm": "relativeMouseMode",
				"click #other_mm": "otherMouseMode",
				"click #active-user": "activeUser",
				"click #hd_turn_on": "hostDisplayOn",
				"click #hd_turn_off": "hostDisplayOff",
				"click #capture_screen": "captureScreen",
				"click #bw_normal": "lowBandwidthNormal",
				"click #bw_8BPP": "lowBandwidth8BPP",
				"click #bw_8BPPBW": "lowBandwidth8BPPBW",
				"click #bw_16BPP": "lowBandwidth16BPP",
				"click #block_priv_request": "blockPrivilegeRequest",
				"click #video_settings_ok": "videoRecSettingsok",
				"closeDialog #kbd_layout_dialog": "closeKbdLayoutDialog",
				"click #kbd_permission_no": "closeKbdLayoutDialog",
				"click #kbd_permission_yes": "sendKeyBoardLang",
                		"click #kbd_mouse_encryption": "keyboardMouseEncryption",
                		"click #addHotKey": "userMacroDialog",
		                "click #usr_macro_close": "closeUserMacroDialog",
		                "click #usr_macro_add": "macroDialog",
		                "click #macro_close": "closeMacroDialog",
		                "click #macro_clrAll": "clearMacroDialog",
		                "click #macro_clr": "clearMacroDialog",
		                "click #macro_win": "displayKeyText",
		                "click #macro_altf4": "displayKeyText",
		                "click #macro_printscr": "displayKeyText",
		                "click #power_ctrl_btn": "onPowerCtrlBtnClick",
		                "click #host_lock_btn": "onHostLockBtn",
				"click #about": "displayHelpContent",
				"click #close_help": "closeHelpDialog"
			},
			
//PILOT III bandwidth
			/*
			LOW_BANDWIDTH_MODE_NORMAL = 0;
	public static final byte  LOW_BANDWIDTH_MODE_8BPP = 1;
	public static final byte  LOW_BANDWIDTH_MODE_16BPP = 2;
	public static final byte  LOW_BANDWIDTH_MODE_8BPP_BW = 3;
			*/

			lowBandwidthNormal: function(ev) {
				ev.preventDefault();
				$("#lbw-select").appendTo($(ev.currentTarget)); 
				this.onSendLowBandwidthMode(0);
			},

			lowBandwidth8BPP: function(ev) {
				ev.preventDefault();
				$("#lbw-select").appendTo($(ev.currentTarget));
				this.onSendLowBandwidthMode(1);
			},

			lowBandwidth8BPPBW: function(ev) {
				ev.preventDefault();
				$("#lbw-select").appendTo($(ev.currentTarget)); 
				this.onSendLowBandwidthMode(2);
			},

			lowBandwidth16BPP: function(ev) {
				ev.preventDefault();
				$("#lbw-select").appendTo($(ev.currentTarget)); 
				this.onSendLowBandwidthMode(3);
			},

			//update media status to video server
			onSendLowBandwidthMode: function(bandwidthMode) {
				// call pause reidrection function first here
				//0x1007 IVTP_LOW_BANDWIDTH_MODE
				var state = new Uint8Array(1);
				state[0] = bandwidthMode;
				hdr = window._video.sock.createHeader(0x1007, 1, 0, state);
				window._video.sock.send(hdr.buffer);
				// call resume reidrection function  here
			},

			//update media status to video server
			onSetBandwidthMode: function(bandwidthMode) {
				// call pause reidrection function first here
				//select the appropriate menu here
				if(bandwidthMode == 0)
				$("#lbw-select").appendTo($(ev.currentTarget)); //select low band width normal
				if(bandwidthMode == 1)
				$("#lbw-select").appendTo($(ev.currentTarget)); //select low band width 8bpp
				if(bandwidthMode == 2)
				$("#lbw-select").appendTo($(ev.currentTarget)); //select low band width 8bpp bw
				if(bandwidthMode == 3)
				$("#lbw-select").appendTo($(ev.currentTarget)); //select low band width 16bpp
				// call resume reidrection function  here
			},
			activeUser: function(ev){
				ev.stopPropagation();
			},

			/* Function which will invoke help contents dialog */
			displayHelpContent: function(ev){
				ev.preventDefault();

				if( $("#help_dialog").css('display') == 'block') {
					// if the window is already opened then do nothing.
					return;
				}

				if ( oem_app.handleOEMHelpContent() == false ) { // if OEM customization is not found then proceed
					// BMC may have different resource file according to the localization.
					// So in that case the modal is designed to update the url value during initialization.
					var url = location.protocol + "//" + location.hostname + OEMPATH + "?file="
					+ COPYRIGHT_FILE_NAME + COPYRIGHT_FILE_LOCALE + COPYRIGHT_FILE_EXTN;

					var model = new OEMResourceModel( { path: url } );

					model.fetch({
						success: function(){
							var copyright_text = model.get('content').toString();

							if ( copyright_text != null ) { // if the response contains proper values then proceed
								copyright_text = copyright_text.replace("<=jviewerversion=>", APP_VERSION + NEW_LINE_TAG);
								copyright_text = copyright_text.replace("<=socversion=>", SOC_VERSION);
								copyright_text = copyright_text.replace("<=soc=>", SOC_NAME + NEW_LINE_TAG);
							} else { // otherwise fall back and load values from local file
								copyright_text = window._video.sock.getDefaultHelpText();
							}

							/* The above model can't be used for fetching image files.
							** So going for XMLHttpRequest method. Here the response data will be stored as Blob object.
							** From that we will generate an Blob Object URL which serves as logo */
							var xhr = new XMLHttpRequest();
							xhr.open('GET', OEMPATH + "?file=" + LOGO_FILE_NAME, true);
							xhr.responseType = 'blob';
							xhr.setRequestHeader("X-CSRFTOKEN", window.sessionStorage.getItem("garc")); // for authentication
							xhr.onload = function() {
								var img_src = "";

								if (this.status == 200) { // SUCCESS value is 200
									// Note: .response instead of .responseText
									var blob = new Blob([this.response], {type: 'image/jpeg'});
									img_src = window.URL.createObjectURL(blob);
								} else { // otherwise fall back and use image from local file
									img_src = DEFAULT_LOGO_PATH;
								}

								// Update the contents of the dialog
								window._video.sock.setHelpContent( img_src, copyright_text, window.CommonStrings.about );
							};
							xhr.send();
						},

						error: function() {
							// if unsucessful fall back and load values from local file
							window._video.sock.setHelpContent( DEFAULT_LOGO_PATH, window._video.sock.getDefaultHelpText(), window.CommonStrings.about );
						}
					});
				}
				$('#help_dialog').show();
			},

			closeHelpDialog: function(ev) {
				ev.preventDefault();
				$('#help_dialog').hide();
			},

			captureScreen: function(ev) {
				ev.preventDefault();
				//console.log("captureScreen");
				// For Internet Explorer, we can use Blob object to export canvas to image file
				if(typeof window.navigator.msSaveBlob !== 'undefined') {
					window.navigator.msSaveBlob( document.getElementById("kvm").msToBlob() /* Canvas to Blob */,
					/* Save file name */ CAPTURE_SCREEN_FILE_NAME );
					return;
				}
				// Works as fall back mechanism, in case of browsers other than IE.
				var link = document.createElement('a');
				// We wan't to save the canvas to JPEG format. So giving parameter value for toDataURL() method, as "image/jpeg".
				// If not mentioned, canvas will be exported as PNG format.
				link.href  = document.getElementById("kvm").toDataURL("image/jpeg");
				link.download = CAPTURE_SCREEN_FILE_NAME; // Save file name
				document.body.appendChild(link); // For Firefox browser
				link.click(); // Start Download
				link.remove(); // For Firefox browser
			},
			ZoomIn: function(ev) {
				ev.preventDefault();
				console.log("Zoom in");
				console.log("Zoom in  window.devicePixelRatio:"+window.devicePixelRatio +"  videoScale:"+videoScale);
				if(videoScale <= 1.5){
					var canvas = document.getElementById("kvm");
					var context = canvas.getContext("2d");
					window.devicePixelRatio = window.devicePixelRatio -(window.devicePixelRatio * 0.1);
					videoScale = videoScale -(videoScale *0.1);
					//console.log("Zoom in  window.devicePixelRatio:"+window.devicePixelRatio);
					context.canvas.style.width = context.canvas.width + "px";
					context.canvas.style.height = context.canvas.height + "px";
					context.canvas.height = context.canvas.height * window.devicePixelRatio;
					context.canvas.width = context.canvas.width * window.devicePixelRatio;
					context.scale(window.devicePixelRatio, window.devicePixelRatio);
				}

				if(videoScale >= 0.5){
					$("#zoom_out").removeClass("disable_a_href");
				}

				if(videoScale>= 1.5){
					$("#zoom_in").addClass("disable_a_href");
				}

			},
			ZoomOut: function(ev) {
				ev.preventDefault();
				console.log("Zoom out");
				console.log("Zoom out  window.devicePixelRatio:"+window.devicePixelRatio+"   videoScale:"+videoScale);
				if(videoScale >= 0.5){
					var canvas = document.getElementById("kvm");
					var context = canvas.getContext("2d");
					window.devicePixelRatio = window.devicePixelRatio +(window.devicePixelRatio * 0.1);
					videoScale = videoScale +(videoScale *0.1);
					//console.log("Zoom out  window.devicePixelRatio:"+window.devicePixelRatio);
					context.canvas.style.width = context.canvas.width + "px";
					context.canvas.style.height = context.canvas.height + "px";
					context.canvas.height = context.canvas.height * window.devicePixelRatio;
					context.canvas.width = context.canvas.width * window.devicePixelRatio;
					context.scale(window.devicePixelRatio, window.devicePixelRatio);
				}

				if(videoScale <= 0.5){
					$("#zoom_out").addClass("disable_a_href");
				}

				if(videoScale >= 0.5){
					$("#zoom_in").removeClass("disable_a_href");
				}

			},

			ShowClientCursor: function(ev) {
				ev.preventDefault();
				//console.log("show client cursor");

				if($("#show_cli_cursor").children("i").length == 0) {
					$("#show_cli_cursor").append("<i class=\"fa fa-check\"></i>");
					document.getElementById("kvm").style.cursor = "auto";
		    		}
    				else {
					$("#show_cli_cursor").children("i").remove();
					document.getElementById("kvm").style.cursor = "none";
				}
			},
			
			ShowHostCursor: function(ev) {
				//console.log("show host cursor");
				ev.preventDefault();
			  	var state = new Uint8Array(1);
				state[0] = 0;
				if($("#show_host_cursor").children("i").length == 0) {
					$("#show_host_cursor").append("<i class=\"fa fa-check\"></i>");
					state[0] = 1;
		    		}
    				else {
					$("#show_host_cursor").children("i").remove();
				}
				var hdr=window._video.sock.createHeader(0x1005,1,0,state); //IVTP_XCURSOR_CONTROL HW cursor
				window._video.sock.send(hdr.buffer);

			},

			onHostLockBtn: function(ev) {
				if( $("#hd_turn_on").children("i").length > 0) {
					$("#hd_turn_off").trigger("click");
				} else{
					$("#hd_turn_on").trigger("click");
				}
			},

			hostDisplayOn: function(ev) {
				ev.preventDefault();
				window._video.sock.updateHostLockStateMenu(0);
				//console.log("hostDisplayOn");
			  	var state = new Uint8Array(1);
				state[0] = 0;
				var hdr = window._video.sock.createHeader(IVTP.CMD_DISPLAY_LOCK_SET, 1, 0, state); //on
				window._video.sock.send(hdr.buffer);
			},

			hostDisplayOff: function(ev) {
				window._video.isHostDisplayOn=false;
				ev.preventDefault();
				window._video.sock.updateHostLockStateMenu(1);
				var state = new Uint8Array(1);
				state[0] = 1;
				//console.log("hostDisplayOff");
				var hdr = window._video.sock.createHeader(IVTP.CMD_DISPLAY_LOCK_SET, 1, 0, state);//off
				window._video.sock.send(hdr.buffer);
			},

				
			sendMouseMode: function(mouseMode) {
				var hdr = window._video.sock.createHeader(IVTP.CMD_SET_MOUSE_MODE, 0, mouseMode, "");
				window._video.sock.send(hdr.buffer);
			},

			absoluteMouseMode: function(ev) {
				ev.preventDefault();
				$("#mm-select").appendTo($(ev.currentTarget));
				this.sendMouseMode(2);
			},

			relativeMouseMode: function(ev) {
				ev.preventDefault();
				$("#mm-select").appendTo($(ev.currentTarget));
				this.sendMouseMode(1);
			},

			otherMouseMode: function(ev) {
				ev.preventDefault();
				$("#mm-select").appendTo($(ev.currentTarget));
				this.sendMouseMode(3);
			},

			pauseVideo: function(ev) {
				window._video.isVideoPaused=true;
				ev.preventDefault();
				var hdr=window._video.sock.createHeader(IVTP.CMD_PAUSE_REDIRECTION,0,0,0);
				window._video.sock.send(hdr.buffer);
				window._video.sock.disableMediaOption();
				window._video.sock.updateKbdLEDBtns();
				window._video.sock.enablePowerCtrlBtn();
				window._video.sock.updateKVMMenuList(pauseMenuList, true);
				$("#pause_video").addClass("disable_a_href");
				$("#refresh_video").addClass("disable_a_href");
				if(window.recordVideo == true)
				{
					//console.log("Stop Recording... as video is paused"+CommonStrings.video_paused_during_record);
					$("#status_body").html(CommonStrings.video_paused_during_record);
					$("#status_dialog").show();
					$("#stop").click();
				}
			},

			resumeVideo: function(ev) {
				window._video.isVideoPaused=false;
				ev.preventDefault();
				var hdr=window._video.sock.createHeader(IVTP.CMD_RESUME_REDIRECTION,0,0,0);
				window._video.sock.send(hdr.buffer);
				window._video.sock.enableMediaOption();
				window._video.sock.updateKbdLEDBtns();
				window._video.sock.enablePowerCtrlBtn();
				window._video.sock.updateKVMMenuList(resumeExceptionList, false);
				$("#pause_video").removeClass("disable_a_href");
				$("#refresh_video").removeClass("disable_a_href");
				// To prevent menus from enabling during partial permission
				if (window.master == false) window._video.sock.updateKVMMenuList(partialPrivilegeMenuList, true);
			},

			refreshVideo: function(ev) {
				ev.preventDefault();
				var hdr=window._video.sock.createHeader(IVTP.CMD_REFRESH_VIDEO_SCREEN,0,0,0);
				window._video.sock.send(hdr.buffer);
			},			  

			powerOn: function(ev) {
				ev.preventDefault();
				var hdr = window._video.sock.createHeader(IVTP.CMD_POWER_CTRL_REQUEST, 0, IVTP.POWER_CONTROL_ON, "");
				window._video.sock.send(hdr.buffer);
			},

			powerReset: function(ev) {
				ev.preventDefault();
				var hdr = window._video.sock.createHeader(IVTP.CMD_POWER_CTRL_REQUEST, 0, IVTP.POWER_CONTROL_HARD_RESET, "");
				window._video.sock.send(hdr.buffer);
			},

			powerCycle: function(ev) {
				ev.preventDefault();
				var hdr = window._video.sock.createHeader(IVTP.CMD_POWER_CTRL_REQUEST, 0, IVTP.POWER_CONTROL_CYCLE, "");
				window._video.sock.send(hdr.buffer);
			},

			powerOffImmediate: function(ev) {
				ev.preventDefault();
				var hdr = window._video.sock.createHeader(IVTP.CMD_POWER_CTRL_REQUEST, 0, IVTP.POWER_CONTROL_OFF_IMMEDIATE, "");
				window._video.sock.send(hdr.buffer);
				window._video.Host_OS_shutdown = true;
			},

			powerOffOrderly: function(ev) {
				ev.preventDefault();
				var hdr = window._video.sock.createHeader(IVTP.CMD_POWER_CTRL_REQUEST, 0, IVTP.POWER_CONTROL_SOFT_RESET, "");
				window._video.sock.send(hdr.buffer);
				window._video.Host_OS_shutdown = true;
			},

			//Power control button event handler.
			onPowerCtrlBtnClick: function(ev){
				ev.preventDefault();
				if(window._video.power_status == 1)//if power status is ON, invoke power OFF
					this.powerOffImmediate(ev);
				else //if power status is OFF, invoke power ON
					this.powerOn(ev);
			},

		        /* This function is used to invoke the user defined macros dialog */
            		userMacroDialog: function(ev) {
                		ev.preventDefault();
	                	$('#err_MaxMacroLimit').hide(MACRO_MSG_DURATION); // hiding error message (if any)
        	        	$("#usr_macros_dialog").show(); // invokes the dialog
            		},

            		/* For closing user defined macros dialog */
		        closeUserMacroDialog: function() {
                		/* Upon closing Send macro list to adviser */
		                var buffer = new ArrayBuffer(SIZE_OF_MACRO);
                		var keycode = new Uint8Array(buffer);
		                var index = 0, bufferIndex = 1;
                		var localMacroMap = getMacroMap();
		                var split_keyevent; // array contains combination of key codes and key locations
		                var data; // data to be write in buffer
		                
				for (var item in localMacroMap) {
                  			if (localMacroMap.hasOwnProperty(item)) {

			                    split_keyevent = localMacroMap[item].split("+");

			                    for (var j = 1; j < split_keyevent.length; j += SINGLE_KEY_SIZE) {
                        			// N+1 value difference between browser key location and standard key location
			                        // Ex: for key location standard -> browser format = 0; standard format = 1;
                        			split_keyevent[j] = parseInt( split_keyevent[j] ) + 1;
                    			    }
			                    
					    split_keyevent.forEach(function(data) {
                        		    	/* If the browser is Firefox then convert to Standard browser keyCode format*/
				                if (isFirefox) {
			                            data = ( keyCodes_Firefox[data] ? keyCodes_Firefox[data] : data );
	                       		    	}
			                    	/* cross map to standard key code format befor sending */
	                        		data =  ( StandardKeyCodeMap[data] ? StandardKeyCodeMap[data] : data );
                        		    	/* write the data to buffer as 4 bytes array values */
                        		    	keycode.set(intToByteArray( parseInt(data) ), index);
                        			index += INT_SIZE;
                    			    });

                    			    // update the index according to single macro length
			                    index = ( bufferIndex * SINGLE_MACRO_LENGTH * SINGLE_KEY_SIZE * INT_SIZE );
			                    bufferIndex++;
                  			}
                		}
                		
				// Forming the user macro packet
                		var header = window._video.sock.createHeader(IVTP.CMD_SET_USER_MACRO, keycode.byteLength, 0, keycode);
                		// Sending the packet to adviser
                		window._video.sock.send(header.buffer);
                		$("#usr_macros_dialog").hide(); // hide the dialog
            		},

            		/* This function is used to invoke the Add Macro dialog */
		        macroDialog: function(ev) {
                		ev.preventDefault();
		                var localMacroMap = getMacroMap();
                		var size = 0; // size of macro map

		                // Calculate the size of current macro map
                		for (var value in localMacroMap) {
		                    if (localMacroMap.hasOwnProperty(value)) {
                		        size++;
                    		    }
                		}

		                // If max limit is reached then don't allow further adding
                		if (size == MACRO_COUNT) {
		                    $('#err_MaxMacroLimit').show(MACRO_MSG_DURATION);
                		    return;
		                }
                
				// proceed otherwise
		                $('#err_MaxMacroLimit').hide(MACRO_MSG_DURATION); // hide the error messages (if any)
                		$("#usr_macros_dialog").hide(); // hide the current dialog
		                $("#macro_dialog").show(); // invoke the add macro dialog
		        },

		        /* For closing Add Macro dialog */
		        closeMacroDialog: function() {
                		var macro_input = document.getElementById("macro_dialog_input");
		                var localMacroMap = getMacroMap();
                		
				// if input dialog contains any values then proceed
		                if (typeof macro_input.value !== 'undefined') {

                			if (macro_input.value.length > 0) {
			                	
						// For handeling duplicate values
                      				if(typeof localMacroMap[macro_input.value] !== 'undefined') {
				                        $('#err_SingleMacroLimit').hide(MACRO_MSG_DURATION);
                        				$('#err_DuplicateMacro').show(MACRO_MSG_DURATION);
                        				return;
                      				}
                      
						// add the entry to macroMap
				                localMacroMap[macro_input.value] = hotkey_code;
                      				setMacroMap(localMacroMap);
                      				constructMacroMenu(macro_input.value);
                  			}
                		}

                		// reset the values
		                macro_input.value = "";
                		hotkey_code = "";

		                // hide unwanted error messages
                		$('#err_SingleMacroLimit').hide(MACRO_MSG_DURATION);
		                $('#err_DuplicateMacro').hide(MACRO_MSG_DURATION);
		                $('#err_MaxMacroLimit').hide(MACRO_MSG_DURATION);
                		$("#macro_dialog").hide(); // hide the current dialog
		                $("#usr_macros_dialog").show(); // invoke the user macro dialog
            		},

            		/* For clearing the value in user macro dialog input box
		        ** Common for both clear and clear all buttons */
            		clearMacroDialog: function(ev) {
                		ev.preventDefault();
                		var macro_input = document.getElementById("macro_dialog_input");
                		/* The button which triggers the event, is identified by using
                 		** event.target.id property */
                		
				switch (ev.target.id) {
                    			// For clear button
			                case "macro_clr": {
                            			// upon pressing, the last value will be removed in the input box
			                        var split_text = macro_input.value.split("+");
                            			var split_Code = hotkey_code.split("+");
                            			var keyText = ""; // contains key text
                            			var codeText = ""; // contains key code ans key locations

                            			/* This will remove the last element in the input box */
                            			if (split_text.length != 0) {
                                			for (var k = 0; k < (split_text.length - 1); k++) {
                                    				keyText = keyText.concat(split_text[k], "+");
                                			}
                                			for (var k = 0; k < (split_Code.length - 2); k++) {
                                    				codeText = codeText.concat(split_Code[k], "+");
                                			}
                            			} else {
                                			codeText = "";
                                			hotkey_code = "";
                                			keyText = "";
                            			}

			                        /* This will remove the '+' after removing the last element (if any)
                            			** in the input box */
                            			if (keyText.length > 1) {
                                			keyText = keyText.substring(0, keyText.lastIndexOf('+'));
                                			hotkey_code = codeText.substring(0, codeText.lastIndexOf('+'));
                            			} else {
                                			hotkey_code = "";
                            			}

                            			macro_input.value = keyText;
                            			break;
                        		}

                        		// For clear all button
                    			case "macro_clrAll":
                    			default:
                        			macro_input.value = "";
                        			hotkey_code = "";
                        			break;
                		}

                		// hide the error messages (if any)
                		$('#err_SingleMacroLimit').hide(MACRO_MSG_DURATION);
                		$('#err_DuplicateMacro').hide(MACRO_MSG_DURATION);
            		},

            		/* For buttons in add macro dialog. Upon clicking, key text for the appropriate
            		** button will be displayed in the input dialog box */
            		displayKeyText: function(ev) {
                		ev.preventDefault();

                		/* The button which triggers the event, is identified by using
                 		** event.target.id property */
                		switch (ev.target.id) {
                    			
					// For windows button
                    			case "macro_win":
                        			displayHotkey(VK_WINDOWS, macroKBD.KEY_LOCATION_LEFT);
                        			break;

                        		// For Alt+F4 button
                    			case "macro_altf4":
                        			displayHotkey(VK_ALT, macroKBD.KEY_LOCATION_LEFT);
                        			displayHotkey(VK_F4, macroKBD.KEY_LOCATION_STANDARD);
                        			break;

                        		// For print Screen button
                    			case "macro_printscr":
		                        displayHotkey(VK_PRINTSCREEN, macroKBD.KEY_LOCATION_STANDARD);
                		        break;
                    
					default:
		                        break;
                		}
            		},

			//Send Host LED status request packet to the adviser.
			requestHostLEDStatus: function() {
				ev.preventDefault();
				var hdr=window._video.sock.createHeader(IVTP.CMD_GET_KBD_LED_STATUS,0,0,0);
				window._video.sock.send(hdr.buffer);
			},

			/* This function is used to send the keyboard layout language packet to adviser.
			** If invoked, on next startup preferred keyboard layout language will be selected in menu */
			sendKeyBoardLang: function(){
				/* syntax: createHeader(command, data length, status, data); */
				var header = window._video.sock.createHeader(IVTP.CMD_SET_KBD_LANG, window.keyboard_language.length, 0, window.keyboard_language);
				window._video.sock.send(header.buffer);
				$("#kbd_layout_dialog").trigger("closeDialog"); // Closing keyboard layout language dialog
			},

			/* For closing keyboard layout language dialog */
			closeKbdLayoutDialog: function() {
				clearInterval(kbddialogticker); // To stop the recursive function call in keyboardLayoutCheck()
				$("#kbd_time_left").html(KBD_LAYOUT_DIALOG_COUNTDOWN); // Reset the countdown
				$("#kbd_layout_dialog").hide(); // hide the dialog
			},

			/* This method prevents navigation to dashboard upon clicking disabled menu item in keyboard language Layout menu */
			keyboardLayoutDisabledCheck: function(ev) {
				ev.preventDefault();
			},

			//block request menu 
			blockPrivilegeRequest: function(ev) {
				ev.preventDefault();
				if($("#block_priv_request").children("i").length == 0) {
					$("#block_priv_request").append("<i class=\"fa fa-check\"></i>");
					KVM_SHARING.KVM_BLOCK_USER_REQUEST = true;
		    		}
    				else {
					$("#block_priv_request").children("i").remove();
					KVM_SHARING.KVM_BLOCK_USER_REQUEST = false;
				}
			},

			handleKvmPermissionRequest: function() {
				window.master = true;
				$("#master_dialog_body").html(window._video.sock.other_session_name + CommonStrings.user_with_ip + window._video.sock.other_session_ip + CommonStrings.is_trying_to_connect);
				$("#no_permission").show();
				this.slave_info = window.atob(sessionStorage.getItem("other_session_info"));
				$("#master_dialog").show();
				var _parent = this;
				this.ticker = setInterval(function() {
					var secs = parseInt($("#master_time_left").html());
					if (secs > 0) {
						$("#master_time_left").html(secs - 1);
					}
					else {
						$("#master_dialog").trigger("resetDialog");
					}
				},DIALOG_TIMER);
			},

			handleMasterRequest: function() {
				$("#master_dialog_body").html(window._video.sock.other_session_name + CommonStrings.user_with_ip + window._video.sock.other_session_ip + CommonStrings.is_requesting_full_access);
				$("#no_permission").hide();
				this.slave_info = window.atob(sessionStorage.getItem("other_session_info"));
				$("#master_dialog").show();
				var _parent = this;
				this.ticker = setInterval(function() {
					var secs = parseInt($("#master_time_left").html());
					if (secs > 0) {
						$("#master_time_left").html(secs - 1);
					}
					else {
						$("#master_dialog").trigger("resetDialog");
					}
				},DIALOG_TIMER);
			},

			resetMasterDialog: function() {
				clearInterval(this.ticker);
				$("#master_time_left").html(30);
				$("#master_dialog").hide();
			},

			cancelMasterRequest: function() {
				$("#master_dialog").trigger("resetDialog");
				var hdr = window._video.sock.createHeader(0x20, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_DENIED << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
			},

			grantFullPermission: function() {
				window.master = false;
				var hdr = window._video.sock.createHeader(0x20, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_ALLOWED << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
				hdr = window._video.sock.createHeader(0x32, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_ALLOWED << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
				if(window.cdrom) {
					$("#media").trigger("click");
				}
				$("#master_dialog").trigger("resetDialog");
			},

			grantPartPermission: function() {
				var hdr = window._video.sock.createHeader(0x20, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_PARTIAL << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
				$("#master_dialog").trigger("resetDialog");
			},

			grantNoPermission: function() {
				var hdr = window._video.sock.createHeader(0x20, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_DENIED << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
				$("#master_dialog").trigger("resetDialog");
			},

			//kvmsharing dialog button action
			blockIncomingPrivRequest: function() {
				//console.log("Block requests is Selected");
				if($("#block_priv_request").children("i").length == 0) {
					$("#block_priv_request").append("<i class=\"fa fa-check\"></i>");
		    		}
				//check block request menu checkbox
				KVM_SHARING.KVM_BLOCK_USER_REQUEST = true;
				var hdr = window._video.sock.createHeader(0x20, this.slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_BLOCKED << 8), this.slave_info);
				window._video.sock.send(hdr.buffer);
				$("#master_dialog").trigger("resetDialog");
				this.enablePartialMenuState();
			},
			
			// Keyboard/mouse encryption  handler
			keyboardMouseEncryption : function(ev){
				ev.preventDefault();
				console.log("keyboardMouseEncryption");		
				
				if($("#kbd_mouse_encryption").children("i").length == 0) {
					$("#kbd_mouse_encryption").append("<i class=\"fa fa-check\"></i>");
					window.encryption=true;
					var hdr=window._video.sock.createHeader(IVTP.CMD_ENABLE_ENCRYPTION,0,0,0);
					window._video.sock.send(hdr.buffer);
		    	}
				else{
						window.encryption=false;
						window.DISABLE_ENCRYPT_FLAG=true;
						$("#kbd_mouse_encryption").children("i").remove();
						var hdr=window._video.sock.createHeader(IVTP.CMD_DISABLE_ENCRYPTION,0,0,0);
						window._video.sock.send(hdr.buffer);	

				}			
			},

			handleSlaveRequest: function() {
				$("#slave_dialog_body").html(CommonStrings.requesting_permission + window._video.sock.other_session_name + CommonStrings.user_with_ip + window._video.sock.other_session_ip);
				$("#slave_dialog").show();
				this.slave_info = window.atob(sessionStorage.getItem("other_session_info"));

				this.ticker = setInterval(function() {
					var secs = parseInt($("#slave_time_left").html());
					if (secs > 0) {
						$("#slave_time_left").html(secs - 1);
					}
					else {
						$("#slave_dialog").trigger("resetDialog");
					}
				},DIALOG_TIMER);
			},

			resetSlaveDialog: function() {
				clearInterval(this.ticker);
				$("#slave_time_left").html(30);
				$("#slave_dialog").hide();
			},

			cancelSlaveRequest: function() {
				$("#slave_dialog").trigger("resetDialog");
				$("#toggle").trigger("click");
			},

			closeStatus: function() {
				$("#status_dialog").hide();
			},

			closeMasterSwitch: function() {
				clearInterval(this.ticker);
				$("#switch_master_time_left").html(SWITCH_MASTER_DIALOG_COUNTDOWN); // Reset the countdown
				$("#switch_master_dialog").hide();
				$("#toggle").trigger("close");
			},

			closeVideoRecSetting: function() {
				$("#video_rec_setting_dialog").hide();
			},

			requestFullAccess: function() {
				hdr = window._video.sock.createHeader(0x32, 0, 0, 0);
				window._video.sock.send(hdr.buffer);
			},

			//update media status to video server
			updateMediaRedirStatus: function(state) {
				//Media redirection state IVTP cmd 0x18 
				hdr = window._video.sock.createHeader(0x18, 0, state, 0);
				window._video.sock.send(hdr.buffer);
			},

			showStatus: function() {
				var status = sessionStorage.getItem("status");

				var user = window._video.sock.other_session_name;
				var ip = window._video.sock.other_session_ip;

				if(status == "stop_session") {
					window.master = true;

					//reset all the menu and buttons to default
					this.UpdateKVMStateOnStop();
					var reason = sessionStorage.getItem("stop_reason");
					if(reason == "kvm_license_exp")
						$("#status_body").html(CommonStrings.kvm_license_exp);
					else if(reason == "kvm_timed_out")
						$("#status_body").html(CommonStrings.kvm_timed_out);
					else if(reason == "kvm_terminated")
						$("#status_body").html(CommonStrings.kvm_terminated);
					else if(reason == "kvm_web_restart")
						$("#status_body").html(CommonStrings.kvm_web_restart);
					else if(reason == "kvm_web_logout")
						$("#status_body").html(CommonStrings.kvm_web_logout);
					else if(reason == "kvm_conf_change")
						$("#status_body").html(CommonStrings.kvm_conf_change);
					else if(reason == "kvm_server_restart")
						$("#status_body").html(CommonStrings.kvm_server_restart);
					else if(reason == "kvm_max_session_reached")
						$("#status_body").html(CommonStrings.kvm_max_session_reached);
					else if(reason == "same_kvm_client_user")
						$("#status_body").html(CommonStrings.same_kvm_client_user);
					// Adding backdrop to the modal to prevent user from clicking Start KVM button when modal is active
					/*$("#status_dialog").modal({
						backdrop: "static"
					});*/
					$('#toggle').hide(); // backdrop is not displaying properly, so hiding the start KVM button
					$("#status_dialog").show();
					$("#status").html("");
					/* One time event listener for close button in Status Dialog.
					** This will close the KVM window after closing the modal dialog */
					$( "#close_status" ).one( "click", function(ev) {
						ev.preventDefault();
						//$('.modal-backdrop').remove(); // Removing the backdrop before closing the window
						window.close(); // Closing KVM window.
					});
				}
				else if(status == "slave_permission_full") {
					//console.log("full_permission ****** slave_permission_full");
					if(window.master != true)
					{
						window.master = true;
						if(((user != "") &&(ip != "")) && ((user != null) &&(ip != null)))
						{
							$("#status_body").html(CommonStrings.full_access_granted_by + user + CommonStrings.user_with_ip + ip);
							$("#status_dialog").show();
							$("#status").html("");
						}
					}
					$("#request_full").hide();
					window._video.sock.enableMediaOption();
					window._video.sock.updateKVMMenuList(resumeExceptionList, false);
				}
				else if(status == "slave_permission_part") {
					//console.log("ONLY VIDEO !!!! ");
					window.master = false;
					$("#status_body").html(CommonStrings.partial_access_granted_by + user + CommonStrings.user_with_ip + ip);
					$("#status_dialog").show();
					$("#status").html(CommonStrings.partial_status_line);
					$("#request_full").show();
					//$("#stop").addClass("disable_a_href");
					window._video.sock.disableMediaOption();
					window._video.sock.updateKVMMenuList(partialPrivilegeMenuList, true);
				}
				else if(status == "slave_permission_none") {
					$("#status_body").html(CommonStrings.slave_none_status_body);
					$("#status_dialog").show();
					$("#toggle").trigger("click");
				}
				else if(status == "slave_quit") {
					$("#close_master").trigger("click");
					$("#status_body").html(CommonStrings.slave_quit);
					$("#status_dialog").show();
				}
				else if(status == "slave_permission_timeout") {
					window.master = true;
					$("#status_body").html(CommonStrings.full_access_received_timeout + user + CommonStrings.user_with_ip + ip);
					$("#status_dialog").show();
					$("#status").html("");
					$("#request_full").hide();
					window._video.sock.enableMediaOption();
					window._video.sock.updateKVMMenuList(resumeExceptionList, false);
				}
				else if(status == "master_blocked_request") {
					window.master = false;
					$("#status_body").html(CommonStrings.master_blocked_request+CommonStrings.partial_access_granted_by + user + CommonStrings.user_with_ip + ip);
					$("#status_dialog").show();
					$("#status").html(CommonStrings.partial_status_line);
					$("#request_full").show();
					window._video.sock.disableMediaOption();
					window._video.sock.updateKVMMenuList(partialPrivilegeMenuList, true);
				}
				else if(status == "master_timeout") {
					window.master = false;
					$("#close_master").trigger("click");
					$("#status_body").html(CommonStrings.granted_full_access_timeout + user + CommonStrings.user_with_ip + ip + CommonStrings.due_to_request_timeout);
					$("#status_dialog").show();
					$("#status").html(CommonStrings.partial_status_line);
					$("#request_full").show();
					window._video.sock.disableMediaOption();
					window._video.sock.updateKVMMenuList(partialPrivilegeMenuList, true);
				}
				else if(status == "switch_master") {
					window.master = false;
					$("#close_master").trigger("click");
					$("#status_body").html(CommonStrings.provided_full_access_to + user + CommonStrings.user_with_ip + ip);
					$("#status_dialog").show();
					$("#status").html(CommonStrings.partial_status_line);
					$("#request_full").show();
					window._video.sock.disableMediaOption();
					window._video.sock.updateKVMMenuList(partialPrivilegeMenuList, true);
				}
				else if(status == "master_reconn") {
					$("#status_body").html(CommonStrings.master_reconn);
					$("#status_dialog").show();
					$("#request_full").show();
				}
				else if(status == "invalid_session") {
					window.master = true;
					var reason = sessionStorage.getItem("invalid_sess_reason");
					if(reason == "kvm_inv_sess")
						$("#status_body").html(CommonStrings.kvm_inv_sess);
					else if(reason == "kvm_disabled")
						$("#status_body").html(CommonStrings.kvm_disabled);
					else if(reason == "kvm_inv_sess_info")
						$("#status_body").html(CommonStrings.kvm_inv_sess_info);
					$("#status_dialog").show();
					$("#status").html("");
				}
				else if(status == "encryption_enable" ) {
					$("#status_body").html(CommonStrings.enable_encryption_status);
					$("#status_dialog").show();
					
				}
				else if(status == "disable_encryption_error_message" ) {
					$("#status_body").html(CommonStrings.disable_encryption_status);
					$("#status_dialog").show();
				}
				window._video.sock.updateKbdLEDBtns();//update the LED status buttons
				window._video.sock.enablePowerCtrlBtn();//update the power control button

			},

			prepareSwitchMaster: function() {
				var client_id
				/*if(window._video.sock.num_clients == 2) {
					if(window._video.sock.client_info[0].id != window._video.sock.current_session_id) {
						client_id = 0;
					}
					else {
						client_id = 1;
					}

					var name = window._video.sock.client_info[client_id].name;
					var ip = window._video.sock.client_info[client_id].ip;
					var id = window._video.sock.client_info[client_id].id;

					var client = new Uint8Array(name.byteLength + ip.byteLength + 1);
					client.set(name);
					client.set(ip, name.byteLength);
					client[name.byteLength + ip.byteLength] = id;
					var hdr = window._video.sock.createHeader(0x32, client.length, KVM_SHARING.KVM_REQ_ALLOWED << 8, client);
					window._video.sock.send(hdr.buffer);
				}
				else {*/
					var options = "";

					for(i = 0; i < window._video.sock.num_clients; i++)
					{
						client_id = window._video.sock.client_info[i].id; 
						if(client_id != window._video.sock.current_session_id) {
							var name_str = String.fromCharCode.apply(null, window._video.sock.client_info[i].name);
							var ip_str = String.fromCharCode.apply(null, window._video.sock.client_info[i].ip);
							options += "<input type=\"radio\" name=\"client\" value=\"" + client_id.toString() + "\">" + name_str + " (" + ip_str + ")</input><br>";
						}
					}
					$("#switch_master_time_left").html(SWITCH_MASTER_DIALOG_COUNTDOWN); // set the countdown time
					$("#switch_master_options").html(options);
					$("#switch_master_dialog").show();

					// Function to be callled recursivley for countdown
					this.ticker = setInterval(function() {
						var secs = parseInt($("#switch_master_time_left").html());
						if (secs > 0) {
							// starts the countdown
							$("#switch_master_time_left").html(secs - 1);
						}
						else {
							// if timer expires close the dialog
							$("#switch_master_dialog").trigger("closeDialog");
						}
					},DIALOG_TIMER);
				//}
			},

			handleSwitchMaster: function() {
				var client_id = $('input[name="client"]:checked').val();
				$("#switch_master_dialog").hide();
				$("#switch_master_options").html("");

				var name = window._video.sock.client_info[client_id].name;
				var ip = window._video.sock.client_info[client_id].ip;
				var id = window._video.sock.client_info[client_id].id;

				var client = new Uint8Array(name.byteLength + ip.byteLength + 1);
				client.set(name);
				client.set(ip, name.byteLength);
				client[name.byteLength + ip.byteLength] = id;
				var hdr = window._video.sock.createHeader(0x32, client.length, KVM_SHARING.KVM_REQ_ALLOWED << 8, client);
				window._video.sock.send(hdr.buffer);

				$("#switch_master_dialog").hide();
				$("#toggle").trigger("close");
			},

			beforeRender: function() {

			},

			afterRender: function() {
				//console.log(Video);
				//console.log(CDROM);
				this.$el.find("#kbd").hide();
				if(this.oemAuthHandled == false)
				{
					this.adviserModel.fetch({
						success: function(model) {
							//console.log(model);
							var kvm_service = model.get("status");
						     // console.log("test kvm_service"+kvm_service);
							  
							  if(kvm_service==0){
								$("#toggle").attr("disabled","disabled");
								$(".alert-danger").removeClass("hide");
								$(".alert-danger").html(CommonStrings.kvm_service_disabled);
							}
						}
					});

				if(!app.isKVM() ){
					$("#toggle").attr("disabled","disabled");
					$(".alert-danger").removeClass("hide");
						$(".alert-danger").html(CommonStrings.kvm_disabled);
					}
				}else{
					oem_app.handleOEMAfterRender();
				}
			},

			showFile: function(ev) {
				if($("#media").html() == CommonStrings.start_media_redirection) {
					//remove the file attribute value inorder to choose same file more than once.
					$("#files").val("");
					$("#files:hidden").trigger('click');
				}
			},

			
			
			
			startKVM: function(ev) {
				window.mediaLicenseStatus = false;
				window.DISABLE_ENCRYPT_FLAG = false;
				window.keyboard_language = null;
				var kvm_el = $(ev.currentTarget);
				var _view = this;
				var webPort = location.port;//Get the port number from the current URL.
				if (!$(kvm_el).data("started")) {
					if(this.oemAuthHandled == false) {
					/* If VMedia privilege is enabled then proceed */
					if( app.isVMedia() ){
						//remove disable attribute to enable media start button
						$("#media").removeAttr("disabled");
					}
					_view.model.fetch({
						global : false ,
						success: function(model) {
							sessionStorage.setItem("token", JSON.stringify(model.toJSON()));
							//window.KVM_TOKEN = model.get('token');
							//Why? Because, document.cookie will not be set for SESSION_COOKIE when customer prefers HTTP-only mode. 
							//So get the session during KVM token call for now. Ideally, this data must be removed - Chandru
							//window.SESSION_COOKIE = model.get('session');
							//console.log(window.KVM_TOKEN, window.SESSION_COOKIE);
							
							

							//The port number could be null, empty or 0, if the port number is not mentioned in case of http or https.
							//In that case we use the default http port number:80, if protocol if http, and the default
							//https port number:443, if the protocol number is https. 
							if(!webPort || webPort == "0"){
								if("http:" == location.protocol){
									webPort = "80";//default http port number.
								}
								else if("https:" == location.protocol){
									webPort = "443";//default https port number.
								}
							}
							if(app.configurations.isFeatureAvailable("WEB_APP_PROXY")) {
								//Add the web port number to the hostname, to avoid connection issues when the web port numberis reconfigured.
								window._video = new Video("kvm", location.protocol + "//" + location.hostname+":"+webPort);
							}
							else {
								window._video = new Video("kvm", location.protocol + "//" + location.hostname + ":9666");
							}
							window._video.sock.showMenu(kvmMenus, true);
							localStorage.isActiveKVM = true; // To avoid closing window when video is running.
							$(kvm_el).data("started", 1);
							window._video.sock.showKbdLEDBtns(true);
							window._video.sock.showPowerCtrlBtn(true);
							window._video.sock.onclose = function() {
								
								if(window._video.isVideoPaused!=true)
								{
									$(kvm_el).html(CommonStrings.start_kvm);
									$(kvm_el).data("started", 0);
									$("#kvm").css("visibility", "hidden");
									_view.model.clear();
									$("#kvm").removeClass("alive");
									//$("#kvm").hide();
									// close the media redirection if its running and disable the button
									if(window.cdrom) {
										$("#media").click();
									}
									$("#media").attr("disabled", "disabled");
								}
							};

							RemoteSessionModel.fetch({
								global : false ,
								success: function(model) {
									var data = model.toJSON();
									window._video.lm_option = data.automatic_off && data.local_monitor_off;

									//console.log("Detected Keyboard layout: " + data.keyboard_language);
									updateKeyBoardLang(data.keyboard_language); // updating checkbox in the dropdown list
									MEDIA_CONFIGS.host_lock_status = data.automatic_off && data.local_monitor_off;
								}
							});
							MediaSessionModel.fetch({
								global : false ,
								success: function(model) {

									window.mediaconf = model.toJSON();
									window._video.sock.syncMediaConf();
									//Dont enable media related stuff's here
									// on receiving KVM privilege we will take care of enabling if required.
									$("#media_redirection").hide();
								}
							});
						}
					});
					} else{
						oem_app.handleOEMStartKVM(ev, app);
					}
					window.CommonStrings = CommonStrings; // For accessing common strings else where in H5Viewer
					
					$(document).click(function() {
						
						// if record setting dialog is open, change the focus to video length box 
						if( $("#video_rec_setting_dialog").css('display') == 'block') {
							$("#video_rec_length_box").focus();
						// if add macro dialog is open, change the focus to macro dialog input box
                        				} else if ($("#macro_dialog").css('display') == 'block') {
                        					$("#macro_dialog_input").focus();
		                   			var macro_input = document.getElementById("macro_dialog_input");
			                		
							// capture the key events
							macro_input.onkeydown = function(event) {
								event.stopPropagation();
								event.preventDefault();
								// For compatibility with all the browsers,
								// support for both 'keyCode' and 'which' properties added
								var keycode = (event.keyCode ? event.keyCode : event.which);
							
								if (isFirefox) {
									keycode = ( keyCodes_Firefox[keycode] ? keyCodes_Firefox[keycode] : keycode );
								}
								displayHotkey(keycode, event.location);
							}
						}
						else
						{
							$("#kvm_textbox").focus();
							// NOTE: This code will be executed only incase of ALT+TAB in H5Viewer
							// check if alt key is pressed and  not released yet.
							// if so then send alt release event on gaining focus.
							if(isAltKeyPressed == true)
							{
								// release the alt key
								usbkbdmsg.set(18, usbkbd.KEY_LOCATION_LEFT, false);
								var rpt = usbkbdmsg.report();
								window._video.sock.send(rpt.buffer);
								delete rpt;
								rpt = null;
							}
						}
					});
					$(kvm_el).html(CommonStrings.stop_kvm);
					$("#css").show();
					$("#kvm").addClass("alive");
					$("#kvm").show();
					$("#kvm").css("visibility", "visible");
					
					
				} else {
					if(window.master && window._video.sock.num_clients > 1) {
						//console.log("switching master");
						$("#switch_master_dialog").trigger("prepareSwitchMaster");
					}
					else {
						$("#toggle").trigger("close");
					}					
				}
				
			},

			UpdateKVMStateOnStop: function(){

				window._video.sock.showMenu(kvmMenus, false);
				window._video.sock.showKbdLEDBtns(false);
				window._video.sock.showPowerCtrlBtn(false);
				// check if video recording is in progress, if so then stop the recording.
				if(window.recordVideo == true)
				{
					$("#stop").click();
				}
				$("#toggle").html(CommonStrings.start_kvm);
				$("#toggle").data("started", 0);
				//TODO: call paintWhiteScreen() in _video instead
				$("#kvm").css("visibility", "hidden");
				if(this.oemAuthHandled == false) {
					this.model.clear();
				}
				
				$("#kvm").removeClass("alive");
				//$("#kvm").hide();
				$("#slave_dialog").trigger("resetDialog");
				$("#status").html("");
				$("#request_full").hide();

				$("#media_redirection").hide();

				//disable button
				$("#media").attr("disabled", "disabled");

				$("#kvm").css({"cursor": "auto"});
			},

			stopKVM: function(ev) {

				if(window.cdrom) {
					if (confirm(CommonStrings.media_redirection_running) == true) {
						$("#media").click();
					} else {
						return;
					}
				}
				this.UpdateKVMStateOnStop();
				window._video.stop();
				/* If not deleted, then KVM window won't be closed properly next time */
				localStorage.removeItem('isActiveKVM');
				window.close(); // Closing KVM window
				if(window.opener != null){    //enable parent window lanuch button
                    window.opener.$("#download").removeAttr("disabled");
				}
			},

			recordVideo: function(ev) {
				ev.preventDefault();
				// proceed only if video is running
				if(window._video.running == true){
					// check session status 
					if(window.master == false){
						//if it is slave session, append the string to partial string status
						$("#record_progress").append("<span>"+CommonStrings.video_record_status+"</span>");
					}
					else{
						//if it is master session, show the record status  
						$("#record_progress").html("<span>"+CommonStrings.video_record_status+"</span>");
					}
						
					$("#record_progress").show();
					window._video.startRecording();
					if(window.recordVideo) {
						
						
					$("#record").addClass("disable_a_href");
					$("#stop").removeClass("disable_a_href");
					//disable setting option if record started
					$("#video_rec_setting").addClass("disable_a_href");	 
					}
				}
			},

			stopRecording: function(ev) {
				ev.preventDefault();

				$("#stop").addClass("disable_a_href");
				$("#record").removeClass("disable_a_href");
				//enable setting option if record stopped
				$("#video_rec_setting").removeClass("disable_a_href");
				window._video.stopRecording();
				// remove the progress status
				$("#record_progress").html("");
				$("#record_progress").hide();
			},

			videoRecordSettings: function(ev) {
				ev.preventDefault();
				var element = document.getElementById('video_rec_length_box');
				element.value = window._video.videoRecDuration;
				$("#video_rec_setting_dialog").show();
				$("#video_rec_length_box").focus();
				//show pop-up to user to configure record duration
			},

			videoRecSettingsok: function(ev){
				ev.stopPropagation();

				var element = document.getElementById('video_rec_length_box'),
				value = element.value;

				console.log("videoRecSettingsok  recDuration: "+value);
				if((value > 0) && (value <= 1800) && (value.indexOf('.') < 0)){
					window._video.videoRecDuration = value;
					$("#video_rec_setting_dialog").hide();
				}
				else{
					alert(CommonStrings.record_duration_invalid);
				}
			},

			startMedia: function(ev) {
				var port = 9999;
				var useSSL = false;
				var token;
				ev.stopPropagation();
				ev.preventDefault();
				if (window.File && window.FileReader && window.FileList && window.Blob) {
					// Great success! All the File APIs are supported.
				} else {
					alert(CommonStrings.alert_apinotsupported);
				}

				if (!window.cdrom) {
					if (!window.file) {
						alert(CommonStrings.select_file);
						return;
					}

					var features = JSON.parse(sessionStorage.getItem("features"));
					var i;
					if (features != null)
					{
						for(i = 0; i < features.length; i++) {
							if(features[i].feature == "WEB_APP_PROXY") {
								port = -1;
								break;
							}
						}
					}

					if(location.protocol == "https:") {
						useSSL = true;
					}

					window.cdrom = new CDROM();
					if(this.oemAuthHandled == false) {
						token = this.model.get("token");
						if (token == null) {
							this.model.fetch({global : false, success: function(model) {
								cdrom.startRedirection(location.host, 'file', token, port, useSSL);
							}});
						} else {
							cdrom.startRedirection(location.host, 'file', token, port, useSSL);
						}
					}
					else
					{
						var oem_token = JSON.parse(sessionStorage.getItem("token"));
						if (oem_token) {
							port = -1;
							token = oem_token.token;
							cdrom.startRedirection(location.host, 'file', token, port, useSSL);
						}
					}

					
					this.updateMediaRedirStatus(1);// redirection started
					$("#media").html(CommonStrings.stop_media_redirection);
					$("#files").hide();
					window.cdrom.sock.onclose = function() {
						window.cdrom = null;
						//clear file name
						window.file = null;
						//this.updateMediaRedirStatus(0);// redirection stopped
						var hdr = window._video.sock.createHeader(0x18, 0, 0, 0);
						window._video.sock.send(hdr.buffer);
						$("#media").html(CommonStrings.start_media_redirection);
						$("#browse").html("Browse File");
						if(window.packetsSent) {
							window.packetsSent = 0;
							$("#sentcount").html("0");
						}
					};
				} else {
					window.cdrom.sock.close();
					window.cdrom = null;
					//clear file name
					window.file = null;
					this.updateMediaRedirStatus(0);// redirection stopped
					$("#media").html(CommonStrings.start_media_redirection);
					$("#browse").html("Browse File");
					if(window.packetsSent) {
						window.packetsSent = 0;
						$("#sentcount").html("0");
					}
				}
			},

			chooseISO: function(ev) {
				var files = ev.target.files;

				if (files.length > 1) {
					alert(CommonStrings.iso_image_redirection);
					return;
				}

				window.file = files[0];

				//console.log(files);

				if (!file) {
					//console.log("File selection not valid");
					return;
				}
			   //allow expected format extensions only
				var fileExtension;
				if(files[0].name) {
					fileExtension=files[0].name.split(".").pop(); 
					if( (fileExtension != "iso") &&
						(fileExtension != "nrg") &&
						(fileExtension != "ISO") &&
						(fileExtension != "NRG")){
						alert(CommonStrings.invalid_image_type +" "+fileExtension);
						return;
					}
				}

				//console.log("size", file.size);
				$("#browse").html(files[0].name);
				

				window.reader = new FileReader();

				window.reader.onloadend = function(e) {
					if (e.target.readyState == FileReader.DONE) {
						//console.log(new Uint8Array(e.target.result));
						$("#log")[0].innerHTML += ("<br/>"+ CommonStrings.read_bytes+" " + (new Date()));
					}
				};

				var blob = file.slice(0, 500);
				window.reader.readAsArrayBuffer(blob);
			},

			animateVideo: function() {},

			load: function(model, collection, xhr) {

			},

			serialize: function() {
				return {
					locale: CommonStrings
				};
			}

		});

		return view;

	});

	
	
