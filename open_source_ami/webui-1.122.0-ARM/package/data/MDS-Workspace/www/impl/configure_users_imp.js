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

var usrListTable;
var tblJSON;
var USERINFO_DATA;
var EMAILFORMAT_DATA;

var g_userindex;
var g_loggedin_user_delerror = 0x846;
var g_logout = false;
var user_operation = -1;

var SNMP_SUPPORT = false;
var userAuthProt;
var userSNMPStatus;

function doInit()
{
	exposeElms(["_usrListHolder",
		"_addUserBtn",
		"_modUserBtn",
		"_delUserBtn",
		"_lblHeader"]);

	if(top.user.isAdmin()) {
		addUserBtn.onclick = doAddUser;
		modUserBtn.onclick = doModifyUser;
		delUserBtn.onclick = doDeleteUser;
	} else if (top.user.isOperator()) {
		disableActions();
	} else {
		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	_begin();
}

function _begin()
{
	SNMP_SUPPORT = checkProjectCfg("SNMP");
	loadCustomPageElements();
	getAllUserInfo();
	getAllEmailFormat();
}

function loadCustomPageElements()
{
	usrListTable = listgrid
	({
		w				: "100%",
		doAllowNoSelect : false
	});
	usrListHolder.appendChild(usrListTable.table);

	try {
		if (!SNMP_SUPPORT) {
			tblJSON = {cols:[
				{text:eLang.getString("common","STR_CONF_USER_ID"), fieldType:2, w:"10%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_NAME"), w:"20%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_ACCESS"), w:"20%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_NWPRIV"), w:"20%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_EMAIL"), w:"30%", textAlign:"center"}
			]};
		} else {
			tblJSON = {cols:[
				{text:eLang.getString("common","STR_CONF_USER_ID"), fieldType:2, w:"8%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_NAME"), w:"20%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_ACCESS"), w:"15%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_NWPRIV"), w:"18%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_SNMP_STATUS"), w:"15%", textAlign:"center"},
				{text:eLang.getString("common","STR_CONF_USER_EMAIL"), w:"24%", textAlign:"center"}
			]};
		}
		usrListTable.loadFromJson(tblJSON);
	} catch(e) {
		alert(e);
	}

	if(top.user.isAdmin())
	{
		usrListTable.ontableselect = function ()
		{
			disableButtons();
			if(this.selected.length)
			{
				selusername = this.getRow(this.selected[0]).cells[1].innerHTML.replace("&nbsp;","").replace(" ","");
				selectedid = parseInt(usrListTable.getRow(usrListTable.selected[0]).cells[0].innerHTML);
				if (USERINFO_DATA[selectedid-1].UserName == "")
					addUserBtn.disabled = false;
				else
				{
					modUserBtn.disabled = false;
					delUserBtn.disabled = (selectedid <= 
						(USERINFO_DATA[selectedid-1].FixedUserCount)) ? 
						true:false;
				}
				if(fnCookie.read("Username") == selusername)
					delUserBtn.disabled = true;
			}
		}

		usrListTable.ondblclick = function()
		{
			selectedid = parseInt(usrListTable.getRow(usrListTable.selected[0]).cells[0].innerHTML);
			if (USERINFO_DATA[selectedid-1].UserName == "")
				doAddUser();
			else
				doModifyUser();
		}
	}
}

function disableButtons()
{
	addUserBtn.disabled = true;
	modUserBtn.disabled = true;
	delUserBtn.disabled = true;
}

function getAllUserInfo()
{
	disableButtons();
	xmit.get({url:"/rpc/getalluserinfo.asp",onrcv:getAllUserInfoRes, status:""});
}

