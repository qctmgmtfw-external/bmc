define(['jquery', 'underscore', 'backbone', 'app',
'models//adviser',
//localize
'i18n!strings/nls/remote_control',
//template
'text!templates/remote_popup.html'
],

function($, _, Backbone, app, AdviserModel,locale, RemotePopUpTemplate) {

    var view = Backbone.View.extend({

        template: _.template(RemotePopUpTemplate),

        initialize: function() {
            this.adviserModel= new AdviserModel();
        },

        events: {
            "click #download":"launch",
            "click #downloadKVM": "download"
        },

        beforeRender: function() {

        },

        afterRender: function() {

            this.adviserModel.fetch({
                success: function(model) {
                    //console.log(model);
                    var kvm_service = model.get("status");
                     // console.log("test kvm_service"+kvm_service);
                      
                      if(kvm_service==0){
                        $("#download").attr("disabled","disabled");
                        $(".alert-danger").removeClass("hide");
                        $(".alert-danger").html(locale.kvm_service_disabled);
                    }
                }
            });

            if(window.kvmWindow){
                if(window.kvmWindow.closed){
                    $("#download").removeAttr("disabled","disabled");
                }else{
                    $("#download").attr("disabled","disabled");
                }
            }else{
                $("#download").removeAttr("disabled","disabled");
            }

            if(!app.isKVM() ){
                $("#download").attr("disabled","disabled");
                $(".alert-danger").removeClass("hide");
                $(".alert-danger").html(locale.kvm_disabled);
            }
            
        },
        launch:function(){
            var win;
            var strpath=app.urlPath() +"viewer.html";
            var is_chrome = navigator.userAgent.toLowerCase();
            is_chrome = (is_chrome.indexOf("chrome") != -1);
            $("#download").attr("disabled","disabled");
            /* IE11 Specific fix.
            ** When opening H5Viewer popup window,
            ** sessionStorage object is not transferred from the WebUI to H5Viewer window.
            ** So copying the values to localStorage.
            ** When H5Viewer window is opened, check for sessionStorage object.
            ** If it contains null values, then update the required values from localStorage.
            ** Clear the localStorage afterwards.*/
            if ( navigator.userAgent.indexOf('.NET') !== -1 ) { // If navigator.userAgent contains .NET then the browser is IE
                this.updateStorage();
            }
            if (is_chrome) {
               win= window.open(strpath,'H5Viewer',"toolbar=no, resizable=no,width=500, height=500, left=450, top=270");
               win.moveTo(0,0);
               win.resizeTo(screen.availWidth,screen.availHeight);
            } else {
               win = window.open(strpath,'H5Viewer'," fullscreen=yes, toolbar=no, resizable=yes, scrollbars=yes"); //fullscreen will work in IE and FF
            }
            window.kvmWindow = win; // Saving the window object in order to close the KVM Window after web session logout.
        },
        /* Function used to copy the sessionStorage object to localStorage */
        updateStorage: function(){
            var sessionStorageItems = [
                'extended_privilege',
                'features',
                'garc',
                'id',
                'kvm_access',
                'privilege',
                'privilege_id',
                'session_id',
                'since',
                'username',
                'vmedia_access'
            ];
            sessionStorageItems.forEach( function(item){
                if (item != null) localStorage.setItem(item, sessionStorage.getItem(item));
            });
        },

        download: function() { 
	        windowURL = window.location.href.split("/");
	        protocol =  windowURL[0];
	        protocol =  protocol.replace(/\:/g,"");
	        windowURL = windowURL[2];
	        if (windowURL.indexOf("]") != -1) {
	            externalIP = windowURL.split("]");
	            externalIP = externalIP[0] + "]";
	        } else {
	            externalarr = windowURL.split(":");
	            externalIP = externalarr[0];
	        }
	        $.ajax({
	            type: "GET",
	            url: "/api/kvmjnlp?" + "&JNLPSTR=" + "JViewer",
	            success: function(response, status, xhr) {
	                var filename = "jviewer.jnlp";
	                var blob = new Blob([response], {
	                    type: 'application/x-java-jnlp-file'
	                });
	                if (typeof window.navigator.msSaveBlob !== 'undefined') {
	                    window.navigator.msSaveBlob(blob, filename);
	                } else {
	                    var URL = window.URL || window.webkitURL;
	                    var downloadURL = URL.createObjectURL(blob);
	                    var a = document.createElement("a");
	                    if (typeof a.download == "undefined") {
	                        window.location = downloadURL;
	                    } else {
	                        $(a).attr("href", downloadURL);
	                        $(a).attr("download", filename);
	                        $(a).attr("target", "_blank");
	                        $(a).css({
	                            "position": "absolute",
	                            "opacity": "0"
	                        });
	                        document.body.appendChild(a);
	                        a.click();
	                    }
	                }
	            },
	            error: function(xhr, textStatus, errorThrown) {         
	            }
	        });
	    },
        load: function() {
			
        },
        add: function(model, collection, options) {

        },

        reset: function(model, collection, options) {

        },

        removeModel: function(model, collection, options) {

            
        },

        change: function(model, collection, options) {

            
        },

        affect: function(model, collection, options) {

        },

        reload: function(model, value) {

        },
        serialize: function() {
            return {
                locale: locale
            };
        }

    });

    return view;

});
