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

// File Name  : configure_remote_session_imp.js
// Brief      : This implementation is to display and configure the remote
// session configuration on the BMC. 
// Author Name: Arockia Selva Rani. A

var RMTSESS_CFG;		//It holds get RPC Remote session configuration response

var KEYBOARD_LANG = false;              //It holds the feature of physical keyboard language support

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_chkKVMEncrypt",
	    "_rowKeyboardLang",
	    "_lstKeyboardLang",		            
		"_chkMediaEncrypt",
		"_lstVMAttach",
		"_btnSave",
		"_btnReset"]);

	if(top.user.isAdmin()) {
		btnSave.onclick = validateRemoteSessionCfg;
		btnReset.onclick = reloadRemoteSessionCfg;
	} else {
		disableActions();
	}
	_begin();
}

/*
 * It will fill data for user controls like list box, if any.
 * It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	KEYBOARD_LANG = checkProjectCfg("KB_LANG_SELECT_SUPPORT");
	if (KEYBOARD_LANG) {
        rowKeyboardLang.className = "visibleRow";
        fillKeyboardLanguage();
	}	
	fillAttachMode();
	getRemoteSessionCfg();
}

/*
 * It will invoke the RPC method to get the remote session configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getRemoteSessionCfg()
{
	xmit.get ({url:"/rpc/getremotesession.asp", onrcv:getRemoteSessionCfgRes, 
		status:""});
}

/*
 * This is the response function for getRemoteSessionCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getRemoteSessionCfgRes(arg)
{
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_RMT_SESS_GETVAL");
		errstr +=(eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		RMTSESS_CFG = WEBVAR_JSONVAR_GETREMOTESESSIONCFG.WEBVAR_STRUCTNAME_GETREMOTESESSIONCFG[0];
		reloadRemoteSessionCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadRemoteSessionCfg()
{
	if (KEYBOARD_LANG) {
        lstKeyboardLang.value = RMTSESS_CFG.KEYBOARDLANG;
	}
	chkKVMEncrypt.checked = RMTSESS_CFG.KVMENCRYPTION?true:false;
	chkMediaEncrypt.checked = RMTSESS_CFG.MEDIAENCRYPTION?true:false;
	lstVMAttach.value = RMTSESS_CFG.VMEDIAATTACH;
}

/*
 * It will validate the data of all user controls before saving it.
 * Here it compares with old data, if any difference then send data to save.
 */
function validateRemoteSessionCfg()
{
	if (KEYBOARD_LANG) {
        if (RMTSESS_CFG.KEYBOARDLANG != lstKeyboardLang.value) {
                setRemoteSessionCfg();
                return;
        }
	}
	if ((RMTSESS_CFG.KVMENCRYPTION == chkKVMEncrypt.checked) &&
		(RMTSESS_CFG.MEDIAENCRYPTION == chkMediaEncrypt.checked) &&
		(RMTSESS_CFG.VMEDIAATTACH == lstVMAttach.value)) {
		return;
	} else {
		setRemoteSessionCfg();
	}
}

/*
 * It will invoke the RPC method to set the remote session configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setRemoteSessionCfg()
{
	var req;			//xmit object to send RPC request with parameters
	if (confirm(eLang.getString("common", "STR_CONF_RMT_SESS_CONFIRM"))) {
		req = new xmit.getset({url:"/rpc/setremotesession.asp", 
			onrcv:setRemoteSessionCfgRes, status:""});
		if (KEYBOARD_LANG) {
			req.add("KEYBOARDLANG", lstKeyboardLang.value);
		}		
		req.add("KVMENCRYPTION", chkKVMEncrypt.checked?1:0);
		req.add("MEDIAENCRYPTION", chkMediaEncrypt.checked?1:0);
		req.add("VMEDIAATTACH", lstVMAttach.value);
		req.send();
		delete req;
	}
}

/*
 * This is the response function for setRemoteSessionCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting virtual media configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setRemoteSessionCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_RMT_SESS_SETVAL");
		errstr +=(eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert(eLang.getString("common", "STR_CONF_RMT_SESS_SAVE_SUCCESS"));
		getRemoteSessionCfg();
	}
}

/*
 * It will fill keyboard supported languages in the list box.
 */
function fillKeyboardLanguage() {
        var index                               //loop counter
        lstKeyboardLang.innerHTML = "";
        for (index = 0; index < 4; index++) {
                lstKeyboardLang.add(new Option(eLang.getString("common",
                        "STR_CONF_RMT_SESS_KEYLANG_" + index), eLang.getString("common",
                        "STR_CONF_RMT_SESS_KEYLANG_INDEX_" + index)), isIE?index:null);
        }
}

/*
 * It will fill attach mode types of Virtual media in the list box.
 */
function fillAttachMode()
{
	var index;				//loop counter
	lstVMAttach.innerHTML = "";
	for (index = 0; index < 2; index++) {
		lstVMAttach.add(new Option(eLang.getString("common", 
			"STR_CONF_RMT_SESS_VMATTACH_" + index), index), isIE?index:null);
	}
}