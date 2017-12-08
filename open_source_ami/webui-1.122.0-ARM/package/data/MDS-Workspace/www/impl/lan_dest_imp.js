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

// File Name  : lan_dest_imp.js
// Brief      : This implementation is to configure LAN destination entries.
// Author Name:

var LANDESTS_DATA;
var USERINFO_DATA;
var lanDestsTable;
var landest_oper;

function initLANDestAction()
{
	getLanChannel();
	addBtn.onclick = doADDLANDest;
	modBtn.onclick = doModifyLANDest;
	delBtn.onclick = doDeleteLANDest;
	sendAlert.onclick = doSendAlert;
}

function loadLANDestElements()
{
	var height = parent.$('pageFrame').offsetHeight - 270;
	height = (height > 45) ? height : 45;
	lanDestsTable = listgrid({
		w				: '100%',
		h				:  height + 'px',
		doAllowNoSelect : false
	});

	listGridHolder.appendChild(lanDestsTable.table);

	try
	{
		tblJSON = {
			cols:[
			{text:eLang.getString('common','STR_LAN_DEST_HEAD1'), fieldName:'alertNo', fieldType:2, w:'15%', textAlign:'center'},
			{text:eLang.getString('common','STR_LAN_DEST_HEAD2'), fieldName:'alertLevel', w:'35%', textAlign:'center'},
			{text:eLang.getString('common','STR_LAN_DEST_HEAD3'), fieldName:'destAddr', w:'47%', textAlign:'center'}
			]
		};

		lanDestsTable.loadFromJson(tblJSON);
	}
	catch(e)
	{
		alert(e);
	}

	if(top.user.isAdmin())
	{
		lanDestsTable.ontableselect = function ()
		{
			disableButtons();
			if(this.selected.length)
			{
				var desttype = lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
				if (desttype == "~")
					addBtn.disabled = false;
				else
				{
					modBtn.disabled = false;
					delBtn.disabled = false;
					sendAlert.disabled = false;
				}
			}
		};

		lanDestsTable.ondblclick = function()
		{
			var desttype = lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
			if (desttype == "~")
				doADDLANDest();
			else
				doModifyLANDest();
		};
	}
}

function getLanChannel()
{
	if (fnCookie.read('gMultiLAN'))	//MulitiLAN support
	{
		lanChannelLabel.style.display = '';
		lanChannel.style.display = '';

		var optind = 0;
		lanChannel.innerHTML = '';
		for (i=0;i<top.settings.lan.length;i++)
			lanChannel.add(new Option(top.settings.lan[i],top.settings.lan[i]), isIE?optind++:null);
	
		if (top.mainFrame.pageFrame.location.hash)
			lanChannel.value = comboParser(top.mainFrame.pageFrame.location.hash, lanChannel.id);	//id->id of the selection box
		lanChannel.onchange = getLANDestsTable;
	}
	else
	{
		lanChannelLabel.style.display = 'none';
		lanChannel.style.display = 'none';
	}
	getAllUserInfo();
}

function getAllUserInfo()
{
	xmit.get({url:"/rpc/getalluserinfo.asp",onrcv:getAllUserInfoRes, status:''});
}

function getAllUserInfoRes (arg)
{
	if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0xD4)	//Insufficient privilege level
	{
		alert (eLang.getString('common',"STR_PERMISSION_DENIED"));
		location.href = 'dashboard.html';
	}
	else if (arg.HAPI_STATUS)
	{
		var errstr =  eLang.getString('common','STR_CONF_USER_GETINFO');
		errstr += (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		USERINFO_DATA = WEBVAR_JSONVAR_HL_GETALLUSERINFO.WEBVAR_STRUCTNAME_HL_GETALLUSERINFO;
	}
	getLANDestsTable();
}

function getLANDestsTable()
{
	disableButtons();
	var req = new xmit.getset({url:"/rpc/getlandeststable.asp", onrcv:getLANDestsRes, status:''});
	if (fnCookie.read('gMultiLAN'))
	{
		lanChannel.disabled = false;
		req.add("CHANNEL_NUM",lanChannel.value);
	}
	else
	{
		req.add("CHANNEL_NUM",1);
	}
	req.send();
	delete req;
}

