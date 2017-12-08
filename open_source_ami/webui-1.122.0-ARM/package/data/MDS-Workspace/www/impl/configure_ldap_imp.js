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

// File Name  : configure_lmedia_imp.js
// Brief      : This implementation is to configure LDAP and Role Group details. 
// It will display the role group in list grid. It contains implementation to 
// add, modify and delete the role group.
// Author Name: Arockia Selva Rani. A

var LDAPCFG_DATA;	//It holds RPC response data of LDAP configuration.
var RG_DATA;		//It holds RPC response data of role group configuration.
var tblRoleGroup;	//List grid object to hold role group details
var tblJSON;		//object to hold role group details in JSON structure
var varRoleGpOper;	//Role group operation, 1-Add, 2-Modify, 3-Delete.

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_lblLDAPDesc",
		"_btnAdvSettings",
		"_lblHeader",
		"_lgdRoleGroup",
		"_btnAdd",
		"_btnModify",
		"_btnDelete"]);

	if(top.user.isAdmin()) {
		btnAdd.onclick = function() {
			doRoleGroup(top.CONSTANTS.ADD);
		};
		btnModify.onclick = function() {
			doRoleGroup(top.CONSTANTS.MODIFY);
		};
		btnDelete.onclick = function() {
			doRoleGroup(top.CONSTANTS.DELETE);
		};
	} else {
		disableActions({id: ["_advSettingsBtn"]});
	}
	btnAdvSettings.onclick = doAdvancedLDAP;
	_begin();
}

/*
 * It will invoke loadCustomPageElements method to load list grid.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	getLDAPCfg();
	loadCustomPageElements();
}

/*
 * It will invoke the RPC method to get the LDAP and Role group configuration.
 * Once it get data from RPC, response function will be called automatically. 
 */
function getLDAPCfg()
{
	btnAdvSettings.disabled = false;
	xmit.get({url:"/rpc/getldapcfg.asp", onrcv:getLDAPCfgRes, status:""});
	disableButtons();
	xmit.get({url:"/rpc/getldaprolegroupcfg.asp", 
		onrcv:getAllRoleGroupInfoRes, status:""});
}

/*
 * This is the response function for getLDAPCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getLDAPCfgRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr =  eLang.getString("common", "STR_CONF_LDAP_GETINFO");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		LDAPCFG_DATA = WEBVAR_JSONVAR_GETLDAPCFG.WEBVAR_STRUCTNAME_GETLDAPCFG[0];
		lblLDAPDesc.innerHTML = eLang.getString("common", 
		"STR_CONF_LDAP_DESC_" + LDAPCFG_DATA.ENABLE);
	}
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid select and double click event handler.  
 */
function loadCustomPageElements()
{
	tblRoleGroup = listgrid({
		w : "100%",
		doAllowNoSelect : false
		});
	lgdRoleGroup.appendChild(tblRoleGroup.table);

	try {
		tblJSON = {cols:[
			{text:eLang.getString("common", "STR_CONF_RG_HEAD1"), fieldType:2,
				w:"10%", textAlign:"center"},
			{text:eLang.getString("common", "STR_CONF_RG_HEAD2"), w:"25%",
				textAlign:"center"},
			{text:eLang.getString("common", "STR_CONF_LDAP_RG_SEARCHBASE"), w:"35%",
				textAlign:"center"},
			{text:eLang.getString("common", "STR_CONF_RG_HEAD4"), w:"30%",
				textAlign:"center"}
			]};

		tblRoleGroup.loadFromJson(tblJSON);
	} catch(e) {
		alert(e);
	}

	/*
	 * This event handler will be invoked when the list grid row is selected.
	 */
	tblRoleGroup.ontableselect = function ()
	{
		disableButtons();
		if(top.user.isAdmin() && (LDAPCFG_DATA.ENABLE)) {
			if(this.selected.length) {
				selectedid = parseInt(tblRoleGroup.getRow(tblRoleGroup.selected[0]).cells[0].innerHTML);
				if(RG_DATA[selectedid-1].ROLEGROUP_NAME == "") {
					btnAdd.disabled = false;
				} else {
					btnModify.disabled = false;
					btnDelete.disabled = false;
				}
			}
		}
	}

	/*
	 * This event handler will be invoked when double click in list grid row.
	 */
	tblRoleGroup.ondblclick = function()
	{
		if(top.user.isAdmin() && (LDAPCFG_DATA.ENABLE))
		{
			selectedid = parseInt(tblRoleGroup.getRow(tblRoleGroup.selected[0]).cells[0].innerHTML);
			if (RG_DATA[selectedid-1].ROLEGROUP_NAME == "") {
				doRoleGroup(top.CONSTANTS.ADD);
			} else {
				doRoleGroup(top.CONSTANTS.MODIFY);
			}
		}
	}
}

