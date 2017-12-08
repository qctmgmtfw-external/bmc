//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2009        **;
//;**                     ALL RIGHTS RESERVED                     **;
//;**                                                             **;
//;**  This computer software, including display screens and      **;
//;**  all related materials, are confidential and the            **;
//;**  exclusive property of American Megatrends, Inc.  They      **;
//;**  are available for limited use, but only pursuant to        **;
//;**  a written license agreement distributed with this          **;
//;**  computer software.  This computer software, including      **;
//;**  display screens and all related materials, shall not be    **;
//;**  copied, reproduced, published or distributed, in whole     **;
//;**  or in part, in any medium, by any means, for any           **;
//;**  purpose without the express written consent of American    **;
//;**  Megatrends, Inc.                                           **;
//;**                                                             **;
//;**                                                             **;
//;**                American Megatrends, Inc.                    **;
//;**           5555 Oakbook Parkway, Building 200                **;
//;**     Norcross,  Georgia - 30071, USA. Phone-(770)-246-8600.  **;
//;**                                                             **;
//;*****************************************************************;
//;*****************************************************************;

var gSettingsCookieStr = [];

function doInit()
{
	// TODO: add page initialization code
	//first check what the reason is for us to be in the login page
	if (top.headerFrame.$("userDetails") != undefined)
		top.headerFrame.$("userDetails").innerHTML = "&nbsp;";
	xmit.get({url:"/rpc/WEBSES/logout.asp",onrcv:function(){}});

	//clear previous lastpage cookies

	fnCookie.erase("lastNav");
	fnCookie.erase("lastPage");
	fnCookie.erase("lItem");
	fnCookie.erase("lastHiLit");
	fnCookie.erase("SessionCookie");
	fnCookie.erase("Username");
	fnCookie.erase("PNO");
	fnCookie.erase("BMC_IP_ADDR");

	$("fileDownload").innerHTML = "(How to " + top.IMG_HELP + ")";
	$("fileDownload").href = "javascript:toggleLoginHelp();";
	updatePopupBlocker();
	commonPageListener();

	xmit.get({url:"/rpc/WEBSES/validate.asp",onrcv:validatePrevLoginRes, status:""});
	$("login_username").focus();

	$("login_username").onkeypress = checkEnter;
	$("login_password").onkeypress = checkEnter;

	top.gWebServer = fnCookie.read("WebServer");
	if (top.gWebServer == undefined || top.gWebServer == null || top.gWebServer == "")
	{
		getWebServerInfo();
	}
}

function openLoginHelp()
{
	var fileAry = top.mainFrame.location.pathname.split("/");
	var file = fileAry[fileAry.length-1];
	var path = top.gHelpDir + top.gLangSetting + "/" + file.split(".")[0] + top.mainFrame.HELP;
	top.openHelp(path);
}

function toggleLoginHelp()
{
	if(top.helpOpen)
		top.closeHelp();
	else
		openLoginHelp();
}

function updatePopupBlocker()
{
	var is_chrome; //to detect specific chrome browser.
	try
	{
		var wnd = window.open("","","width=10,height=10,scrollbars=no");
		if(!wnd)
		{
			$("popupBlock").innerHTML = top.IMG_WRONG;
		} else {
			is_chrome = navigator.userAgent.toLowerCase();
			is_chrome = (is_chrome.indexOf("chrome") != -1);
			if (is_chrome) {
				setTimeout(function() {
					if ((wnd.innerWidth == 0) && (wnd.innerHeight == 0)) {
						$("popupBlock").innerHTML = top.IMG_WRONG;
					} else {
						wnd.close();
						$("popupBlock").innerHTML = top.IMG_RIGHT;
					}
					wnd = null;
				}, 100);
			} else {
				wnd.close();
				$("popupBlock").innerHTML = top.IMG_RIGHT;
			}
		}
	}catch(e)
	{
		$("popupBlock").innerHTML = top.IMG_WRONG;
	}
}

