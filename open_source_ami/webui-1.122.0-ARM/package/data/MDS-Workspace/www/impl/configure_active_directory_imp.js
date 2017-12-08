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

var roleGroupTable;
var tblJSON;
var RG_DATA;
var AD_CFG;

function doInit()
{
	exposeElms(["_adDesc",
	 			"_advSettingsBtn",
	 			"_roleGroupHolder",
	 			"_addRoleGroupBtn",
	 			"_modRoleGroupBtn",
	 			"_delRoleGroupBtn",
	 			"_lblHeader"]);

	if(top.user.isAdmin())
	{
		addRoleGroupBtn.onclick = doAddRoleGroup;
		modRoleGroupBtn.onclick = doModifyRoleGroup;
		delRoleGroupBtn.onclick = doDeleteRoleGroup;
	}
	else
	{
		disableActions({id: ["_advSettingsBtn"]});
	}
	advSettingsBtn.onclick = doAdvancedAD;
	_begin();
}

function _begin()
{
	getADCfg();
	loadCustomPageElements();
}

function getADCfg()
{
	advSettingsBtn.disabled = false;
	xmit.get({url:"/rpc/getactivedircfg.asp",onrcv:getADCfgRes,status:""});
	disableButtons();
	xmit.get({url:"/rpc/getallrolegroupcfg.asp",onrcv:getAllRoleGroupInfoRes, status:""});
}

function getADCfgRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr =  eLang.getString("common","STR_CONF_AD_GETINFO");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		AD_CFG = WEBVAR_JSONVAR_GETADCONFIG.WEBVAR_STRUCTNAME_GETADCONFIG[0];
		if (AD_CFG.AD_ENABLE)
			adDesc.innerHTML = eLang.getString("common","STR_CONF_AD_ADENABLE_DESC");
		else
			adDesc.innerHTML = eLang.getString("common","STR_CONF_AD_ADDISABLE_DESC");
	}
}

function loadCustomPageElements()
{
	roleGroupTable = listgrid({
		w				: "100%",
		doAllowNoSelect : false
	});
	roleGroupHolder.appendChild(roleGroupTable.table);

	try
	{
		tblJSON = {cols:[
			{text:eLang.getString("common","STR_CONF_AD_HEAD1"), fieldName:"rg_id", fieldType:2, w:"10%", textAlign:"center"},
			{text:eLang.getString("common","STR_CONF_AD_HEAD2"), fieldName:"rg_name", w:"25%", textAlign:"center"},
			{text:eLang.getString("common","STR_CONF_AD_HEAD3"), fieldName:"rg_domain", w:"35%", textAlign:"center"} ,
			{text:eLang.getString("common","STR_CONF_AD_HEAD4"), fieldName:"rg_priv", w:"30%", textAlign:"center"}
			]};

		roleGroupTable.loadFromJson(tblJSON);
	}
	catch(e)
	{
		alert(e);
	}

	roleGroupTable.ontableselect = function ()
	{
		disableButtons();
		if(top.user.isAdmin() && (AD_CFG.AD_ENABLE))
		{
			if(this.selected.length)
			{
				selectedid = parseInt(roleGroupTable.getRow(roleGroupTable.selected[0]).cells[0].innerHTML);
				if(RG_DATA[selectedid-1].ROLEGROUP_NAME == "")
				{
					addRoleGroupBtn.disabled = false;
				}
				else
				{
					modRoleGroupBtn.disabled = false;
					delRoleGroupBtn.disabled = false;
				}
			}
		}
	}
	roleGroupTable.ondblclick = function()
	{
		if(top.user.isAdmin() && (AD_CFG.AD_ENABLE))
		{
			selectedid = parseInt(roleGroupTable.getRow(roleGroupTable.selected[0]).cells[0].innerHTML);
			if (RG_DATA[selectedid-1].ROLEGROUP_NAME == "")
				doAddRoleGroup();
			else
				doModifyRoleGroup();
		}
	}
}

function disableButtons()
{
	addRoleGroupBtn.disabled = true;
	modRoleGroupBtn.disabled = true;
	delRoleGroupBtn.disabled = true;
}

