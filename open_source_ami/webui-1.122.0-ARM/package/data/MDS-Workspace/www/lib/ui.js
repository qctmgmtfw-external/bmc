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

gDebugShowElms=false;
gCXPDebug=false;
var isIE = window.ActiveXObject;

//get all GET variables
HTTP_GET_VARS = [];
var query = location.search.substr(1);
var vars = query.split('&');
for(var q=0; q<vars.length; q++)
{
	var tmp = vars[q].split('=');
	HTTP_GET_VARS[tmp[0]] = tmp[1];
}

delete query;
delete vars;

// Little function to take an array of IDs and expose them
// in the global DOM.  IDs should have a leading underscore,
// and they will be exposed without it, for example an ID of
// _txtStatus will be exposed globally as txtStatus
function exposeElms(array)
{
	if (location.pathname.indexOf("oem") != -1)	//OEM Feature
		top.gOemDir = "../../../";
	else
		top.gOemDir = "../";

	validateSession();

	if(array.length)
	{
		for (var i=0;i<array.length;i++)
		{
			var actualElm=array[i].substr(1);
			eval(actualElm+"=document.getElementById('"+array[i]+"');");
			if (gDebugShowElms)
			{
				eval(actualElm+".style.backgroundColor='#ffdf00';");	// Color the elms with yellow to show where they are
				eval(actualElm+".style.border='1px dashed purple';");	// Color the elms with yellow to show where they are
				eval(actualElm+".title='"+actualElm+"';");	// Show the elms ID on hover
			}
		}
	}
	commonPageListener();
}

HELP = "_hlp.html";
IMPL = "_imp.js";
STR  = "_str.js";

function commonPageListener()
{
	events.register('keydown',keyListener,document);
	events.register('mousedown',mouseListener,document);
	/* register click event for menus */
	if(parent.oMenu!=undefined)
	{
		events.register("click",parent.oMenu.closeAllNavigations,document);
		//document.onclick = parent.oMenu.closeAllNavigations;
		//events.register("resize",optimizedBody,window);
	}

	if(top.helpOpen)
	{
		top.helpFrame.location.href = getCurPageSection(HELP);
	}

	//showWait(false);
}

function getCurPageSection(section)
{
	switch(section)
	{
		case HELP:
			path = top.gHelpDir+top.gLangSetting+"/";
		break;
		case IMPL:
			path = top.gImplDir;
		break;
		case STR:
			path = top.gStrDir+top.gLangSetting+"/";
		break;
	}
	return '..'+path+top.mainFrame.pageFrame.curfile+section;
}

function validateSession()
{
	if(window.location.href.indexOf('http')!= -1)
	{
		if(window.location.pathname.indexOf('oem') != -1)	//OEM Feature
			xmit.get({url:"../../../../rpc/WEBSES/validate.asp",onrcv:validateSessResp, status:''});
		else
			xmit.get({url:"/rpc/WEBSES/validate.asp",onrcv:validateSessResp, status:''});
	}
}

var validateSessResp = function(arg)
{
	if(arg.HAPI_STATUS)
	{
		top.gLogout = 1;
		top.location.href = "/sessionexpired.asp";
	}
	else
	{
		try
		{
			top.gConsoleOpen = WEBVAR_JSONVAR_WEB_SESSION_VALIDATE.WEBVAR_STRUCTNAME_WEB_SESSION_VALIDATE[0].JViewerCount;
		}catch(e)
		{
			top.gConsoleOpen = 0;
		}
	}
}


