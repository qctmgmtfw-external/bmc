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

var gConsoleOpen;
var gSSLCert = false;
var gMultiLAN = false;
var gUserPressedF5 = false;
var gPEFInfo;
var PRJ_CFG;

function doInit()
{
	loadFrames();
	application_init();
}

function application_init()
{
	gLogout = false;
	gUserPressedF5 = false;
	gConsoleOpen = false;
	var sessionexpire = fnCookie.read("SessionExpired");

	if((fnCookie.read("SessionCookie") == "") && (sessionexpire != undefined && 
		sessionexpire != null && sessionexpire == "true")) {
		gLoggedOut = false;
	} else {
		gLoggedOut = true;
	}
	gFlashMode = false;

	events.register("beforeunload", application_exit, window);
	events.register("keydown", keyListener, document);
	events.register("mousedown", mouseListener, document);
}

function $(id)
{
	return document.getElementById(id);
}

function getCurFile(lObj)
{
	var fileAry = lObj.pathname.split('/');
	var file = fileAry[fileAry.length-1];

	return	file.split('.')[0];
}

function toggleHelp()
{
	if(helpOpen)
		closeHelp();
	else
		openHelp();
}

helpOpen = false;
function closeHelp()
{
	$('frmHolder').cols = '*,0';
	helpOpen = false;
}

function getActiveConsoleStatus()
{
	xmit.get({url:"/rpc/getactiveconsolestatus.asp",onrcv:function(arg)
	{
		if(arg.HAPI_STATUS==0)
		{
			gConsoleOpen = WEBVAR_JSONVAR_CONSOLE_STATUS.WEBVAR_STRUCTNAME_CONSOLE_STATUS[0].ConsoleStatus;
		}
		confirmLogout();
	}});
}

function logoutWeb()
{
	headerFrame.$('userDetails').innerHTML = '&nbsp;';
	xmit.get({url:'/rpc/WEBSES/logout.asp',onrcv:function()
	{
		top.mainFrame.location.href='../page/login.html';
	}});
}

function confirmLogout()
{
	if (gConsoleOpen || gSSLCert)
	{
		logoutstr = eLang.getString('common',"STR_GENERAL_LOGOUT");
		if (gConsoleOpen)
		{
			logoutstr += "\n" + eLang.getString('common',"STR_WARNING") + eLang.getString('common',"STR_CONSOLE_CONNECTED");
			if (gSSLCert)
				logoutstr += "\n" + eLang.getString('common',"STR_SSLCERT_ABORT");
		}
		else if (gSSLCert)
		{
			logoutstr += "\n" + eLang.getString('common',"STR_WARNING") + eLang.getString('common',"STR_SSLCERT_ABORT");
		}
		if (!confirm(logoutstr))
		{
			return;
		}
		else
		{
			gSSLCert = false;
			gConsoleOpen = false;
		}
	}
	logoutWeb();
}

function openHelp(target)
{
	if(!helpOpen && target==undefined)
	{
		helpFrame.location.href = mainFrame.getCurPageSection(mainFrame.HELP);
	}
	if(target!=undefined)
	{
		helpFrame.location.href = target;
	}
	$('frmHolder').cols = '*,250';
	helpOpen = true;
}

function getBrowserLang()
{
	var lang = CONSTANTS.DEFAULT_LANG;
	if(navigator.userLanguage!=undefined)
	{
		lang = navigator.userLanguage;
	}else if(navigator.language!=undefined)
	{
		lang = navigator.language;
	}else
	{
		alert("Browser language could not be determined. Using English(US) by default");
	}

	var lary = lang.split('-');

	lang = lary[0].toUpperCase();

	if(eLang.title_strings[lang]!=undefined)
	{
		return lang;
	}else
	{
		return CONSTANTS.DEFAULT_LANG;  // default
	}
}

/* quick logout preparation */
function getMethod()
{
	var req;
	if(window.XMLHttpRequest)
	{
		req = new XMLHttpRequest();
	}else
	{
		if(window.ActiveXObject)
		{
			req = new ActiveXObject('Microsoft.XMLHTTP');
		}else
		{
			req = null;
		}
	}
	return req;
}

