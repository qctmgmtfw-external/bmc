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
            "click #download":"launch"
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

            if(!app.isKVM() ){
                $("#download").attr("disabled","disabled");
                $(".alert-danger").removeClass("hide");
                $(".alert-danger").html(locale.kvm_disabled);
            }
            
        },
        launch:function(){
            var win;
            var strpath=app.urlPath() +"#remote-control";
            var is_chrome = navigator.userAgent.toLowerCase();
            is_chrome = (is_chrome.indexOf("chrome") != -1);
            if (is_chrome) {
               win= window.open(strpath,'H5Viewer',"toolbar=no, resizable=no,width=500, height=500, left=450, top=270");
               win.moveTo(0,0);
               win.resizeTo(screen.availWidth,screen.availHeight);
            } else {
                window.open(strpath,'H5Viewer',"fullscreen"); //fullscreen will work in IE and FF
            }
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