function packWithRoundedTable(htmlObj)
{
	var tbl = document.createElement('table');
	var tbdy = document.createElement('tbody');
	tbl.appendChild(tbdy);

	cellspacing = document.createAttribute('cellspacing');
	cellspacing.nodeValue = 0;
	cellpadding = document.createAttribute('cellpadding');
	cellpadding.nodeValue = 0;
	tbl.setAttributeNode(cellspacing);
	tbl.setAttributeNode(cellpadding);
	tbl.border = '0';

	var trTop = document.createElement('tr');
	tbdy.appendChild(trTop);

	var tdl = trTop.insertCell(0);
	tdl.className = 'roundedtable_topleft';
	var tdm = trTop.insertCell(1);
	tdm.className = 'roundedtable_top';
	var tdr = trTop.insertCell(2);
	tdr.className = 'roundedtable_topright';

	var trMid = document.createElement('tr');
	tbdy.appendChild(trMid);

	var tdl = trMid.insertCell(0);
	tdl.className = 'roundedtable_left';
	var tdm = trMid.insertCell(1);
	tdm.appendChild(htmlObj);
	var tdr = trMid.insertCell(2);
	tdr.className = 'roundedtable_right';

	var trBtm = document.createElement('tr');
	tbdy.appendChild(trBtm);

	var tdl = trBtm.insertCell(0);
	tdl.className = 'roundedtable_btmleft';
	var tdm = trBtm.insertCell(1);
	tdm.className = 'roundedtable_btm';
	var tdr = trBtm.insertCell(2);
	tdr.className = 'roundedtable_btmright';

	return tbl;
}

function copyProps(target, src)
{
	for (var i in src)
	{
		if (typeof src[i] =='object' )
		{
			copyProps(target[i],src[i]);
		}
		else
		{
			target[i]=src[i];
		}
	}
}


//______________________________________________________________________________
//==============================================================================
//                                 showWait
//------------------------------------------------------------------------------
//

function showWait(enable, text)
{
	if ((text==undefined)||(text=="") )
	{
		text= eLang.getString('common',"STR_WAIT")
	}
	if(parent==top.mainFrame)
	{
		top.mainFrame.loader.style.display = enable?"block":"none";
		top.mainFrame.loaderStatus.innerHTML = text;
	}
}

if(top!=self)
{
	if(self==top.mainFrame.pageFrame)
	{
		function loading_next()
		{
			showWait(true);
		}
		parent.oMenu.highLight(location);
		events.register('unload',loading_next,window);
	}
}

function findCurrentLink(item)
{
	if(item.href==location.href)
	{
		parent.oMenu.highLight(item);
	}
}

function MessageBox(title, text, btnArgs, withoutClose, topvalue)
{
	disableActions();
	withoutClose = (withoutClose) ? withoutClose : false;
	var descFrame = getDescFrame(withoutClose, topvalue);
	var heading = document.createElement('h3');
	heading.innerHTML = title;

	var description = document.createElement('p');
	description.appendChild(text);

	var btnTray = document.createElement('div');
	btnTray.className = 'btnTray';
	for(var i=0; i<btnArgs.length; i++)
	{
		btnTray.appendChild(btnArgs[i]);
	}

	descFrame.appendChild(heading);
	descFrame.appendChild(description);
	descFrame.appendChild(btnTray);
	if (withoutClose != true)
		reloadHelp();

	return descFrame;
}

function getDescFrame(withoutClose, topvalue)
{
	if($("descFrame")) {
		return $("descFrame");
	} else {
		var hoverDiv = document.createElement("div");
		hoverDiv.id = "hoverDiv";
		hoverDiv.style.background = "#FFF";
		hoverDiv.style.position = "absolute";
		hoverDiv.style.left = "0px";
		hoverDiv.style.top = "0px";
		if(isIE)
		{
			hoverDiv.style.filter = "alpha(opacity=75)";
		}else
		{
			hoverDiv.style.opacity = .75;
		}
		hoverDiv.style.width ="100%";
		hoverDiv.style.height =document.body.scrollHeight+"px";

		var descFrame = document.createElement("div");
		descFrame.className = "eventDescription";
		descFrame.id = "descFrame";

		if(withoutClose!=true)
		{
			var closeBtn = document.createElement("a");
			closeBtn.innerHTML = "<img src='" + top.gOemDir + "res/widget_close.gif' border='0'/>";
			closeBtn.className = "floatRight";
			closeBtn.style.width = "15px";
			closeBtn.href = "javascript://";
			descFrame.close = closeBtn.onclick = function()
			{
				document.body.removeChild($("hoverDiv"));
				document.body.removeChild($("descFrame"));
				reloadHelp();
				if(descFrame.onclose)
				{
					descFrame.onclose();
				}
			}
			descFrame.appendChild(closeBtn);
		}

		document.body.appendChild(hoverDiv);
		document.body.appendChild(descFrame);
		positionDescFrame(topvalue);
		return descFrame;
	}
}

