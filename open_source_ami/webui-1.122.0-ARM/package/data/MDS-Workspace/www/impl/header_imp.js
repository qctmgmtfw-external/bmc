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

function doInit()
{
	// TODO: add page initialization code
	if(top.user.name=='')
	{
		top.user.name = top.fnCookie.read('Username');
		top.user.pno = top.fnCookie.read('PNO');
		top.user.privilege = top.CONSTANTS.privilege[top.user.pno];
		eval("top.settings = "+top.fnCookie.read('settings'));
	}
	initOemStyle();
}

function updateLogin()
{
	$('username').innerHTML = top.user.name;
	$('priv').innerHTML = " ("+top.user.privilege+")";
	$('logoutBtn').onclick = logout;
	$('refreshBtn').onclick = refresh;
	$('printBtn').onclick = framePrint;
}

function onlogin()
{
	$("userDetails").innerHTML = "<img src='../res/user.gif' border='0' "+
		"title='" + eLang.getString("common", "STR_USER_TOOLTIP") + "'/>" +
		"<span id='username'></span><small id='priv'></small>&nbsp;"+
		"<a href='javascript://' id='refreshBtn' title='" + 
		eLang.getString("common", "STR_REFRESH_TOOLTIP") + "'>" +
		"<img src='../res/refresh.gif' border='0' /> Refresh</a>&nbsp;"+
		"<a href='javascript://' id='printBtn' title='" + 
		eLang.getString("common", "STR_PRINT_TOOLTIP") + "'>" +
		"<img src='../res/print.gif' border='0' /> Print</a>&nbsp;"+
		"<a href='javascript://' id='logoutBtn' title='" + 
		eLang.getString("common","STR_LOGOUT_TOOLTIP") + "'>" +
		"<img src='../res/logout.gif' border='0'> Logout</a>";

	updateLogin();
}

function clearActions()
{
	with($('userDetails'))
	{
		removeChild($('refreshBtn'));
		removeChild($('printBtn'));
		removeChild($('logoutBtn'));
	}
}

function refresh()
{
	var urlPath = "";
	if (top.gSSLCert)
	{
		top.mainFrame.pageFrame.location.href = top.mainFrame.pageFrame.location;
		return;
	}

	var tabCtrl = top.mainFrame.pageFrame.document.getElementsByTagName("td");
	for(i = 0; i < tabCtrl.length; i++) {
		if ((tabCtrl[i].className == "classicTab") && 
			(tabCtrl[i].style.fontWeight == "bold")) {
			urlPath = top.mainFrame.pageFrame.location.pathname + "#";
			urlPath += tabCtrl[i].id + ",";
			break;
		}
	}
	var lstCtrl = top.mainFrame.pageFrame.document.getElementsByTagName("select");
	if ((lstCtrl.length >= 1) && (urlPath == "")) {
		urlPath = top.mainFrame.pageFrame.location.pathname + "#";
	}

	for(i = 0; i < lstCtrl.length; i++) {
		urlPath += lstCtrl[i].id + "=" + lstCtrl[i].value + ",";
	}

	top.mainFrame.pageFrame.location.href = urlPath;
	top.mainFrame.pageFrame.location.reload();

	//This flag will be used to reload the values for graph from an RPC in common_fn.js - Manoj (Bug: 11734)
	top.reloadValues = true;
}

function framePrint()
{
	var printFrame = parent.mainFrame.pageFrame;
	printFrame.focus();
	printFrame.print();
}

function logout()
{
	top.getActiveConsoleStatus();
	top.closeHelp();
}

function initOemStyle()
{
	xmit.head("/res/oem/banner_bg.jpg", function(head){
		if (head.status == 200)
			$("trBanner").setAttribute("class", "oem_banner");
	});
	xmit.head("/res/oem/banner_left.jpg", function(head){
		if (head.status == 200)
			$("ImgLeftBanner").setAttribute("src", "/res/oem/banner_left.jpg");
	});
	xmit.head("/res/oem/banner_right.jpg", function(head){
		if (head.status == 200)
			$("ImgRightBanner").setAttribute("src", "/res/oem/banner_right.jpg");
	});

}