function getAllRoleGroupInfoRes(arg)
{
	var JSONRows = new Array();
	if (arg.HAPI_STATUS != 0)
	{
		errstr =  eLang.getString("common","STR_CONF_AD_RG_GETINFO");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		roleGroupTable.clear();
		RG_DATA = WEBVAR_JSONVAR_GETALLROLEGROUPCFG.WEBVAR_STRUCTNAME_GETALLROLEGROUPCFG;

		var tRoleGroupCnt = 0;
		for (i=0;i<RG_DATA.length;i++)
		{
			// Use ~ char to indicate free slot so it will sort alphabetically
			rgnametodisplay = (RG_DATA[i].ROLEGROUP_NAME == "")?"~":RG_DATA[i].ROLEGROUP_NAME;
			rgdomaintodisplay = (RG_DATA[i].ROLEGROUP_DOMAIN == "")?"~":RG_DATA[i].ROLEGROUP_DOMAIN;
			rgprivtodisplay = (RG_DATA[i].ROLEGROUP_PRIVILEGE)?IPMIPrivileges[RG_DATA[i].ROLEGROUP_PRIVILEGE]:"~";
			if(rgnametodisplay!="~")
				tRoleGroupCnt++;

			try
			{
				JSONRows.push({cells:[
					{text:RG_DATA[i].ROLEGROUP_ID, value:RG_DATA[i].ROLEGROUP_ID},
					{text:rgnametodisplay, value:rgnametodisplay},
					{text:rgdomaintodisplay, value:rgdomaintodisplay},
					{text:rgprivtodisplay, value:rgprivtodisplay}
				]});
			}
			catch(e)
			{
				alert(e);
			}
		}

		tblJSON.rows = JSONRows;
		roleGroupTable.loadFromJson(tblJSON);
		lblHeader.innerHTML = eLang.getString("common","STR_CONF_AD_RG_CNT") + tRoleGroupCnt + eLang.getString("common","STR_BLANK");
	}
}

function doAdvancedAD()
{
	frmAdvancedAD();
	loadAdvancedADCfg();
}

function frmAdvancedAD()
{
	var frm = new form("advancedADFrm","POST","javascript://","general");
	settings = frm.addCheckBox(eLang.getString("common","STR_CONF_AD_ADV_ENABLEAD"),"adAuth",{"enable":"Enable"},false,["enable"]);
	enableAD = settings.enable;
	domainName = frm.addTextField(eLang.getString("common","STR_CONF_AD_ADV_DOMAINNAME"),"_domainName","",{"maxLength":255},"bigclassicTxtBox");
	timeOut = frm.addTextField(eLang.getString("common","STR_CONF_AD_ADV_TIMEOUT"),"_timeOut","",{"maxLength":3},"smallclassicTxtBox");
	domainAddress1 = frm.addTextField(eLang.getString("common","STR_CONF_AD_ADV_DOMAINSRVR1"),"_domainAddress1","",{"maxLength":40},"classicTxtBox");
	domainAddress2 = frm.addTextField(eLang.getString("common","STR_CONF_AD_ADV_DOMAINSRVR2"),"_domainAddress2","",{"maxLength":40},"classicTxtBox");
	domainAddress3 = frm.addTextField(eLang.getString("common","STR_CONF_AD_ADV_DOMAINSRVR3"),"_domainAddress3","",{"maxLength":40},"classicTxtBox");

	var btnAry = [];
	btnAry.push(createButton("saveBtn",eLang.getString("common","STR_CONF_AD_ADV_SAVE"),isADCfgChange));
	btnAry.push(createButton("cancelBtn",eLang.getString("common","STR_CANCEL"),closeForm));
	enableAD.onclick = enableADOptions;
	wnd = MessageBox(eLang.getString("common","STR_CONF_AD_ADV_TITLE"),frm.display(),btnAry);
	wnd.onclose = getADCfg;
	enableAD.focus();
	if(!top.user.isAdmin())
	{
		disableActions({id: ["_advSettingsBtn","_cancelBtn"]});
	}
}

function getAdvancedADCfg()
{
	xmit.get({url:"/rpc/getactivedircfg.asp",onrcv:getAdvancedADCfgRes,status:""});
}

function loadAdvancedADCfg()
{
	enableAD.checked = AD_CFG.AD_ENABLE?true:false;
	enableADOptions();
	domainName.value = AD_CFG.AD_DOMAINNAME;
	timeOut.value = AD_CFG.AD_TIMEOUT;
	domainAddress1.value = AD_CFG.AD_DOMAINSRVR1;
	domainAddress2.value = AD_CFG.AD_DOMAINSRVR2;
	domainAddress3.value = AD_CFG.AD_DOMAINSRVR3;
}