function getAllUserInfoRes (arg)
{
	if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0xD4) {	//Insufficient privilege level
		alert (eLang.getString("common","STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
	} else if (arg.HAPI_STATUS) {
		var errstr =  eLang.getString("common","STR_CONF_USER_GETINFO");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		var JSONRows = [];
		var tUsrCnt = 0;
		usrListTable.clear();
		USERINFO_DATA = WEBVAR_JSONVAR_HL_GETALLUSERINFO.WEBVAR_STRUCTNAME_HL_GETALLUSERINFO;

		for (i = 0; i < USERINFO_DATA.length; i++) {
			// Use ~ char to indicate free slot so it will sort alphabetically
			usernametodisplay = (USERINFO_DATA[i].UserName == "") ? "~" : USERINFO_DATA[i].UserName;
			userstatustodisplay = (USERINFO_DATA[i].UserStatus) ? "Enabled" : "Disabled";
			userprivtodisplay = interpretPrivileges(USERINFO_DATA[i].PrivLimit_Network);
			if (SNMP_SUPPORT) {
				snmpstatustodisplay = (USERINFO_DATA[i].SNMPStatus) ? "Enabled" : "Disabled";
			}
			emailtodisplay = (USERINFO_DATA[i].EmailID == "")?"~":USERINFO_DATA[i].EmailID;
			
			if(usernametodisplay != "~") {
				tUsrCnt++;
			} else {
				userstatustodisplay = "~";
				userprivtodisplay = "~";
				if (SNMP_SUPPORT) {
					snmpstatustodisplay = "~";
				}
			}

			try {
				if (!SNMP_SUPPORT) {
					JSONRows.push({cells:[
						{text:(i+1), value:(i+1)},
						{text:usernametodisplay, value:usernametodisplay},
						{text:userstatustodisplay, value:userstatustodisplay},
						{text:userprivtodisplay, value:userprivtodisplay},
						{text:emailtodisplay, value:emailtodisplay}
					]});
				} else {
					JSONRows.push({cells:[
						{text:(i+1), value:(i+1)},
						{text:usernametodisplay, value:usernametodisplay},
						{text:userstatustodisplay, value:userstatustodisplay},
						{text:userprivtodisplay, value:userprivtodisplay},
						{text:snmpstatustodisplay, value:snmpstatustodisplay},
						{text:emailtodisplay, value:emailtodisplay}
					]});
				}
			} catch(e) {
				alert(e);
			}
		}

		tblJSON.rows = JSONRows;
		usrListTable.loadFromJson(tblJSON);
		lblHeader.innerHTML = eLang.getString("common","STR_CONF_USER_CNT") + tUsrCnt + eLang.getString("common","STR_BLANK");
	}
}

function getAllEmailFormat()
{
	xmit.get({url:"/rpc/getemailformat.asp", onrcv:getAllEmailFormatRes,status:""});
}

function getAllEmailFormatRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		var errstr =  eLang.getString("common","STR_CONF_EMAILFORMAT_GETINFO");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else	
	{
		EMAILFORMAT_DATA = WEBVAR_JSONVAR_GETEMAILFORMAT.WEBVAR_STRUCTNAME_GETEMAILFORMAT;
	}
}

function interpretPrivileges(privbyte)
{
	var privlevel = getbits(privbyte,3,0);
	return IPMIPrivileges[privlevel];
}

