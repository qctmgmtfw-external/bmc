define(["cc", "app",
    //data

    "collection/notifications",
    "collection/messages",
    "models/notification",
    "models/message",
    //localize
    "i18n!strings/nls/error"
], function(CC, app,
    NotificationCollection, MessageCollection,
    NotificationModel, MessageModel, errorStrings) {

    var error_handler = function(xhr, textStatus, errorThrown) {
        var NL = "\n";
        var msg = "";
        var code = 0;
        var errorLog = 0;

        switch (xhr.status) {
            case CC.HTTP.UNAUTHORIZED:
                var Cookiename = "refresh_disable";
                var regexp = new RegExp("(?:^" + Cookiename + "|;\s*"+ Cookiename + ")=(.*?)(?:;|$)", "g");
                var CookieVal = regexp.exec(document.cookie);
                var myCookie = (CookieVal === null) ? null : CookieVal[1];
                if(myCookie == 1){
                alert("Session Expired");
                    document.cookie = "refresh_disable=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                }
                document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                //app.router.navigate("login");
                location.href = "#login";
                location.reload();
                break;
            case CC.HTTP.NO_PRIVILEGE_ACCESS:
                //app.hidePage();
                errorLog = 1;
                msg = "";
                alert(errorStrings["non_privilege_user_error_message"]);
                break;
            case CC.HTTP.NOT_FOUND:
                msg = ("Requested resource not found")+NL;
                break;
            case CC.HTTP.FEATURE_NOT_ENABLED:
                errorLog = 2;
                msg = errorStrings["feature_not_enabled_error_message"];
                alert(msg);
                break;
            case CC.HTTP.SERVER_ERROR:
                msg = "System error encountered."+NL;
                break;
            //Quanta---
            case CC.HTTP.STATUS_OK:
            	console.log("Unknow case happened, it's should be ok. Status code: " + xhr.status);
            		return;
            	break;
            //---Quanta    
            default:
				msg = "Network or Unknown error encountered, maybe 0x2ef3 error."+NL+ " [status: " + xhr.status + "]";;
                break;
        }

        if(xhr.responseJSON) {
            code = xhr.responseJSON.code
        } else {
            code = xhr.status;
        }

        if(errorLog != 0){
            var redirectURL = location.hash;
            if(redirectURL.indexOf("maintenance") > -1){
                location.href = "#maintenance";
            }
            else if(redirectURL.indexOf("settings") > -1){
                location.href = "#settings";
            }else{
                location.href = "#dashboard";
            }
        }else{
            var error_alert = "";
            var temp = "error_code_"+code;
            var error_alert = errorStrings[temp];
            if(error_alert && $('.alert-danger').length>0){
                $('.alert-danger').removeClass("hide");
                $('.alert-danger').html(error_alert);
            }
        }



        if (xhr.responseJSON) {
            switch (xhr.responseJSON.code) {
                case CC.KVM_TOKEN_GEN_FAILED:
                    if(errorLog != 2){
                        msg +=  xhr.responseJSON.error;
                    }
                    msg += NL + " [code: " + xhr.responseJSON.code + "]";
                    break;
                default:
                    if(errorLog != 2){
                        msg += xhr.responseJSON.error;
                    }
                    msg += NL + " [code: " + xhr.responseJSON.code + "]";
                    break;
            }
        } else {

        }

        NotificationCollection.add(new NotificationModel({
            id: code+(new Date()).getTime(),
            message_group: "failure",
            message: msg,
            severity: "danger"
        }));

        //alert(msg);

    };

    return error_handler;
})