function enableADOptions()
{
	if (top.user.isAdmin())
	{
		optenable = !enableAD.checked;

		domainName.disabled = optenable;
		timeOut.disabled = optenable;
		domainAddress1.disabled = optenable;
		domainAddress2.disabled = optenable;
		domainAddress3.disabled = optenable;
		if(!optenable)
			domainName.focus();
	}
	if (!enableAD.checked)
	{
		domainName.value = AD_CFG.AD_DOMAINNAME;
		timeOut.value = AD_CFG.AD_TIMEOUT;
		domainAddress1.value = AD_CFG.AD_DOMAINSRVR1;
		domainAddress2.value = AD_CFG.AD_DOMAINSRVR2;
		domainAddress3.value = AD_CFG.AD_DOMAINSRVR3;
	}
}

/*
 * This function is used to compare the configuration values with the data in 
 * the controls.
 */
function isADCfgChange()
{
	if ((AD_CFG.AD_ENABLE == enableAD.checked) && 
		(AD_CFG.AD_DOMAINNAME == domainName.value) && 
		(AD_CFG.AD_TIMEOUT == timeOut.value) && 
		(AD_CFG.AD_DOMAINSRVR1 == domainAddress1.value) && 
		(AD_CFG.AD_DOMAINSRVR2 == domainAddress2.value) && 
		(AD_CFG.AD_DOMAINSRVR3 == domainAddress3.value)) {
		return;
	}
	validateADCfg();
}

function validateADCfg()
{
	if(enableAD.checked)
	{
		if(!eVal.domainname(domainName.value,1))
		{
			alert(eLang.getString("common","STR_CONF_AD_ADV_INVALID_DOMAINNAME") + eLang.getString("common","STR_HELP_INFO"));
			domainName.focus();
			return false;
		}
		if(!eVal.isnumstr(timeOut.value, 15, 300))
		{
			alert(eLang.getString("common","STR_INVALID_TIMEOUT") + eLang.getString("common","STR_HELP_INFO"));
			timeOut.focus();
			return false;
		}

		if ((eVal.isblank(domainAddress1.value)) && (eVal.isblank(domainAddress2.value)) && (eVal.isblank(domainAddress3.value)))
		{
			alert(eLang.getString("common","STR_CONF_AD_ADV_INVALID_DOMAINSRVR"));
			domainAddress1.focus();
			return false;
		}
		if (!eVal.isblank(domainAddress1.value))
		{
			if (!eVal.ip(domainAddress1.value) && !eVal.ipv6(domainAddress1.value))
			{
				alert(eLang.getString("common","STR_CONF_AD_ADV_INVALID_DOMAINSRVR1") + eLang.getString("common","STR_HELP_INFO"));
				domainAddress1.focus();
				return false;
			}
		}
		if (!eVal.isblank(domainAddress2.value))
		{
			if ((!eVal.ip(domainAddress2.value)) && !eVal.ipv6(domainAddress2.value))
			{
				alert(eLang.getString("common","STR_CONF_AD_ADV_INVALID_DOMAINSRVR2") + eLang.getString("common","STR_HELP_INFO"));
				domainAddress2.focus();
				return false;
			}
		}
		if (!eVal.isblank(domainAddress3.value))
		{
			if ((!eVal.ip(domainAddress3.value)) && !eVal.ipv6(domainAddress3.value))
			{
				alert(eLang.getString("common","STR_CONF_AD_ADV_INVALID_DOMAINSRVR3") + eLang.getString("common","STR_HELP_INFO"));
				domainAddress3.focus();
				return false;
			}
		}
		if ((eVal.compareip(domainAddress1.value,domainAddress2.value)) || 
			(eVal.compareip(domainAddress1.value,domainAddress3.value)) || 
			(eVal.compareip(domainAddress2.value,domainAddress3.value)))
		{
			alert(eLang.getString("common","STR_CONF_AD_ADV_DIFF_DOMAINSRVR"));
			return false;
		}
		if ((eVal.comparev6ip(domainAddress1.value,domainAddress2.value)) || 
			(eVal.comparev6ip(domainAddress1.value,domainAddress3.value)) || 
			(eVal.comparev6ip(domainAddress2.value,domainAddress3.value)))
		{
			alert(eLang.getString("common","STR_CONF_AD_ADV_DIFF_DOMAINSRVR"));
			return false;
		}
	}
	doSetADCfg();
	return true;
}

