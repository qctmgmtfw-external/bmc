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

// File Name  : alert_policy_imp.js
// Brief      : This implementation is to configure alert policy entries.
// Author Name:

var ALERTPOLICY_DATA;
var alertPolicyTable;
var policy_oper;

function initPolicyAction()
{
	addBtn.onclick = doAddPolicy;
	modBtn.onclick = doModifyPolicy;
	delBtn.onclick = doDeletePolicy;
}

function loadAlertPolicyElements()
{
	var height = parent.$('pageFrame').offsetHeight - 270;
	height = (height > 45) ? height : 45;
	alertPolicyTable = listgrid
	({
		w				: '100%',
		h				:  height + 'px',
		doAllowNoSelect : false
	});
	listGridHolder.appendChild(alertPolicyTable.table);

	try
	{
		tblJSON =
		{
			cols:[
				{text:eLang.getString('common',"STR_POLICY_ENTRY"), fieldName:'policy_entry', fieldType:2, w:'10%', textAlign:'center'},
				{text:eLang.getString('common',"STR_POLICY_NO"), fieldName:'policy_no', fieldType:2, w:'10%', textAlign:'center'},
				{text:eLang.getString('common',"STR_POLICY_SETTING"), fieldName:'policy_setting', w:'20%', textAlign:'center'},
				{text:eLang.getString('common',"STR_POLICY_SET"), fieldName:'policy_set', w:'40%', textAlign:'center'},
				{text:eLang.getString('common',"STR_CHANNEL_NO"), fieldName:'channel_no', w:'10%', textAlign:'center'},
				{text:eLang.getString('common',"STR_DEST_SELECT"), fieldName:'dest_select', w:'10%', textAlign:'center'}
			]};

		alertPolicyTable.loadFromJson(tblJSON);
	}
	catch(e)
	{
		alert(e);
	}

	if(top.user.isAdmin())
	{
		alertPolicyTable.ontableselect = function ()
		{
			disableButtons();
			if(this.selected.length)
			{
				var policyset = alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
				if (policyset == "~")
					addBtn.disabled = false;
				else
				{
					modBtn.disabled = false;
					delBtn.disabled = false;
				}
			}
		}

		alertPolicyTable.ondblclick = function()
		{
			var policyset = alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
			if (policyset == "~")
				doAddPolicy();
			else
				doModifyPolicy();
		}
	}

}

function getAllPolicyCfg()
{
	disableButtons();
	xmit.get({url:'/rpc/getallpolicycfg.asp', onrcv:getAllPolicyCfgRes, status:''});
}

function getAllPolicyCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_POLICY_GETVAL");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		ALERTPOLICY_DATA = WEBVAR_JSONVAR_HL_GETPOLICYTABLE.WEBVAR_STRUCTNAME_HL_GETPOLICYTABLE;
		loadAlertPolicyTable();
	}
}