function getLANDestsRes (arg)
{
	if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0xD4)	//Insufficient privilege level
	{
		alert (eLang.getString('common','STR_PERMISSION_DENIED'));
		location.href = 'dashboard.html';
	}
	else if (arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_LAN_DEST_GETVAL')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		LANDESTS_DATA = WEBVAR_JSONVAR_HL_GETLANDESTSTABLE.WEBVAR_STRUCTNAME_HL_GETLANDESTSTABLE;
		if (LANDESTS_DATA.length == 0)
			alert(eLang.getString('common','NO_LANALERTDESTS_STRING'));
		loadLANDestTable();
	}
}

function loadLANDestTable()
{
	var tLANDestCnt = 0;
	var JSONRows = new Array();
	var dest_type_todisplay;
	var dest_addr_todisplay;
	var userIndex;

	lanDestsTable.clear();

	for (i=0;i<LANDESTS_DATA.length;i++)
	{
		// Use ~ char to indicate free slot so it will sort alphabetically
		dest_type_todisplay = '~';
		dest_addr_todisplay = '~';
		if (! ((LANDESTS_DATA[i].DestAddr == eLang.getString('common','STR_IPV4_ADDR0') ||
			LANDESTS_DATA[i].DestAddr == eLang.getString('common','STR_IPV6_ADDR0') ||
			LANDESTS_DATA[i].DestAddr == '') && (LANDESTS_DATA[i].UserID == 0)))
		{
			tLANDestCnt++;
			dest_type_todisplay = eLang.getString('common','STR_DEST_TYPE_' + LANDESTS_DATA[i].DestType);
			if (LANDESTS_DATA[i].DestType == 0)		//SNMP Trap IP Address
			{
				dest_addr_todisplay = LANDESTS_DATA[i].DestAddr;
			}
			else if (LANDESTS_DATA[i].DestType == 6)		//Email Alert for User
			{
				userIndex = LANDESTS_DATA[i].UserID - 1;		//User ID starts with index 0.
				dest_addr_todisplay = USERINFO_DATA[userIndex].UserName;
				if(USERINFO_DATA[userIndex].EmailID != "")
				{
					dest_addr_todisplay += "(" + USERINFO_DATA[userIndex].EmailID + ")";
				}
			}
		}
		try
		{
			JSONRows.push({cells:[
				{text:(i+1), value:(i+1)},
				{text:dest_type_todisplay, value:dest_type_todisplay},
				{text:dest_addr_todisplay, value:dest_addr_todisplay}
			]});
		}
		catch(e)
		{
			alert(e);
		}
	}

	tblJSON.rows = JSONRows;
	lanDestsTable.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common','STR_LAN_DEST_CNT') + 
						"</strong>" + tLANDestCnt + eLang.getString('common','STR_BLANK');
}

