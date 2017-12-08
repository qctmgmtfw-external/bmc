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

// File Name  : configure_vmedia_devices_imp.js
// Brief      : This implementation is to display and configure the virtual 
// media devices in the BMC.
// Author Name: Arockia Selva Rani. A

var VMEDIACFG_DATA;			//It holds the get RPC virtual media response data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_lstFDCount",
		"_lstCDCount",
		"_lstHDCount",
		"_btnSave",
		"_btnReset"]);

	if(top.user.isAdmin()) {
		btnSave.onclick = validateVMediaCfg;
		btnReset.onclick = reloadVMediaCfg;
	} else {
		disableActions();
	}
	_begin();
}

/*
 * It will fill data for user controls like list box, if any.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	var index;				//loop counter
	var optind = 0;			//Optional index needed for IE, starts with 0
	for (index = 1; index < 3; index++,optind++) {
		lstFDCount.add(new Option(index,index), isIE?optind:null);
		lstCDCount.add(new Option(index,index), isIE?optind:null);
		lstHDCount.add(new Option(index,index), isIE?optind:null);
	}
	getVMediaCfg();
}

/*
 * It will invoke the RPC method to get the virtual media configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getVMediaCfg()
{
	xmit.get({url:"/rpc/getvmediacfg.asp", onrcv:getVMediaCfgRes, status:""});
}

/*
 * This is the response function for getVMediaCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getVMediaCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_VMEDIA_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		VMEDIACFG_DATA = WEBVAR_JSONVAR_GETVMEDIACFG.WEBVAR_STRUCTNAME_GETVMEDIACFG[0];
		reloadVMediaCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadVMediaCfg()
{
	lstFDCount.value = VMEDIACFG_DATA.V_NUM_FD;
	lstCDCount.value = VMEDIACFG_DATA.V_NUM_CD;
	lstHDCount.value = VMEDIACFG_DATA.V_NUM_HD;
}

/*
 * It will validate the data of all user controls before saving it.
 * Here it compares with old data, if any difference then send data to save.
 */
function validateVMediaCfg()
{
	if ((VMEDIACFG_DATA.V_NUM_FD == lstFDCount.value) &&
		(VMEDIACFG_DATA.V_NUM_CD == lstCDCount.value) &&
		(VMEDIACFG_DATA.V_NUM_HD == lstHDCount.value)) {
		return;
	} else {
		setVMediaCfg();
	}
}

/*
 * It will invoke the RPC method to set the virtual media configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setVMediaCfg()
{
	var req;			//xmit object to send RPC request with parameters
	if (confirm(eLang.getString("common", "STR_CONF_VMEDIA_CONFIRM"))) {
		req = new xmit.getset({url:"/rpc/setvmediacfg.asp", 
			onrcv:setVMediaCfgRes, status:""});
		req.add("V_NUM_FD", lstFDCount.value);
		req.add("V_NUM_CD", lstCDCount.value);
		req.add("V_NUM_HD", lstHDCount.value);
		req.send();
		delete req;
	}
}

/*
 * This is the response function for setVMediaCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting virtual media configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setVMediaCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_VMEDIA_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert (eLang.getString("common", "STR_CONF_VMEDIA_SAVE_SUCCESS"));
		getVMediaCfg();
	}
}