function loadAlertPolicyTable()
{
	var tPolicyCnt = 0;
	var JSONRows = new Array();
	var policy_entry_todisplay, policy_number_todisplay;
	var policy_setting_todisplay, policy_set_todisplay;
	var channel_no_todisplay, dest_select_todisplay;

	alertPolicyTable.clear();

	for (i=0;i<ALERTPOLICY_DATA.length;i++)
	//for (i=0; i<15; i++)
	{
		// Use ~ char to indicate free slot so it will sort alphabetically
		policy_number_todisplay = '~';
		policy_setting_todisplay = '~';
		policy_set_todisplay = '~';
		channel_no_todisplay = '~';
		dest_select_todisplay = '~';

//		if (!((ALERTPOLICY_DATA[i].PolicyNumber == 0) && (ALERTPOLICY_DATA[i].EnablePolicy == 0) &&
		if (!((ALERTPOLICY_DATA[i].EnablePolicy == 0) &&
			(ALERTPOLICY_DATA[i].PolicySet == 0) && (ALERTPOLICY_DATA[i].ChannelNumber == 0) &&
			(ALERTPOLICY_DATA[i].DestSelector == 0) && (ALERTPOLICY_DATA[i].EventSpecific == 0) &&
			(ALERTPOLICY_DATA[i].AlertString == 0)))
		{
			tPolicyCnt++;
			policy_number_todisplay = ALERTPOLICY_DATA[i].PolicyNumber;
			policy_setting_todisplay = ALERTPOLICY_DATA[i].EnablePolicy ? eLang.getString('common', 'STR_ENABLED') : eLang.getString('common', 'STR_DISABLED') 
			policy_set_todisplay = eLang.getString('common','STR_POLICY_SET_STR_' + ALERTPOLICY_DATA[i].PolicySet);
			channel_no_todisplay = ALERTPOLICY_DATA[i].ChannelNumber;
			dest_select_todisplay = ALERTPOLICY_DATA[i].DestSelector;
		}
		try
		{
			JSONRows.push({cells:[
				{text:ALERTPOLICY_DATA[i].PolicyEntryNumber, value:ALERTPOLICY_DATA[i].PolicyEntryNumber},
				{text:policy_number_todisplay, value:policy_number_todisplay},
				{text:policy_setting_todisplay, value:policy_setting_todisplay},
				{text:policy_set_todisplay, value:policy_set_todisplay},
				{text:channel_no_todisplay, value:channel_no_todisplay},
				{text:dest_select_todisplay, value:dest_select_todisplay}
			]});
		}
		catch(e)
		{
			alert(e);
		}
	}

	tblJSON.rows = JSONRows;
	alertPolicyTable.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common','STR_POLICY_CNT') + 
						"</strong>" + tPolicyCnt + eLang.getString('common','STR_BLANK');
}

