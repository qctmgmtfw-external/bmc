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

var oper = 0;

var UPLOAD_IMAGE = 1;
var UPLOAD_SIGNKEY = 2;

var PRSRVCFG_DATA;
var tblJSON;		//object to hold Preserve configuration in JSON structure
var tblPrsrvCfg;	//List grid object to hold Preserve configuration

function doInit()
{
	exposeElms(["_wizard",
		"_clientRequests",
		"_prepareDevice",
		"_uploadImage",
		"_verifyImage",
		"_flashImage",
		"_percentageFlashed",
		"_resetDevice",
		"_btnKeyUpload",
		"_btnFWUpdate",
		"_section",
		"_divPrsrvAll",
		"_chkPrsrvAll",
		"_lblPrsrvAllDesc",
		"_divPrsrvCfg",
		"_lgdPrsrvCfg",
		"_btnPrsrvCfg"
		]);
	clearSection();
	if(top.user.isAdmin())
	{
		btnFWUpdate.onclick = doEnterUpgrade;
		_begin();
	}
	else
	{
		alert(eLang.getString("common", "STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
}

function _begin()
{
	doSignImageSupport();
	doPreserveCfg();
}

function doEnterUpgrade()
{
	if(!confirm(eLang.getString("common", "STR_FW_UPDATE_CONFIRM1")))
		return;

	btnFWUpdate.disabled = true;
	btnFWUpdate.onclick = function() {};
	btnKeyUpload.disabled = true;
	btnKeyUpload.onclick = function() {};

	divPrsrvAll.className = "hiddenRow";
	divPrsrvCfg.className = "hiddenRow";
	btnPrsrvCfg.disabled = true;
	btnPrsrvCfg.onclick = function() {};

	wizard.style.display = "block";
	progressIcon = document.createElement("img");
	progressIcon.src = "../res/process.gif";

	setFlashMode(true);
	prepareClient();
}

function prepareClient()
{
	clientRequests.appendChild(progressIcon);
	clientRequests.className = "hiLite";

	prepareDeviceShutdown();

	with(clientRequests)
	{
		className = "normal";
		style.color = "#000";
		firstChild.src = "../res/prg_success.png";
	}
	prepareFlash();
}

function prepareFlash()
{
	prepareDevice.appendChild(progressIcon);
	prepareDevice.className = "hiLite";
	var p = new xmit.getset({url:"/rpc/prepflash.asp", onrcv:onReceive,timeout:60});
	p.send();
}

function onReceive (arg)
{
	if(arg.HAPI_STATUS)
	{
		//Display the error code and proper message here...
		prepareDevice.firstChild.src = "../res/prg_failure.png";
		prepareDevice.removeChild(progressIcon);
		setFlashMode(false);
		if(!top.user.isAdmin())
		{
			alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
			return;
		}
		switch(arg.HAPI_STATUS)
		{
			case 4:
				alert(eLang.getString("common","STR_FW_FLASH_PROGRESS"));
			break;		
			default:
				errstr =  eLang.getString("common","STR_FW_PREPARE_FLASH");
				errstr += (eLang.getString("common","STR_IPMI_ERROR") + GET_ERROR_CODE(arg.HAPI_STATUS));
				alert(errstr);
			break;
		}
		//window.close();
	}
	else
	{
		with(prepareDevice)
		{
			className = "normal";
			style.color = "#000";
			firstChild.src = "../res/prg_success.png";
		}
		gs = xmit.getset({url:"/rpc/getromfilesize.asp",status:"",onrcv:uploadFirmware});
		gs.send();
	}
}

function uploadFirmware(arg)
{
	var filename;
	uploadImage.className = "hiLite";
	uploadImage.appendChild(progressIcon);
	if (fnCookie.read("WebServer").indexOf("lighttpd") != -1)
		filename = "/mnt/rom.ima";
	else
		filename = "/mnt/rom.ima?20000000";
	
	try{
		if(arg.HAPI_STATUS == 0)
		{
			GetRomFileSize = WEBVAR_JSONVAR_GETROMFILESIZE.WEBVAR_STRUCTNAME_GETROMFILESIZE;
			if (fnCookie.read("WebServer").indexOf("lighttpd") != -1)
				filename = GetRomFileSize[0].V_FILE_LOCATION;
			else
				filename = GetRomFileSize[0].V_FILE_LOCATION + "?" + GetRomFileSize[0].V_FILE_SIZE;
		}
	}catch(e)
	{
	}


	var p = document.createElement("p");
	p.innerHTML = "Please select the firmware image to flash";
	p.innerHTML += "<br/><form action='file_upload.html' name='fwUpload' method='POST' enctype='multipart/form-data' target='hiddenFrame'>"+
					"<input type='file' name='"+filename+"' id='brwsUpld' size='40' />"+
					"</form>";

	var btnUpload = document.createElement("input");
	btnUpload.type = "button";
	btnUpload.value = "Upload";
	btnUpload.onclick = function()
	{
		var filepath = new String($("brwsUpld").value);
		if(filepath.length == 0)
		{
			alert(eLang.getString("common","STR_FW_UPDATE_ERR1"));
			$("brwsUpld").focus();
			return;
		}
		xmit.get({url:"/rpc/flashfile_upload_starts.asp", onrcv:flashFileUploadStartRes, status:""});
	}

	var btnAry = [];
	btnAry.push(btnUpload);
	btnAry.push(createButton("_cancelUpload","Cancel",cancelWizard));

	updateSection("Upload Firmware",p,btnAry);
}

function flashFileUploadStartRes()
{
	showWait(true,"Uploading");
	oper = UPLOAD_IMAGE;		//Used to indicate flash image is uploading.
	document.forms["fwUpload"].submit();
	clearSection();
}

function uploadComplete()
{
	showWait(false);

	if (oper == UPLOAD_IMAGE) {		//Flash image upload complete
		with(uploadImage) {
			className = "normal";
			style.color = "#000";
			firstChild.src = "../res/prg_success.png";
		}
		verifyFirmware();
	} else if (oper == UPLOAD_SIGNKEY){	//SignImage Key file upload complete
		validateSignImageKey();
	}
}

function verifyFirmware()
{
	verifyImage.appendChild(progressIcon);
	verifyImage.className = "hiLite";
	var p = new xmit.getset({url:"/rpc/verifyimage.asp", onrcv:onReceiveVerify, timeout:120});
	p.send();
}

var ImageSizeChanged = 0;

function onReceiveVerify (arg)
{
	if(WEBVAR_JSONVAR_VERIFYIMAGE.HAPI_STATUS) {
		verifyImage.firstChild.src = "../res/prg_failure.png";
		verifyImage.removeChild(progressIcon);
		switch(WEBVAR_JSONVAR_VERIFYIMAGE.WEBVAR_STRUCTNAME_VERIFYIMAGE[0].IMGCOMPSTATUS) {
		case 48: case 40:
			alert(eLang.getString("common", "STR_FW_UPDATE_ERR4"));
			break;
		default:
			alert(eLang.getString("common", "STR_FW_UPDATE_ERR2"));
			break;
		}
		//Already the prepareDeviceShutddown was called in prepareClient,  So no need to call this again here.
		//prepareDeviceShutdown();
		setFlashMode(false);
		rebootDevice();
	}
	else
	{
		var fwveriinfo = WEBVAR_JSONVAR_VERIFYIMAGE.WEBVAR_STRUCTNAME_VERIFYIMAGE;
		var status = fwveriinfo[0]["IMGCOMPSTATUS"];

		var p = document.createElement("p");
		p.innerHTML = "<strong>Current Image Version: </strong>"+fwveriinfo[0]["CURIMGVERSION"] +
			"<br/><strong>New Image Version: </strong>"+fwveriinfo[0]["NEWIMGVERSION"];

		var btnAry = [];
		btnAry.push(createButton("_proceed","Proceed",proceedFlash));
		btnAry.push(createButton("_cancelFlashing","Cancel",cancelWizard));

		if(getbits(status,0,0)) {
			if(getbits(status,2,2)) {
				p.innerHTML += "<br/><br/>"+(eLang.getString("common","STR_FW_VERIFY_DIFFVERSION"));
			} else if(getbits(status,1,1)) {
				p.innerHTML += "<br/><br/>"+(eLang.getString("common","STR_FW_VERIFY_DIFFSIZE"));
			}
			if(getbits(status,1,1)) {
				ImageSizeChanged = 1;
			}
		} else {
			p.innerHTML += "<br/><br/>"+(eLang.getString("common","STR_FW_VERIFY_SAME"));
		}
		//wnd = MessageBox("Firmware Version",p,btnAry);
		updateSection("Firmware Version",p,btnAry);
	}
}

function updateSection(title,content,btnAry)
{
	section.className = "wizardsection";
	section.style.display = "";
	var hldrTitle = document.createElement("h3");
	hldrTitle.className = "title";
	hldrTitle.innerHTML = title;
	var hldrBtn = document.createElement("div");
	hldrBtn.className = "btnTray";
	for(var i=0; i<btnAry.length; i++) {
		hldrBtn.appendChild(btnAry[i]);
	}
	section.appendChild(hldrTitle);
	section.appendChild(content);
	section.appendChild(hldrBtn);
}

function proceedFlash()
{
	if(ImageSizeChanged)
	{
		if(!confirm(eLang.getString("common","STR_FW_UPDATE_CONFIRM3")))
		{
			return;
		}
	}

	if(!confirm(eLang.getString("common","STR_FW_UPDATE_CONFIRM4")))
	{
		return;
	}

	clearSection();

	var req = new xmit.getset({url:"/rpc/startflash.asp", onrcv:onReceiveStartFlash});
	req.add("WEBVAR_PRESERVECFG", chkPrsrvAll.checked ? 1 : 0);
	req.send();
	setFlashMode(false);
}


function onReceiveStartFlash(arg)
{
	if (arg.HAPI_STATUS == 0) {
		with(verifyImage)
		{
			className = "normal";
			style.color = "#000";
			firstChild.src = "../res/prg_success.png";
		}
		flashImage.appendChild(progressIcon);
		flashImage.className = "hiLite";
		FlashStatus();
	} else {
		verifyImage.firstChild.src = "../res/prg_failure.png";
		verifyImage.removeChild(progressIcon);
		alert (eLang.getString("common", "STR_FW_UPDATE_ERR3"));
		setFlashMode(false);
		rebootDevice();
	}
}

timerid = -1;
function FlashStatus()
{
	if(timerid != -1)
		clearTimeout(timerid);
	var p = new xmit.getset({url:"/rpc/flashstatus.asp", onrcv:onReceiveStatus});
	p.send();
}

function onReceiveStatus()
{
	if(timerid != -1)
		clearTimeout(timerid);

	var flashstat = WEBVAR_JSONVAR_FLASHPROGRESS.WEBVAR_STRUCTNAME_FLASHPROGRESS;

	var str = "";
	var curstate = flashstat[0]["FLSTATE"];

	percentageFlashed.innerHTML = "("+flashstat[0]["FLASHPROGRESS"]+")";

	if(WEBVAR_JSONVAR_FLASHPROGRESS.HAPI_STATUS == 0xff)
	{
		fnResetDevice();
	}
	else
	{
		timerid = setTimeout("FlashStatus()",2000);
	}
}

function fnResetDevice()
{
	with(flashImage)
	{
		className = "normal";
		style.color = "#000";
		firstChild.src = "../res/prg_success.png";
	}
	resetDevice.className = "hiLite";
	resetDevice.appendChild(progressIcon);
	setFlashMode(false);
	rebootDevice(true);

	with(resetDevice)
	{
		//className = "normal"; nice without white
		style.color = "#000";
		firstChild.src = "../res/prg_success.png";
		removeChild(progressIcon);
	}
}

function cancelWizard()
{
	if(!confirm(eLang.getString("common","STR_FW_UPDATE_CONFIRM2"))) return;

	progressIcon.parentNode.removeChild(progressIcon);
	clearSection();
	setFlashMode(false);
	rebootDevice();
}

function clearSection()
{
	section.className = "section";
	section.innerHTML = "";
	section.style.display = "none";
}

function doSignImageSupport()
{
	if(checkProjectCfg("ENC_IMAGE")) {
		btnKeyUpload.className = "visibleRow";
		btnKeyUpload.disabled = false;
		btnKeyUpload.onclick = getSignImageKey;
	} else {
		btnKeyUpload.className = "hiddenRow";
		btnKeyUpload.onclick = function() {};
	}
	btnFWUpdate.disabled = false;
}

function getSignImageKey()
{
	xmit.get({url:"/rpc/getsignimagekey.asp", onrcv:getSignImageKeyRes, 
		status:""});
}

function getSignImageKeyRes(arg)
{
	if(arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString("common", "STR_FW_SIGNKEY_GETVAL");
		errstr +=  (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		SIGNKEY_DATA = WEBVAR_JSONVAR_GETSIGNIMAGEKEYINFO.WEBVAR_STRUCTNAME_GETSIGNIMAGEKEYINFO[0];
	}
	frmUploadImageKey();;
}

function frmUploadImageKey()
{
	var frm = new form("uploadSignImageKey", "POST", "javascript://", 
		"general");

	signImageKeyInfo = frm.addTextField(eLang.getString("common", 
		"STR_FW_SIGNKEY_INFO"), "_signImageKeyInfo", 
		SIGNKEY_DATA.PublicKeyInfo, {"readOnly":true}, "bigclassicTxtBox");
	var fileUpload = document.createElement("div");
	if (fnCookie.read("WebServer").indexOf("lighttpd") != -1) {
		fileUpload.innerHTML = "<form name='signImageKeyUpload' " +
			"id='_signImageKeyUpload' method='POST' " +
			"enctype='multipart/form-data' action='file_upload.html' " +
			"target='hiddenFrame' style='margin-bottom:0'><input type='file' " +
			"name='/tmp/public.pem' id='_signKeyBrowse' size='35' /></form>";
	} else {
		fileUpload.innerHTML = "<form name='signImageKeyUpload' " +
			"id='_signImageKeyUpload' method='POST' " +
			"enctype='multipart/form-data' action='file_upload.html' " +
			"target='hiddenFrame' style='margin-bottom:0'><input type='file' " +
			"name='/tmp/public.pem?16384' id='_signKeyBrowse' size='35' />" +
			"</form>";
	}
	fileBrowseRow = frm.addRow(eLang.getString("common", 
		"STR_FW_NEW_SIGN_KEY"), fileUpload);

	var btnAry = [];
	btnAry.push(createButton("UploadBtn", eLang.getString("common", 
		"STR_UPLOAD"), uploadSignImageKey));
	btnAry.push(createButton("cancelBtn", eLang.getString("common", 
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_FW_SIGNKEY_TITLE"), 
		frm.display(), btnAry);
	//Ignores backspace functionality
	signImageKeyInfo.onkeydown = checkBackspace;
	wnd.onclose = doSignImageSupport;
}

function uploadSignImageKey()
{
	var signKeyBrowse = $("_signKeyBrowse");
	if (eVal.isblank(signKeyBrowse.value))
	{
		alert(eLang.getString("common","STR_FW_SIGNKEY_INVALID1"));
		return;
	}
	if( !eVal.endsWith(signKeyBrowse.value, ".pem") )
	{
		alert(eLang.getString("common","STR_FW_SIGNKEY_INVALID2"));
		return;
	}

	showWait(true,"Uploading");
	oper = UPLOAD_SIGNKEY;		//Used to indicate signImage public key file is uploading.
	document.forms["signImageKeyUpload"].submit();
}

function validateSignImageKey()
{
	xmit.get({url:"/rpc/validatesignkey.asp", onrcv:validateSignImageKeyRes, 
		status:""});
}

function validateSignImageKeyRes(arg)
{
	var signImageValid = 0;
	if(arg.HAPI_STATUS == 0) {
		signImageValid = WEBVAR_JSONVAR_VALIDATESIGNIMGKEY.WEBVAR_STRUCTNAME_VALIDATESIGNIMGKEY[0].SIGNIMAGE_VALID;
		switch (signImageValid) {
		case 0:
			alert (eLang.getString("common", "STR_FW_SIGNKEY_SUCCESS"));
			closeForm();
			break;
		case 1: case 3: case 5:
			alert(eLang.getString("common", "STR_FW_SIGNKEY_ERR" + 
				signImageValid));
			break;
		default:
			alert(eLang.getString("common", "STR_FW_SIGNKEY_ERR4"));
			break;
		}
	} else {
		errstr = eLang.getString("common", "STR_FW_SIGNKEY_VALIDATE_ERR");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

function closeForm()
{
	wnd.close();
	btnFWUpdate.disabled = false;
	chkPrsrvAll.disabled = false;
	btnPrsrvCfg.disabled = chkPrsrvAll.checked;
}

/*
 * It will check for the Preserve configuration support, If the user clicks the 
 * button, then it will redirect to the Preserve Configuration page under 
 * Maintenance.
 */
function doPreserveCfg()
{
	if(checkProjectCfg("PRESERVECONF")) {
		lblPrsrvAllDesc.innerHTML = eLang.getString("common", 
			"STR_FW_PRSRV_CFG_DESC");
		btnPrsrvCfg.className = "visibleRow";
		btnPrsrvCfg.disabled = false;
		btnPrsrvCfg.onclick = function () {
			location.href = "preserve_cfg.html";
		};
		divPrsrvCfg.className = "visibleRow";
		loadCustomPageElements();
		getPreserveCfg();
		chkPrsrvAll.onclick = enablePreserveAllCfg;
	} else {
		lblPrsrvAllDesc.innerHTML = "";
		btnPrsrvCfg.className = "hiddenRow";
		btnPrsrvCfg.onclick = function() {};
		divPrsrvCfg.className = "hiddenRow";
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
		loadPreserveCfg(false);
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
		doAllowNoSelect : true
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
 * list grid. Also it will pass the disabled argument to listgrid, which will 
 * load data in disabled/enabled format based on the argument value.
 * @param bopt boolean, true-enabled, false-disabled the listgrid data.
 */
function loadPreserveCfg(bopt)
{
	var rowIndex = 1;	//Row Index
	var rowJSON = [];	//Object of array of rows to load list grid
	var prsrvStatus;	//String of the Preserve status

	tblPrsrvCfg.clear();
	for(i = 0; i < PRSRVCFG_DATA.length; i++) {
		prsrvStatus = (PRSRVCFG_DATA[i].STATUS == 0) ?
			eLang.getString("common", "STR_OVERWRITE") :
			eLang.getString("common", "STR_PRESERVE");
		try {
			rowJSON.push({cells:[
				{text:rowIndex, value:rowIndex},
				{text:PRSRVCFG_DATA[i].CFG_NAME, 
				value:PRSRVCFG_DATA[i].CFG_NAME},
				{text:prsrvStatus, value:prsrvStatus}
			], disabled:bopt});
			rowIndex++;
		} catch(e) {
			alert(e);
		}
	}

	tblJSON.rows = rowJSON;
	tblPrsrvCfg.loadFromJson(tblJSON);
}

/*
 * This will enable or disable the Preserve configuration controls, based on 
 * the Preserve All check box value.
 */
function enablePreserveAllCfg()
{
	var bopt;	//boolean, checkbox value
	bopt = chkPrsrvAll.checked;
	btnPrsrvCfg.disabled = bopt;;
	loadPreserveCfg(bopt);
}