function doSetADCfg()
{
	if (top.user.isAdmin())
	{
		var rpc_adcfg = new xmit.getset({url:"/rpc/setactivedircfg.asp", onrcv:doSetADCfgRes,status:""});

		rpc_adcfg.add("AD_ENABLE", enableAD.checked?1:0);
		rpc_adcfg.add("AD_DOMAINNAME", domainName.value);
		rpc_adcfg.add("AD_TIMEOUT", parseInt(timeOut.value,10));
		rpc_adcfg.add("AD_DOMAINSRVR1", domainAddress1.value);
		rpc_adcfg.add("AD_DOMAINSRVR2", domainAddress2.value);
		rpc_adcfg.add("AD_DOMAINSRVR3", domainAddress3.value);
		rpc_adcfg.send();
		delete rpc_adcfg;
	}
	else
		alert(eLang.getString("common","STR_CONF_AD_ADV_ADMINPRIV"));
}

function doSetADCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr =  eLang.getString("common","STR_CONF_AD_SETINFO");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_ADV_SAVE_SUCCESS"));
		closeForm();
	}
}

function doAddRoleGroup()
{
	if ((roleGroupTable.selected.length != 1) || (roleGroupTable.selected[0].cells[0] == undefined) || (roleGroupTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_ERR1"));
		disableButtons();
	}
	else
	{
		selectedrgid = parseInt(roleGroupTable.getRow(roleGroupTable.selected[0]).cells[0].innerHTML);
		selectedrgname = roleGroupTable.getRow(roleGroupTable.selected[0]).cells[1].innerHTML;
		selectedrgdomain = roleGroupTable.getRow(roleGroupTable.selected[0]).cells[2].innerHTML;
		selectedrgpriv = getbits(RG_DATA[selectedid-1].ROLEGROUP_PRIVILEGE,3,0);
		if ("" != RG_DATA[selectedid-1].ROLEGROUP_NAME)
		{
			if (confirm(eLang.getString("common","STR_CONF_AD_CONFIRM1")))
			{
				frmModifyRoleGroup({"rgindex":selectedrgid,
								"rgname":selectedrgname,
								"rgdomain":selectedrgdomain,
								"rgpriv":selectedrgpriv});
			}
		}
		else
		{
			frmAddRoleGroup({"rgindex":selectedrgid});
		}
	}
}

function frmAddRoleGroup(arg)
{
	var frm = new form("addRoleGroup","POST","javascript://","general");
	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName","",{"maxLength":64},"classicTxtBox");
	rgDomain = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_DOMAIN"),"_rgDomain","",{"maxLength":255},"bigclassicTxtBox");

	var privVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
	rgPriv = frm.addSelectBox(eLang.getString("common","STR_CONF_AD_RG_PRIV"),"_rgPriv",privVals,"","","","classicTxtBox");

	var btnAry = [];
	btnAry.push(createButton("addBtn",eLang.getString("common","STR_ADD"),addRoleGroup));
	btnAry.push(createButton("cancelBtn",eLang.getString("common","STR_CANCEL"),closeForm));
	rgindex = arg.rgindex;

	wnd = MessageBox(eLang.getString("common","STR_CONF_AD_RG_ADD_TITLE"),frm.display(),btnAry);
	wnd.onclose = getADCfg;
	rgName.focus();
}

function addRoleGroup()
{
	if (validateRoleGroup())
	{
		var req = new xmit.getset({url:"/rpc/addrolegroup.asp",onrcv:addRoleGroupRes});
		req.add("ROLEGROUP_ID",rgindex);
		req.add("ROLEGROUP_NAME",rgName.value);
		req.add("ROLEGROUP_DOMAIN",rgDomain.value);
		req.add("ROLEGROUP_PRIV",rgPriv.value);
		req.send();
		delete req;
	}
}

function addRoleGroupRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		switch(GET_ERROR_CODE(arg.HAPI_STATUS))
		{
			case 0xcc:
				rErrStr = eLang.getString("common","STR_CONF_AD_RG_ERR3");
				alert(rErrStr);
			break;
			default:
				rErrStr = eLang.getString("common","STR_CONF_AD_RG_ADDINFO");
				rErrStr += (eLang.getString("common","STR_IPMI_ERROR")+GET_ERROR_CODE(arg.HAPI_STATUS));
				alert(rErrStr);
			break;
		}
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_RG_SAVE_SUCCESS_1"));
	}
	closeForm();
}

function validateRoleGroup()
{
	if (!eVal.str(rgName.value))
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_INVALID_RGNAME")+eLang.getString("common","STR_HELP_INFO"));
		return false;
	}
	if (!eVal.domainname(rgDomain.value,1))
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_INVALID_RGDOMAIN")+eLang.getString("common","STR_HELP_INFO"));
		return false;
	}
	return true;
}