function doAddPolicy()
{
	if ((alertPolicyTable.selected.length != 1) || 
		(alertPolicyTable.selected[0].cells[0] == undefined) || 
		(alertPolicyTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_CONF_POLICY_ERR1'));
		disableButtons();
	}
	else
	{
		var policyset = alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
		policyentry = parseInt(alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[0].innerHTML);
		if (policyset != "~")
		{
			if (confirm(eLang.getString('common','STR_CONF_POLICY_CONFIRM1')))
				frmModifyPolicy({'policyentry':policyentry});
		}
		else
			frmAddPolicy({'policyentry':policyentry});
	}
}

function doModifyPolicy()
{
	if ((alertPolicyTable.selected.length != 1) || 
		(alertPolicyTable.selected[0].cells[0] == undefined) || 
		(alertPolicyTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_CONF_POLICY_ERR2'));
		disableButtons();
	}
	else
	{
		var policyset = alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
		policyentry = parseInt(alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[0].innerHTML);
		if (policyset == "~")
		{
			if (confirm(eLang.getString('common','STR_CONF_POLICY_CONFIRM2')))
				frmAddPolicy({'policyentry':policyentry});
		}
		else
			frmModifyPolicy({'policyentry':policyentry});
	}
}

function doDeletePolicy()
{
	if ((alertPolicyTable.selected.length != 1) || 
		(alertPolicyTable.selected[0].cells[0] == undefined) || 
		(alertPolicyTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_CONF_POLICY_ERR1'));
		disableButtons();
	}
	else if (alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[2].innerHTML.replace('&nbsp;','').replace(' ','') == "~")
	{
		alert (eLang.getString('common','STR_CONF_POLICY_ERR2'));
	}
	else
		if (confirm(eLang.getString('common','STR_CONFIRM_DELETE')))
			deletePolicyCfg(parseInt(alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[0].innerHTML),
			parseInt(alertPolicyTable.getRow(alertPolicyTable.selected[0]).cells[1].innerHTML));
}

function frmAddPolicy(arg)
{
	policy_oper = 1;			//Adding Alert Policy Entry
	var frm = new form('addAlertPolicyForm','POST','javascript://','general');

	policyEntry = frm.addTextField(eLang.getString('common','STR_POLICY_ENTRY'),
								'_policyEntry',arg.policyentry,{'readOnly':true},'smallclassicTxtBox');

	policyNo = frm.addSelectBox(eLang.getString('common','STR_POLICY_NO'),
								'_policyNo','','','','','smallclassicTxtBox');
	fillPolicyNumber();

	policyEnableSettings = frm.addCheckBox(eLang.getString('common','STR_POLICY_SETTING'),
								'',{'_policyEnable':'Enable'},false,['']);
	policyEnable = policyEnableSettings._policyEnable;

	policySet = frm.addSelectBox(eLang.getString('common','STR_POLICY_SET'),
								'_policySet','','','','','smallclassicTxtBox');
	fillPolicySet();

	channelNo = frm.addSelectBox(eLang.getString('common','STR_CHANNEL_NO'),
								'_channelNo','','','','','smallclassicTxtBox');
	fillChannelNumber();

	destSelector = frm.addSelectBox(eLang.getString('common','STR_DEST_SELECT'),
								'_destSelector','','','','','smallclassicTxtBox');
	fillDestSelector();

	eventSpecificVals = frm.addCheckBox(eLang.getString('common','STR_ALERT_STRING'),
								'',{'_eventSpecific':eLang.getString('common','STR_EVENT_SPECIFIC')},false,['']);
	eventSpecific = eventSpecificVals._eventSpecific;

	alertString = frm.addSelectBox(eLang.getString('common','STR_ALERT_STRING_KEY'),
								'_alertString','','','','','smallclassicTxtBox');
	fillAlertStringKey();
	
	var btnAry = [];
	btnAry.push(createButton('addBtn',eLang.getString('common','STR_ADD'),setPolicyCfg));
	btnAry.push(createButton('cancelBtn',eLang.getString('common','STR_CANCEL'),closeAlertPolicyForm));

	wnd = MessageBox(eLang.getString('common','STR_ADD_ALERT_POLICY'),frm.display(),btnAry);
	//Ignores backspace functionality
	policyEntry.onkeydown = checkBackspace;
	wnd.onclose = getAllPolicyCfg;
}

function frmModifyPolicy(arg)
{
	policy_oper = 2;			//Modifying Alert Policy Entry
	var frm = new form('modifyAlertPolicyForm','POST','javascript://','general');

	policyEntry = frm.addTextField(eLang.getString('common','STR_POLICY_ENTRY'),
								'_policyEntry',arg.policyentry,{'readOnly':true},'smallclassicTxtBox');

	policyNo = frm.addSelectBox(eLang.getString('common','STR_POLICY_NO'),
								'_policyNo','','','','','smallclassicTxtBox');
	fillPolicyNumber();
	policyNo.value = ALERTPOLICY_DATA[arg.policyentry-1].PolicyNumber;

	policyEnableSettings = frm.addCheckBox(eLang.getString('common','STR_POLICY_SETTING'),
								'',{'_policyEnable':'Enable'},false,['']);
	policyEnable = policyEnableSettings._policyEnable;
	policyEnable.checked = ALERTPOLICY_DATA[arg.policyentry-1].EnablePolicy ? true : false;

	policySet = frm.addSelectBox(eLang.getString('common','STR_POLICY_SET'),
								'_policySet','','','','','smallclassicTxtBox');
	fillPolicySet();
	policySet.value = ALERTPOLICY_DATA[arg.policyentry-1].PolicySet;

	channelNo = frm.addSelectBox(eLang.getString('common','STR_CHANNEL_NO'),
								'_channelNo','','','','','smallclassicTxtBox');
	fillChannelNumber();
	channelNo.value = ALERTPOLICY_DATA[arg.policyentry-1].ChannelNumber;

	destSelector = frm.addSelectBox(eLang.getString('common','STR_DEST_SELECT'),
								'_destSelector','','','','','smallclassicTxtBox');
	fillDestSelector();
	destSelector.value = ALERTPOLICY_DATA[arg.policyentry-1].DestSelector;

	eventSpecificVals = frm.addCheckBox(eLang.getString('common','STR_ALERT_STRING'),
								'',{'_eventSpecific':eLang.getString('common','STR_EVENT_SPECIFIC')},false,['']);
	eventSpecific = eventSpecificVals._eventSpecific;
	eventSpecific.checked = ALERTPOLICY_DATA[arg.policyentry-1].EventSpecific ? true : false;

	alertString = frm.addSelectBox(eLang.getString('common','STR_ALERT_STRING_KEY'),
								'_alertString','','','','','smallclassicTxtBox');
	fillAlertStringKey();
	alertString.value = ALERTPOLICY_DATA[arg.policyentry-1].AlertString;

	var btnAry = [];
	btnAry.push(createButton('modifyBtn',eLang.getString('common','STR_MODIFY'),setPolicyCfg));
	btnAry.push(createButton('cancelBtn',eLang.getString('common','STR_CANCEL'),closeAlertPolicyForm));

	wnd = MessageBox(eLang.getString('common','STR_MODIFY_ALERT_POLICY'),frm.display(),btnAry);
	//Ignores backspace functionality
	policyEntry.onkeydown = checkBackspace;
	wnd.onclose = getAllPolicyCfg;
}

function setPolicyCfg()
{
	var req = new xmit.getset({url:'/rpc/setalertpolicy.asp', onrcv:setPolicyCfgRes, status:''});
	req.add("PolicyEntryNumber",policyEntry.value);
	req.add("PolicyNumber",policyNo.value);
	req.add("EnablePolicy",policyEnable.checked ? 1 : 0);
	req.add("PolicySet",policySet.value);
	req.add("ChannelNumber",channelNo.value);
	req.add("DestSelector",destSelector.value);
	req.add("EventSpecific",eventSpecific.checked ? 1 : 0);
	req.add("AlertString",alertString.value);
	req.send();
	delete req;
}

function setPolicyCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_POLICY_SETVAL");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		if(policy_oper == 1)			//Add Alert Policy Entry
			alert (eLang.getString('common', 'STR_CONF_POLICY_ADD_SUCCESS'));
		else if(policy_oper == 2)		//Modify Alert Policy Entry
			alert (eLang.getString('common', 'STR_CONF_POLICY_MOD_SUCCESS'));
		closeAlertPolicyForm();
	}
}

function deletePolicyCfg(policyentry, policyno)
{
	var req = new xmit.getset({url:'/rpc/setalertpolicy.asp', onrcv:deletePolicyCfgRes, status:''});
	req.add("PolicyEntryNumber", policyentry);
	req.add("PolicyNumber", policyno);
	req.add("EnablePolicy", 0);
	req.add("PolicySet", 0);
	req.add("ChannelNumber", 0);
	req.add("DestSelector", 0);
	req.add("EventSpecific", 0);
	req.add("AlertString", 0);
	req.send();
	delete req;
}

function deletePolicyCfgRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_POLICY_DELETEVAL");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		alert (eLang.getString('common', 'STR_CONF_POLICY_DELETE_SUCCESS'));
		getAllPolicyCfg();
	}
}