function doAddUser()
{
	if ((usrListTable.selected.length != 1) || (usrListTable.selected[0].cells[0] == undefined) || (usrListTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_USER_ERR1"));
		disableButtons();
	}
	else
	{
		//we need to get the exact value of user id
		selectedid = parseInt(usrListTable.getRow(usrListTable.selected[0]).cells[0].innerHTML);
		if (USERINFO_DATA[selectedid-1].UserName != "")
		{
			if (confirm(eLang.getString("common","STR_CONF_USER_CONFIRM1")))
				initFrmModifyUser(selectedid);
		}
		else
			frmAddUser({"userindex":selectedid});
	}
}

function frmAddUser(arg)
{
	user_operation = 0;
	var frm = new form("addUserForm","POST","javascript://","general");

	userName = frm.addTextField(eLang.getString("common","STR_CONF_USER_NAME"),
					"_userName","",{"maxLength":16},"classicTxtBox");

	pwordSize = frm.addRadioField(eLang.getString("common","STR_CONF_USER_PWORDSIZE"),
					"_pwordSize",{"byte16":"16 Bytes","byte20":"20 Bytes"},false,["byte16"]);

	passWord = frm.addPasswordField(eLang.getString("common","STR_PASSWORD"),
					"_passWord","",{"maxLength":16},"classicTxtBox");
	cpassWord = frm.addPasswordField(eLang.getString("common","STR_CONF_USER_CNFMPWORD"),
					"_cpassWord","",{"maxLength":16},"classicTxtBox");

	userAccess = frm.addCheckBox(eLang.getString("common","STR_CONF_USER_ACCESS"),
					"_userAccess",{"enable":"Enable"},false,[""]);
	userAccess = userAccess.enable;

	var nwPrivVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
	nwPriv = frm.addSelectBox(eLang.getString("common","STR_CONF_USER_NWPRIV"),
					"_nwPriv",nwPrivVals,"4","","","classicTxtBox");

	if (SNMP_SUPPORT) {
		chkSNMPStatus = frm.addCheckBox(eLang.getString("common", 
			"STR_CONF_SNMP_STATUS"), "_chkSNMPStatus", {"_chkSNMPStatus" : "Enable"},
			false, [""]);
		chkSNMPStatus = chkSNMPStatus._chkSNMPStatus;
		chkSNMPStatus.onclick = enableSNMP;

		var snmpAccessVals = {1:"Read Only", 2:"Read Write"};
		lstSNMPAccess = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_ACCESS"), "_lstSNMPAccess", snmpAccessVals, 1, "",
			"", "classicTxtBox");

		var authProtVals = {1:"SHA", 2:"MD5"};
		lstAuthProt = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_AUTHPROT"), "_lstAuthProt", authProtVals, 1, "",
			"", "smallclassicTxtBox");

		var privProtVals = {1:"DES", 2:"AES"};
		lstPrivProt = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_PRIVPROT"), "_lstPrivProt", privProtVals, 1, "",
			"", "smallclassicTxtBox");
	}
	
	emailID = frm.addTextField(eLang.getString("common","STR_CONF_USER_EMAIL"),
								"_emailID","",{"maxLength":63},"bigclassicTxtBox");

	if (EMAILFORMAT_DATA.length){
		emailFormat = frm.addSelectBox(eLang.getString("common","STR_CONF_USER_EMAIL_FORMAT"),
							"_emailFormat","","","","","classicTxtBox");
		fillEmailFormat();
	}
	var fileupload = document.createElement("div");

	if (fnCookie.read("WebServer").indexOf("lighttpd") != -1)
	{
		fileupload.innerHTML = "<form name='sshUpload' id='_sshUpload' method='POST'" +
			" enctype='multipart/form-data' action='file_upload.html' target='hiddenFrame' style='margin-bottom:0'>" +
			"<input type='file' name='/tmp/authorized_keys' id='_sshFileBrowse' size='35' />" +
			"</form>";
	}
	else
	{
		fileupload.innerHTML = "<form name='sshUpload' id='_sshUpload' method='POST'" +
			" enctype='multipart/form-data' action='file_upload.html' target='hiddenFrame' style='margin-bottom:0'>" +
			"<input type='file' name='/tmp/authorized_keys?16384' id='_sshFileBrowse' size='35' />" +
			"</form>";
	}
	filebrowserow = frm.addRow(eLang.getString("common","STR_CONF_USER_NEWSSH"),fileupload);

	var btnAry = [];
	btnAry.push(createButton("addBtn",eLang.getString("common","STR_ADD"),validateUserCfg));
	btnAry.push(createButton("cancelBtn",eLang.getString("common","STR_CANCEL"),closeForm));

	g_userindex = arg.userindex;

	wnd = MessageBox(eLang.getString("common","STR_ADD_USER"),frm.display(),btnAry, false, 50);
	$("byte16").checked = true;
	$("byte16").onblur = $("byte16").onclick = doPasswordSize;
	$("byte20").onblur = $("byte20").onclick = doPasswordSize;
	wnd.onclose = getAllUserInfo;
	userName.focus();
	if (SNMP_SUPPORT) {
		enableSNMP();
	}
}