function doModifyRoleGroup()
{
	if ((roleGroupTable.selected.length != 1) || (roleGroupTable.selected[0].cells[0] == undefined) || (roleGroupTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_ERR1"));
		disableButtons();
		return;
	}
	else
	{
		selectedrgid = parseInt(roleGroupTable.getRow(roleGroupTable.selected[0]).cells[0].innerHTML);
		selectedrgname = roleGroupTable.getRow(roleGroupTable.selected[0]).cells[1].innerHTML;
		selectedrgdomain = roleGroupTable.getRow(roleGroupTable.selected[0]).cells[2].innerHTML;
		selectedrgpriv = getbits(RG_DATA[selectedid-1].ROLEGROUP_PRIVILEGE,3,0);

		if ("" == RG_DATA[selectedid-1].ROLEGROUP_NAME)
		{
			if (confirm(eLang.getString("common","STR_CONF_AD_CONFIRM2")))
			{
				frmAddRoleGroup({"rgindex":selectedrgid});
			}
		}
		else
		{
			frmModifyRoleGroup({"rgindex":selectedrgid,
							"rgname":selectedrgname,
							"rgdomain":selectedrgdomain,
							"rgpriv":selectedrgpriv});
		}
	}
}

function frmModifyRoleGroup(arg)
{
	var frm = new form("modRoleGroup","POST","javascript://","general");
	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName",arg.rgname,{"maxLength":64},"classicTxtBox");
	rgDomain = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_DOMAIN"),"_rgDomain",arg.rgdomain,{"maxLength":255},"bigclassicTxtBox");

	var privVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
	rgPriv = frm.addSelectBox(eLang.getString("common","STR_CONF_AD_RG_PRIV"),"_rgPriv",privVals,arg.rgpriv,"","","classicTxtBox");

	var btnAry = [];
	btnAry.push(createButton("modBtn",eLang.getString("common","STR_MODIFY"),modifyRoleGroup));
	btnAry.push(createButton("cancelBtn",eLang.getString("common","STR_CANCEL"),closeForm));
	rgindex = arg.rgindex;

	wnd = MessageBox(eLang.getString("common","STR_CONF_AD_RG_MODIFY_TITLE"),frm.display(),btnAry);
	wnd.onclose = getADCfg;
}

function modifyRoleGroup()
{
	if (validateRoleGroup())
	{
		var req = new xmit.getset({url:"/rpc/modrolegroup.asp",onrcv:modifyRoleGroupRes});
		req.add("ROLEGROUP_ID",rgindex);
		req.add("ROLEGROUP_NAME",rgName.value);
		req.add("ROLEGROUP_DOMAIN",rgDomain.value);
		req.add("ROLEGROUP_PRIV",rgPriv.value);
		req.send();
		delete req;
	}
}

function modifyRoleGroupRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		switch(GET_ERROR_CODE(arg.HAPI_STATUS))
		{
			case 0xcc:
				errstr = eLang.getString("common","STR_CONF_AD_RG_ERR3");
				alert(errstr);
			break;
			default:
				errstr = eLang.getString("common","STR_CONF_AD_RG_MODINFO");
				errstr += (eLang.getString("common","STR_IPMI_ERROR")+GET_ERROR_CODE(arg.HAPI_STATUS));
				alert(errstr);
			break;
		}
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_RG_SAVE_SUCCESS_2"));
	}
	closeForm();
}

function doDeleteRoleGroup()
{
	if ((roleGroupTable.selected.length != 1) || (roleGroupTable.selected[0].cells[0] == undefined) || (roleGroupTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_ERR1"));
		disableButtons();
	}
	else
	{
		selectedid = parseInt(roleGroupTable.getRow(roleGroupTable.selected[0]).cells[0].innerHTML);
		if (RG_DATA[selectedid - 1].ROLEGROUP_NAME == "")
			alert (eLang.getString("common","STR_CONF_AD_RG_ERR2"));
		else if (confirm(eLang.getString("common","STR_CONFIRM_DELETE")))
		{
			var req = new xmit.getset({url:"/rpc/delrolegroup.asp",onrcv:deleteRoleGroupRes});
			req.add("ROLEGROUP_ID",selectedid);
			req.send();
			delete req;
		}
	}
}

function deleteRoleGroupRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString("common","STR_CONF_AD_RG_DELINFO");
		errstr +=  (eLang.getString("common","STR_IPMI_ERROR") +GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_AD_RG_DELETE_SUCCESS"));
		getADCfg();
	}
}

function closeForm()
{
	wnd.close();
}
