//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2011-2016        **;
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
// Brief      : This implementation is to display and configure the preserve
// configuration items, which is used for restore factory to default 
// configuration.
// Author Name: Arockia Selva Rani. A

var tblJSON;		//object to hold Preserve configuration in JSON structure
var tblPrsrvCfg;	//List grid object to hold Preserve configuration
var PRSRVCFG_DATA;	//It holds the get RPC Preserve configuration response data
var tmpPrsrvCfg = []; //Holds Preserve configuration checkboxes data

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method.
 */
function doInit()
{
	exposeElms(["_lblHeader",
		"_lgdPrsrvCfg",
		"_btnCheck",
		"_btnUncheck",
		"_btnSave",
		"_btnReset"]);

	if(top.user.isAdmin()) {
		btnCheck.onclick = function () {
			checkPreserveCfg(true);
		}
		btnUncheck.onclick = function () {
			checkPreserveCfg(false);
		}
		btnSave.onclick = setPreserveCfg;
		btnReset.onclick = reloadPreserveCfg;
	} else {
		disableActions();
	}
	_begin();
}

/*
 * It will invoke the RPC method to get the preserve configuration data for 
 * the page.
 */
function _begin()
{
	loadCustomPageElements();
	getPreserveCfg();
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
		reloadPreserveCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadPreserveCfg()
{
	var i;		//loop counter
	var count = 0;
	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		try {
			$("_chkPrsrvStatus" + i).checked = (PRSRVCFG_DATA[i].STATUS) ? 
				true : false;
			count += PRSRVCFG_DATA[i].STATUS ? 1 :0;
		} catch(e) {
			continue;
		}
	}
	lblHeader.innerHTML = "<strong class='st'>" + 
		eLang.getString("common", "STR_PRSRV_CFG_CNT") + "</strong>" + 
		count + eLang.getString("common", "STR_BLANK");
}

/*
 * It will invoke the RPC method to set all preserve configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setPreserveCfg()
{
	var req;	//xmit object to send RPC request with parameters
	var i;		//loop counter
	var data;
	var prsrvcfgcount = 0;
	var prsrv_select = "";
	var prsrvcfg = "";


	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		try {
			data = ($("_chkPrsrvStatus" + i).checked) ? 1 : 0;
			if (data == PRSRVCFG_DATA[i].STATUS) {
				continue;
			}
			prsrv_select += PRSRVCFG_DATA[i].SELECTOR + ",";
			prsrvcfg += data + ",";
			prsrvcfgcount++;
		} catch(e) {
			continue;
		}
	}

	if (prsrvcfgcount > 0) {
		req = new xmit.getset({url:"/rpc/setpreservecfg.asp", 
			onrcv:setPreserveCfgRes, status:""});
		req.add("PRSRV_CFG_CNT", prsrvcfgcount);
		req.add("PRSRV_SELECT", prsrv_select);
		req.add("PRSRV_CFG", prsrvcfg);
		req.send();
		delete req;
	}
}

/*
 * This is the response function for setPreserveCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting network bonding configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setPreserveCfgRes(arg)
{
	var errstr;	//Error string
	var i;		//loop counter

	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_PRSRV_SETVAL");
		STATUS_DATA = WEBVAR_JSONVAR_SETTRIGCFG.WEBVAR_STRUCTNAME_SETTRIGCFG;
		for(i = 0; i < STATUS_DATA.length; i++) {
			if (STATUS_DATA[i].STATUS != 0) {
				errstr += eLang.getString("common", "STR_NEWLINE");
				errstr += PRSRVCFG_DATA[i].CFG_NAME;
			}
		}
		alert(errstr);
	} else {
		errstr = eLang.getString("common", "STR_PRSRV_SUCCESS");
		alert(errstr);
	}
	getPreserveCfg();
}

/*
 * It will check or uncheck all the check boxes in UI, as per the argument
 * passed
 * @param bopt boolean, true-check, false-uncheck all the checkboxes.
 */
function checkPreserveCfg(bopt)
{
	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		try {
			$("_chkPrsrvStatus" + i).checked = bopt;
		} catch(e) {
			continue;
		}
	}
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid before sort and after sort event handler.
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
		textAlign:"center", sort:false}
		]};
	tblPrsrvCfg.loadFromJson(tblJSON);

	/*
	 * In this case, checkboxes were loaded in listgrid, so after sorting 
	 * listbox was reloaded without data. So to keep the data back, this 
	 * function will store the check boxes live data into the temp variable 
	 * before sorting takes place for the list grid.
	 */
	tblPrsrvCfg.onbeforesort = function ()
	{
		var i;		//loop counter
		for(i = 0; i < PRSRVCFG_DATA.length; i++) {
			try {
				tmpPrsrvCfg[i] = $("_chkPrsrvStatus" + i).checked;
			} catch(e) {
				continue;
			}
		}
	};

	/*
	 * In this case, checkboxes were loaded in listgrid, so after sorting 
	 * listbox was reloaded without data. So to keep the data back, this 
	 * function will load back the stored the check boxes live data  from 
	 * the temp variable into the appropriate check boxes. 
	 */
	tblPrsrvCfg.onaftersort = function ()
	{
		var i;		//loop counter
		for(i = 0; i < PRSRVCFG_DATA.length; i++) {
			try {
				$("_chkPrsrvStatus" + i).checked = tmpPrsrvCfg[i];
			} catch(e) {
				continue;
			}
		}
	};
}

/*
 * This function is used to load the rpc response from the global variable to 
 * list grid.
 */
function loadPreserveCfg()
{
	var rowIndex = 1;	//Row Index
	var rowJSON = [];	//Object of array of rows to load list grid
	var prsrvStatus;
	tblPrsrvCfg.clear();

	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		prsrvStatus = "<input type='checkbox' id='_chkPrsrvStatus" + i + "'>";
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

	tblJSON.rows = rowJSON;
	tblPrsrvCfg.loadFromJson(tblJSON);
}