function doADDLANDest()
{
	if ((lanDestsTable.selected.length != 1) || 
		(lanDestsTable.selected[0].cells[0] == undefined) || 
		(lanDestsTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR1'));
		disableButtons();
	}
	else
	{
		var desttype = lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
		destid = parseInt(lanDestsTable.getRow(lanDestsTable.selected[0]).cells[0].innerHTML);
		if (desttype != "~")
		{
			if (confirm(eLang.getString('common','STR_LAN_DEST_CONFIRM1')))
				frmModifyLANDest({'destid':destid});
		}
		else
			frmAddLANDest({'destid':destid});
	}
}

function doModifyLANDest()
{
	if ((lanDestsTable.selected.length != 1) || 
		(lanDestsTable.selected[0].cells[0] == undefined) || 
		(lanDestsTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR2'));
		disableButtons();
	}
	else
	{
		var desttype = lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
		destid = parseInt(lanDestsTable.getRow(lanDestsTable.selected[0]).cells[0].innerHTML);
		if (desttype == "~")
		{
			if (confirm(eLang.getString('common','STR_LAN_DEST_CONFIRM2')))
				frmAddLANDest({'destid':destid});
		}
		else
			frmModifyLANDest({'destid':destid});
	}
}

function doDeleteLANDest()
{
	if ((lanDestsTable.selected.length != 1) || 
		(lanDestsTable.selected[0].cells[0] == undefined) || 
		(lanDestsTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR1'));
		disableButtons();
	}
	else if (lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','') == "~")
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR2'));
	}
	else
		if (confirm(eLang.getString('common','STR_CONFIRM_DELETE')))
			deleteLANDest(parseInt(lanDestsTable.getRow(lanDestsTable.selected[0]).cells[0].innerHTML));
}

function doSendAlert()
{
	if ((lanDestsTable.selected.length != 1) || 
		(lanDestsTable.selected[0].cells[0] == undefined) || 
		(lanDestsTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR1'));
		disableButtons();
		return;
	}
	if (lanDestsTable.getRow(lanDestsTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','') == "~")
	{
		alert (eLang.getString('common','STR_LAN_DEST_ERR2'));
		disableButtons();
		return;
	}
	sendTestAlert(parseInt(lanDestsTable.getRow(lanDestsTable.selected[0]).cells[0].innerHTML));
}

var AddrFormat = 0;		//IPv4 Address Format hardcoded.
function frmAddLANDest(arg)
{
	landest_oper = 1;			//Adding LAN Destination Entry
	var frm = new form('addLANDestForm','POST','javascript://','general');

	if (fnCookie.read('gMultiLAN')) {
		lanChan = frm.addTextField('LAN Channel Number', '_lanChan', lanChannel.value, {'readOnly':true}, 'smallclassicTxtBox');
	}
	destID = frm.addTextField(eLang.getString('common','STR_LAN_DEST_HEAD1'), '_destID', arg.destid, {'readOnly':true}, 'smallclassicTxtBox');
	destTypeVals = {
				0:eLang.getString('common','STR_DEST_TYPE_0'),
				6:eLang.getString('common','STR_DEST_TYPE_6')
				};
	destType = frm.addSelectBox(eLang.getString('common','STR_LAN_DEST_HEAD2'), '_alertType', destTypeVals, '','','','classicTxtBox');
	destType.onchange = clearDestInfo;

	destAddr = frm.addTextField(eLang.getString('common','STR_LAN_DEST_HEAD3'), '_destAddr', '', {'maxLength':63}, 'bigclassicTxtBox');

	userName = frm.addSelectBox(eLang.getString('common','STR_CONF_USER_NAME'), '_userName', '','','','','classicTxtBox');
	fillUserName();

	subject = frm.addTextField(eLang.getString('common','STR_EMAIL_SUBJECT'), '_subject', '', {'maxLength':31},'classicTxtBox');
	message = frm.addTextField(eLang.getString('common','STR_EMAIL_MESSAGE'), '_message', '', {'maxLength':63},'bigclassicTxtBox');
	doDestType();

	var btnAry = [];
	btnAry.push(createButton('addBtn',eLang.getString('common','STR_ADD'),validateLANDest));
	btnAry.push(createButton('cancelBtn',eLang.getString('common','STR_CANCEL'),closeLANDestForm));

	wnd = MessageBox(eLang.getString('common','STR_ADD_LAN_DEST'),frm.display(),btnAry);
	if (fnCookie.read("gMultiLAN")) {
		//Ignores backspace functionality
		lanChan.onkeydown = checkBackspace;
	}
	//Ignores backspace functionality
	destID.onkeydown = checkBackspace;
	wnd.onclose = getAllUserInfo;
}

function frmModifyLANDest(arg)
{
	var index = arg.destid-1;		//index starts with 0.
	landest_oper = 2;			//Modifying LAN Destination Entry
	var frm = new form('modifyLANDestForm','POST','javascript://','general');

	if (fnCookie.read('gMultiLAN')) {
		lanChan = frm.addTextField('LAN Channel Number', '_lanChan', lanChannel.value, {'readOnly':true}, 'smallclassicTxtBox');
	}
	destID = frm.addTextField(eLang.getString('common','STR_LAN_DEST_HEAD1'), '_destID', arg.destid, {'readOnly':true}, 'smallclassicTxtBox');
	destTypeVals = {
				0:eLang.getString('common','STR_DEST_TYPE_0'),
				6:eLang.getString('common','STR_DEST_TYPE_6')
				};
	destType = frm.addSelectBox(eLang.getString('common','STR_LAN_DEST_HEAD2'), '_alertType', destTypeVals, '','','','classicTxtBox');
	destType.onchange = clearDestInfo;
	destType.value = LANDESTS_DATA[index].DestType;

	destAddr = frm.addTextField(eLang.getString('common','STR_LAN_DEST_HEAD3'), '_destAddr', 
						LANDESTS_DATA[index].DestAddr, {'maxLength':63}, 'bigclassicTxtBox');

	userName = frm.addSelectBox(eLang.getString('common','STR_CONF_USER_NAME'), '_userName', '','','','','classicTxtBox');
	fillUserName();
	userName.value = LANDESTS_DATA[index].UserID;

	subject = frm.addTextField(eLang.getString('common','STR_EMAIL_SUBJECT'), '_subject', 
						LANDESTS_DATA[index].Subject, {'maxLength':31},'classicTxtBox');

	message = frm.addTextField(eLang.getString('common','STR_EMAIL_MESSAGE'), '_message', 
						LANDESTS_DATA[index].Message, {'maxLength':63},'bigclassicTxtBox');
	doDestType();

	var btnAry = [];
	btnAry.push(createButton('modifyBtn',eLang.getString('common','STR_MODIFY'),validateLANDest));
	btnAry.push(createButton('cancelBtn',eLang.getString('common','STR_CANCEL'),closeLANDestForm));

	wnd = MessageBox(eLang.getString('common','STR_MODIFY_LAN_DEST'),frm.display(),btnAry);
	if (fnCookie.read("gMultiLAN")) {
		//Ignores backspace functionality
		lanChan.onkeydown = checkBackspace;
	}
	//Ignores backspace functionality
	destID.onkeydown = checkBackspace;
	wnd.onclose = getAllUserInfo;
}

function validateLANDest()
{
	if (destType.value == 0)		//SNMP Trap
	{
		if (eVal.ip(destAddr.value))
		{
			AddrFormat = 0;
		}
		else if(eVal.ipv6(destAddr.value, true, false))
		{
			AddrFormat = 1;
		}
		else
		{
			alert(eLang.getString('common','STR_LAN_DEST_ADDR_ERR')+eLang.getString('common','STR_HELP_INFO'));
			destAddr.focus();
			return;
		}
	}
	else if(destType.value == 6)	//Email Alert
	{
		if(eVal.isblank(USERINFO_DATA[(userName.value-1)].EmailID))
		{
			if (!confirm(eLang.getString("common","STR_LAN_NO_EMAIL_CONFIRM")))
			{
				userName.focus();
				return;
			}
		}
		if(eVal.isblank(eVal.trim(subject.value)))
		{
			alert(eLang.getString('common','STR_LAN_EMAIL_SUB_ERR'));
			subject.focus();
			return;
		}
		if(eVal.isblank(eVal.trim(message.value)))
		{
			alert(eLang.getString('common','STR_LAN_EMAIL_MSG_ERR'));
			message.focus();
			return;
		}
	}
	setLANDest();
}

function setLANDest()
{
	var req = new xmit.getset({url:'/rpc/setlandest.asp', onrcv:setLANDestRes, status:''});

	if (fnCookie.read('gMultiLAN'))
		req.add('LANChannel',lanChan.value);
	else
		req.add('LANChannel',1);

	req.add('LANDestIndex', destID.value);
	req.add('DestType', destType.value);
	req.add('AddressFormat', AddrFormat);
	req.add('DestAddress', destAddr.value);
	req.add('UserID',userName.value);
	req.add('Subject', subject.value);
	req.add('Message', message.value);
	req.send();
	delete req;
}

function setLANDestRes(arg)
{
	if (arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common','STR_LAN_DEST_SETVAL')
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		if(landest_oper == 1)			//Add Alert Policy Entry
			alert (eLang.getString('common', 'STR_LAN_DEST_ADD_SUCCESS'));
		else if(landest_oper == 2)		//Modify Alert Policy Entry
			alert (eLang.getString('common', 'STR_LAN_DEST_MOD_SUCCESS'));
		closeLANDestForm();
	}
}

function deleteLANDest(destid)
{
	var req = new xmit.getset({url:'/rpc/setlandest.asp', onrcv:deleteLANDestRes, status:''});

	if (fnCookie.read('gMultiLAN'))
		req.add("LANChannel",lanChannel.value);
	else
		req.add("LANChannel",1);

	req.add('LANDestIndex', destid);
	req.add('DestType', 0);
	req.add('AddressFormat', 0);
	req.add('DestAddress', eLang.getString('common','STR_IPV4_ADDR0'));
	req.add('UserID',0);
	req.add('Subject', "");
	req.add('Message', "");
	req.send();
	delete req;
}

function deleteLANDestRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_LAN_DEST_DELETEVAL");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		alert (eLang.getString('common', 'STR_LAN_DEST_DEL_SUCCESS'));
		getAllUserInfo();
	}
}

function sendTestAlert(destid)
{
	var req = new xmit.getset({url:'/rpc/testalert.asp', onrcv:sendTestAlertRes, status:''});
	req.add("WEBVAR_DSTSEL",destid);
	if (fnCookie.read('gMultiLAN'))
		req.add("WEBVAR_LANCHANNEL",lanChannel.value);
	else
		req.add("WEBVAR_LANCHANNEL",1);
	req.send();
	delete req;
}

function sendTestAlertRes(arg)
{
	switch (GET_ERROR_CODE(arg.HAPI_STATUS)) {
	case 0x0:
		alert(eLang.getString('common','STR_LAN_DEST_ALERT_SUCCESS'));
		break;
	case 0x80: case 0x81: case 0x82:
		alert (eLang.getString('common','STR_SMTP_FAILURE' + (GET_ERROR_CODE(arg.HAPI_STATUS) - 0x7F)));
		break;
	case 0xF:
		alert (eLang.getString('common','STR_EMAIL_FAILURE1'));
		break;
	case 0x10:
		alert (eLang.getString('common','STR_EMAIL_FAILURE2'));
		break;
	case 0xD3:
		alert (eLang.getString('common','STR_EMAIL_TESTALERT'));
		break;
	case 0x16:
		alert (eLang.getString('common','STR_EMAIL_USER_ACCESS'));
		break;
	default:
			errstr = eLang.getString('common','STR_LAN_DEST_ALERT_FAILURE');
			errstr += eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS);
			alert (errstr);
	}
}

function doDestType()
{
	var bopt = (destType.value == 0) ? true : false;		//0 -> SNMP Alert, 6 -> Email Alert
	destAddr.disabled = !bopt;
	userName.disabled = bopt;
	subject.disabled = bopt;
	message.disabled = bopt;
}

function clearDestInfo()
{
	destAddr.value = '';
	subject.value = '';
	message.value = '';
	doDestType();
}

function closeLANDestForm()
{
	wnd.close();
}

function fillUserName()
{
	var optind = 0;
	userName.innerHTML = '';
	for(var i=0; i<USERINFO_DATA.length; i++)
	{
		if ((USERINFO_DATA[i].UserName != undefined) && (USERINFO_DATA[i].UserName != ""))
		{
			userName.add(new Option(USERINFO_DATA[i].UserName, (i+1)),isIE?optind++:null);
		}
	}
}
