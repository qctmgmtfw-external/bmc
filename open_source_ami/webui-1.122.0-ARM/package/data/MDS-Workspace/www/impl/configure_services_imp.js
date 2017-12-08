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

// File Name  : configure_services_imp.js
// Brief      : This implementation is to display the services in list grid. 
// It contains implementation to modify the services.
// Author Name: Arockia Selva Rani. A

var SERVICES_DATA;	//It holds RPC response data of services configuration.
var serviceID;
var tblJSON;		//object to hold services information in JSON structure
var tblServices;	//List grid object to hold services information

var INT32U_NA = 0xFFFFFFFF;
var INT32U_NE = 0x80000000;
var INT32U_NE_MASK = 0x7FFFFFFF;
var INT8U_NA = 0xFF;
var INT8U_NE = 0x80;
var INT8U_NE_MASK = 0x7F;

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_lblHeader",
		"_lgdServices",
		"_btnModify"]);

	if(top.user.isAdmin()) {
		btnModify.onclick = doModifyService;
	} else {
		disableActions();
	}
	_begin();
}

/*
 * It will invoke loadCustomPageElements method to load list grid.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	loadCustomPageElements();
	getAllServicesCfg();
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid select and double click event handler.  
 */
function loadCustomPageElements()
{
	lgdServices.innerHTML = "";
	tblServices = listgrid({
		w : "100%",
		doAllowNoSelect : false
	});

	lgdServices.appendChild(tblServices.table);

	tblJSON = {cols:[
		{text:eLang.getString("common", "STR_HASH"), w:"4%", fieldType:2,
		textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_NAME"), w:"12%",
		textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_STATE"), w:"12%",
		textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_IFC"), w:"12%",
		textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_NSPORT"), w:"12%",
		fieldType:2, textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_SECPORT"), w:"12%",
		fieldType:2, textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_TIMEOUT"), w:"12%",
		fieldType:2, textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_MAXSESS"), w:"12%",
		fieldType:2, textAlign:"center"},
		{text:eLang.getString("common", "STR_CONF_SERVICES_ACTIVESESS"), 
		w:"12%", fieldType:2, textAlign:"center"}
		]};

	tblServices.loadFromJson(tblJSON);

	/*
	 * This event handler will be invoked when the list grid row is selected.
	 */
	tblServices.ontableselect = function()
	{
		var servicename;		//Selected service name
		disableButtons();
		if (top.user.isAdmin()) {
			if (this.selected.length) {
				servicename = tblServices.getRow(tblServices.selected[0]).cells[1].innerHTML;
				servicename.replace("&nbsp;", "").replace(" ", "");
				if (servicename != "~") {
					btnModify.disabled = false;
				}
			}
		}
	}

	/*
	 * This event handler will be invoked when double click in list grid row.
	 */
	tblServices.ondblclick = function()
	{
		var servicename;		//Selected service name
		if(top.user.isAdmin()) {
			servicename = tblServices.getRow(tblServices.selected[0]).cells[1].innerHTML;
			servicename.replace("&nbsp;", "").replace(" ", "");
			if (servicename != "~") {
				doModifyService();
			}
		}
	}
}

/*
 * It will invoke the RPC method to get all the services configuration.
 * Once it get data from RPC, response function will be called automatically. 
 */
function getAllServicesCfg()
{
	xmit.get({url:"/rpc/getallservicescfg.asp", onrcv:getAllServicesCfgRes, 
		status:""});
}