function validateUserCfg()
{
	var errmsg = eLang.getString("common","STR_INVALID_FIELDS");
	var err = 0;
	var checkpword = true;

	if (user_operation == 0)	//Username validation for Add User.
	{
		if (!eVal.username(userName.value, "IPMI", 1, 16))		//2nd argument "IPMI" is to select the specific filter in eVal
		{
			err = 1;
			errmsg += "* " + eLang.getString("common","STR_CONF_USER_NAME") + eLang.getString("common","STR_NEWLINE");
		}
	}
	else if (user_operation == 1){
		checkpword = changePwd.checked;
	}

	if (checkpword)
	{
		if ($("byte16").checked)
		{
			if (!eVal.password(passWord.value, 1, 16))
			{
				err = 1;
				errmsg += "* " + eLang.getString("common","STR_PASSWORD") + eLang.getString("common","STR_NEWLINE");
			}
		}
		else if ($("byte20").checked)
		{
			if (!eVal.password(passWord.value, 1, 20))
			{
				err = 1;
				errmsg += "* " + eLang.getString("common","STR_PASSWORD") + eLang.getString("common","STR_NEWLINE");
			}
		}
		else
		{
			err = 1;
			errmsg += "* " + eLang.getString("common","STR_CONF_USER_PWORDSIZE") + eLang.getString("common","STR_NEWLINE");
		}
		if (passWord.value != cpassWord.value)
		{
			err = 1;
			errmsg += "* " + eLang.getString("common","STR_CONF_USER_CNFMPWORD") + eLang.getString("common","STR_NEWLINE");
		}
	}

	if ((SNMP_SUPPORT) && (user_operation == 1)) {
		if (chkSNMPStatus.checked) {
			if (chkSNMPStatus.checked != userSNMPStatus) {
				if (eVal.isblank(eVal.trim(passWord.value))) {
					err = 1;
					errmsg += "* " + eLang.getString("common", 
						"STR_CONF_SNMP_ACCESS") + eLang.getString("common", 
						"STR_NEWLINE");
				}
			}

			if (lstAuthProt.value != userAuthProt) {
				if (eVal.isblank(eVal.trim(passWord.value))) {
					err = 1;
					errmsg += "* " + eLang.getString("common", 
						"STR_CONF_SNMP_AUTHPROT") + eLang.getString("common", 
						"STR_NEWLINE");
				}
			}
		}
	}

	if (!eVal.email(emailID.value) && !eVal.isblank(emailID.value))
	{
		err = 1;
		errmsg += "* " + eLang.getString("common","STR_CONF_USER_EMAIL") + eLang.getString("common","STR_NEWLINE");
	}

	var sshFileBrowse = $("_sshFileBrowse");
	if (!eVal.isblank (sshFileBrowse.value) && (!eVal.endsWith(sshFileBrowse.value,".pub")))
	{
		err = 1;
		errmsg += "* " + eLang.getString("common","STR_CONF_USER_NEWSSH") + eLang.getString("common","STR_NEWLINE");
	}

	if (err == 1)
	{
		errmsg += eLang.getString("common","STR_NEWLINE") + eLang.getString("common","STR_HELP_INFO");
		alert (errmsg);
		return;
	}
	if (user_operation == 1)	//Modify User
	{
		if ((top.user.name == userName.value) &&
			(top.user.pno != nwPriv.value || changePwd.checked))
		{
			var cnfmstr = eLang.getString("common","STR_CONF_USER_LOGCONFIRM");
			if (top.gConsoleOpen)
				cnfmstr += "\n" + eLang.getString("common","STR_WARNING") + eLang.getString("common","STR_CONSOLE_CONNECTED");
			if (confirm(cnfmstr))
				g_logout = true;
			else
				return;
		}
	}
	uploadSSHKey();
}

