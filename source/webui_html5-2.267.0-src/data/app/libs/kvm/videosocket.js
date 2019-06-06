    /* H5Viewer app version
    ** Note: this version should be updated according to the H5viewer_common package version */
    APP_VERSION = "2.118.0";

    var KVM_SHARING = {
        STATUS_KVM_PRIV_REQ_CANCEL: 0x00, // reserved. not used for now.
        STATUS_KVM_PRIV_REQ_MASTER: 0x01, // to user 1
        STATUS_KVM_PRIV_WAIT_SLAVE: 0x02, // to user 2
        STATUS_KVM_PRIV_REQ_TIMEOUT_TO_MASTER: 0x03, // to user 1
        STATUS_KVM_PRIV_RESPONSE_TO_SLAVE: 0x04, // to user 2
        STATUS_KVM_PRIV_RES_USER1: 0x05, // from user 1
        STATUS_KVM_PRIV_SWITCH_MASTER: 0x06, // To user 1
        KVM_REQ_ALLOWED: 0,
        KVM_REQ_DENIED: 1,
        KVM_REQ_PARTIAL: 2,
        KVM_REQ_TIMEOUT: 3,//by default gra
        KVM_NOT_MASTER: 4, 
        KVM_REQ_PROGRESS: 5,
        KVM_REQ_BLOCKED : 6,
        KVM_REQ_MASTER_RECONN: 7, //Master reconnect

	KVM_BLOCK_USER_REQUEST: false
    };

    MEDIA_CONFIGS = {
        reconn_retry_count : 3,
        reconn_retry_interval : 15, 
        mouse_mode : -1,
        host_lock_status : 1,		//enabled
        attach_cd : 0, 
        attach_fd : 0, 
        attach_hd : 0, 
        enable_boot_once : 0, 
        num_cd : 1,
        num_fd : 1,
        num_hd : 1,
        lmedia_enable : 1,
        rmedia_enable : 1, 
        sdserver_enable : 1,
        powerSaveMode  : 0,
        kvm_cd : 1, 
        kvm_fd : 1, 
        kvm_hd : 1

    };

    COLOR = {
        white : "btn-default",
        green : "btn-success",
        red : "btn-danger",
        darkblue : "btn-primary",
        blue : "btn-info",
        yellow : "btn-warning"
    };

	// list of menu items available in h5viewer. 
	// NOTE: THIS MENU LIST SHOULD BE UPDATED WHEN THERE IS ANY CHANGE IN H5VIEWER MENU LIST
	kvmMenuItems = new Array(/*Video Menu*/"#pause_video", "#resume_video", "#refresh_video", "#hd_turn_on", "#hd_turn_off", "#capture_screen",
						/*Mouse Menu*/"#show_cli_cursor", "#absolute_mm", "#relative_mm", "#other_mm", 
						/*Options Menu*/ "#block_priv_request", "#kbd_mouse_encryption",
						/*Keyboard Menu*/ "#kbd_layout_eng", "#kbd_layout_ger",
						/*Send Keys*/"#r_ctrl", "#r_alt", "#r_win_hold", "#l_ctrl", "#l_alt", "#l_win_hold", "#cad", "#l_win_press", "#r_win_press", "#context", "#prtscn",
						/*HotKeys*/"#button_HotKeys",
						/*Video Record*/"#record", "#stop", "#video_rec_setting",
						/*Power Menu*/ "#power_reset", "#power_off_immediate", "#power_off_orderly", "#power_on", "#power_cycle");
	// list of menu's to be enabled on pause 
	pauseMenuList = new Array("#resume_video", "#capture_screen");
	// list of menu's to be enabled on partial privilege
	partialPrivilegeMenuList = new Array("#pause_video", "#refresh_video", "#capture_screen","#record", "#video_rec_setting");
	// list of menu's to be enabled on power off 
	powerOffMenuList = new Array("#capture_screen","#record", "#video_rec_setting", "#power_on");

	// list of exception menu's to be left out on resume
	resumeExceptionList = new Array ( "#resume_video", "#hd_turn_on","#hd_turn_off","#power_on", "#stop");

	HOST_LOCK_STATUS = {
		HOST_DISPLAY_UNLOCK : 0x00,
		HOST_DISPLAY_LOCK : 0x01,
		HOST_DISPLAY_UNLOCKED_AND_DISABLED : 0x02,
		HOST_DISPLAY_LOCKED_AND_DISABLED : 0x03
	};

	KBD_LED = {
		NUM_LOCK : 0x01,
		CAPS_LOCK : 0x02,
		SCR_LOCK : 0x04,
	};

    IPMI_PRIV = [[0,"RESERVED"],[1,"CALLBACK"],[2,"UR"],[3,"OP"],[4,"AD"],[5,"PR"]];
    MouseMenu_ID = [[0,"NONE"],[1,"#relative_mm"],[2,"#absolute_mm"],[3,"#other_mm"]];
    SERVER_CONF=[];

    NEW_LINE_TAG = "<BR>"; // line break tag for help contents

    var VideoSocket = function(ip, _video) {

        var _parent = this;
        this.ws_proto;
        this.oem_app = null;
        this.keep_alive_timer = null;
        this.powerStatusTimer = null;
        this.oem_app = new OEM_APP();
        delete this.nwBuffer;
        this.nwBuffer = null;
        this.nwBuffer = new DataStream();
        this.server = ip;
        this.pos = 0;
        this.video = _video;
        delete this.newBuffer;
        this.newBuffer = null;
        
        this.firstPacket = true;
        this.isVideoPaused=false;
        this.isHostDisplayOn=true;
        this.Host_OS_shutdown = false;
        this.fps_timer = null;
        if(ip.indexOf("https://") != -1) {
            this.ws_proto = "wss://";
        }
        else {
            this.ws_proto = "ws://";
        }
		this.bmc_ip = ip;
        if(ip.indexOf("://") != -1) {
            this.bmc_ip = ip.substr(ip.indexOf("://") + 3, ip.length);
        }

	//Create new web socket
	this.createWebSocket();

        this.video.blank_count = 0;
        window.master = false;
        this.video.stopped = false;
        window.mousemode = -1 * 1; //Invalid Mouse mode

        this.video.blank_count = 0;
        this.other_session_name ="";
        this.other_session_ip = "";

        this.video.host_led_status = 0x00;
        this.video.kbdLEDBtns = new Array("#host_lock_btn","#l_win_hold_btn", "#r_win_hold_btn", "#num_lock","#caps_lock","#scr_lock","#l_alt_btn","#r_alt_btn","#l_ctrl_btn","#r_ctrl_btn");


        this.ws.onopen = function(e) {
            //console.log("WebSocket connection open", this.protocol);
            if (this.protocol) {
                _parent.wsmode = this.protocol;
            } else {
                _parent.wsmode = "base64";
            }
            _parent.onopen && _parent.onopen(e);
        };
        this.ws.onmessage = function(e) {

            _parent.pauseRead = true;

            if (_parent.pos == _parent.nwBuffer.byteLength) {
                delete _parent.nwBuffer;
                _parent.nwBuffer = null;
                _parent.nwBuffer = new DataStream();
                _parent.pos = 0;
            }

            var u8data = null;

            if (_parent.wsmode == "binary") {
                u8data = new Uint8Array(e.data);
            } else {
                u8data = Base64Binary.decode(e.data);
            }

            _parent.nwBuffer.writeUint8Array(u8data, DataStream.LITTLE_ENDIAN);
            _parent.pauseRead = false;


            //execute only once
            if (!_parent.startReading) {
                _parent.startReading = true;
                setTimeout(function() {
                    _parent.startRead.call(_parent);
                }, 2);
            }

            _parent.onmessage && _parent.onmessage(e);
        };
        this.ws.onclose = function(e) {
            console.log("Video WebSocket closed message", e);

            /*if(window.invalidPass==true) {
            console.log("logging pass", ip);
            window.invalidPass = false;
            _video.sock = new VideoSocket(ip, _video);
        }*/

            _parent.onclose && _parent.onclose(e);
        };
        this.ws.onerror = function(e) {
            console.log("Video WebSocket error message", e);
            _parent.onerror && _parent.onerror(e);
        };
        
    };

    VideoSocket.prototype = {

        createHeader: function(type, pktsize, status, data) {
            var stream = new DataStream(8 + pktsize, 0, DataStream.LITTLE_ENDIAN);
            stream.writeUint16(type, DataStream.LITTLE_ENDIAN);
            stream.writeUint32(pktsize, DataStream.LITTLE_ENDIAN);
            stream.writeUint16(status, DataStream.LITTLE_ENDIAN);

            if (data instanceof Uint8Array) {
                stream.writeUint8Array(data);
            } else {
                data && stream.writeString(data);
            }

            if (this.wsmode == "binary") {
                return stream;
            } else {
                return {buffer: base64ArrayBuffer(stream.buffer)};
            }
        },

        send: function(data) {
                this.ws.send(data);
        },

	enablePowerControls: function(state) {
            if(state == true) {
			//list of menu items to be enabled/disabled on power on /power off.
		    //var menuOnPower = new Array("#pause_video", "#resume_video", "#refresh_video", "#hd_turn_on", "#hd_turn_off", "#show_cli_cursor", "#absolute_mm", "#relative_mm", "#other_mm", "#r_ctrl", "#r_alt", "#r_win_hold", "#l_ctrl", "#l_alt", "#cad", "#l_win_press", "#r_win_press","#l_win_hold", "#context", "#prtscn", "#power_reset", "#power_off_immediate", "#power_off_orderly", "#power_cycle");
			if(this.video.power_status == 0) // server power off
			{
				//for(var i = 0; i < menuOnPower.length; i++)
					//$(menuOnPower[i]).addClass("disable_a_href");
					this.updateKbdLEDStatus(0x0);//reset the keyboard LEDs when the host is powered OFF
			}//else if(this.video.power_status == 1) // server power on
			//{
				//for(var i = 0; i < menuOnPower.length; i++)
					//$(menuOnPower[i]).removeClass("disable_a_href");
			//}
			this.updateKbdLEDBtns();
			this.updatePowerCtrlBtn();
		}
      	},

	onGetPowerControlStatus: function(status) {

		if(status == 1) { // server power on

			clearInterval(this.powerStatusTimer);
			this.powerStatusTimer = null;
		}
		else if(status == 0){// server power off
			this.video.blank_count++;
			this.video.paintBlankScreen();
			if(this.powerStatusTimer == null) {
					 var that = this;
					// if the host is powered off, h5viewer should query the adviser for power status in a timely manner
					// so that h5viewer will get notified when the host is powered on.
					this.powerStatusTimer = setInterval(function () {
					var powerStatus = that.createHeader(IVTP.CMD_POWER_STATUS, 0, 0);
					that.send(powerStatus.buffer);
					}, 30000);
			}
		}
		this.enablePowerControls(true);
      	},

	onPowerControlResponse: function(status) {
		if(status != 0)
		{
			// TODO show "#power_command_failed" message
		}
		else
		{
			// TODO show "#power_command_success" message
		}
		this.enablePowerControls(true);
      	},

    updateActiveUserMenu : function(data) {
        //clear existing menu items first
        var user = $("#active-user");
        user.html("");

        for(i = 0; i < this.num_clients; i++) {
            var name = window._video.sock.client_info[i].name;
            var ip = window._video.sock.client_info[i].ip;
            var id = window._video.sock.client_info[i].id;
            var ipmi_priv = window._video.sock.client_info[i].ipmi_priv;

            var $dropdown = $(".dropdown-menu.au");
            //add terminate button only to master session
            if((parseInt(id) == parseInt(this.current_session_id)) || (window.master == false)){
                $dropdown.append("<li>"+String.fromCharCode.apply(null,name)+"  ("+IPMI_PRIV[ipmi_priv][1]+") "+String.fromCharCode.apply(null,ip)+"</li>");
            }
            else{
                $dropdown.append("<li><span>"+String.fromCharCode.apply(null,name)+"  ("+IPMI_PRIV[ipmi_priv][1]+") "+String.fromCharCode.apply(null,ip)+"</span><span class= \"auser ion-ios-close\" \" id="+id+"></span></li>");
            }
        }

        //Active user clicked
        $(".auser").click(function(){
            var client_id = $(this).attr("id") * 1;
            console.log("KVM Session id to be terminated :"+client_id);
            var killClient = new Uint8Array(5);  //TERMINATE_SESSION_PKT_SIZE
            killClient[0]= 5; // KVM session type is 5
            killClient[1] = client_id;
            var hdr = window._video.sock.createHeader(0x36, killClient.length,0, killClient);
            window._video.sock.send(hdr.buffer);
        }); 
    },

	UpdateMouseModeMenu:function(prevMode,currentMode){
	
		if((prevMode >=1) && (prevMode <=3))
		{
			if($(MouseMenu_ID[prevMode][1]).children("i").length > 0){
				$(MouseMenu_ID[prevMode][1]).children("i").remove();
			}
		}

		if((currentMode >=1) && (currentMode <=3))
		{
			if($(MouseMenu_ID[currentMode][1]).children("i").length == 0) {
				$(MouseMenu_ID[currentMode][1]).append("<i class=\"fa fa-check\"></i>");
			}
			window.mousemode = MEDIA_CONFIGS.mouse_mode = currentMode;
		}
		if(window.mousemode != 2) {
			$("#kvm").css({"cursor": "none"});
		}else {
			$("#kvm").css({"cursor": "auto"});
		}
    	},

	// accepts a list of menu items and state. Enables or disables the menu's based on the state.
	showMenu: function(menu, state){
		for(var i = 0; i < menu.length; i++) {
			if(state == true ) {
				$(menu[i]).show();
			} else {
				$(menu[i]).hide();
			}
		}
		if(state == true ){
			$("#resume_video").addClass("disable_a_href");
			$("#stop").addClass("disable_a_href");
			$("#hd_turn_on").addClass("disable_a_href");
			//$("#zoom_in").hide();
			//$("#zoom_out").hide();
		}
	},
			
	//disable media options
	disableMediaOption: function() {
		//stop media redirection if running
		if(window.cdrom) {
			//On video is in pause state dont stop the media redirection
			if(window._video.isVideoPaused == false)
				$("#media").click();
		}
		//hide media redirection
		$("#media_redirection").hide();
		//disable button
		$("#media").attr("disabled", "disabled");
	},

	//enable media options
	enableMediaOption: function() {
		var media_priv = window.sessionStorage.getItem('vmedia_access') * 1;
		var cdEnabled = 0;
		// make sure server conf array is filled with data before using it.
		if(SERVER_CONF.length > 0)
		{
			cdEnabled = SERVER_CONF[1].currentState;
		}
		if((media_priv == 1) &&
			(MEDIA_CONFIGS.num_cd > 0) &&
			(cdEnabled == 1) &&
			(window.mediaLicenseStatus == true) &&
			(window.master == true)){ // prevents enabling media menu during partial permission
			$("#media").removeAttr("disabled");
			$("#media_redirection").show();
		}
		else{
			this.disableMediaOption();
		}
	},

	//checks media license status
	checkMediaLicenseStatus: function(state) {
		if(state == 1){
			window.mediaLicenseStatus = true;
			this.enableMediaOption();
		}
		else {
			window.mediaLicenseStatus = false;
			this.disableMediaOption();
		}
	},
	
	//Handle encryption status message from server.
	OnEncryptionStatus: function(state ) {
		
		// ignore encryption notification if encryption is enabled by this  client.
		if( window.encryption == true ){
			return;
		}			
		window.encryption = true;			
		// other sessions are running, so notify about encryption operation.
		if(window.DISABLE_ENCRYPT_FLAG == true){
			sessionStorage.setItem("status", "disable_encryption_error_message");
			$("#status_dialog").trigger("showStatus");
			if($("#kbd_mouse_encryption").children("i").length == 0) {
				$("#kbd_mouse_encryption").append("<i class=\"fa fa-check\"></i>");
			}
			window.DISABLE_ENCRYPT_FLAG = false;
		}else
		{
			sessionStorage.setItem("status", "encryption_enable");
			$("#status_dialog").trigger("showStatus");
		}
	},
	
	
	/*
	* Initial encryption status message handler. This message is received during the new
	* session establishment after encryption is enabled. In that case enable the encryption for new client.
	*/
	
	 
	OnInitialEncryptionStatus: function() {
		
		window.encryption=true;
		
		sessionStorage.setItem("status", "encryption_enable");
        $("#status_dialog").trigger("showStatus");
		if($("#kbd_mouse_encryption").children("i").length == 0) {
			$("#kbd_mouse_encryption").append("<i class=\"fa fa-check\"></i>");
		}
	},
	
	updateMediaMenuState:function(){

		if((MEDIA_CONFIGS.num_cd == 0) ||
			(SERVER_CONF[1].currentState == 0))
		{
			this.disableMediaOption();
		}

		if((MEDIA_CONFIGS.num_cd > 0) &&
			(SERVER_CONF[1].currentState == 1))
			
		{
			this.enableMediaOption();
		}
    	},

	compareConfData:function(service_conf,index){
	//	console.log("compareConfData service_conf.serviceName:"+service_conf.serviceName);
		if(service_conf.currentState != SERVER_CONF[index].currentState){
			SERVER_CONF[index].currentState = service_conf.currentState;
		}
		if((service_conf.interfaceName) != (SERVER_CONF[index].interfaceName )){
			service_conf.interfaceName =SERVER_CONF[index].interfaceName ;
		}
		if(service_conf.nonSecureAccessPort  != SERVER_CONF[index].nonSecureAccessPort){
			SERVER_CONF[index].nonSecureAccessPort = service_conf.nonSecureAccessPort;
		}
		if(service_conf.secureAccessPort != SERVER_CONF[index].secureAccessPort){
			SERVER_CONF[index].secureAccessPort = service_conf.secureAccessPort;
		}
		if(service_conf.sessionInactivityTimeout  != SERVER_CONF[index].sessionInactivityTimeout){
			SERVER_CONF[index].sessionInactivityTimeout = service_conf.sessionInactivityTimeout;
		}
		if(service_conf.maxAllowedSessions  != SERVER_CONF[index].maxAllowedSessions){
			SERVER_CONF[index].maxAllowedSessions = service_conf.maxAllowedSessions;
		}
		if(service_conf.currentActiveSession  != SERVER_CONF[index].currentActiveSession){
			SERVER_CONF[index].currentActiveSession = service_conf.currentActiveSession;
		}
		if(service_conf.maxSessionInactivityTimeout   != SERVER_CONF[index].maxSessionInactivityTimeout){
			SERVER_CONF[index].maxSessionInactivityTimeout = service_conf.maxSessionInactivityTimeout;
		}
		if(service_conf.minSessionInactivityTimeout   != SERVER_CONF[index].minSessionInactivityTimeout){
			SERVER_CONF[index].minSessionInactivityTimeout = service_conf.minSessionInactivityTimeout;
		}
	},

	updateMediaConfigChange:function(){
		console.log("updateMediaConfigChange " );
		this.updateMediaMenuState();

    	},

	// to update kvm menu lists on power on/off, kvm sharing and video pause/resume etc
	updateKVMMenuList: function(menuList, state) {
	//		console.log("updateKVMMenuList state" + state);
		var fullMenuList = kvmMenuItems;
			
		/*change and remove the  menu list items to state*/
		for(var i = 0; i < menuList.length; i++){
			var index = fullMenuList.indexOf(menuList[i]);
			if (index > -1) {
  				  fullMenuList.splice(index, 1);
			}	
			this.enableMenuOrButton(menuList[i], state);
		}

		/*change the remaining menu list items to !state */
	   	for(var j = 0; j < fullMenuList.length; j++){
			this.enableMenuOrButton(fullMenuList[j], !state);
		}
		// update host lock menu state based on if video is paused
		this.updateHostLockStateMenu(this.video.lm_option);
		
		// Disable all Power menu item when the current window is not a master
		if (window.master != true) {
			var list = document.getElementById("id_power_menu");
			var listItems = list.getElementsByTagName ("a");
			for(i = 0; i < listItems.length; i++) {
				var class_string = listItems[i].getAttribute("class");
				//console.log("id power menu : ", listItems[i], class_string);
				
				if ((class_string) && (class_string.indexOf("disable_a_href") == -1)) {		//Add only disable_a_href class is not present.
					listItems[i].setAttribute("class", class_string + " disable_a_href");
				} else {
					listItems[i].setAttribute("class", "disable_a_href");
				}
			}
		}
	},

	// enable/disable menu or button elements
	enableMenuOrButton: function(element, state){
		var index = element.indexOf("#button_");
		if(index > -1){
			if(state == true){
				$(element).removeAttr("disabled");
			}
			else{
				$(element).attr("disabled", "disabled");
			}
		}
		else{      	
			if(state == true){
				$(element).removeClass("disable_a_href");
			}
			else{
				$(element).addClass("disable_a_href");
			}	
		}
	},

	/* create Menu item at run-time
	 * MenuIdList - Menu Id
	 * menuName - menu Name 
	 * parentId - indicates the menu id (id of <ul>) under which new menu items will be added
	 * position - position to insert the menu item
	 * separator - if 1 create menu separator
	 * className - used to group the menu
	 * functionName - Event handler
	 */
	createMenuItem : function(MenuIdList,menuName,parentId,position,functionName, separator, className){
		var exist = 0;
		if( (MenuIdList != null) && (menuName != null) && parentId != null && parseInt(position) != NaN ){
			for(var i=0;i<MenuIdList.length;i++){
				//returns values greater than zero, if menu with 'MenuIdList' exist
				exist = $('#' + parentId + ' li ' + MenuIdList[i] + '').length;
				if( exist> 0)
				{
					break;
				}
				//create separator before adding menu's
				//don't create separator if the menu going to be added is in the first position
				//create separator only if the flag is true.
				if((i == 0) && (position != 0) && (separator == true)){
					this.createSeparator(parentId, position,className);
				}
				//creates a menu item
				menuId =MenuIdList[i].replace("#","");
				var list = document.getElementById(parentId);
				var li = document.createElement("li");
				var a = document.createElement("a");
				var menu_name= document.createTextNode(menuName[i]);
				a.setAttribute("id",menuId);
				if(className != null){
					a.setAttribute("class",className);
				}
				a.appendChild(menu_name);
				a.href = "#";
				li.appendChild(a);
				list.insertBefore(li, list.children[position]);
				//add Event Listener for the created Menu
				document.getElementById(menuId).addEventListener("click",functionName,false);
			}
		}
	},

	// create Line separator
	createSeparator : function(parentId,position,classname) {
		var li = document.createElement("li");
		li.setAttribute("role","separator");
		
		if (classname != null)
			li.setAttribute("class","divider "+ classname);
		else
			li.setAttribute("class","divider");
		var list = document.getElementById(parentId);
		list.insertBefore(li,list.children[position]);
	},

	syncMediaConf:function(){
//		MEDIA_CONFIGS.mouse_mode = -1;
		MEDIA_CONFIGS.num_cd = window.mediaconf.num_cd;
		MEDIA_CONFIGS.num_fd= window.mediaconf.num_fd;
		MEDIA_CONFIGS.num_hd = window.mediaconf.num_hd;
		MEDIA_CONFIGS.powerSaveMode  = window.mediaconf.power_save_mode;
		MEDIA_CONFIGS.kvm_cd = window.mediaconf.kvm_num_cd;
		MEDIA_CONFIGS.kvm_fd = window.mediaconf.kvm_num_fd; 
		MEDIA_CONFIGS.kvm_hd = window.mediaconf.kvm_num_hd;

	},

	updateHostLockStateMenu:function(lm_option){

		//unlock( 0 ,2)	- host display on
		//lock(1,3)	- host display off

		// update host lock menu state based on if video is paused
		if( window._video.isVideoPaused == true ) {
			$("#hd_turn_off").addClass("disable_a_href");
			$("#hd_turn_on").addClass("disable_a_href");
			$("#host_lock_btn").addClass("disable_a_href");
			return;
		}

		if((lm_option ==  HOST_LOCK_STATUS.HOST_DISPLAY_UNLOCK) ||
			(lm_option == HOST_LOCK_STATUS.HOST_DISPLAY_UNLOCKED_AND_DISABLED)){
			if($("#hd_turn_off").children("i").length > 0){
				$("#hd_turn_off").children("i").remove();
			}

			if( $("#hd_turn_on").children("i").length == 0) {
				$("#hd_turn_on").append("<i class=\"fa fa-check\"></i>");
				this.updateColor("#host_lock_btn",COLOR.green,true);//green
			}
		}

		if((lm_option ==  HOST_LOCK_STATUS.HOST_DISPLAY_LOCK) ||
			(lm_option == HOST_LOCK_STATUS.HOST_DISPLAY_LOCKED_AND_DISABLED)){
			if($("#hd_turn_on").children("i").length > 0){
				$("#hd_turn_on").children("i").remove();
			}

			if( $("#hd_turn_off").children("i").length == 0) {
				$("#hd_turn_off").append("<i class=\"fa fa-check\"></i>");
				this.updateColor("#host_lock_btn",COLOR.red,false);//red
			}
		}

		//if slave session disable both the menu
		if(window.master == false){
			 $("#hd_turn_off").addClass("disable_a_href");
			 $("#hd_turn_on").addClass("disable_a_href");
			 $("#host_lock_btn").addClass("disable_a_href");
		}
		else{
			$("#host_lock_btn").removeClass("disable_a_href");
			switch(lm_option){
				case HOST_LOCK_STATUS.HOST_DISPLAY_UNLOCK :
					 $("#hd_turn_on").addClass("disable_a_href");
					 $("#hd_turn_off").removeClass("disable_a_href");
					 break;
				case HOST_LOCK_STATUS.HOST_DISPLAY_LOCK :
					 $("#hd_turn_off").addClass("disable_a_href");
					 $("#hd_turn_on").removeClass("disable_a_href");
					 break;
				case HOST_LOCK_STATUS.HOST_DISPLAY_UNLOCKED_AND_DISABLED :
				case HOST_LOCK_STATUS.HOST_DISPLAY_LOCKED_AND_DISABLED :
					 $("#hd_turn_off").addClass("disable_a_href");
					 $("#hd_turn_on").addClass("disable_a_href");
					 $("#host_lock_btn").addClass("disable_a_href");
				default:
					break;
			}
		}
	},

	/*
	* id  - button id
	* Name - 0 to 5 (if Name is invalid default color(btn-default) is set).
	* state - true/false
	*/
	updateColor:function(id,Name,state){
		if(id != null ){
			Name = (Name != null ? Name : "white");
			// get class attribute value as array
			className = $(id).attr("class").split(" ");
			if(this.oem_app.handleColor(id,Name,state) == false){
				className.forEach( function(value,idx) {
					// class "btn-xs" shouldn't be removed as it specify button size. 
					if( value != "btn-xs" && value !== undefined)
						// replace string starting with " btn- " except btn-xs ; 
						className[idx] = value.replace(/^btn-.*/g,Name);
				} );
				// replace the "," in className and set it as class attribute value of the element
				$(id).attr("class",className.join().replace(/,/g, " ") );
			}
		}
	},
	//Update the Keyboard LED status buttons based on the Host LED status value
	updateKbdLEDStatus:function(lm_led_status)
	{
		if(this.video.host_led_status != lm_led_status)
		{
			if(0x0 != (lm_led_status & KBD_LED.NUM_LOCK))
			{
				this.updateColor("#num_lock",COLOR.green,true);//green
			}
			else
			{
				this.updateColor("#num_lock",COLOR.white,false);//default
			}
			if(0x0 != (lm_led_status & KBD_LED.CAPS_LOCK))
			{
				this.updateColor("#caps_lock",COLOR.green,true);//green
			}
			else
			{
				this.updateColor("#caps_lock",COLOR.white,false);//default
			}
			if(0x0 != (lm_led_status & KBD_LED.SCR_LOCK))
			{
				this.updateColor("#scr_lock",COLOR.green,true);//green
			}
			else
			{
				this.updateColor("#scr_lock",COLOR.white,false);//default
			}
			this.video.host_led_status = lm_led_status;
		}
	},

	//Shows/Hides the keyboard LED status buttuns
	showKbdLEDBtns: function(state)
	{
		for(var i = 0; i < this.video.kbdLEDBtns.length; i++) 
		{
			if(state == true ) {
				$(this.video.kbdLEDBtns[i]).show();
			} else {
				$(this.video.kbdLEDBtns[i]).hide();
			}
		}
	},

	//Show/hide the powercontrol button
	showPowerCtrlBtn: function(state){
		if(state)
			$("#power_div").show();//show the container div
		else
			$("#power_div").hide();//hide the container div
	},

	//Update the keyboard LED status buttons
	updateKbdLEDBtns: function()
	{
		//If the current session has master permission and the power status is ON, and the video is not paused, 
		//then the LED buttons should be enabed.
		var state = (window.master && (this.video.power_status == 1) && !(window._video.isVideoPaused));
		for(var i = 0; i < this.video.kbdLEDBtns.length; i++) 
		{
			if(state == true ) {//make buttons clickable
				$(this.video.kbdLEDBtns[i]).removeClass("disable_a_href");
			} else {//make buttons NOT clickable
				$(this.video.kbdLEDBtns[i]).addClass("disable_a_href");
			}
		}
	},

	//Enable/Disable power control button.
	enablePowerCtrlBtn:function(){
		var state = (window.master && !(window._video.isVideoPaused));
		if(true == state){//make buttons clickable
			$("#power_ctrl_btn").removeClass("disable_a_href");
		}
		else{
			$("#power_ctrl_btn").addClass("disable_a_href");
		}
	},

	//Change the power control button color based on power status.
	updatePowerCtrlBtn:function(){
		if(this.video.power_status == 1) //power ON
			this.updateColor("#power_ctrl_btn",COLOR.green,true);//green
		else //power OFF
			this.updateColor("#power_ctrl_btn",COLOR.red,false);//red
	},

    startRead: function() {

        var token = JSON.parse(sessionStorage.getItem("token"));
        window.LOCAL_HOSTNAME = token.client_ip;
        var
            web_cookie = token.session,
            kvm_token = token.token,
            ip = window.LOCAL_HOSTNAME || "No IP",
            uname = window.LOCAL_USERNAME || "domain/username",
            mac = window.LOCAL_MAC || "00-00-00-00-00-00";

        var _parent = this;
        if (!this.pauseRead) {
            var ihab = this.nwBuffer.buffer.slice(this.pos, this.pos + 8);
            delete this.newBuffer;
            this.newBuffer = null;
            this.newBuffer = new DataStream(ihab);
             var  ivtp_hdr;

            try {
                    ivtp_hdr = this.newBuffer.readStruct(
                    ['type', 'uint16',
                        'pkt_size', 'uint32',
                        'status', 'uint16'
                    ]
                );
            } catch (e) {
                delete this.newBuffer;
                delete ivtp_hdr;
                this.newBuffer = null;
                ivtp_hdr = null;
                if(this.video.running == true){
                    setTimeout(function() {
                        _parent.startRead.call(_parent);
                    }, 1);
                }
                return;
            }

            //console.log('hdr type', ivtp_hdr.type);
            
            if (this.pos + 8 + ivtp_hdr.pkt_size > this.nwBuffer.buffer.byteLength) {
                setTimeout(function() {
                    _parent.startRead.call(_parent);
                }, 1);
                return;
            }

            this.pos += IVTP.HDR_SIZE;

            if (ivtp_hdr.type != IVTP.CMD_VIDEO_PACKETS) {
                delete this.newBuffer;
                this.newBuffer = null;
                this.prev_complete = true;
                var ab = this.nwBuffer.buffer.slice(this.pos, this.pos + ivtp_hdr.pkt_size);
                this.pos += ivtp_hdr.pkt_size;
                this.newBuffer = new DataStream(ab);
            }
	    // oem ivtp commands should be handled first
	    if(this.oem_app.isOEMCommand(ivtp_hdr.type) == true) {
	    }
	    else {
		switch (ivtp_hdr.type) {
		case IVTP.CMD_CONNECTION_ALLOWED:
			// add soc menu's to kvm Menu list only if it doesn't exist  
			if($(this.video.soc_menu_list[0]).index() == -1){
				kvmMenuItems =kvmMenuItems.concat(this.video.soc_menu_list);
			}
			this.video.updateSocMenu();
			this.video.running = true;
			var features = JSON.parse(sessionStorage.getItem("features"));
			var i;
			if (features != null)
			{
				for(i = 0; i < features.length; i++) {
					if(features[i].feature == "KVM_SESSION_RECONNECT") {
						var reconnect = this.createHeader(0x0, 0x0, 0, "");
						this.send(reconnect.buffer);
						break;
					}
				}
			}

		    //var cnxn_complete = this.createHeader(IVTP.CMD_CONNECTION_COMPLETE_PKT, 0x0, 0, "");
		    //this.send(cnxn_complete.buffer);


		    //var resume = this.createHeader(IVTP.CMD_RESUME_REDIRECTION, 0, 0);
		    //this.send(resume.buffer);

		    var auth_pkt = new DataStream(373);
		    auth_pkt.writeUint8(0); //flag
		    auth_pkt.writeCString(kvm_token, 129); //token
		    auth_pkt.writeCString(ip, 65); //ip
		    auth_pkt.writeCString(uname, 129); //uname
		    auth_pkt.writeCString(mac, 49); //mac

		    //total data 373B
		    //total with ivtp hdr 381B
		    //console.log(token.length);
		    auth_pkt.seek(0);
		    var session = this.createHeader(IVTP.CMD_VALIDATE_VIDEO_SESSION, auth_pkt.byteLength, 1, auth_pkt.readUint8Array());
		    this.send(session.buffer);
		    delete auth_pkt;
		    auth_pkt = null;

		    break;
		case IVTP.CMD_STOP_SESSION_IMMEDIATE:
		    //console.log("STOP Session Immediate : "+ ivtp_hdr.status);
		    sessionStorage.setItem("status", "stop_session");
		    switch(ivtp_hdr.status)
		    {
			case 2: //video server restart
			sessionStorage.setItem("stop_reason", "kvm_server_restart");
			break;
			case 5: //conf changed
			sessionStorage.setItem("stop_reason", "kvm_conf_change");
			break;
			case 7: //Web log out
			sessionStorage.setItem("stop_reason", "kvm_web_logout");
			break;
			case 8: //License expiry
			sessionStorage.setItem("stop_reason", "kvm_license_exp");
			break;
			case 9: //KVm timeout
			sessionStorage.setItem("stop_reason", "kvm_timed_out");
			break;
			case 10: //session terminated
			sessionStorage.setItem("stop_reason", "kvm_terminated");
			break;
			case 11: //web server restarted
			sessionStorage.setItem("stop_reason", "kvm_web_restart");
			break;
			default:-
			    sessionStorage.setItem("stop_reason", "");
		    }
		    $("#status_dialog").trigger("showStatus");
		    this.video.stopped = true;
		    break;
		case IVTP.CMD_SET_FPS:
		    //console.log("Setting FPS");
		    break;
		case IVTP.CMD_USB_MOUSE_MODE:
		    //console.log("Get USB Mouse Mode");
		    var mode = this.newBuffer.readUint8()*1;
		    if(window.mousemode != mode)
			this.UpdateMouseModeMenu(window.mousemode,mode);

		    //	var resume = this.createHeader(IVTP.CMD_RESUME_REDIRECTION, 0, 0);
		    //	this.send(resume.buffer);
		    //usbmouse.m_mousemode = mousemode;
		    break;
		case IVTP.CMD_GET_FULL_SCREEN:
		    //	console.log("get full screen");
		    break;
		case IVTP.CMD_GET_KBD_LED_STATUS:
		    //	console.log("received kbd led");
		    var led_status = this.newBuffer.readUint8();
		    this.updateKbdLEDStatus(led_status);
		    break;
		case IVTP.CMD_MAX_SESSION_CLOSE:
		    //console.log("max session closing");
		    sessionStorage.setItem("status", "stop_session");
		    switch(ivtp_hdr.status)
		    {
			case 0: //Max session reached
			sessionStorage.setItem("stop_reason", "kvm_max_session_reached");
			break;
			case 1: //KVM session running in same client
			sessionStorage.setItem("stop_reason", "same_kvm_client_user");
			break;
			default:-
			    sessionStorage.setItem("stop_reason", "");
		    }
		    $("#status_dialog").trigger("showStatus");
		    this.video.stopped = true;
		    break;
		case IVTP.CMD_VALIDATED_VIDEO_SESSION:
		    //console.log("Validated video session response");
		    var pkt = this.newBuffer.readUint8();
		    sessionStorage.setItem("status", "invalid_session");
		    if( (pkt == 0) ||
			(pkt == 2) ||
			(pkt == 3))
		    {
			if (pkt == 0)
			    sessionStorage.setItem("invalid_sess_reason", "kvm_inv_sess");
			else if (pkt == 2)
			    sessionStorage.setItem("invalid_sess_reason", "kvm_disabled");
			else if (pkt == 3)
			    sessionStorage.setItem("invalid_sess_reason", "kvm_inv_sess_info");
			$("#status_dialog").trigger("showStatus");
			this.validated = 0 ;
			this.video.stopped = true;
		    }
		    this.validated = 1;

		    //get current session index number
		    if(ivtp_hdr.pkt_size > 1){
			var sindex = this.newBuffer.readUint8();
			this.current_session_id = sindex;
			console.log("session index: "+this.current_session_id);
		    }

		    if(this.video.lm_option == 1) {
			// host lock need to be done on KVM session launch
			$("#hd_turn_on").click();
		    }

		    if(this.keep_alive_timer == null) {
			var that = this;
			this.keep_alive_timer = setInterval(function () {
			    var keep_alive = that.createHeader(IVTP.CMD_KEEP_ALIVE_PKT, 0, 0);
			    that.send(keep_alive.buffer);
			}, 3000);
		    }
		    break;
		case IVTP.CMD_SET_NEXT_MASTER:
		    //console.log("Set next master");
		    var low_byte = ivtp_hdr.status & 255;
		    var high_byte = ivtp_hdr.status >> 8;

		    var onPowerstate = this.createHeader(0x22, 0, 0);
		    this.send(onPowerstate.buffer);

		    var pkt = this.newBuffer.readString();
		    if(pkt != "") {
			sessionStorage.setItem("other_session_info", window.btoa(pkt));
		    }
		    else {
			pkt = window.atob(sessionStorage.getItem("other_session_info"));
		    }

		    this.other_session_name = pkt.substring(0, 64);
		    this.other_session_ip = pkt.substring(64, 129);

		    if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER) {
			if(KVM_SHARING.KVM_BLOCK_USER_REQUEST == true){
			    console.log("KVM master permission request blocked");
			    var slave_info = window.atob(sessionStorage.getItem("other_session_info"));
			    var hdr = this.createHeader(0x20, slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_BLOCKED << 8), slave_info);
			    this.send(hdr.buffer);
			}
			else{
			    $("#master_dialog").trigger("requestMasterPermission");
			}
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_WAIT_SLAVE) {
			$("#slave_dialog").trigger("waitForPermission");
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_RESPONSE_TO_SLAVE) {
			if(high_byte == KVM_SHARING.KVM_REQ_ALLOWED) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_full");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_PARTIAL) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_part");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_TIMEOUT) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_timeout");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_BLOCKED) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "master_blocked_request");
			    $("#status_dialog").trigger("showStatus");
			}
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_SWITCH_MASTER) {
			$("#status").html("");
			$("#request_full").hide();
			if(window.master == false) {
			    sessionStorage.setItem("status", "slave_permission_full");
			    $("#status_dialog").trigger("showStatus");
			}
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_REQ_TIMEOUT_TO_MASTER) {
			if(high_byte == KVM_SHARING.KVM_REQ_TIMEOUT) {
			    sessionStorage.setItem("status", "master_timeout");
			    $("#status_dialog").trigger("showStatus");
			}
		    }
		    this.updateActiveUserMenu();
		    break;
		case IVTP.CMD_PAINT_BLANK_SCREEN:
			// if the host video is blank, the adviser will send only one or two blank screens.
			// so increment the blank screen count first and then call paintBlankScreen.
			this.video.blank_count++;
			this.video.paintBlankScreen();
		    break;
		case IVTP.CMD_VIDEO_PACKETS:
		    this.video.cmdOnVideoPackets(ivtp_hdr);
		    if(this.fps_timer == null)
		    {
		    	var that = this;
				this.fps_timer = setInterval(function() {
			    	if((typeof window.frames_received != 'undefined') && (typeof window.frames_processed != 'undefined'))
			    	{
			    		var diff = (window.frames_received - window.frames_processed);
			    		if(diff < 0)
			    			diff = -diff;
			    		var fps_packet = that.createHeader(IVTP.CMD_FPS_DIFF, 0, diff);
			    		that.send(fps_packet.buffer);
			    		window.frames_received = window.frames_processed = 0;
			    		delete fps_packet;
			    		fps_packet = null;
			    	}
			    }, 100);
		    }
		    break;
		case IVTP.CMD_GET_USER_MACRO:
		    var keycodes = this.newBuffer.readUint8Array();
		    //console.log("user macro detail", keycodes, keycodes.byteLength);
		    /* parsing the received data */
		    parseUserMacroData(keycodes);
		    break;
		case IVTP.CMD_ACTIVE_CLIENTS:
		    var arr = this.newBuffer.readUint8Array();
		    //	console.log("Active clients detail", arr, arr.byteLength);
		    this.client_info = [];
		    this.num_clients = Math.round(arr.byteLength / 134);

		    var start = 0;
		    //64+65+1+4(UserName+ClientIP+SessionID+ipmi priv)
		    for(i = 0; i < this.num_clients; i++) {
			var name = arr.subarray(start, start + 64);
			var ip = arr.subarray(start + 64, start + 129);
			var id = arr[start + 129];
			var ipmi_priv = arr[start + 130];
			this.client_info.push({name: name, ip: ip, id: id,ipmi_priv:ipmi_priv});
			start += 134;
		    }

		    this.updateActiveUserMenu();

		    if(typeof this.current_session_id == 'undefined') {
			this.current_session_id = this.client_info[this.client_info.length - 1].id;
		    }
		    //request full screen here
		    var requestFullScreen = this.createHeader(IVTP.CMD_GET_FULL_SCREEN, 0x0, 0, "");
		    this.send(requestFullScreen.buffer);
		    break;
		case IVTP.CMD_DISPLAY_CONTROL_STATUS:
		    console.log("Display control status");
		    var status = ivtp_hdr.status;

		    if(this.video.lm_option != status){
			this.video.lm_option = status;
		    }
		    else{
			//same status received so no need to do anything
			//break;
		    }
		    this.updateHostLockStateMenu(this.video.lm_option);

		    break;
		case IVTP.CMD_MEDIA_LICENSE_STATUS:
		    //console.log("Media license status");
		    this.checkMediaLicenseStatus(ivtp_hdr.status);
		    //var resume = this.createHeader(IVTP.CMD_RESUME_REDIRECTION, 0, 0);
		    //this.send(resume.buffer);
		    var pwr = this.createHeader(IVTP.CMD_POWER_STATUS, 0, 0);
		    this.send(pwr.buffer);
		    var lock_set_value = new Uint8Array(1);
		    lock_set_value[0] = 2;
		    var display_lock_set = this.createHeader(IVTP.CMD_DISPLAY_LOCK_SET, 1, 0, lock_set_value);
		    this.send(display_lock_set.buffer);
		    var user_macro = this.createHeader(IVTP.CMD_GET_USER_MACRO, 0, 0);
		    this.send(user_macro.buffer);
		    var auth = this.createHeader(IVTP.CMD_GET_WEB_TOKEN, 0x23, 0, web_cookie);
		    //console.log(auth);
		    this.send(auth.buffer);
		    delete lock_set_value;

		    break;
		case IVTP.CMD_POWER_STATUS:
		    //console.log("Power Status");
		    this.video.power_status = ivtp_hdr.status;
		    if(this.Host_OS_shutdown == false)
		    {
			this.onGetPowerControlStatus(this.video.power_status);
		    }
		    //host OS still power on, request to get power status again
		    else {
			if(this.video.power_status == 1)
			{
			    var that = this;
			    // runs only once
			    this.requestPowerStatusTimer = setTimeout(function () {
				var powerStatus = that.createHeader(IVTP.CMD_POWER_STATUS, 0, 0);
				that.send(powerStatus.buffer);
			    }, 2000);
			}
			else
			{
			    this.onGetPowerControlStatus(this.video.power_status);
			    this.Host_OS_shutdown = false;
			}
		    }
		    // Enable Power menu only in the master window
		    if (window.master == true) {
		        if(this.video.power_status == 1)
			        this.updateKVMMenuList(resumeExceptionList, false);
		        else
			        this.updateKVMMenuList(powerOffMenuList, true);
		    }
		    //To prevent menus from enabling during partial permission
		    else this.updateKVMMenuList(partialPrivilegeMenuList, true);
		    break;
		case IVTP.CMD_POWER_CTRL_RESP:
		    this.onPowerControlResponse(ivtp_hdr.status);
		    break;
		case IVTP.CMD_KVM_SHARING:
		    //console.log("kvm sharing");
		    var pkt = this.newBuffer.readString();
		    if(pkt != "") {
			sessionStorage.setItem("other_session_info", window.btoa(pkt));
		    }
		    else {
			pkt = window.atob(sessionStorage.getItem("other_session_info"));
		    }

		    this.other_session_name = pkt.substring(0, 64);
		    this.other_session_ip = pkt.substring(64, 129);

		    var low_byte = ivtp_hdr.status & 255;
		    var high_byte = ivtp_hdr.status >> 8;

		    //from Advsd to client1
		    if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER) {
			if(KVM_SHARING.KVM_BLOCK_USER_REQUEST == true){
			    console.log("KVM master permission request blocked");
			    var slave_info = window.atob(sessionStorage.getItem("other_session_info"));
			    var hdr = this.createHeader(0x20, slave_info.length, KVM_SHARING.STATUS_KVM_PRIV_REQ_MASTER + (KVM_SHARING.KVM_REQ_BLOCKED << 8), slave_info);
			    this.send(hdr.buffer);

			}
			else{
			    $("#master_dialog").trigger("requestKvmPermission");
			}
		    }
		    //from Advsd to client2 wait state
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_WAIT_SLAVE) {
			$("#slave_dialog").trigger("waitForPermission");
		    }
		    //from Advsd to client1, if the client2 quit before the permission given
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_REQ_CANCEL) {
			sessionStorage.setItem("status", "slave_quit");
			$("#status_dialog").trigger("showStatus");

		    }
		    //from advsd to client2, packet with permission status
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_RESPONSE_TO_SLAVE) {
			if(high_byte == KVM_SHARING.KVM_REQ_ALLOWED) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_full");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_PARTIAL) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_part");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_DENIED) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_none");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_TIMEOUT) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "slave_permission_timeout");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_BLOCKED) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "master_blocked_request");
			    $("#status_dialog").trigger("showStatus");
			}
			else if(high_byte == KVM_SHARING.KVM_REQ_MASTER_RECONN) {
			    $("#slave_dialog").trigger("resetDialog");
			    sessionStorage.setItem("status", "master_reconn");
			    $("#status_dialog").trigger("showStatus");
			}
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_SWITCH_MASTER) {
			sessionStorage.setItem("status", "switch_master");
			$("#status_dialog").trigger("showStatus");
		    }
		    else if(low_byte == KVM_SHARING.STATUS_KVM_PRIV_REQ_TIMEOUT_TO_MASTER) {
			if(high_byte == KVM_SHARING.KVM_REQ_TIMEOUT) {
			    sessionStorage.setItem("status", "master_timeout");
			    $("#status_dialog").trigger("showStatus");
			}
		    }
		    else {
			//console.log("other kvm sharing message", ivtp_hdr.status);
		    }
		    this.updateActiveUserMenu();
		    break;
		case IVTP.CMD_KEEP_ALIVE_PKT:
		    break;
		case IVTP. CMD_INITIAL_ENCRYPTION_STATUS:
		    this.OnInitialEncryptionStatus();
		    break;

		case IVTP.CMD_ENCRYPTION_STATUS:
		    this.OnEncryptionStatus(ivtp_hdr.status);
		    break;
		case IVTP.CMD_SERVICE_INFO:
		    console.log("Service info received");
		    var first = false;

		    var arr = this.newBuffer.readUint8Array();
		    var NAME_MAX_LENGTH = 17;


		    this.server_conf = [];
		    var serviceName,currentState,interfaceName,nonSecureAccessPort,secureAccessPort,sessionInactivityTimeout,
			maxAllowedSessions,currentActiveSession,maxSessionInactivityTimeout,minSessionInactivityTimeout;
		    // start is set to 57 so that we can skip adding the web configuration to server conf array.
		    var start = 57;
		    if(SERVER_CONF.length == 0)
			first = true;
		    // index is set to 4 (kvm,cd,fd,hd)
		    for(var index = 0; index < 4; index++) {

			serviceName = arr.subarray(start, start + NAME_MAX_LENGTH); //0, 17
			currentState = arr[start + NAME_MAX_LENGTH] *1;//18
			interfaceName = arr[start + 18,start+(35)]; //18+17
			nonSecureAccessPort = arr[start + 35,start+(39)] *1;
			secureAccessPort= arr[start + 39,start+(43)] *1;
			sessionInactivityTimeout = arr[start + 43,start+(47)] *1;
			maxAllowedSessions = arr[start + 47] *1;
			currentActiveSession = arr[start + 48] *1;
			maxSessionInactivityTimeout  = arr[start + 48,start+(52)] *1;
			minSessionInactivityTimeout  = arr[start + 52,start+(56)] *1;

			if(first == true){
			    SERVER_CONF.push({serviceName: serviceName, currentState: currentState, interfaceName: interfaceName,nonSecureAccessPort:nonSecureAccessPort,secureAccessPort: secureAccessPort, sessionInactivityTimeout: sessionInactivityTimeout, maxAllowedSessions: maxAllowedSessions,currentActiveSession:currentActiveSession,maxSessionInactivityTimeout: maxSessionInactivityTimeout,minSessionInactivityTimeout:minSessionInactivityTimeout});
			}
			else{
			    this.server_conf.push({serviceName: serviceName, currentState: currentState, interfaceName: interfaceName,nonSecureAccessPort:nonSecureAccessPort,secureAccessPort: secureAccessPort, sessionInactivityTimeout: sessionInactivityTimeout, maxAllowedSessions: maxAllowedSessions,currentActiveSession:currentActiveSession,maxSessionInactivityTimeout: maxSessionInactivityTimeout,minSessionInactivityTimeout:minSessionInactivityTimeout});
			    this.compareConfData(this.server_conf[index],index);
			}
			start += 57;
		    }
		    //if(first == true)
		    //	this.server_conf = SERVER_CONF;

		    //this.compareConfData();
		    this.updateMediaConfigChange();
		    break;
		case IVTP.CMD_KVM_MEDIA_INFO:
		    console.log("CMD_KVM_MEDIA_INFO", ivtp_hdr.status);
		    var mediaConfChanged = false;

		    var reconn_retry_count = this.newBuffer.readUint32() *1;
		    var reconn_retry_interval = this.newBuffer.readUint32() *1;


		    var mouse_mode =  this.newBuffer.readUint8();   /** Says whether to use Absolute mouse or relative mouse */
		    var host_lock_status =  this.newBuffer.readUint8();   /** Says host lock status */
		    var attach_cd = this.newBuffer.readUint32() *1;        /** Attach mode for CD */
		    var attach_fd = this.newBuffer.readUint32()*1;                /** Attach mode for FD */
		    var attach_hd = this.newBuffer.readUint32()*1;                   /** Attach mode for HD */
		    var enable_boot_once = this.newBuffer.readUint32()*1;            /** Enable boot once or not */
		    var num_cd = this.newBuffer.readUint32()*1;        /** Number of CD Instances */
		    var num_fd = this.newBuffer.readUint32()*1;              /** Number of FD Instances */
		    var num_hd = this.newBuffer.readUint32()*1;                /** Number of HD Instances */
		    var lmedia_enable = this.newBuffer.readUint32()*1;          /** Enable the LMedia Feature */
		    var rmedia_enable = this.newBuffer.readUint32()*1;          /** Enable the RMedia Feature */
		    var sdserver_enable = this.newBuffer.readUint32()*1;          /** Enable the sdserver Feature */
		    var powerSaveMode = this.newBuffer.readUint32()*1;          /** Power COnsumption Feature status*/
		    var kvm_cd = this.newBuffer.readUint32()*1;
		    var kvm_fd = this.newBuffer.readUint32()*1;
		    var kvm_hd = this.newBuffer.readUint32()*1;

		    //console.log("mouse: "+mouse_mode+":"+attach_cd+":"+attach_fd+":"+attach_hd+":"+enable_boot_once+":"+num_cd+":"+num_fd+":"+num_hd+":"+lmedia_enable);
		    //console.log("rmedia: "+rmedia_enable+":"+sdserver_enable+":"+powerSaveMode+":"+kvm_cd+":"+kvm_fd+":"+kvm_hd);

		    MEDIA_CONFIGS.powerSaveMode = powerSaveMode;

		    //check reconnect featue
		    {
			MEDIA_CONFIGS.reconn_retry_count = reconn_retry_count;
			MEDIA_CONFIGS.reconn_retry_interval = reconn_retry_interval;
		    }

		    if((MEDIA_CONFIGS.num_cd != num_cd) ||
		       (MEDIA_CONFIGS.num_fd != num_fd) ||
		       (MEDIA_CONFIGS.num_hd != num_hd))

		    {
			//H5viewer supports only CD so consider CD num change alone
			if(MEDIA_CONFIGS.num_cd != num_cd)
			{
			    mediaConfChanged = true;
			    MEDIA_CONFIGS.num_cd = num_cd;
			}
			MEDIA_CONFIGS.num_fd = num_fd;
			MEDIA_CONFIGS.num_hd = num_hd;
		    }

		    //check kvm count feature enabled
		    {
			if((MEDIA_CONFIGS.kvm_cd != kvm_cd) ||
			   (MEDIA_CONFIGS.kvm_fd != kvm_fd) ||
			   (MEDIA_CONFIGS.kvm_hd != kvm_hd))
			{
			    //H5viewer supports only 1 CD so it will not comply with KVM cd count Dont set the flag
			    //mediaConfChanged = true;
			    MEDIA_CONFIGS.kvm_cd = kvm_cd;
			    MEDIA_CONFIGS.kvm_fd = kvm_fd;
			    MEDIA_CONFIGS.kvm_hd = kvm_hd;
			}
		    }

		    //if mouse mode changes update mode properly
		    if(MEDIA_CONFIGS.mouse_mode != mouse_mode)
		    {
			this.UpdateMouseModeMenu(MEDIA_CONFIGS.mouse_mode,mouse_mode);
		    }

		    if(mediaConfChanged == true)
		    {
			this.updateMediaMenuState();
		    }


		    break;
		case IVTP.CMD_MEDIA_FREE_INSTANCE_STATUS:
			//TODO:
			break;
		default:
		    console.log("default: onSocControlMessage ivtp_hdr.type "+ivtp_hdr.type);
		    //handle soc cmd
		    this.video.onSocControlMessage(ivtp_hdr,this.newBuffer);
		    }
	    }
		    //nwBuffer.byteOffset += IVTP_HEADER_SIZE + ivtp_hdr.pkt_size;
	    delete this.newBuffer;
	    delete ivtp_hdr;
                this.newBuffer = null;
                ivtp_hdr = null;

                if (_parent.pos == _parent.nwBuffer.byteLength) {
                    //console.log("deleting and creating ");
                    delete _parent.nwBuffer;
                    _parent.nwBuffer = null;
                    _parent.nwBuffer = new DataStream();
		_parent.pos = 0;
	    }
	}

	if(this.video.stopped == true){
	    this.close();
	}

	if(this.video.running == true){
	    setTimeout(function() {
		_parent.startRead.call(_parent);
                }, 1);
	}

    },

        close: function() {
            clearInterval(this.keep_alive_timer);
            this.keep_alive_timer = null;
            var stop = this.createHeader(0x08, 0, 0);
            this.send(stop.buffer);
            //console.log("Closing socket");
            this.validated = 0;
            this.video.running = false;
            var _parent = this;
            setTimeout(function(){
                _parent.ws.close();
            }, 100);
        },

        /* Function used to update the help contents
        ** img_src -> path of the image file to be used as logo
        ** copyright_text -> text ( in html format ) to be displayed as help content
        ** title_text -> String value to be used as dialog title */
        setHelpContent: function(img_src, copyright_text, title_text){
            $('#help_title').html(title_text);
            $('#help_body').addClass("row"); // the dialog consists of a single row with two columns <-- | logo | text | -->
            $('#help_body').html('<div class=\"col-md-4\"><img class=\"img-responsive\" id=\"logo\" alt=\"Logo\"></div><div class=\"col-md-7\" id=\"copyright\"></div>');
            $('#copyright').html(copyright_text);
            $('#logo').attr('src',img_src);
        },

        // Function will return the default help text to be displayed.
        getDefaultHelpText: function() {
            return window.CommonStrings.kvm_version + APP_VERSION + NEW_LINE_TAG
            + window.CommonStrings.plugin_version + SOC_VERSION + window.CommonStrings.for
            + SOC_NAME + NEW_LINE_TAG + window.CommonStrings.copyright;
		},

		/**
		  Creates the KVM web socket
		 */
		createWebSocket: function(){
			var _parent = this;
			if (this.bmc_ip.indexOf("[") != -1 && /Trident\/|MSIE /.test(window.navigator.userAgent)) {
				//for IE  ,IPV6 should be in "Literal IPv6 addresses in UNC path names"
				//request should be in following format
				// ws://2001-db8-100-f101-211-22ff-fe33-4444.ipv6-literal.net:85/kvm
				var ipaddr = location.hostname;

				ipaddr= ipaddr.split(":").join("-");        //replace : with -
				ipaddr = ipaddr.replace(/[\[\]]+/g, '');    //remove [ ]
				ipaddr += ".ipv6-literal.net";              //append at the end
				ipaddr += (location.port) ? (":"+location.port) : "";           // append : with port number
				this.ws = new WebSocket(this.ws_proto +ipaddr+"/kvm", ["binary", "base64"]);
			}
			else{
				//console.log("Connecing to : "+this.ws_proto + this.bmc_ip + "/kvm");
				this.ws = new WebSocket(this.ws_proto + this.bmc_ip + "/kvm", ["binary", "base64"]);
			}
			this.ws.binaryType = "arraybuffer";

			this.ws.onopen = function(e) {
				if (this.protocol) {
					_parent.wsmode = this.protocol;
				} else {
					_parent.wsmode = "base64";
				}
				_parent.onopen && _parent.onopen(e);
				if(_parent.is_reconnecting == true){
					_parent.retry_count = 1;
					//Show message on reconnect success.
					_parent.showProcessingPopup(window.CommonStrings.reconnect_success, 1);
				}
			};
			this.ws.onmessage = function(e) {

				_parent.pauseRead = true;

				if (_parent.pos == _parent.nwBuffer.byteLength) {
					delete _parent.nwBuffer;
					_parent.nwBuffer = null;
					_parent.nwBuffer = new DataStream();
					_parent.pos = 0;
				}

				var u8data = null;

				if (_parent.wsmode == "binary") {
					u8data = new Uint8Array(e.data);
				} else {
					u8data = Base64Binary.decode(e.data);
				}

				_parent.nwBuffer.writeUint8Array(u8data, DataStream.LITTLE_ENDIAN);
				_parent.pauseRead = false;


				//execute only once
				if (!_parent.startReading) {
					_parent.startReading = true;
					setTimeout(function() {
						_parent.startRead.call(_parent);
					}, 2);
				}

				_parent.onmessage && _parent.onmessage(e);
			};
			this.ws.onclose = function(e) {
				//console.log("Detected Network Loss!!!");
				if(window.reconnect_enabled == true &&
						_parent.video.stopped == false){
					_parent.is_reconnecting = true;
					sessionStorage.setItem("status", "reconnect");
					//Show messsage while reconnecting
					_parent.showProcessingPopup(window.CommonStrings.reconnect, null);
					// Check for any active media redirection
					if(MEDIA_REDIR.status == MEDIA_REDIR.CONNECTED){
						/* In case of media reconnect, GUI will kept unchanged.
						So that it will appear like as if it is connected.
						Updating the following flag for such purpose. */
						window.reconnectMedia = true;
						/* Sometimes during network loss media socket close event
						is not fired. So forcibly closing the media socket. */
						if (window.cdrom != null) {
							if (window.cdrom.sock != null) {
								window.cdrom.sock.close();
							}
						}
					}
				}
				if(_parent.is_reconnecting == true &&
						_parent.retry_count <= MEDIA_CONFIGS.reconn_retry_count){

					//console.log("Going for retry : "+_parent.retry_count);
					//Normally we try to reconnect after a delay specified by configured retry interval.
					//Since in some cases, the browser itself is detecting the socket failure after a delay of 15~20 seconds,
					//we are resrticting the minimum retry interval for H5Viewer to 20 seconds.
					//If the configured retry interval is more than MIN_RETRY_INTERVAL, then we will delay the
					//socket creation by a time interval equal to (configured retry interval - MIN_RETRY_INTERVAL).
					var retry_interval = 0;
					var date = new Date();
					var close_evt_time = date.getTime();
					//This calculation is to find out the difference between the time when socket on_close event was triggered,
					//and when network was actually disconnected. 
					var diff_time = (MIN_RETRY_INTERVAL*1000) - (close_evt_time - _parent.lastRecvTime);
					if(diff_time < 0)
						diff_time = 0;

					if(MEDIA_CONFIGS.reconn_retry_interval > MIN_RETRY_INTERVAL){
						retry_interval = MEDIA_CONFIGS.reconn_retry_interval - MIN_RETRY_INTERVAL;
					}
					//This calculation is to find out the time duration for which the socket creation should be delayed,
					//in order to match the retry interval.
					retry_interval = (retry_interval*1000) + diff_time;
					if(retry_interval < 0){
						retry_interval = 0;
					}
					this.retryTimer = setTimeout(function(){
						//The connecting message should be shown till the connection is 
						//established. So keeping the time duration value as null.
						_parent.ws = null;
						_parent.retry_count++;
						//Create new web socket
						_parent.createWebSocket();
						//Updating the last received time to facilitate the above failure time caluculation.
						_parent.updateLastRecvTime();
					}, (retry_interval));
					e.preventDefault();

				}
				else{
//					console.log("GOING TO CLOSE");
					//Show reconnect failure message if reconnect feature is enabled.
					if((window.reconnect_enabled == true) && (_parent.is_reconnecting == true)){
						//Display status message when reconnect fails.
						sessionStorage.setItem("status", "connection_failure");
						$("#status_dialog").trigger("showStatus");
					}
					else{
						_parent.onclose && _parent.onclose(e);
					}
				}
			};
			this.ws.onerror = function(e) {
//				console.log("<<< Video SOcket ERROR!!!")
				//Set is_reconnecting flag to true if reconnect feature is enabled
				if(window.reconnect_enabled == true){
					_parent.is_reconnecting = true
				}

				if(_parent.is_reconnecting == true &&
						_parent.retry_count < MEDIA_CONFIGS.reconn_retry_count){

					e.preventDefault();
				}
				else{
					_parent.onerror && _parent.onerror(e);
				}
			};
		},

		/**
		 * Sends video session token packet to adviser while initial connection.
		 * Sends connection complete packet filled with video session token while reconnecting.
		 */
		sendValidateVideoSessionPkt: function(){

			this.video.running = true;
			if(this.is_reconnecting == true){//On reconnect, send Connection Complete packet
				//console.log("Send Cmd CMD_CONNECTION_COMPLETE_PKT");
				var auth_pakt = new DataStream(IVTP.VIDEO_PACKET_SIZE+1);//374
				auth_pakt.writeCString(this.kvm_token, (IVTP.SSI_HASH_SIZE+1)); //token
				auth_pakt.writeCString(this.client_ip, IVTP.CLINET_OWN_IP_LENGTH); //ip
				auth_pakt.writeCString(this.uname, IVTP.CLIENT_USERNAME_LENGTH); //uname
				auth_pakt.writeCString(this.mac, IVTP.CLINET_OWN_MAC_LENGTH); //mac
				auth_pakt.writeUint8(this.current_session_id);
				//total data 374B
				//total with ivtp hdr 382B
				//console.log(token.length);
				auth_pakt.seek(0);
				this.is_reconnecting = false;
				var cnxn_complete = this.createHeader(IVTP.CMD_CONNECTION_COMPLETE_PKT, auth_pakt.byteLength, 1, auth_pakt.readUint8Array());
				this.send(cnxn_complete.buffer);

				delete auth_pakt;
				auth_pakt = null;
			}
			else{//Send Valiadte Video Session Token packet
				//Send connection complete packet if reconnect feature is enabled.
				if(window.reconnect_enabled == true){
					var cnxn_complete = this.createHeader(IVTP.CMD_CONNECTION_COMPLETE_PKT, 0, 1);
					this.send(cnxn_complete.buffer);
					delete cnxn_complete;
					cnxn_complete = null;
				}
				var that = this;
				// send the next packet with a delay so that the packets are not corrupted
				// this delay is added so that irrespective of whether the browser gets cpu or not,
				// these packets shall be sent with a delay and not get corrupted
				setTimeout(function() {
					var auth_pkt = new DataStream(IVTP.VIDEO_PACKET_SIZE);
					auth_pkt.writeUint8(0); //flag
					auth_pkt.writeCString(that.kvm_token, IVTP.SSI_HASH_SIZE); //token
					auth_pkt.writeCString(that.client_ip, IVTP.CLINET_OWN_IP_LENGTH); //ip
					auth_pkt.writeCString(that.uname, IVTP.CLIENT_USERNAME_LENGTH); //uname
					auth_pkt.writeCString(that.mac, IVTP.CLINET_OWN_MAC_LENGTH); //mac
					//total data 373B
					//total with ivtp hdr 381B
					//console.log(token.length);
					auth_pkt.seek(0);
					//The packet status should be set as 1, since it is H5Viewer session.
					var session = that.createHeader(IVTP.CMD_VALIDATE_VIDEO_SESSION, auth_pkt.byteLength, 1, auth_pkt.readUint8Array());
					that.send(session.buffer);

					delete auth_pkt;
					auth_pkt = null;

					//Send a resume command packet to get a full screen video buffer.
					var resume = that.createHeader(IVTP.CMD_RESUME_REDIRECTION, 0, 0);
					that.send(resume.buffer);
					delete resume;
					resume = null;
				}, 1);
			}
		},

		/** Invokes the processing popup
		 ** @param msg - Message to be displayed
		 ** @param duration - message display time duration.
		 ** If duration is not mentioned the the pop will be
		 ** displayed until hideProcessingPopup() is called.
		 **/
		showProcessingPopup: function(msg, duration) {
			if (msg == null) { // if no valid text is found then set the default text value
				msg = DEFAULT_PROCESSING_TEXT;
			}

			var oldProcessingContent = $("#processing_content").text().toString(); // preserve old text
			$("#processing_content").html(msg); // add the new text
			$("#processing_maindiv").show(); // show the popup
			var parent = this;
			if (duration != null && parseInt(duration) != NaN) { // if no valid int value is found then set the default timer value
				var timer = setInterval(function() { // start the countdown timer
					if (duration > 0) {
						duration--;
					}
					else {
						parent.hideProcessingPopup(timer, oldProcessingContent); // trigger the popup close event
					}
				},DIALOG_TIMER);
			}
		},

		/** Hides the processing popup
		 ** @param timer - setInterval object to be cleared
		 ** @param oldProcessingContent - text to be reverted in popup */
		hideProcessingPopup: function(timer, oldProcessingContent) {
			if(timer != null) { // if not null then stop the recursive function call
				clearInterval(timer);
			}
			if (oldProcessingContent == null) { // if no valid text is found then set the default text value
				oldProcessingContent = DEFAULT_PROCESSING_TEXT;
			}
			$("#processing_content").html(oldProcessingContent); // revert the text value
			$("#processing_maindiv").hide(); // hide the popup
		},
		
		/** Updates the last packet received time.
		*/
		updateLastRecvTime: function(){
			var date = new Date();
			this.lastRecvTime = date.getTime();
		}


};
if(typeof define=="function" && define.amd) {
    define(function(){
        return VideoSocket;
    });
}