function getWebServerInfo()
{
	xmit.head("/", getWebServerInfoRes);
}

function getWebServerInfoRes(arg)
{
	var headerinfo = arg.responseHeaders;
	for (var i=0; i<headerinfo.length; i++)
	{
		if (headerinfo[i].indexOf("Server") != -1)
		{
			top.document.cookie = "WebServer=" + headerinfo[i] + ";path=/";
			break;
		}
	}
}

function disableBtn(bool)
{
	with(document.forms["login"].btnLogin)
	{
		disabled = bool;
		if(bool)
			value = "Logging in..";
		else
			value = "Login";
	}
}

function validate(frm)
{
	disableBtn(true);

	if(frm.username.value=="")
	{
		alert(eLang.getString("common","STR_USER_REQUIRED"));
		disableBtn(false);
		return;
	}
	if(frm.username.value.length>800)
	{
		alert(eLang.getString("common","STR_USER_LENGTH"));
		disableBtn(false);
		return;
	}
	if(frm.password.value=="")
	{
		alert(eLang.getString("common","STR_PWD_REQUIRED"));
		disableBtn(false);
		return;
	}
	if(frm.password.value.length>300)
	{
		alert(eLang.getString("common","STR_PWD_LENGTH"));
		disableBtn(false);
		return;
	}
	if (!logincalled) {
		if (checkBrowserSupport()) {
			doLogin();
		} else {
			$("msglbl").innerHTML = eLang.getString("common",
				 "STR_LOGIN_BWSRNOTSPRT");
		}
	}
}
/*
 * This function will check for the support of the Browser and its specific 
 * version by the software. 
 * @return boolean, true - Browser and its specific version is supported, 
 * false - Browser and its specific version is not supported.
 */
function checkBrowserSupport() 
{	
	var version; // holds Browser version number.
	var browserObj = {}; 
	 //Collection of BrowserName and its supported versions.
	browserObj = [{name : "Firefox", version : 2.0, split : ""},
		{name : "Chrome", version : 2.0, split : ""},
		{name : "Opera", version : 9.64, split : ""},
		{name : "Safari", version : 3.0, split : "Version/"}];
	if (!window.XMLHttpRequest) { //check for IE 6.
		alert(eLang.getString("common", "STR_LOGIN_BWSRMSG"));
		return false;
	}
	for(var i in browserObj) {
		if(navigator.userAgent.indexOf(browserObj[i].name) != -1) {
			if (browserObj[i].split != "") { //Specific for Safari
				version = navigator.userAgent.split(browserObj[i].split);
			} else { 
				version=navigator.userAgent.split(browserObj[i].name + "/");
			}
			if (version[1] != null) {
				if (parseFloat(version[1]) < browserObj[i].version) {
					alert(eLang.getString("common", "STR_LOGIN_BWSRMSG"));
					return false;
				}
			}
		}
	}
	return true;
}

var doLogin = function()
{
	logincalled=true;

	//note that we have to give the full path for the RPC page here since we are still not initialized fully
	var login_rpc = xmit.getset({url:"/rpc/WEBSES/create.asp",onrcv:loginRes, status:"",timeout:60});
	login_rpc.add("WEBVAR_USERNAME",$("login_username").value);
	login_rpc.add("WEBVAR_PASSWORD",$("login_password").value);
	login_rpc.send();
}

function loginRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		switch(arg.HAPI_STATUS)
		{
			case 3:
				$("msglbl").innerHTML = eLang.getString("common","STR_LOGIN_ERROR_3");
			break;
			case 0xD4:
				$("msglbl").innerHTML = eLang.getString("common","STR_LOGIN_NOACCESS");
			break;
			default:
				$("msglbl").innerHTML = eLang.getString("common","STR_LOGIN_ERROR");
		}			
		//$("login_username").value = "";
		$("login_password").value = "";
		$("login_password").focus();
		logincalled=false;
		disableBtn(false);
		return;
	}
	logincalled = false;
	login_cookie = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].SESSION_COOKIE;
	top.document.cookie = "SessionCookie="+login_cookie+";path=/";
	top.document.cookie = "Language=EN;path=/";
	
	BMC_IP_ADDR = WEBVAR_JSONVAR_WEB_SESSION.WEBVAR_STRUCTNAME_WEB_SESSION[0].BMC_IP_ADDR;
	top.document.cookie = "BMC_IP_ADDR=" + BMC_IP_ADDR + ";path=/";
	if (navigator.cookieEnabled)
	{
		if (fnCookie.read("SessionCookie") != null)
		{
			xmit.get({url:"/rpc/getrole.asp",onrcv:getLanInfo});
		}
		else
		{
			alert(eLang.getString("common","STR_FIRST_COOKIES_BLOCK"));
			logincalled=false;
			disableBtn(false);
		}
	}
	else
	{
		alert(eLang.getString("common","STR_COOKIES_ENABLE"));
		logincalled=false;
		disableBtn(false);
	}
}

/*
 * This is the response function for getrole RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the user information data to the cookie variable and invoke
 * the RPC method to get the LAN channel information. 
 * @param arg object, RPC response data from xmit library
 */
function getLanInfo(arg)
{
	var userDetails;		//RPC User information response data
	if(arg.HAPI_STATUS == 0) {
		userDetails =  WEBVAR_JSONVAR_GET_ROLE.WEBVAR_STRUCTNAME_GET_ROLE[0];
		top.document.cookie = "Username=" + userDetails.CURUSERNAME + ";path=/";
		top.document.cookie = "PNO=" + userDetails.CURPRIV + ";path=/";

		top.user.name = userDetails.CURUSERNAME;
		top.user.privilege = top.CONSTANTS.privilege[userDetails.CURPRIV];
		top.user.pno = userDetails.CURPRIV;
	} else {
		alert(eLang.getString("common", "STR_USER_ROLE_ERROR"));
	}
	xmit.get({url:"/rpc/getlanchannelinfo.asp", onrcv:getLanInfoRes,
		status:""});
}

/*
 * This is the response function for getLanInfo RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the lan information data to the cookie variable and invoke
 * the RPC method to get the project configuration. 
 * @param arg object, RPC response data from xmit library
 */
function getLanInfoRes(arg)
{
	var eth_obj = [];		//eth index value
	var eth_str = [];		//eth interface name
	var lan_obj = [];		//lan channel value
	var LANINFO;			//RPC LAN response data

	switch(arg.HAPI_STATUS) {
	case 0x0:
		LANINFO = WEBVAR_JSONVAR_GETLANINFO.WEBVAR_STRUCTNAME_GETLANINFO;
		for (i = 0; i < LANINFO.length; i++) {
			eth_obj.push(LANINFO[i].ETH_INDEX);
			eth_str.push("\'" + LANINFO[i].INTERFACE_NAME + "\'");
			lan_obj.push(LANINFO[i].CHANNEL_NUM);
		}

		if (eth_obj.length > 1) {
			top.gMultiLAN = true;
			top.document.cookie = "gMultiLAN=true;path=/;";
		} else {
			top.gMultiLAN = false;
			top.document.cookie = "gMultiLAN=false;path=/;";
		}

		gSettingsCookieStr.push("eth:[" + eth_obj.join(",") + "]");
		gSettingsCookieStr.push("ethstr:[" + eth_str.join(",") + "]");
		gSettingsCookieStr.push("lan:[" + lan_obj.join(",") + "]");
	break;

	case 0x1D4:
	break;

	default:
		alert(eLang.getString("common", "STR_LAN_CHANNEL_ERROR"));
	break;
	}
	xmit.get({url:"/rpc/getprojectcfg.asp", onrcv:getProjectConfigRes, 
		status:""});
}