function positionDescFrame(topvalue)
{
	var ch;
	var cw = document.body.clientWidth;
	topvalue = (topvalue == undefined) ? 100 : topvalue;
	ch = document.body.scrollTop + topvalue;

	if(!$("descFrame")) {
		return;
	} else {
		var df = $("descFrame");
		df.style.width = "75%";
		df.style.left = ((cw / 2) - (df.offsetWidth / 2)) + "px";
		df.style.top = ch + "px";
	}
}

function p(str)
{
	var para = document.createElement("p");
	para.innerHTML = str;
	return para;
}

function createButton(name,value,fnPtr)
{
	var btn = document.createElement("input");
	btn.type = "button";
	btn.id = "_"+name;
	btn.name = name;
	btn.value = value;
	btn.onclick= fnPtr;
	return btn;
}

function $(id)
{
	return document.getElementById(id);
}

/**
 * Disable actions for all the form elements in the page.
 *
 * expArg = {
 *          	name: ['fieldName1','fieldName2'],
 *          	id:   ['fieldId1','fieldId2']
 *          }
**/

function disableActions(expArg)
{
	var fields = [];
	var expNameAry = "";
	var expIdAry = "";
	var i = 0;

	//alertTxt = ("bOpt = "+bOpt+"\n");
	//expArg is an optional Argument. In that either name or id can be null or undefined.
	if (expArg == undefined)
		expArg = {};
	if (expArg.name == undefined)
		expArg.name = "";
	if (expArg.id == undefined)
		expArg.id = "";

	alertTxt = ("expArg = "+expArg+"\n");

	// Get all the input, select and textarea fields
	var fInputs = document.getElementsByTagName("input");
	var fButtons = document.getElementsByTagName("button");
	var fSelects = document.getElementsByTagName("select");
	var fTextAreas = document.getElementsByTagName("textarea");

	fields.push(fInputs);
	fields.push(fButtons);
	fields.push(fSelects);
	fields.push(fTextAreas);


	for (i=0; i<expArg.name.length; i++) {
		expNameAry += expArg.name[i] + "|";
	}

	alertTxt += ("expNameAry = " + expNameAry + "\n");


	for (i=0; i<expArg.id.length; i++) {
		expIdAry += expArg.id[i] + "|";
	}

	alertTxt += ("expIdAry = "+expIdAry+"\n");

	for(var nType = 0; nType<fields.length; nType++) {
		alertTxt += ("FIELDS = "+fields[nType]+"\n");
		for(var nFld = 0; nFld<fields[nType].length; nFld++) {
			var eName = fields[nType][nFld].getAttribute("name");
			var eId = fields[nType][nFld].id;

			alertTxt += ("EName"+nFld+" = "+eName+"\n");
			alertTxt += ("EId"+nFld+" = "+eId+"\n");

			/*
			if the elements are not in expectional name array
			and the elements are not in expectional id array
			or expectional name and id array are empty then
			do enable/disable according to bOpt
			*/
			if((expNameAry.indexOf(eName+"|")==-1 &&
				expIdAry.indexOf(eId+"|")==-1) ||
				(expArg.name.length==0 && expArg.id.length==0)) {
				alertTxt += ("Going to disable element "+fields[nType][nFld].nodeName+"\n");
				fields[nType][nFld].disabled = true;
			} else {
				// if the element is in expectional name array
				// do the negation of the bOpt
				// disable take negation by default so -(-)=+
				alertTxt += ("Excluding element "+fields[nType][nFld].nodeName+" from disable operation"+"\n");
				fields[nType][nFld].disabled = false;
			}
		}
	}
}