/*
 * This is the response function for getAllServicesCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getAllServicesCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_SERVICES_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		SERVICES_DATA = WEBVAR_JSONVAR_GETALLSERVICESCFG.WEBVAR_STRUCTNAME_GETALLSERVICESCFG;
		loadServicesTable();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function loadServicesTable()
{
	var state_todisplay;		//Service current state to display in List grid
	var nsport_todisplay;		//Non secure port to display in List grid
	var secport_todisplay;		//Secure port to display in List grid
	var timeout_todisplay;		//Timeout to display in List grid
	var maxsess_todisplay;
	var actsess_todisplay;
	var index;					//loop counter
	var rowJSON = [];			//Object of array of rows to load list grid

	tblServices.clear();
	for (index = 0; index < SERVICES_DATA.length; index++) {
		state_todisplay = (SERVICES_DATA[index].STATE == 0) ? 
			eLang.getString("common", "STR_INACTIVE") : 
			eLang.getString("common", "STR_ACTIVE");
		ifc_todisplay = checkForNA(SERVICES_DATA[index].IFCNAME, 16) ? 
			eLang.getString("common", "STR_NOT_APPLICABLE") : 
			SERVICES_DATA[index].IFCNAME;
		nsport_todisplay = (SERVICES_DATA[index].NSPORT == INT32U_NA) ? 
			eLang.getString("common", "STR_NOT_APPLICABLE") : 
			(SERVICES_DATA[index].NSPORT & INT32U_NE_MASK);
		secport_todisplay = (SERVICES_DATA[index].SECPORT == INT32U_NA) ? 
			eLang.getString("common", "STR_NOT_APPLICABLE") : 
			(SERVICES_DATA[index].SECPORT & INT32U_NE_MASK);
		timeout_todisplay = (SERVICES_DATA[index].SERVICE_TIMEOUT == INT32U_NA)
			? eLang.getString("common", "STR_NOT_APPLICABLE") : 
			(SERVICES_DATA[index].SERVICE_TIMEOUT & INT32U_NE_MASK);
		maxsess_todisplay = (SERVICES_DATA[index].MAXSESS == INT8U_NA) ? 
			eLang.getString("common", "STR_NOT_APPLICABLE") :
			(SERVICES_DATA[index].MAXSESS & INT8U_NE_MASK);
		actsess_todisplay = (SERVICES_DATA[index].CURSESS == INT8U_NA) ?
			eLang.getString("common", "STR_NOT_APPLICABLE") :
			(SERVICES_DATA[index].CURSESS & INT8U_NE_MASK);

		try {
			rowJSON.push({cells:[
				{text:(index+1), value:(index+1)},
				{text:SERVICES_DATA[index].SERVICENAME, 
				value:SERVICES_DATA[index].SERVICENAME},
				{text:state_todisplay, value:state_todisplay},
				{text:ifc_todisplay, value:ifc_todisplay},
				{text:nsport_todisplay, value:nsport_todisplay},
				{text:secport_todisplay, value:secport_todisplay},
				{text:timeout_todisplay, value:timeout_todisplay},
				{text:maxsess_todisplay, value:maxsess_todisplay},
				{text:actsess_todisplay, value:actsess_todisplay}
			]});
		} catch(e) {
			alert(e);
		}
	}

	tblJSON.rows = rowJSON;
	tblServices.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + 
		eLang.getString("common", "STR_CONF_SERVICES_CNT") + "</strong>" + 
		SERVICES_DATA.length + eLang.getString("common", "STR_BLANK");
}

/*
 * This will get index from the selected row of list grid, invoke the method
 * to display a Modify form for Service configuration.
 */
function doModifyService()
{
	var index;		//Index of the list grid
	if ((tblServices.selected.length != 1) || 
		(tblServices.selected[0].cells[0] == undefined) || 
		(tblServices.selected[0].cells[0] == null)) {
		alert(eLang.getString("common", "STR_CONF_SERVICES_ERR"));
		disableButtons();
	} else {
		index = parseInt(tblServices.getRow(tblServices.selected[0]).cells[0].innerHTML);
		frmModifyService(index);
	}
}

/*
 * It will display a form, which contains UI controls to modify a service 
 * configuration.
 * @param index number, index of the selected slot in list grid.
 */