function getProjectConfigRes(arg)
{
	if (arg.HAPI_STATUS != 0){
		alert(eLang.getString("common","STR_PROJECT_CFG_ERROR"));
	}
	else
	{
		PRJ_CFG = WEBVAR_JSONVAR_GETPROJECTCFG.WEBVAR_STRUCTNAME_GETPROJECTCFG;

		var strFeatures = "";
		for (i=0;i<PRJ_CFG.length;i++)
		{
			if(!PRJ_CFG[i].FEATURES)
				continue;
			strFeatures += PRJ_CFG[i].FEATURES + ",";
		}
		if (strFeatures.length > 1){
			gSettingsCookieStr.push("features:'"+strFeatures+"'");
		}
	}
	loadMain();
}

function loadMain()
{
	disableBtn(false);
	top.document.cookie = "settings={" + gSettingsCookieStr.join(",") + "};path=/";
	eval("top.settings = " + top.fnCookie.read("settings"));
	if (top.HTTP_GET_VARS["error"] != undefined) {
		top.location.href = "/index.html";
	} else {
		document.location = "main.html";
	}
}

function validatePrevLoginRes(arg)
{
	errcod = arg.HAPI_STATUS;
	var sessionexpire = top.fnCookie.read("SessionExpired");
	if (sessionexpire != undefined && sessionexpire != null && sessionexpire == "true")
	{
		$("msglbl").innerHTML = eLang.getString("common","STR_LOGIN_SESSION_EXPIRED");
		with($("msglbl").style)
		{
			color = "#900";
		}
		top.document.cookie = "SessionExpired=false;path=/";
	}
	else if(errcod != 0 && errcod != 9 )
	{
		$("msglbl").innerHTML = eLang.getString("common","STR_LOGIN_ERROR");
		with($("msglbl").style)
		{
			color = "#900";
		}
	}else
	{
		$("msglbl").innerHTML = "";//eLang.getString("common","STR_LOGIN");
		with($("msglbl").style)
		{
			color = "#354968";
		}
	}
}

function checkEnter(e)
{
	if(!e) e = window.event;

	if(e.keyCode==13)
	{
		if(!$("LOGIN_VALUE_1").disabled)
			validate(document.forms["login"]);
	}
}

logincalled=false;

function forgotPassword()
{
	if($("login_username").value=="")
	{
		alert(eLang.getString("common","STR_USER_REQUIRED"));
		disableBtn(false);
		return;
	}
	if (confirm(eLang.getString("common","STR_RESET_PSWD_CONFIRM")))
	{
		var req = new xmit.getset({url:"/rpc/WEBSES/forgotpassword.asp",onrcv:forgotPasswordRes, status:"",timeout:60});
		req.add("USERNAME",$("login_username").value);
		req.send();
		delete req;
	}
}

function forgotPasswordRes(arg)
{
	var returncode = arg.HAPI_STATUS & 0xff;
	switch (returncode)
	{
		case 0x0:
			alert (eLang.getString("common","STR_RESET_PSWD_SUCCESS"));
		break;
		case 0x12:
		case 0x13:
		case 0x14:
			//Possible SMTP Failures
			alert (eLang.getString("common","STR_SMTP_FAILURE" + (returncode - 0x11)));
		break;
		case 0x3:
			alert (eLang.getString("common","STR_EMAIL_FAILURE1"));
		break;
		case 0x4:
			alert (eLang.getString("common","STR_RESET_PSWD_FAILURE1"));
		break;
		case 0x10:
			alert (eLang.getString("common","STR_EMAIL_FAILURE2"));
		break;
		case 0x15:
			alert (eLang.getString("common","STR_EMAIL_FRGT_PSWD"));
		break;
		default:
			errstr = eLang.getString("common","STR_RESET_PSWD_FAILURE");
			errstr += eLang.getString("common","STR_IPMI_ERROR") + returncode;
			alert (errstr);
	}
}
