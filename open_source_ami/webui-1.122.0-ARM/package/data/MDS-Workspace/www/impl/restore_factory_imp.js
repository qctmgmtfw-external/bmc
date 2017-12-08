//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2010-2015        **;
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

// File Name  : preserve_cfg_imp.js
// Brief      : This implementation is to perform restore factory to default 
// configuration for the BMC.

var wndConfirm;
var PRSRVCFG_DATA;
var tblJSON;		//object to hold Preserve configuration in JSON structure
var tblPrsrvCfg;	//List grid object to hold Preserve configuration

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_divPrsrvCfg",
		"_lgdPrsrvCfg",
		"_btnPrsrvCfg",
		"_btnRestore"]);

	if(top.user.isAdmin()) {
		btnRestore.onclick = doRestoreDefault;
		_begin();
	} else {
		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
	}
}

/*
 * It will invoke the method to check for the Preserve configuration support.
 */
function _begin()
{
	doPreserveCfg();
}

/*
 * It will display a confirmation dialog, to further continue to do Restore 
 * Factory defaults. It will send RPC request to perform Restore Factory 
 * Defaults, Once it is initiated, BMC gets rebooted. So UI is ended 
 * with message.
 */
function doRestoreDefault()
{
	if(confirm(eLang.getString("common", "STR_RSTR_FCTRY_CNFRM"))) {
		btnRestore.disabled = true;
		btnRestore.onclick = function() {};
		prepareDeviceShutdown();

		xmit.get({url:"/rpc/setfactorydefaults.asp",onrcv:function(){}});
		var mb = MessageBox(eLang.getString("common", "STR_DEVICE_RESET_TITLE"),
			p(eLang.getString("common", "STR_DEVICE_RESET_DESC")), [], true);
	}
}

/*
 * It will close the confirmation dialog which is used for Restore Defaults.
 */
function closeDialog()
{
	wndConfirm.close();
	btnRestore.disabled = false;
}


/*
 * It will check for the Preserve configuration support, If the user clicks the 
 * button, then it will redirect to the Preserve Configuration page under 
 * Maintenance.
 */
function doPreserveCfg()
{
	if(checkProjectCfg("PRESERVECONF")) {
		btnPrsrvCfg.className = "visibleRow";
		btnPrsrvCfg.disabled = false;
		btnPrsrvCfg.onclick = function () {
			location.href = "preserve_cfg.html";
		};
		divPrsrvCfg.className = "visibleRow";
		loadCustomPageElements();
		getPreserveCfg();
	} else {
		btnPrsrvCfg.className = "hiddenRow";
		divPrsrvCfg.className = "hiddenRow";
		btnPrsrvCfg.onclick = function() {};
	}
	
}

/*
 * It will invoke the RPC method to get all preserve configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getPreserveCfg()
{
	xmit.get({url:"/rpc/getpreservecfg.asp", onrcv:getPreserveCfgRes, status:""});
}

/*
 * This is the response function for getPreserveCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getPreserveCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_PRSRV_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		PRSRVCFG_DATA = WEBVAR_JSONVAR_GETPRESERVECFG.WEBVAR_STRUCTNAME_GETPRESERVECFG;
		loadPreserveCfg();
	}
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid select and double click event handler.  
 */
function loadCustomPageElements()
{
	lgdPrsrvCfg.innerHTML = "";
	tblPrsrvCfg = listgrid({
		w : "100%",
		doAllowNoSelect : false
	});

	lgdPrsrvCfg.appendChild(tblPrsrvCfg.table);

	tblJSON = {cols:[
		{text:eLang.getString("common", "STR_HASH"), w:"10%", fieldType:2,
		textAlign:"center"},
		{text:eLang.getString("common", "STR_PRSRV_CFG_ITEM"), w:"50%",
		textAlign:"center"},
		{text:eLang.getString("common","STR_PRSRV_STATUS"), w:"40%",
		textAlign:"center"}
		]};

	tblPrsrvCfg.loadFromJson(tblJSON);
}

/*
 * This function is used to load the rpc response from the global variable to 
 * list grid.
 */
function loadPreserveCfg()
{
	var rowIndex = 1;	//Row Index
	var rowJSON = [];	//Object of array of rows to load list grid
	var prsrvStatus;	//String of the Preserve status

	tblPrsrvCfg.clear();
	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		if (PRSRVCFG_DATA[i].STATUS != 0) {
			prsrvStatus = (PRSRVCFG_DATA[i].STATUS == 0) ? 
				eLang.getString("common", "STR_OVERWRITE") :
				eLang.getString("common", "STR_PRESERVE");
			try {
				rowJSON.push({cells:[
					{text:rowIndex, value:rowIndex},
					{text:PRSRVCFG_DATA[i].CFG_NAME, 
					value:PRSRVCFG_DATA[i].CFG_NAME},
					{text:prsrvStatus, value:prsrvStatus}
				]});
				rowIndex++;
			} catch(e) {
				alert(e);
			}
		}
	}

	tblJSON.rows = rowJSON;
	tblPrsrvCfg.loadFromJson(tblJSON);
}