function frmModifyService(index)
{
	var frm = new form("modifyServiceForm", "POST", "javascript://", "general");

	serviceID = index - 1;
	txtServicename = frm.addTextField(eLang.getString("common", 
		"STR_CONF_SERVICES_NAME"), "_txtServicename", 
		SERVICES_DATA[serviceID].SERVICENAME, {"readOnly" : true}, 
		"smallclassicTxtBox");

	chkServiceState = frm.addCheckBox(eLang.getString("common", 
		"STR_CONF_SERVICES_STATE"), "_chkServiceState", {"active" : "Active"},
		false, ["active"]);
	chkServiceState = chkServiceState.active;
	chkServiceState.checked = (SERVICES_DATA[serviceID].STATE == 0) ? false : true;
	chkServiceState. onclick = enableServiceCfg;

	if (!checkForNA(SERVICES_DATA[serviceID].IFCNAME, 16)) {
		lstInterfaces = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SERVICES_IFC"), "_lstInterfaces", "", "", "", "", 
			"smallclassicTxtBox");
		fillInterfaces();
		lstInterfaces.value = SERVICES_DATA[serviceID].IFCNAME;
	}

	if (SERVICES_DATA[serviceID].NSPORT != INT32U_NA) {
		txtNSPort = frm.addTextField(eLang.getString("common", 
			"STR_CONF_SERVICES_NSPORT"), "_txtNSPort", 
			(SERVICES_DATA[serviceID].NSPORT & INT32U_NE_MASK), 
			{"maxLength":5}, "smallclassicTxtBox");
	}

	if (SERVICES_DATA[serviceID].SECPORT != INT32U_NA) {
		txtSecPort = frm.addTextField(eLang.getString("common", 
			"STR_CONF_SERVICES_SECPORT"), "_txtSecPort", 
			(SERVICES_DATA[serviceID].SECPORT & INT32U_NE_MASK), 
			{"maxLength":5}, "smallclassicTxtBox");
	}

	if (SERVICES_DATA[serviceID].SERVICE_TIMEOUT != INT32U_NA) {
		txtTimeout = frm.addTextField(eLang.getString("common", 
			"STR_CONF_SERVICES_TIMEOUT"), "_txtTimeout", 
			(SERVICES_DATA[serviceID].SERVICE_TIMEOUT & INT32U_NE_MASK), 
			{"maxLength":4}, "smallclassicTxtBox", "seconds");
	}

	if (SERVICES_DATA[serviceID].MAXSESS != INT8U_NA) {
		txtMaxSess = frm.addTextField(eLang.getString("common", 
			"STR_CONF_SERVICES_MAXSESS"), "_txtMaxSess", 
			(SERVICES_DATA[serviceID].MAXSESS & INT8U_NE_MASK), "", 
			"smallclassicTxtBox");
	}

	if (SERVICES_DATA[serviceID].CURSESS != INT8U_NA) {
		txtCurrSess = frm.addTextField(eLang.getString("common", 
			"STR_CONF_SERVICES_ACTIVESESS"), "_txtCurrSess", 
			(SERVICES_DATA[serviceID].CURSESS & INT8U_NE_MASK), "", 
			"smallclassicTxtBox");
	}

	var btnAry = [];
	btnAry.push(createButton("btnModifyService", eLang.getString("common", 
		"STR_MODIFY"), validateServiceCfg));
	btnAry.push(createButton("btnCancel", eLang.getString("common", 
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_CONF_SERVICES_MODIFY"), 
		frm.display(), btnAry);
	doNonEditable();
	enableServiceCfg();
	wnd.onclose = getAllServicesCfg;
}

/*
 * It will parse the controls and enable the readonly property of it,
 * When the service configuration data implies those are non editable fields.
 */
function doNonEditable()
{
	//Ignores backspace functionality
	txtServicename.onkeydown = checkBackspace;
	try {
		if ((SERVICES_DATA[serviceID].NSPORT & INT32U_NE) == INT32U_NE) {
			txtNSPort.readOnly = true;
			txtNSPort.onkeydown = checkBackspace;
		}
	} catch (e) {}

	try {
		if ((SERVICES_DATA[serviceID].SECPORT & INT32U_NE) == INT32U_NE) {
			txtSecPort.readOnly = true;
			txtSecPort.onkeydown = checkBackspace;
		}
	} catch (e) {}

	try {
		if ((SERVICES_DATA[serviceID].SERVICE_TIMEOUT & INT32U_NE) == INT32U_NE) {
			txtTimeout.readOnly = true;
			txtTimeout.onkeydown = checkBackspace;
		}
	} catch (e) {}

	try {
		if ((SERVICES_DATA[serviceID].MAXSESS & INT8U_NE) == INT8U_NE) {
			txtMaxSess.readOnly = true;
			txtMaxSess.onkeydown = checkBackspace;
		}
	} catch (e) {}

	try {
		if ((SERVICES_DATA[serviceID].CURSESS & INT8U_NE) == INT8U_NE) {
			txtCurrSess.readOnly = true;
			txtCurrSess.onkeydown = checkBackspace;
		}
	} catch (e) {}
}

/*
 * It will validate the data of all user controls before saving it.
 */
function validateServiceCfg()
{
	if (chkServiceState.checked) {
		if (txtServicename.value == "web") {
			try {
				if (!eVal.isnumstr(txtTimeout.value, 300, 1800)) {
					alert(eLang.getString("common", "STR_INVALID_TIMEOUT") + 
						eLang.getString("common", "STR_HELP_INFO"));
					txtTimeout.focus();
					return;
				}
			} catch (e) {}
		}

		if ((txtServicename.value == "ssh") || 
			(txtServicename.value == "telnet")) {
			try {
				if (!eVal.isnumstr(txtTimeout.value, 60, 1800)) {
					alert(eLang.getString("common", "STR_INVALID_TIMEOUT") + 
						eLang.getString("common", "STR_HELP_INFO"));
					txtTimeout.focus();
					return;
				}
				if (parseInt(txtTimeout.value, 10) % 60 != 0) {
					alert(eLang.getString("common", "STR_INVALID_TIMEOUT") + 
							eLang.getString("common", "STR_HELP_INFO"));
						txtTimeout.focus();
						return;
				}
			} catch (e) {}
		}

		try {
			if (!eVal.port(txtNSPort.value)) {
				alert(eLang.getString("common", "STR_INVALID_PORT") + 
					eLang.getString("common", "STR_HELP_INFO"));
				txtNSPort.focus();
				return;
			}
		} catch (e) {}

		try {
			if (!eVal.port(txtSecPort.value)) {
				alert(eLang.getString("common", "STR_INVALID_PORT") + 
					eLang.getString("common", "STR_HELP_INFO"));
				txtSecPort.focus();
				return;
			}
		} catch (e) {}
	}
	setServicesCfg();
}

/*
 * It will invoke the RPC method to set the service configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setServicesCfg()
{
	var confirmstr = "";
	if ((txtServicename.value == "web") || 
		(txtServicename.value == "ssh") ||
		(txtServicename.value == "telnet")) {
		confirmstr = eLang.getString("common", "STR_CONF_SERVICES_CNFM" + 
			txtServicename.value.toUpperCase());
	}

	if (confirm(eLang.getString("common", "STR_CONF_SERVICES_CONFIRM") + confirmstr)) {
		var req;			//xmit object to send RPC request with parameters
		req = new xmit.getset({url:"/rpc/setservicecfg.asp", 
			onrcv:setServicesCfgRes, status:""});
		req.add("SERVICEBIT", serviceID);
		req.add("STATE", chkServiceState.checked ? 1 : 0);

		try {
			req.add("IFCNAME", lstInterfaces.value);
		} catch (e) {
			req.add("IFCNAME", SERVICES_DATA[serviceID].IFCNAME);
		}
		try {
			req.add("NSPORT", txtNSPort.value);
		} catch (e) {
			req.add("NSPORT", SERVICES_DATA[serviceID].NSPORT);
		}
		try {
			req.add("SECPORT", txtSecPort.value);
		} catch (e) {
			req.add("SECPORT", SERVICES_DATA[serviceID].SECPORT);
		}
		try {
			req.add("SERVICE_TIMEOUT", txtTimeout.value);
		} catch (e) {
			req.add("SERVICE_TIMEOUT", SERVICES_DATA[serviceID].SERVICE_TIMEOUT);
		}
		try {
			req.add("MAXSESS", txtMaxSess.value);
		} catch (e) {
			req.add("MAXSESS", SERVICES_DATA[serviceID].MAXSESS);
		}
		try {
			req.add("CURSESS", txtCurrSess.value);
		} catch (e) {
			req.add("CURSESS", SERVICES_DATA[serviceID].CURSESS);
		}

		req.send();
		delete req;
	}
}

/*
 * This is the response function for setServicesCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then service configuration is success, intimate proper message to 
 * end user.
 * @param arg object, RPC response data from xmit library
 */
function setServicesCfgRes(arg)
{
	var errstr;		//Error string
	switch (GET_ERROR_CODE(arg.HAPI_STATUS)) {
	case 0x0:
		if (txtServicename.value != "web") {
			alert (eLang.getString("common", "STR_CONF_SERVICES_SUCCESS"));
			closeForm();
		} else {
			document.body.removeChild($("hoverDiv"));
			document.body.removeChild($("descFrame"));
			prepareDeviceShutdown();
			var mb = MessageBox(eLang.getString("common", "STR_WEB_RESET_TITLE"), 
				p(eLang.getString("common", "STR_WEB_RESET_DESC")), [], true);
			clearCookies();
		}
		break;
	case 0x83:
		alert(eLang.getString("common", "STR_INVALID_PORT"));
		break;
	case 0x84: case 0x86:
		alert(eLang.getString("common", "STR_CONF_SERVICES_SETERR_" + 
			GET_ERROR_CODE(arg.HAPI_STATUS)));
		break;
	case 0x85:
		alert(eLang.getString("common", "STR_INVALID_TIMEOUT"));
		break;
	case 0x80: case 0x81:
	case 0x82: default:
		errstr = eLang.getString("common", "STR_CONF_SERVICES_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

function enableServiceCfg()
{
	var bopt;
	bopt = !chkServiceState.checked;

	try {
		lstInterfaces.disabled = bopt;
	} catch (e) {}

	try {
		txtNSPort.disabled = bopt;
	} catch (e) {}

	try {
		txtSecPort.disabled = bopt;
	} catch (e) {}

	try {
		txtTimeout.disabled = bopt;
	} catch (e) {}

	try {
		txtMaxSess.disabled = bopt;
	} catch (e) {}

	try {
		txtCurrSess.disabled = bopt;
	} catch (e) {}

	if(bopt) {
		try {
			lstInterfaces.value = SERVICES_DATA[serviceID].IFCNAME;
		} catch (e) {}

		try {
			txtNSPort.value = SERVICES_DATA[serviceID].NSPORT;
		} catch (e) {}

		try {
			txtSecPort.value = SERVICES_DATA[serviceID].SECPORT;
		} catch (e) {}

		try {
			txtTimeout.value = SERVICES_DATA[serviceID].SERVICE_TIMEOUT;
		} catch (e) {}

		try {
			txtMaxSess.disabled = SERVICES_DATA[serviceID].MAXSESS;
		} catch (e) {}

		try {
			txtCurrSess.disabled = SERVICES_DATA[serviceID].CURSESS;
		} catch (e) {}
	}
}

/*
 * It will fill LAN interfaces name in Interfaces list box
 */
function fillInterfaces()
{
	var i;		//loop counter
	lstInterfaces.innerHTML = "";
	for (i = 0; i < top.settings.ethstr.length; i++) {
		lstInterfaces.add(new Option(top.settings.ethstr[i], 
			top.settings.ethstr[i]), isIE?i:null);
	}
	if (top.settings.ethstr.length > 1) {
		lstInterfaces.add(new Option("both", "both"), isIE?i:null);
	}
}

/*
 * It will check the given interface name for Not Applicable. If the whole
 * string contains value as "F", then it implies it is Not applicable.
 * @param ifcName string, Interface name to be checked.
 * @param len number, total size of the interface name.
 * @return boolean, true-Not applicable field, false-applicable field.
 */
function checkForNA(ifcName, len)
{
	var i;		//loop counter
	for (i = 0; i < len; i++) {
		if (ifcName[i] != 'F') {
			return false;
		}
	}
	return true;
}

/*
 * Used to close the form which is used to modify the service
 */
function closeForm()
{
	wnd.close();
}

/*
 * It will disable the buttons.
 */
function disableButtons()
{
	btnModify.disabled = true;
}
