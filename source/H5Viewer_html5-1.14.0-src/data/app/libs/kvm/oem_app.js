var OEM_APP = function() {
};

OEM_APP.prototype = {

    // returns true if the ivtp command is handled here.
    // returns false if the ivtp command should be handled in h5viewer_common.
    isOEMCommand: function (headerType) {
	// sample code to handle oem ivtp command
	// if(headerType == IVTP.CMD_USB_MOUSE_MODE) {
	//     var mode = window._video.sock.newBuffer.readUint8()*1;
        //     if(window.mousemode != mode)
	// 	window._video.sock.UpdateMouseModeMenu(window.mousemode,mode);
	//     return true;
	// }
	return false;
    },

    // menu events shall be handled like this.
    
    // $("#element_id").click(function(ev){
    // 	ev.preventDefault();
    //  code to handle action on menu click goes here
    // });

    // NOTE: This function should return true incase if oem handles Auth functionality on HTML KVM launch
    isOEMAuthHandled: function(){
        //NOTE : handle start Media Button color and return true only if OEM handles Auth functionality
    	return false;
    },

    handleOEMAfterRender: function(){
    },
    
    handleOEMStartKVM: function(ev, app){

    },

    // NOTE: This function should return true incase if oem handles help content functionality.
    handleOEMHelpContent: function(){
        // By calling window._video.sock.setHelpContent(), OEM can customize the help contents.
        //
        // The parameter details for window._video.sock.setHelpContent(img_src, copyright_text, title_text) is given by,
        // -- img_src -> path of the image file to be used as logo. < Recommended logo image properties -> dimension:- 162 x 49 pixels  | type:- jpeg >
        // -- copyright_text -> text ( in html format ) to be displayed as help content.
        // -- title_text -> String value to be used as dialog title.
        //
        // Note: The model is invoked by pressing Help -> About H5Viewer.
        //       If OEM wants to customize the menu item text ( About H5Viewer ) then they should mention the following code in handleOEMStartKVM()
        //       $('#about').html( <Custom String> );
        return false;
    },

    /*
    * id  - button id
    * Name - 0 to 5 ,if Name if invalid default color(btn-default) is set
    * state - true/false
    */
    // NOTE: This function should return true if OEM handle Button color status.
    handleColor: function(id,Name,state) {
	// set color of the given id
        return false;
    },


    // sample get web session create.asp ajax call
    /*getWebSession: function() {
    	console.log("getWebSession")
        $.ajax({
                url:"http://10.0.124.230/rpc/WEBSES/create.asp",
                dataType: "text",
                type: "POST",
                data: {WEBVAR_USERNAME: "admin", WEBVAR_PASSWORD: "admin"},
                success: function(data) {
                	console.log("SUCCESS ON CREATE.ASP!!!!!!!!!!!");
                	console.log(data);
                    eval(data);
                    window.SESSION_COOKIE = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].SESSION_COOKIE;
                    document.cookie = "SessionCookie=" + window.SESSION_COOKIE + "; path=/";
                    window.CSRFTOKEN = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].CSRFTOKEN;
                    //getKVMToken(nostart);
                },
                error: function() {
                    alert("Authentication failed. Please check username and password");
                }
                
            });
    }*/
};

if (typeof define == "function" && define.amd) {
    define(function() {
        return OEM_APP;

    });
}

