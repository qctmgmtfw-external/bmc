var IUSBSession = function() {};

IUSBSession.AUTH_USER_PKT_SIZE = 98;
IUSBSession.WEB_AUTH_PKT_MAX_SIZE = IUSBSession.AUTH_USER_PKT_SIZE;

if (typeof define == "function" && define.amd) {
    define(function() {
        return IUSBSession;

    });
}