function fillPolicyNumber()
{
	var optind = 0;
	policyNo.innerHTML = '';
	for(var i=1; i<16; i++)
		policyNo.add(new Option(i,i),isIE?optind++:null);
}

function fillPolicySet()
{
	var optind = 0;
	policySet.innerHTML = '';
	for(var i=0; i<5; i++)
		policySet.add(new Option(i,i),isIE?optind++:null);
}

function fillChannelNumber()
{
	var optind = 0;
	channelNo.innerHTML = '';
	if (fnCookie.read('gMultiLAN')) 	//MulitiLAN support
	{
		for (i=0;i<top.settings.lan.length;i++)
			channelNo.add(new Option(top.settings.lan[i],top.settings.lan[i]), isIE?optind++:null);
	}
	else
	{
		channelNo.add(new Option(1,1),isIE?optind++:null);
	}
}

function fillDestSelector()
{
	var optind = 0;
	destSelector.innerHTML = '';
	for(var i=1; i<16; i++)
		destSelector.add(new Option(i,i),isIE?optind++:null);
}

function fillAlertStringKey()
{
	var optind = 0;
	alertString.innerHTML = '';
	for(var i=0; i<128; i++)
		alertString.add(new Option(i,i),isIE?optind++:null);
}

function closeAlertPolicyForm()
{
//	getAllPolicyCfg();
	wnd.close();
}