function uploadSSHKey()
{
	var sshFileBrowse = $("_sshFileBrowse");
	if (!eVal.isblank(sshFileBrowse.value))
	{
		showWait(true,"Uploading");
		document.forms["sshUpload"].submit();
	}
	else
	{
		uploadComplete();
	}
}

function uploadComplete()
{
	showWait(false);
	setUserInfo();
}

function setUserInfo()
{
	var req = new xmit.getset({url:"/rpc/addmoduser.asp", onrcv:setUserInfoRes});
	req.add("UserOperation", user_operation);
	req.add("UserIndex", g_userindex);
	req.add("UserName", userName.value);

	req.add("PwordSize", ($("byte16").checked) ? 0 : 1);	//0-16bytes, 1-20bytes
	req.add("PassWord", passWord.value);
	if(user_operation == 1) {	//Need to send this argument for Modify User.
		req.add("IsPwdChange", (changePwd.checked) ? 1 : 0);
	}

	req.add("UserStatus", userAccess.checked ? 1 : 0);
	req.add("PrivLimit_Network", nwPriv.value);
	req.add("PrivLimit_Serial", nwPriv.value);

	if(SNMP_SUPPORT) {
		req.add("SNMPStatus", chkSNMPStatus.checked ? 1 : 0);
		if (chkSNMPStatus.checked) {
			req.add("SNMPAccess", lstSNMPAccess.value);
			req.add("AUTHProtocol", lstAuthProt.value);
			req.add("PrivProtocol", lstPrivProt.value);
		}
	}

	req.add("EmailID", emailID.value);
	if (EMAILFORMAT_DATA.length) {
		req.add("EmailFormat", emailFormat.value);
	} else {
		req.add("EmailFormat", "");
	}

	if (eVal.isblank($("_sshFileBrowse").value)) {
		req.add("SSHValidate", 0);
	} else {
		req.add("SSHValidate", 1);
	}
	req.send();
	delete req;
}

function setUserInfoRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0xCC)
		{
			if ((userName.value == "admin") && (user_operation == 1)){
				alert(eLang.getString("common","STR_CONF_USER_SUCCESS1"));
				if (g_logout){
					top.gConsoleOpen = false;
					top.logoutWeb();
				}else{
					closeForm();
				}
			} else {
				alert (eLang.getString("common","STR_CONF_USER_ERR5"));
			}
		}else if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0x17){
			alert (eLang.getString("common","STR_CONF_USER_ERR7"));
		}else if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0x04){
			alert (eLang.getString("common","STR_CONF_USER_ERR5"));
		}else{
			errstr = eLang.getString("common","STR_CONF_USER_SETINFO");
			errstr +=  (eLang.getString("common","STR_IPMI_ERROR")+GET_ERROR_CODE(arg.HAPI_STATUS));
			alert(errstr);
		}
	}
	else
	{
		sshkeyvalid = WEBVAR_JSONVAR_HL_ADD_MOD_USER.WEBVAR_STRUCTNAME_HL_ADD_MOD_USER[0]["SSHKEY_VALID"];
		switch(sshkeyvalid)
		{
			case 0:
				alert(eLang.getString("common","STR_CONF_USER_SUCCESS" + user_operation));
				if (g_logout)
				{
					top.gConsoleOpen = false;
					top.logoutWeb();
				}
				else
					closeForm();
				break;
			case 1: case 4: case 5: 
				alert(eLang.getString("common","STR_CONF_SSH_ERR" + sshkeyvalid));
				break;
			case 3:	default:		//Validation fails
				alert(eLang.getString("common","STR_CONF_SSH_VALIDATE_ERR"));
		}
	}
}