function keyListener(e)
{
	var i,len;
	if(116 == e.keyCode) //F5 Key = 116
	{
		if(isIE)
		{
			top.mainFrame.pageFrame.location.href = "dashboard.html";
			event.returnValue = false; 
			event.keyCode = 0; 
		}
		else	
			top.gUserPressedF5 = true;
	}

	if(27 == e.keyCode) //ESC Key
		if($("descFrame"))
			if($("descFrame").close)
				$("descFrame").close();
}

function mouseListener(e)
{
	if (e.button == 2)		//Blocking Mouse Right click
		return false;
}

function sslCertUpload(alertstr)
{
	if(top.gSSLCert)
	{
		if(!confirm(eLang.getString('common',alertstr)))
			return false;
		else
			top.gSSLCert = false;
	}
	return true;
}

function reloadHelp()
{
	if (top.helpOpen) {
		//alert(top.helpOpen);
//		top.helpFrame.location.reload();
		top.helpFrame.location.href = getCurPageSection(HELP);

	}
}

//=======================================
if(debugWnd==undefined && gCXPDebug){
var debugWnd = window.open('','debugger','width=640, height=480, scrollbars=yes');
}
function _debug(str)
{
	if(debugWnd == undefined || !gCXPDebug) return;
	debugWnd.document.writeln("<br>===========================================================");
	//debugWnd.document.writeln("<br><b style='color:#009900'>Called from : "+window.location.href+"</b><br>");
	//debugWnd.document.writeln("<i style='color:#0000CC'>Function Name: "+arguments.caller.name+' <br><br> '+arguments.caller.toString()+"</i>");
	//debugWnd.document.writeln("<br>************************************************************<br>");
	debugWnd.document.writeln(nl2br(str));
	debugWnd.document.writeln("<br>===========================================================");

	debugWnd.scrollTo(0,1000000);
}

function nl2br(str)
{
	return str.replace(/\n/g,"<br>").replace(/\s/g,"&nbsp;");
}

var fnCookie = {
	read: function(name)
	{
		var nameEQ = name + "=";
		var ca = top.document.cookie.split(';');
		for(var i=0;i < ca.length;i++) {
			var c = ca[i];
			while (c.charAt(0)==' ') c = c.substring(1,c.length);
			if (c.indexOf(nameEQ) == 0) {
				var cookieval = c.substring(nameEQ.length,c.length);
				if (cookieval == "true") {
					return true;
				} else if (cookieval == "false") {
					return false;
				} else {
					return cookieval;
				}
			}
		}
		return null;
	},
	create: function(name, value)
	{
		top.document.cookie = name+"="+value+"; path=/";
	},
	erase: function(name)
	{
		//set old to delete it
		top.document.cookie = name+"=; expires=Fri, 3 Aug 2001 20:47:11 UTC; path=/";
	}
};

/*
 * This function is used to stop the functionality of backspace key event.
 * @return boolean, false-if backspace event occured.
 */
function checkBackspace() {
	var keycode;
	if (window.event) {
		keycode = window.event.keyCode;
		if(keycode == 8) {	//8-backspace keycode
			return false;
		}
	}
}

/*
 * This function is used to clear the contents of the table tag, by deleting 
 * each row one by one.
 * @param tblId object, ID of the HTML Table object
 */
function clearTable(tblId)
{
	try{
		while (tblId.rows.length) {
			tblId.deleteRow(0);
		}
	} catch(e) {
		alert("table" + e);
	}
}

ui={loaded:true};

