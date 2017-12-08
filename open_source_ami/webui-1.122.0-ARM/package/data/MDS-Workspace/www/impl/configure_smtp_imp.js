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

// File Name  : configure_smtp_imp.js
// Brief      : This implementation is to display and configure the SMTP 
// on the BMC.
// Author Name: Arockia Selva Rani. A

var SMTPCFG_DATA;			//It holds the get RPC SMTP response data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_rowLANChannel",
				"_lstLANChannel",
				"_txtSenderAddr",
				"_txtMachineName",
				"_txtPort1",
				"_txtSMTPServer1",
				"_chkSMTPAuth1",
				"_txtUsername1",
				"_txtPassword1",
				"_txtPort2",
				"_txtSMTPServer2",
				"_chkSMTPAuth2",
				"_txtUsername2",
				"_txtPassword2",
				"_btnSave",
				"_btnReset"]);

	if(top.user.isAdmin()) {
		chkSMTPAuth1.onclick = function() {
			enableSMTPAuth(1);
		}
		chkSMTPAuth2.onclick = function() {
			enableSMTPAuth(2);
		}
		btnSave.onclick = validateSMTPCfg;
		btnReset.onclick = reloadSMTPCfg;
	} else {
		disableActions({id: ["_lstLANChannel"]});
	}
	_begin();
}

/*
 * It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	getLanChannel();
	getSMTPCfg();
}

/*
 * It will load the LAN channel information if MultiLAN support is enabled.
 */
function getLanChannel()
{
	if (fnCookie.read('gMultiLAN')){		//MultiLAN support
		rowLANChannel.className = "visibleRow";
		for (i=0;i<top.settings.lan.length;i++) {
			lstLANChannel.add(new Option(top.settings.lan[i], 
				top.settings.lan[i]), isIE ? i : null);
		}
	
		if (top.mainFrame.pageFrame.location.hash) {
			lstLANChannel.value = comboParser(top.mainFrame.pageFrame.location.hash, 
				lstLANChannel.id);	//id->id of the selection box
		}
		lstLANChannel.onchange = getSMTPCfg;
	} else {
		rowLANChannel.className = "hiddenRow";
	}
}

/*
 * It will invoke the RPC method to get the SMTP configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getSMTPCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = xmit.getset({url:"/rpc/getsmtpcfg.asp", onrcv:getSMTPCfgRes, 
		status:"", timeout:10000});
	if (fnCookie.read('gMultiLAN')){
		req.add ("CHANNEL_NUM", lstLANChannel.value);
	} else {
		req.add ("CHANNEL_NUM", 1);
	}
	req.send();
	delete req;
}

/*
 * This is the response function for getSMTPCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getSMTPCfgRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_CONF_SMTP_GETVAL");
		errstr +=(eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		SMTPCFG_DATA = WEBVAR_JSONVAR_GETSMTPCFG.WEBVAR_STRUCTNAME_GETSMTPCFG[0];
		reloadSMTPCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadSMTPCfg()
{
	txtSenderAddr.value = SMTPCFG_DATA.SENDERADDR;
	txtMachineName.value = SMTPCFG_DATA.MACHINENAME;

//	txtPort1.value = SMTPCFG_DATA.SMTPPORT1;
	if (SMTPCFG_DATA.SMTPSERVER1 == eLang.getString("common", 
		"STR_IPV4_ADDR0")) {
		txtSMTPServer1.value = "";
	} else {
		txtSMTPServer1.value = SMTPCFG_DATA.SMTPSERVER1;
	}
	chkSMTPAuth1.checked = (SMTPCFG_DATA.SMTPENABLE1 == 1) ? true : false;
	enableSMTPAuth(1);
	txtUsername1.value = SMTPCFG_DATA.USERNAME1;
	txtPassword1.value = "";

//	txtPort2.value = SMTPCFG_DATA.SMTPPORT2;
	if (SMTPCFG_DATA.SMTPSERVER2 == eLang.getString("common", 
		"STR_IPV4_ADDR0")) {
		txtSMTPServer2.value = "";
	} else {
		txtSMTPServer2.value = SMTPCFG_DATA.SMTPSERVER2;
	}
	chkSMTPAuth2.checked = (SMTPCFG_DATA.SMTPENABLE2 ==1) ? true : false;
	enableSMTPAuth(2);
	txtUsername2.value = SMTPCFG_DATA.USERNAME2;
	txtPassword2.value = "";
}

/*
 * It will validate the data of all user controls before saving it.
 * Sender Address - Email
 * Machine Name - String
 * Port - Port
 * SMTP Server - IPv4/IPv6 Address
 * User Name - SMTP user name
 * Password - Password, 4-64
 */