function doModifyUser()
{
	if ((usrListTable.selected.length != 1) || (usrListTable.selected[0].cells[0] == undefined) || (usrListTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_USER_ERR1"));
		disableButtons();
		return;
	}
	else
	{
		//we need to get the exact value of user id
		selectedid = parseInt(usrListTable.getRow(usrListTable.selected[0]).cells[0].innerHTML);
		if (USERINFO_DATA[selectedid-1].UserName == "")
		{
			if (confirm(eLang.getString("common","STR_CONF_USER_CONFIRM2")))
				frmAddUser({"userindex":selectedid});
		}
		else
			initFrmModifyUser(selectedid);
	}
}

function initFrmModifyUser(selectindex)
{
	var selectUserRow = USERINFO_DATA[selectindex-1];		//index value starts with 0
	if (!SNMP_SUPPORT) {
		frmModifyUser({"userindex"	: selectindex,
			"username"		: usrListTable.getRow(usrListTable.selected[0]).cells[1].innerHTML,
			"useraccess"	: selectUserRow.UserStatus,
			"nwpriv"		: getbits(selectUserRow.PrivLimit_Network,3,0),
			"serialpriv"	: getbits(selectUserRow.PrivLimit_Serial,3,0),
			"email"			: selectUserRow.EmailID,
			"emailformat"	: selectUserRow.EmailFormat,
			"sshkeyexists"	: selectUserRow.SSHKeyStatus,
			"sshkeyinfo"	: selectUserRow.SSHKeyInfo
			});
	} else {
		frmModifyUser({"userindex"	: selectindex,
			"username"		: usrListTable.getRow(usrListTable.selected[0]).cells[1].innerHTML,
			"useraccess"	: selectUserRow.UserStatus,
			"nwpriv"		: getbits(selectUserRow.PrivLimit_Network,3,0),
			"serialpriv"	: getbits(selectUserRow.PrivLimit_Serial,3,0),
			"snmpstatus"	: selectUserRow.SNMPStatus,
			"snmpaccess"	: selectUserRow.SNMPAccess,
			"authprot"		: selectUserRow.AUTHProtocol,
			"privprot"		: selectUserRow.PrivProtocol,
			"email"			: selectUserRow.EmailID,
			"emailformat"	: selectUserRow.EmailFormat,
			"sshkeyexists"	: selectUserRow.SSHKeyStatus,
			"sshkeyinfo"	: selectUserRow.SSHKeyInfo
			});
	}
}

function frmModifyUser(arg)
{
	user_operation = 1;
	var frm = new form("modUserForm","POST","javascript://","general");
	userName = frm.addTextField(eLang.getString("common","STR_CONF_USER_NAME"),
					"_userName",arg.username,{"readOnly":true},"classicTxtBox");

	pwdSettings = frm.addCheckBox("","cPwd",{"_changePwd":"Change Password"},false);
	changePwd = pwdSettings._changePwd;

	pwordSize = frm.addRadioField(eLang.getString("common","STR_CONF_USER_PWORDSIZE"),
					"_pwordSize",{"byte16":"16 Bytes","byte20":"20 Bytes"},false,["byte16"]);

	passWord = frm.addPasswordField(eLang.getString("common","STR_PASSWORD"),
					"_passWord","",{"maxLength":16},"classicTxtBox");

	cpassWord = frm.addPasswordField(eLang.getString("common","STR_CONF_USER_CNFMPWORD"),
					"_cpassWord","",{"maxLength":16},"classicTxtBox");

	userAccess = frm.addCheckBox(eLang.getString("common","STR_CONF_USER_ACCESS"),
								"_userAccess",{"enable":"Enable"},false,[""]);
	userAccess = userAccess.enable;
	userAccess.checked = (arg.useraccess == 1) ? true : false;
//	if (arg.userindex == fnCookie.read("UserIndex"))
	if (arg.username == fnCookie.read("Username"))
	{
		userAccess.checked = true;
		userAccess.disabled = true;
	}

	var nwPrivVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
	nwPriv = frm.addSelectBox(eLang.getString("common","STR_CONF_USER_NWPRIV"),
					"_nwPriv",nwPrivVals,arg.nwpriv,"","","classicTxtBox");

	if (SNMP_SUPPORT) {
		chkSNMPStatus = frm.addCheckBox(eLang.getString("common", 
			"STR_CONF_SNMP_STATUS"), "_chkSNMPStatus", {"_chkSNMPStatus" : "Enable"},
			false, [""]);
		chkSNMPStatus = chkSNMPStatus._chkSNMPStatus;
		chkSNMPStatus.onclick = enableSNMP;

		var snmpAccessVals = {1:"Read Only", 2:"Read Write"};
		lstSNMPAccess = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_ACCESS"), "_lstSNMPAccess", snmpAccessVals, arg.snmpaccess, "",
			"", "classicTxtBox");

		userAuthProt = arg.authprot;
		var authProtVals = {1:"SHA", 2:"MD5"};
		lstAuthProt = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_AUTHPROT"), "_lstAuthProt", authProtVals, arg.authprot, "",
			"", "smallclassicTxtBox");

		var privProtVals = {1:"DES", 2:"AES"};
		lstPrivProt = frm.addSelectBox(eLang.getString("common", 
			"STR_CONF_SNMP_PRIVPROT"), "_lstPrivProt", privProtVals, arg.privprot, "",
			"", "smallclassicTxtBox");
	}
	
	emailID = frm.addTextField(eLang.getString("common","STR_CONF_USER_EMAIL"),
					"_emailID",arg.email,{"maxLength":63},"bigclassicTxtBox");

	if (EMAILFORMAT_DATA.length){
		emailFormat = frm.addSelectBox(eLang.getString("common","STR_CONF_USER_EMAIL_FORMAT"),
							"_emailFormat","","","","","classicTxtBox");
		fillEmailFormat();
		emailFormat.value = arg.emailformat;
	}
	sshfileinfo = frm.addTextField(eLang.getString("common","STR_CONF_USER_UPLOADSSH"),
						"_sshFileInfo",arg.sshkeyinfo,{"readOnly":true},"bigclassicTxtBox");

	var fileupload = document.createElement("div");

	if (fnCookie.read("WebServer").indexOf("lighttpd") != -1)
	{
		fileupload.innerHTML = "<form name='sshUpload' id='_sshUpload' method='POST' " + 
			"enctype='multipart/form-data' action='file_upload.html' target='hiddenFrame' style='margin-bottom:0'>" +
			"<input type='file' name='/tmp/authorized_keys' id='_sshFileBrowse' size='35' />" +
			"</form>";
	}
	else
	{
		fileupload.innerHTML = "<form name='sshUpload' id='_sshUpload' method='POST' " + 
			"enctype='multipart/form-data' action='file_upload.html' target='hiddenFrame' style='margin-bottom:0'>" +
			"<input type='file' name='/tmp/authorized_keys?16384' id='_sshFileBrowse' size='35' />" +
			"</form>";
	}
	filebrowserow = frm.addRow(eLang.getString("common","STR_CONF_USER_NEWSSH"),fileupload);

	if(arg.username=="root")
		nwPriv.disabled = true;

	var btnAry = [];
	btnAry.push(createButton("modBtn",eLang.getString("common","STR_MODIFY"),validateUserCfg));
	btnAry.push(createButton("cancelBtn",eLang.getString("common","STR_CANCEL"),closeForm));

	g_userindex = arg.userindex;
	changePwd.onclick = pwdChange;
	wnd = MessageBox(eLang.getString("common", "STR_MODIFY_USER"), frm.display(), btnAry, false, 10);
	pwdChange();
	$("byte16").onblur = $("byte16").onclick = doPasswordSize;
	$("byte20").onblur = $("byte20").onclick = doPasswordSize;
	//Ignores backspace functionality
	userName.onkeydown = checkBackspace;
	//Ignores backspace functionality
	sshfileinfo.onkeydown = checkBackspace;
	wnd.onclose = getAllUserInfo;
	if (SNMP_SUPPORT) {
		chkSNMPStatus.checked = (arg.snmpstatus) ? true : false;
		userSNMPStatus = chkSNMPStatus.checked;
		enableSNMP();
	}
}