var greq = getMethod();
var flashreq = getMethod();

function unload_resp()
{
	if (greq.readyState == 4)
		gLoggedOut = true;
}

function unload_flash_resp()
{
	if (flashreq.readyState == 4)
		gLoggedOut = true;
}

if(greq)
{
	greq.open("get", "/rpc/WEBSES/logout.asp");
	greq.setRequestHeader("Content-type", "x-www-form-urlencoded");
	greq.onreadystatechange = unload_resp;
}

if(flashreq)
{
	flashreq.open("get", "/rpc/flash_browserclosed.asp");
	flashreq.setRequestHeader("Content-type", "x-www-form-urlencoded");
	flashreq.onreadystatechange = unload_flash_resp;
}

function application_exit()
{
	if (top.mainFrame.location.href.indexOf("login.html") == -1) {
	//Check if it is not login page, then clear the cache.
		if (gFlashMode) {
			flashreq.send(null);
			alert(eLang.getString("common", "STR_FWFLASH_ABORT"));
			top.onbeforeunload = function(){};
		} else {
			greq.send(null);
			if (gConsoleOpen || gSSLCert) {
				alert(eLang.getString("common", "STR_PROCESS_ABORT"));
			} else if(!gLogout && !gLoggedOut && !gUserPressedF5 && 
				!gFlashMode) {
				alert(eLang.getString("common", "STR_LOGOUT_SUCCESS"));
			}
			top.onbeforeunload = function(){};
		}
		if (!gUserPressedF5) {
			fnCookie.erase("SessionCookie");
		}
	}
}

/* end application close or refresh event */

CONSTANTS = {};

CONSTANTS.DEFAULT_LANG = "EN";
CONSTANTS.CALLBACK = 1;
CONSTANTS.USER = 2;
CONSTANTS.OPERATOR = 3;
CONSTANTS.ADMIN = 4;
CONSTANTS.OEM = 5;
CONSTANTS.NOACCESS = 0xf;
CONSTANTS.privilege = [];
CONSTANTS.privilege[0] = 'Reserved';
CONSTANTS.privilege[CONSTANTS.CALLBACK] = 'Callback';
CONSTANTS.privilege[CONSTANTS.USER] = 'User';
CONSTANTS.privilege[CONSTANTS.OPERATOR] = 'Operator';
CONSTANTS.privilege[CONSTANTS.ADMIN] = 'Administrator';
CONSTANTS.privilege[CONSTANTS.OEM] = 'OEM Proprietary';
CONSTANTS.privilege[CONSTANTS.NOACCESS] = 'No Access';
CONSTANTS.ERROR = {};
CONSTANTS.ERROR.SESSION_EXPIRED = 6;

CONSTANTS.IPSOURCE_DHCP = 0x02;
CONSTANTS.IPSOURCE_STATIC = 0x01;

CONSTANTS.ADD = 1;
CONSTANTS.MODIFY = 2;
CONSTANTS.DELETE = 3;

CONSTANTS.WEB_SERVICE_ID_BIT = 0;
CONSTANTS.KVM_SERVICE_ID_BIT = 1;
CONSTANTS.MEDIA_SERVICE_ID_BIT = 2;
CONSTANTS.SSH_SERVICE_ID_BIT = 3;
CONSTANTS.TELNET_SERVICE_ID_BIT = 4;
CONSTANTS.IPMI_SERVICE_ID_BIT = 5;

IMG_RIGHT = "<img src='../res/ok.png' border='0'>";
IMG_WRONG = "<img src='../res/crit.png' border='0'>";
IMG_HELP = "<img src='../res/helpicon.png' title='Help' border='0'>";

var user =
{
	name:'',
	privilege:'',
	pno:CONSTANTS.NOACCESS,

	isAdmin: function()
	{
		return (user.pno==CONSTANTS.ADMIN);
	},

	isOperator: function()
	{
		return (user.pno==CONSTANTS.OPERATOR);
	},

	isUser: function()
	{
		return (user.pno==CONSTANTS.USER);
	}
};

var settings =
{
	lan_channel:'',
	console:''
}