/*
 * It will disable the buttons.
 */
function disableButtons()
{
	btnAdd.disabled = true;
	btnModify.disabled = true;
	btnDelete.disabled = true;
}

/*
 * This is the response function for getAllRoleGroup RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getAllRoleGroupInfoRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr =  eLang.getString("common", "STR_CONF_RG_GETINFO");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		RG_DATA = WEBVAR_JSONVAR_GETALLROLEGROUPCFG.WEBVAR_STRUCTNAME_GETALLROLEGROUPCFG;
		loadRoleGroupTable();
	}
}

/*
 * It will load response data from global variable to list grid control in UI.
 */
function loadRoleGroupTable()
{
	var JSONRows = new Array();
	var tRoleGroupCnt = 0;

	tblRoleGroup.clear();
	for (i = 0; i < RG_DATA.length; i++) {
		// Use ~ char to indicate free slot so it will sort alphabetically
		rgnametodisplay = (RG_DATA[i].ROLEGROUP_NAME == "") ? "~" : 
			RG_DATA[i].ROLEGROUP_NAME;
		rgdomaintodisplay = (RG_DATA[i].ROLEGROUP_DOMAIN == "") ? "~" : 
			RG_DATA[i].ROLEGROUP_DOMAIN;
		rgprivtodisplay = (RG_DATA[i].ROLEGROUP_PRIVILEGE) ? 
			IPMIPrivileges[RG_DATA[i].ROLEGROUP_PRIVILEGE] : "~";
		if(rgnametodisplay!="~") {
			tRoleGroupCnt++;
		}

		try {
			JSONRows.push({cells:[
				{text:RG_DATA[i].ROLEGROUP_ID, value:RG_DATA[i].ROLEGROUP_ID},
				{text:rgnametodisplay, value:rgnametodisplay},
				{text:rgdomaintodisplay, value:rgdomaintodisplay},
				{text:rgprivtodisplay, value:rgprivtodisplay}
			]});
		} catch(e) {
			alert(e);
		}
	}

	tblJSON.rows = JSONRows;
	tblRoleGroup.loadFromJson(tblJSON);
	lblHeader.innerHTML = eLang.getString("common", "STR_CONF_RG_CNT") +
		tRoleGroupCnt + eLang.getString("common", "STR_BLANK");
}

/*
 * It will invoke the method to design the Advanced LDAP configuration form and
 * also load the data in the Advanced LDAP configuration form.
 */
function doAdvancedLDAP()
{
	frmAdvancedLDAP();
	loadAdvancedLDAPCfg();
}

/*
 * This will design the UI controls for Advanced LDAP configuration form.
 */