function pwdChange()
{
	var bopt = !changePwd.checked;

	$("byte16").disabled = bopt;
	$("byte20").disabled = bopt;
	if (!bopt) {
		$("byte16").checked = true;
	} else {
		$("byte16").checked = false;
		$("byte20").checked = false;
	}

	passWord.value = "";
	cpassWord.value = "";
	passWord.disabled = bopt;
	cpassWord.disabled = bopt;
}

function doDeleteUser()
{
	if ((usrListTable.selected.length != 1) || (usrListTable.selected[0].cells[0] == undefined) || (usrListTable.selected[0].cells[0] == null))
	{
		alert(eLang.getString("common","STR_CONF_USER_ERR1"));
		disableButtons();
	}
	else
	{
		//we need to get the exact value of user id
		selectedid = parseInt(usrListTable.getRow(usrListTable.selected[0]).cells[0].innerHTML);
		selectedname = usrListTable.getRow(usrListTable.selected[0]).cells[1].innerHTML.replace("&nbsp;","").replace(" ","");

		if (USERINFO_DATA[selectedid-1].UserName == "")
			alert(eLang.getString("common","STR_CONF_USER_ERR3"));
		else
		{
			if (confirm(eLang.getString("common","STR_CONFIRM_DELETE")))
				deleteUser(selectedid,selectedname);
		}
	}
}

