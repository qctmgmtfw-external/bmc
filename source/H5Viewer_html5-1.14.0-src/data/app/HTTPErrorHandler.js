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
        var msg = null;
        var code = 0;
        switch (xhr.status) {
            case CC.HTTP.UNAUTHORIZED:
                alert("Session Expired");
                document.cookie = "QSESSIONID=1;expires=Thu, 01 Jan 1970 00:00:00 GMT";
                //app.router.navigate("login");
                location.href = "#login";
                location.reload();
                break;
           case CC.HTTP.NO_PRIVILEGE_ACCESS:
                //app.hidePage();
                alert(errorStrings["non_privilege_user_error_message"]);
                var redirectURL = location.hash;
                if(redirectURL.indexOf("maintenance") > -1){
                    location.href = "#maintenance";
                }
                else if(redirectURL.indexOf("settings") > -1){
                    location.href = "#settings";
                }else{
                    location.href = "#dashboard"; 
                }
                
                break;
            case CC.HTTP.NOT_FOUND:
                msg = ("Requested resource not found");
                break;
            case CC.HTTP.SERVER_ERROR:
                msg = "Server error encountered.";
            default:
                if (xhr.responseJSON) {
                    switch (xhr.responseJSON.code) {
                        case CC.KVM_TOKEN_GEN_FAILED:
                            msg += NL + xhr.responseJSON.error;
                            msg += NL + " [code: " + xhr.responseJSON.code + "]";
                            break;
                        default:
                            msg += NL + xhr.responseJSON.error;
                            msg += NL + " [code: " + xhr.responseJSON.code + "]";
                            break;
                    }
                } else {

                }
        }

        if(xhr.responseJSON) {
            code = xhr.responseJSON.code                
        } else {
            code = xhr.status;
        }

        var error_alert = "";
        var temp = "error_code_"+code;
        var error_alert = errorStrings[temp];
        if(error_alert && $('.alert-danger').length>0){
            $('.alert-danger').removeClass("hide");
            $('.alert-danger').html(error_alert);
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