function frmAdvancedLDAP()
{
	var frm = new form("advancedLDAPFrm", "POST", "javascript://", "general");

	settings = frm.addCheckBox(eLang.getString("common", 
		"STR_CONF_LDAP_ENABLE"), "chkLDAPEnable", {"enable":"Enable"}, false, 
		["enable"]);
	chkLDAPEnable = settings.enable;

	txtIP = frm.addTextField(eLang.getString("common", "STR_CONF_LDAP_IP"),
		"_txtIP", "", {"maxLength":40}, "classicTxtBox");

	txtPort = frm.addTextField(eLang.getString("common", "STR_CONF_LDAP_PORT"),
		"_txtPort", "", {"maxLength":5}, "smallclassicTxtBox");

	txtBindDN = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LDAP_BINDDN"), "_txtBindDN", "", {"maxLength":64},
		"bigclassicTxtBox");

	txtBindPword = frm.addPasswordField(eLang.getString("common", "STR_PASSWORD"),
		"_txtBindPword", "", {"maxLength":48}, "bigclassicTxtBox");

	txtSearchBase = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LDAP_SEARCHBASE"), "_txtSearchBase", "", {"maxLength":64},
		"bigclassicTxtBox");

	txtAttributeOfUserLogin = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LDAP_ATTRIBUTE_OF_USER_LOGIN"), "_txtAttributeOfUserLogin", "", {"maxLength":5},
		"smallclassicTxtBox");

	var btnAry = [];
	btnAry.push(createButton("btnSave", eLang.getString("common",
		"STR_SAVE"), isLDAPCfgChange));
	btnAry.push(createButton("btnCancel", eLang.getString("common",
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_CONF_LDAP_TITLE"),
		frm.display(), btnAry);
	wnd.onclose = getLDAPCfg;

	chkLDAPEnable.onclick = enableLDAPCfg;
	chkLDAPEnable.checked = LDAPCFG_DATA.ENABLE ? true : false;
	enableLDAPCfg();

	if(!top.user.isAdmin()) {
		disableActions({id:["_btnAdvSettings", "_btnCancel"]});
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function loadAdvancedLDAPCfg()
{
	txtIP.value = LDAPCFG_DATA.IP;
	txtPort.value = LDAPCFG_DATA.PORTNUM;
	txtBindDN.value = LDAPCFG_DATA.BINDDN;
	txtBindPword.value = "";
	txtSearchBase.value = LDAPCFG_DATA.SEARCHBASE;
	txtAttributeOfUserLogin.value = LDAPCFG_DATA.ATTRIBUTEOFUSERLOGIN;
}

/*
 * This will enable or disable the LDAP UI controls based on the LDAP 
 * authentication Enable check box value.
 */
function enableLDAPCfg()
{
	var bopt;
	if (top.user.isAdmin()) {
		bopt = !chkLDAPEnable.checked;
		txtIP.disabled = bopt;
		txtPort.disabled = bopt;
		txtBindDN.disabled = bopt;
		txtBindPword.disabled = bopt;
		txtSearchBase.disabled = bopt;
		txtAttributeOfUserLogin.disabled = bopt;
		if(!bopt) {
			txtIP.focus();
		} else {
			loadAdvancedLDAPCfg();
		}
	}
}

/*
 * This function is used to compare the configuration values with the data in 
 * the controls.
 */
function isLDAPCfgChange()
{
	if ((LDAPCFG_DATA.ENABLE == chkLDAPEnable.checked) && 
		(LDAPCFG_DATA.IP == txtIP.value) && 
		(LDAPCFG_DATA.PORTNUM == txtPort.value) && 
		(LDAPCFG_DATA.BINDDN == txtBindDN.value) && 
		("" == txtBindPword.value) && 
		(LDAPCFG_DATA.SEARCHBASE == txtSearchBase.value) && (LDAPCFG_DATA.ATTRIBUTEOFUSERLOGIN == txtAttributeOfUserLogin.value)) {
		return;
	}
	validateLDAPCfg();
}

/*
 * This will validate all the UI controls value of LDAP configuration before 
 * saving it.
 */
function validateLDAPCfg()
{
	if(chkLDAPEnable.checked) {
		if (!eVal.ip(txtIP.value) && !eVal.ipv6(txtIP.value)) {
			alert(eLang.getString("common", "STR_INVALID_IP") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtIP.focus();
			return false;
		}

		if(!eVal.port(txtPort.value)) {
			alert(eLang.getString("common", "STR_INVALID_PORT") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtPort.focus();
			return false;
		}

		if (!eVal.ldap(txtBindDN.value, "binddn")) {
			alert(eLang.getString("common", "STR_CONF_LDAP_INVALID_BINDDN") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtBindDN.focus();
			return false;
		}

		if (!eVal.password(txtBindPword.value, 1, 48)) {
			alert(eLang.getString("common", "STR_INVALID_PASSWORD") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtBindPword.focus();
			return false;
		}

		if (!eVal.ldap(txtSearchBase.value, "searchbase")) {
			alert(eLang.getString("common", "STR_INVALID_SEARCHBASE") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtSearchBase.focus();
			return false;
		}

		if (eVal.isblank(txtAttributeOfUserLogin.value)) {
			alert(eLang.getString("common", "STR_CONF_LDAP_INVALID_ATTRIBUTE_OF_USERLOGIN") +
				eLang.getString("common", "STR_HELP_INFO"));
			txtuserlogin.focus();
			return false;
		}
	}
	setLDAPCfg();
	return true;
}

/*
 * It will invoke the RPC method to set the LDAP configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setLDAPCfg()
{
	if (top.user.isAdmin()) {
		var req = new xmit.getset({url:"/rpc/setldapcfg.asp", 
			onrcv:setLDAPCfgRes, status:""});
		req.add("ENABLE", chkLDAPEnable.checked ? 1 : 0);
		req.add("PORTNUM", parseInt(txtPort.value, 10));
		req.add("IP", txtIP.value);
		req.add("BINDDN", txtBindDN.value);
		req.add("PASSWORD", txtBindPword.value);
		req.add("SEARCHBASE", txtSearchBase.value);
		req.add("ATTRIBUTEOFUSERLOGIN", txtAttributeOfUserLogin.value);
		req.send();
		delete req;
	} else {
		alert(eLang.getString("common", "STR_CONF_ADMIN_PRIV"));
	}
}

/*
 * This is the response function for setLDAPCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then LDAP configuration is success, intimate proper message to end 
 * user.
 * @param arg object, RPC response data from xmit library
 */
function setLDAPCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS) {
		errstr =  eLang.getString("common", "STR_CONF_LDAP_SETINFO");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert(eLang.getString("common", "STR_CONF_LDAP_SAVE_SUCCESS"));
		closeForm();
	}
}

/*
 * This will get index from the selected row of list grid, based on the data
 * in selected row, it will invoke either add or modify role group method.
 * @param oper integer, Role Group operation. 1-Add, 2-Modify, 3-Delete.
 */
function doRoleGroup(oper)
{
	var selectedrow;
	var selectedrgid;
	var selectedrgpriv;
	if ((tblRoleGroup.selected.length != 1) ||
		(tblRoleGroup.selected[0].cells[0] == undefined) ||
		(tblRoleGroup.selected[0].cells[0] == null)) {
		alert(eLang.getString("common", "STR_CONF_RG_ERR1"));
		disableButtons();
		return;
	} else {
		selectedrow = tblRoleGroup.getRow(tblRoleGroup.selected[0]);
		selectedrgid = parseInt(selectedrow.cells[0].innerHTML);
		selectedrgpriv = getbits(RG_DATA[selectedrgid-1].ROLEGROUP_PRIVILEGE, 3, 0);
		switch(oper) {
		case top.CONSTANTS.ADD:
			if ("" != RG_DATA[selectedrgid-1].ROLEGROUP_NAME) {
				if (confirm(eLang.getString("common", "STR_CONF_RG_CONFIRM1"))) {
					frmRoleGroup({"rgindex":selectedrgid,
						"rgname":selectedrow.cells[1].innerHTML,
						"rgsearchbase":selectedrow.cells[2].innerHTML,
						"rgpriv":selectedrgpriv},
						top.CONSTANTS.MODIFY);
				}
			} else {
				frmRoleGroup({"rgindex":selectedrgid}, top.CONSTANTS.ADD);
			}
			break;
		case top.CONSTANTS.MODIFY:
			if ("" == RG_DATA[selectedrgid-1].ROLEGROUP_NAME) {
				if (confirm(eLang.getString("common", "STR_CONF_RG_CONFIRM2"))) {
					frmRoleGroup({"rgindex":selectedrgid}, top.CONSTANTS.ADD);
				}
			} else {
				frmRoleGroup({"rgindex":selectedrgid,
					"rgname":selectedrow.cells[1].innerHTML,
					"rgsearchbase":selectedrow.cells[2].innerHTML,
					"rgpriv":selectedrgpriv},
					top.CONSTANTS.MODIFY);
			}
			break;
		case top.CONSTANTS.DELETE:
			if ("" == RG_DATA[selectedrgid-1].ROLEGROUP_NAME) {
				alert (eLang.getString("common", "STR_CONF_RG_ERR2"));
			} else {
				if (confirm(eLang.getString("common", "STR_CONFIRM_DELETE"))) {
					var req = new xmit.getset({url:"/rpc/setldaprolegroupcfg.asp", 
						onrcv:deleteRoleGroupRes, status:""});
					req.add("ROLEGROUP_ID", selectedrgid);
					req.add("ROLEGROUP_OPER", top.CONSTANTS.DELETE)
					req.send();
					delete req;
				}
			}
		}
	}
}

/*
 * It will display a form, which contains UI controls to add or modify the role 
 * group details.
 * @param arg object, contains role group details of selected slot in list grid.
 * @param oper integer, Role Group operation. 1-Add, 2-Modify.
 */
function frmRoleGroup(arg, oper)
{
	var formname = "";
	var btnname = "";
	var btnvalue = "";
	switch (oper) {
	case top.CONSTANTS.ADD:
		formname = "addRoleGroup";
		btnname = "addBtn";
		btnvalue = eLang.getString("common", "STR_ADD");
		break;
	case top.CONSTANTS.MODIFY:
		formname = "modRoleGroup";
		btnname = "modBtn";
		btnvalue = eLang.getString("common", "STR_MODIFY");
		break;
	}

	varRoleGpOper = oper;
	var frm = new form(formname, "POST", "javascript://", "general");
	txtRGName = frm.addTextField(eLang.getString("common",
		"STR_CONF_RG_NAME"), "_txtRGName", "", {"maxLength":255},
		"classicTxtBox");

	txtRGSearchBase = frm.addTextField(eLang.getString("common",
		"STR_CONF_RG_SEARCHBASE"), "_txtRGSearchBase", "", {"maxLength":255},
		"bigclassicTxtBox");

	var privVals = {4:"Administrator", 3:"Operator", 2:"User", 
		5:"OEM Proprietary", 0xf:"No Access"};
	lstRGPriv = frm.addSelectBox(eLang.getString("common",
		"STR_CONF_RG_PRIV"), "_lstRGPriv", privVals,"", "", "",
		"classicTxtBox");

	var btnAry = [];
	btnAry.push(createButton(btnname, btnvalue, setRoleGroup));
	btnAry.push(createButton("cancelBtn", eLang.getString("common",
		"STR_CANCEL"), closeForm));
	rgindex = arg.rgindex;

	wnd = MessageBox(eLang.getString("common", "STR_CONF_RG_TITLE_" + oper),
		frm.display(), btnAry);
	wnd.onclose = getLDAPCfg;
	if (oper == top.CONSTANTS.MODIFY) {
		txtRGName.value = arg.rgname;
		txtRGSearchBase.value = arg.rgsearchbase;
		lstRGPriv.value = arg.rgpriv;
	}
}

/*
 * It will invoke the RPC method to set the role group configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setRoleGroup()
{
	if (validateRoleGroup()) {
		var req = new xmit.getset({url:"/rpc/setldaprolegroupcfg.asp", 
			onrcv:setRoleGroupRes, status:""});
		req.add("ROLEGROUP_ID", rgindex);
		req.add("ROLEGROUP_NAME", txtRGName.value);
		req.add("ROLEGROUP_DOMAIN", txtRGSearchBase.value);
		req.add("ROLEGROUP_PRIV", lstRGPriv.value);
		req.add("ROLEGROUP_OPER", varRoleGpOper)
		req.send();
		delete req;
	}
}

/*
 * This is the response function for setRoleGroup RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then role group configuration saved successfully, intimate proper 
 * message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setRoleGroupRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		switch(GET_ERROR_CODE(arg.HAPI_STATUS)) {
		case 0xcc:
			errstr = eLang.getString("common", "STR_CONF_RG_ERR3");
			alert(errstr);
			break;
		default:
			errstr = eLang.getString("common", "STR_CONF_RG_SETINFO");
			errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
				GET_ERROR_CODE(arg.HAPI_STATUS));
			alert(errstr);
			break;
		}
	} else {
		alert(eLang.getString("common", "STR_CONF_RG_SAVE_SUCCESS_" +
			varRoleGpOper));
		closeForm();
	}
}

/*
 * This is the response function for deleteRoleGroup RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then role group configuration deleted successfully, intimate proper 
 * message to end user.
 * @param arg object, RPC response data from xmit library
 */
function deleteRoleGroupRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_RG_DELINFO");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") +
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert(eLang.getString("common", "STR_CONF_RG_DELETE_SUCCESS"));
		getLDAPCfg();
	}
}

/*
 * This will validate all the UI controls value of role group configuration 
 * before saving it.
 */
function validateRoleGroup()
{
	if (!eVal.domainname(txtRGName.value, 0)) {
		alert(eLang.getString("common", "STR_INVALID_RGNAME") +
			eLang.getString("common", "STR_HELP_INFO"));
		txtRGName.focus();
		return false;
	}

	if (!eVal.ldap(txtRGSearchBase.value, "searchbase")) {
		alert(eLang.getString("common", "STR_INVALID_SEARCHBASE") +
			eLang.getString("common", "STR_HELP_INFO"));
		txtRGSearchBase.focus();
		return false;
	}
	return true;
}

/*
 * Used to close the form which is used to add or modify the role group and 
 * used to configure the LDAP information.
 */
function closeForm()
{
	wnd.close();
}