function deleteUser(userid, username)
{
	req = new xmit.getset({url:"/rpc/deluser.asp",onrcv:deleteUserRes});
	req.add("WEBVAR_USERINDEX",userid);
	req.add("WEBVAR_USERNAME",username);
	req.send();
	delete req;
}

function deleteUserRes(arg)
{
	if (arg.HAPI_STATUS == g_loggedin_user_delerror)
	{
		alert(eLang.getString("common","STR_CONF_USER_ERR4"));
	}
	else if (arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString("common","STR_CONF_USER_DELINFO");
		errstr +=  (eLang.getString("common","STR_IPMI_ERROR") +GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		alert(eLang.getString("common","STR_CONF_USER_DELETE_SUCCESS"));
		getAllUserInfo();
	}
}

function closeForm()
{
	wnd.close();
}

function fillEmailFormat()
{
	var optind = 0;
	emailFormat.innerHTML = "";
	for(var i=0; i<EMAILFORMAT_DATA.length; i++)
	{
		emailFormat.add(new Option(EMAILFORMAT_DATA[i].EMAIL_FORMAT, 
			EMAILFORMAT_DATA[i].EMAIL_FORMAT),isIE?optind++:null);
	}
}

function doPasswordSize()
{
	passWord.value = "";
	cpassWord.value = "";
	if ($("byte16").checked){
		passWord.maxLength = 16;
		cpassWord.maxLength = 16;
	}else if ($("byte20").checked){
		passWord.maxLength = 20;
		cpassWord.maxLength = 20;
	}
}

function enableSNMP()
{
	var bopt;
	if (SNMP_SUPPORT) {
		bopt = !chkSNMPStatus.checked;
		lstSNMPAccess.disabled = bopt;
		lstAuthProt.disabled = bopt;
		lstPrivProt.disabled = bopt;
	}
}
