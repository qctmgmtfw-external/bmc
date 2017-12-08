--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/login_imp.js Thu Aug 30 13:29:34 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/login_imp.js Thu Aug 30 17:37:16 2012
@@ -43,6 +43,7 @@
 	fnCookie.erase("SessionCookie");
 	fnCookie.erase("Username");
 	fnCookie.erase("PNO");
+	fnCookie.erase("BMC_IP_ADDR");
 
 	$("fileDownload").innerHTML = "(How to " + top.IMG_HELP + ")";
 	$("fileDownload").href = "javascript:toggleLoginHelp();";
@@ -252,6 +253,9 @@
 	login_cookie = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].SESSION_COOKIE;
 	top.document.cookie = "SessionCookie="+login_cookie+";path=/";
 	top.document.cookie = "Language=EN;path=/";
+	
+	BMC_IP_ADDR = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].BMC_IP_ADDR;
+	top.document.cookie = "BMC_IP_ADDR=" + BMC_IP_ADDR + ";path=/";
 	if (navigator.cookieEnabled)
 	{
 		if (fnCookie.read("SessionCookie") != null)