function validateSMTPCfg()
{
	if (!eVal.email(txtSenderAddr.value)) {
		alert(eLang.getString("err",0x06) + "-" + 
			eLang.getString("common", "STR_CONF_SMTP_SENDERADDR"));
		txtSenderAddr.focus();
		return;
	}
	if (!eVal.str(txtMachineName.value)) {
		alert(eLang.getString("common", "STR_CONF_SMTP_INVALID_MACHINENAME"));
		txtMachineName.focus();
		return;
	}
/*	if(!eVal.port(txtPort1.value)) {
		alert(eLang.getString("common","STR_INVALID_PORT"));
		txtPort1.focus();
		return;
	}*/
	if ((!eVal.ip(txtSMTPServer1.value)) && 
		(!eVal.ipv6(txtSMTPServer1.value, true, false))) {
		alert(eLang.getString("common", "STR_CONF_SMTP_PRIMARY") +
			eLang.getString("common", "STR_INVALID_IP") + 
			eLang.getString("common", "STR_HELP_INFO"));
		txtSMTPServer1.focus();
		return;
	}
	if (chkSMTPAuth1.checked) {
		if (!eVal.username(txtUsername1.value, "SMTP", 4, 64)) {
			alert(eLang.getString("err", 0x01) + 
				eLang.getString("common", "STR_HELP_INFO"));
			txtUsername1.focus();
			return;
		}
		if (!eVal.password(txtPassword1.value, 4, 64)) {
			alert(eLang.getString("err", 0x03) + 
				eLang.getString("common", "STR_HELP_INFO"));
			txtPassword1.focus();
			return;
		}
	}
	if (!eVal.isblank(txtSMTPServer2.value)) {
/*		if(!eVal.port(txtPort2.value))
		{
			alert(eLang.getString("common","STR_INVALID_PORT"));
			txtPort2.focus();
			return;
		}*/
		if ((!eVal.ip(txtSMTPServer2.value)) && 
			(!eVal.ipv6(txtSMTPServer2.value, true, false))) {
			alert(eLang.getString("common", "STR_CONF_SMTP_SECONDARY") + 
				eLang.getString("common", "STR_INVALID_IP") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtSMTPServer2.focus();
			return;
		}
		if (chkSMTPAuth2.checked) {
			if (!eVal.username(txtUsername2.value, "SMTP", 4, 64)) {
				alert(eLang.getString("err", 0x01) + 
					eLang.getString("common", "STR_HELP_INFO"));
				txtUsername2.focus();
				return;
			}
			if (!eVal.password(txtPassword2.value, 4, 64)) {
				alert(eLang.getString("err", 0x03) + 
					eLang.getString("common", "STR_HELP_INFO"));
				txtPassword2.focus();
				return;
			}
		}
		if ((eVal.compareip(txtSMTPServer1.value, txtSMTPServer2.value)) || 
			(eVal.comparev6ip(txtSMTPServer1.value, txtSMTPServer2.value))) {
			alert(eLang.getString("common", "STR_CONF_SMTP_DIFF_SMTPSRVR") + 
				eLang.getString("common", "STR_HELP_INFO"));
			txtSMTPServer2.focus();
			return;
		}
	}
	setSMTPCfg();
}

/*
 * It will invoke the RPC method to set the SMTP configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setSMTPCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/setsmtpcfg.asp", onrcv:setSMTPCfgRes, 
		status:""});
	if (fnCookie.read('gMultiLAN')) {
		req.add("CHANNEL_NUM", lstLANChannel.value);
	} else {
		req.add("CHANNEL_NUM", 1);
	}
	req.add("SENDERADDR", txtSenderAddr.value);
	req.add("MACHINENAME", txtMachineName.value);

//	req.add("SMTPPORT1", txtPort1.value);
	req.add("SMTPPORT1", 25);
	req.add("SMTPSERVER1", txtSMTPServer1.value);
	req.add("SMTPENABLE1", chkSMTPAuth1.checked ? 1 : 0);
	req.add("USERNAME1", txtUsername1.value);
	req.add("PASSWORD1", txtPassword1.value);

	if (!eVal.isblank(txtSMTPServer2.value)) {
	//	req.add("SMTPPORT2", txtPort1.value);
		req.add("SMTPPORT2", 25);
		req.add("SMTPSERVER2", txtSMTPServer2.value);
		req.add("SMTPENABLE2", chkSMTPAuth2.checked ? 1 : 0);
		req.add("USERNAME2", txtUsername2.value);
		req.add("PASSWORD2", txtPassword2.value);
	} else {
	//	req.add("SMTPPORT2", txtPort1.value);
		req.add("SMTPPORT2", 25);
		req.add("SMTPSERVER2", "");
		req.add("SMTPENABLE2", 0);
		req.add("USERNAME2", "");
		req.add("PASSWORD2", "");
	}
	req.send();
	delete req;
}

/*
 * This is the response function for setSMTPCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting network bonding configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setSMTPCfgRes(arg)
{
	var errstr;		//Error string
	switch(arg.HAPI_STATUS) {
	case 0x0:
		alert(eLang.getString("common", "STR_CONF_SMTP_SAVE_SUCCESS"));
		getSMTPCfg();
	break;
	case 0xE2:
		alert(eLang.getString("common", "STR_INVALID_IP") + 
			eLang.getString("common", "STR_HELP_INFO"));
	break;
	default:
		errstr = eLang.getString("common", "STR_CONF_SMTP_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

/*
 * It will disable or enable the user controls based on SMTP authentication 
 * check box option of Primary or Secondary server.
 * @param server number, SMTP server 1-Primary server, 2-Secondary server.
 */
function enableSMTPAuth(server)
{
	var bopt;		//boolean value to enable/disable
	bopt = !$("_chkSMTPAuth" + server).checked;
	$("_txtUsername" + server).disabled = bopt;
	$("_txtPassword" + server).disabled = bopt;
	if (!bopt) {
		$("_txtUsername" + server).focus();
	} else {
		$("_txtUsername" + server).value = eval("SMTPCFG_DATA.USERNAME" + server);
		$("_txtPassword" + server).value = "";
	}
}