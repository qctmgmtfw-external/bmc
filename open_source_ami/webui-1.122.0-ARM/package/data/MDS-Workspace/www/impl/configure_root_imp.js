//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2010        **;
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

// File Name  : configure_root_imp.js
// Brief      : This implementation is to display and configure the root user
// in the BMC.
// Author Name: Arockia Selva Rani. A

var ROOTCFG_DATA;			//It holds the get RPC NCSI response data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(['_txtUsername',
				'_chkUserAccess',
				'_chkChangePword',
				'_txtPassword',
				'_txtConfirmPword',
				'_btnSave',
				'_btnReset']);

	if(top.user.isAdmin()) {
		chkUserAccess.onclick = enableUserAccess;
		chkChangePword.onclick = enablePassword;
		btnSave.onclick = validateRootCfg;
		btnReset.onclick = reloadRootCfg;
	} else {
		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
	}
	_begin();
}

/*
 * It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	getRootCfg();
}

/*
 * It will invoke the RPC method to get the root user configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getRootCfg()
{
	xmit.get({url:"/rpc/getrootcfg.asp", onrcv:getRootCfgRes, status:""});
}

/*
 * This is the response function for getRootCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getRootCfgRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CFG_ROOT_GETVAL");
		errstr +=(eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		ROOTCFG_DATA = WEBVAR_JSONVAR_GETROOTCFG.WEBVAR_STRUCTNAME_GETROOTCFG[0];
		reloadRootCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadRootCfg()
{
	txtUsername.value = ROOTCFG_DATA.SYSADMIN_NAME;
	//Ignores backspace functionality
	txtUsername.onkeydown = checkBackspace;
	chkUserAccess.checked = ROOTCFG_DATA.USERACCESS == 1 ? true : false;
	chkChangePword.checked = false;
	enableUserAccess();
}

/*
 * It will validate the data of all user controls before saving it.
 */
function validateRootCfg()
{
	if (chkChangePword.checked) {
		if (!eVal.password(txtPassword.value, 8, 64)) {
			alert (eLang.getString("common", "STR_INVALID_PASSWORD") + 
				eLang.getString("common", "STR_HELP_INFO"));
			txtPassword.focus();
			return;
		}

		if (txtPassword.value != txtConfirmPword.value) {
			alert (eLang.getString("common", "STR_INVALID_CPWORD"));
			txtConfirmPword.focus();
			return;
		}
	}
	setRootCfg();
}

/*
 * It will invoke the RPC method to set the root user configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setRootCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/setrootcfg.asp", onrcv:setRootCfgRes, 
		status:""});
	req.add("USERACCESS", chkUserAccess.checked ? 1 : 0);
	req.add("ISPWDCHANGE", chkChangePword.checked ? 1 : 0);
	req.add("PASSWORD", txtPassword.value);
	req.send();
	delete req;
}

/*
 * This is the response function for setRootCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then NCSI configuration is success, intimate proper message to end 
 * user.
 * @param arg object, RPC response data from xmit library
 */
function setRootCfgRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common","STR_CFG_ROOT_SETVAL");
		errstr +=(eLang.getString("common","STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert (eLang.getString("common","STR_CFG_ROOT_SUCCESS"));
	}
}

/*
 * It will disable or enable the user controls based on User access check box 
 * option.
 */
function enableUserAccess()
{
	if (!chkUserAccess.checked) {		//User Access is disabled
		chkChangePword.checked = false;
	}
	chkChangePword.disabled = !chkUserAccess.checked;
	enablePassword();
}

/*
 * It will disable or enable the user controls based on Change password check box 
 * option.
 */
function enablePassword()
{
	var bopt;		//boolean value to enable/disable
	bopt = !chkChangePword.checked;
	txtPassword.disabled = bopt;
	txtConfirmPword.disabled = bopt;
	if (bopt) {
		txtPassword.value = "";
		txtConfirmPword.value = "";
	}
}