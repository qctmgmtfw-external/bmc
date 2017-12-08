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

// File Name  : configure_lmedia_imp.js
// Brief      : This implementation is to display the image files in list grid. 
// It contains implementation to add, replace and delete the image files.
// Author Name: Arockia Selva Rani. A

var LMEDIA_DATA;	//It holds RPC response data of local media images.
var LMEDIA_ENABLE;	//It holds RPC response data of local media enable.
var tblJSON;		//object to hold image information in JSON structure
var tblImage;		//List grid object to hold image information
var varImageOper;
var varImageType = 0;
var varFilePath = "";

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_lblLMediaDesc",
		"_btnAdvSettings",
		"_lblHeader",
		"_lgdImage",
		"_btnAdd",
		"_btnReplace",
		"_btnDelete"]);

	if(top.user.isAdmin()) {
		btnAdd.onclick = function() {
			doProcessImage(top.CONSTANTS.ADD);
		}
		btnReplace.onclick = function() {
			doProcessImage(top.CONSTANTS.MODIFY);
		}
		btnDelete.onclick = function() {
			doProcessImage(top.CONSTANTS.DELETE);
		}
	} else {
		disableActions();
	}

	btnAdvSettings.onclick = frmAdvLMediaCfg;
	_begin();
}

/*
 * It will invoke loadCustomPageElements method to load list grid.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	loadCustomPageElements();
	getLMediaCfg();
}

/*
 * It will invoke the RPC method to get the local media and local media area 
 * configuration. Once it get data from RPC, response function will be called 
 * automatically. 
 */
function getLMediaCfg()
{
	btnAdvSettings.disabled = false;
	xmit.get({url:"/rpc/getremotesession.asp", onrcv:getLMediaCfgRes, 
		status:""});
	disableButtons();
	xmit.get({url:"/rpc/getlmediaimage.asp",onrcv:getLMediaAreaRes,status:""});
}

/*
 * This is the response function for get local media RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getLMediaCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_LMEDIA_CFG_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		LMEDIA_ENABLE = WEBVAR_JSONVAR_GETREMOTESESSIONCFG.WEBVAR_STRUCTNAME_GETREMOTESESSIONCFG[0].LMEDIAENABLE;
		lblLMediaDesc.innerHTML = eLang.getString("common", 
			"STR_CONF_LMEDIA_DESC_" + LMEDIA_ENABLE);
	}
}

/*
 * This will design the UI controls for Advanced LMedia configuration form.
 */
function frmAdvLMediaCfg()
{
	var frm = new form("advancedLMediaFrm", "POST", "javascript://", "general");

	settings = frm.addCheckBox(eLang.getString("common", 
		"STR_CONF_LMEDIA_ENABLE"), "chkLMediaEnable", {"enable":"Enable"}, false, 
		["enable"]);
	chkLMediaEnable = settings.enable;

	frm.addRow(eLang.getString("common", "STR_NEWLINE"), 
		eLang.getString("common", "STR_NEWLINE"));

	var btnAry = [];
	btnAry.push(createButton("btnSave", eLang.getString("common",
		"STR_SAVE"), setLMediaCfg));
	btnAry.push(createButton("btnCancel", eLang.getString("common",
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_CONF_LMEDIA_CFG_TITLE"),
		frm.display(), btnAry);
	wnd.onclose = getLMediaCfg;

	chkLMediaEnable.checked = LMEDIA_ENABLE ? true : false;
	if(!top.user.isAdmin()) {
		disableActions({id:["_btnAdvSettings", "_btnCancel"]});
	}
}

function setLMediaCfg()
{
	var req;			//xmit object to send RPC request with parameters
	if (confirm(eLang.getString("common", "STR_CONF_LMEDIA_CFG_CONFIRM"))) {
		req = new xmit.getset({url:"/rpc/setlmediacfg.asp", 
			onrcv:setLMediaCfgRes, status:""});
		req.add("ENABLE", (chkLMediaEnable.checked) ? 1 : 0);
		req.send();
		delete req;
	}
}

function setLMediaCfgRes(arg)
{
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_LMEDIA_CFG_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert (eLang.getString("common", "STR_CONF_LMEDIA_CFG_SUCCESS"));
		closeForm();
	}
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid select and double click event handler.  
 */
function loadCustomPageElements()
{
	lgdImage.innerHTML = "";
	tblImage = listgrid({
		w : "100%",
		doAllowNoSelect : false
	});

	lgdImage.appendChild(tblImage.table);

	tblJSON = {cols:[
		{text:eLang.getString("common","STR_HASH"), fieldName:"img_no", 
			fieldType:2, w:"10%", textAlign:"center"},
		{text:eLang.getString("common","STR_CONF_LMEDIA_IMGTYPE"), 
			fieldName:"img_type", w:"30%", textAlign:"center"},
		{text:eLang.getString("common","STR_CONF_LMEDIA_IMGNAME"), 
			fieldName:"img_name", w:"30%", textAlign:"center"},
		{text:eLang.getString("common","STR_CONF_LMEDIA_IMGINFO"), 
			fieldName:"img_info", w:"30%", textAlign:"center"}
		]};

	tblImage.loadFromJson(tblJSON);

	/*
	 * This event handler will be invoked when the list grid row is selected.
	 */
	tblImage.ontableselect = function()
	{
		var imgname;		//Selected image name
		disableButtons();
		if(top.user.isAdmin() && LMEDIA_ENABLE) {
			if (this.selected.length) {
				imgname = tblImage.getRow(tblImage.selected[0]).cells[2].innerHTML;
				imgname.replace("&nbsp;","").replace(" ","");
				if (imgname == "~") {
					btnAdd.disabled = false;
				} else {
					btnReplace.disabled = false;
					btnDelete.disabled = false;
				}
			}
		}
	}

	/*
	 * This event handler will be invoked when double click in list grid row.
	 */
	tblImage.ondblclick = function()
	{
		var imgname;		//Selected image name
		if(top.user.isAdmin() && LMEDIA_ENABLE) {
			imgname = tblImage.getRow(tblImage.selected[0]).cells[2].innerHTML;
			imgname.replace("&nbsp;","").replace(" ","");
			if (imgname == "~") {
				doProcessImage(1);		//Adding image to Local Media
			} else {
				doProcessImage(2);		//Replacing image to Local Media
			}
		}
	}
}

/*
 * This is the response function for get the local media area RPC.
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getLMediaAreaRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_LMEDIA_IMG_GETVAL");
		errstr += (eLang.getString("common","STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		LMEDIA_DATA = WEBVAR_JSONVAR_GETLMEDIAIMAGE.WEBVAR_STRUCTNAME_GETLMEDIAIMAGE;
		loadImageTable();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function loadImageTable()
{
	var imagename_todisplay;	//File name of image to display in List grid
	var imagetype_todisplay;	//Image type to display in List grid
	var imageinfo_todisplay;	//Image information to display in List grid
	var image_count = 0;		//Number of available image files
	var index;					//loop counter
	var rowJSON = [];			//Object of array of rows to load list grid

	tblImage.clear();
	for (index = 0; index < LMEDIA_DATA.length; index++) {
		// Use ~ char to indicate free slot so it will sort alphabetically
		imagetype_todisplay = LMEDIA_DATA[index].IMAGE_TYPE_STR;
		imagename_todisplay = "~";
		imageinfo_todisplay = "~";

		if ((LMEDIA_DATA[index].IMAGE_NAME != "") && 
			(LMEDIA_DATA[index].IMAGE_INFO != "") && 
			(LMEDIA_DATA[index].IMAGE_EXISTS != 0)) {
			imagename_todisplay = LMEDIA_DATA[index].IMAGE_NAME;
			imageinfo_todisplay = LMEDIA_DATA[index].IMAGE_INFO;
			image_count++;
		}
		try {
			rowJSON.push({cells:[
				{text:(index+1), value:(index+1)},
				{text:imagetype_todisplay, value:imagetype_todisplay},
				{text:imagename_todisplay, value:imagename_todisplay},
				{text:imageinfo_todisplay, value:imageinfo_todisplay}
			]});
		} catch(e) {
			alert(e);
		}
	}

	tblJSON.rows = rowJSON;
	tblImage.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + 
		eLang.getString("common", "STR_CONF_LMEDIA_IMG_CNT") + "</strong>" + 
		image_count + eLang.getString("common", "STR_BLANK");
}

/*
 * This will get index from the selected row of list grid, based on the data
 * in selected row, it will invoke either add or replace image method.
 * @param imageoper integer, Image operation. 1-Add, 2-Replace, 3-Delete.
 */
function doProcessImage(imageoper)
{
	var index;		//Index of the list grid
	if ((tblImage.selected.length != 1) || 
		(tblImage.selected[0].cells[0] == undefined) || 
		(tblImage.selected[0].cells[0] == null)) {
		alert(eLang.getString("common", "STR_CONF_LMEDIA_ERR1"));
		disableButtons();
	} else {
		index = parseInt(tblImage.getRow(tblImage.selected[0]).cells[0].innerHTML);
		varImageOper = imageoper;	//Storing operation value for future use
		switch(imageoper) {
		case top.CONSTANTS.ADD:
			if (LMEDIA_DATA[index-1].IMAGE_EXISTS != 0) {
				if (confirm(eLang.getString("common", "STR_CONF_LMEDIA_CONFIRM1"))) {
					frmReplaceImage({"index" : index});
				}
			} else {
				frmAddImage({"index" : index});
			}
			break;
		case top.CONSTANTS.MODIFY:
			if (LMEDIA_DATA[index-1].IMAGE_EXISTS == 0) {
				if (confirm(eLang.getString("common", "STR_CONF_LMEDIA_CONFIRM2"))) {
					frmAddImage({"index" : index});
				}
			} else {
				frmReplaceImage({"index":index});
			}
			break;
		case top.CONSTANTS.DELETE:
			if (LMEDIA_DATA[index-1].IMAGE_EXISTS == 0) {
				alert (eLang.getString("common", "STR_CONF_LMEDIA_ERR2"));
			} else {
				if (confirm(eLang.getString("common", "STR_CONFIRM_DELETE"))) {
					deleteImageCfg(index);
				}
			}
			break;
		}
	}
}

/*
 * It will display a form, which contains UI controls to add a new image to local media.
 * @param arg object, contains index of the selected slot in list grid.
 */
function frmAddImage(arg)
{
	var frm = new form("addImageForm", "POST", "javascript://", "general");

	txtImageType = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LMEDIA_IMGTYPE"), "_txtImageType", 
		LMEDIA_DATA[arg.index-1].IMAGE_TYPE_STR, {"readOnly":true}, "classicTxtBox");
	varImageType = LMEDIA_DATA[arg.index-1].IMAGE_TYPE;

	var divFileupload = document.createElement("div");
	divFileupload.innerHTML = "<form name='frmImageUpload' id='_frmImageUpload'" +
		" method='POST' enctype='multipart/form-data' action='file_upload.html'" +
		" target='hiddenFrame' style='margin-bottom:0'><input type='file'" +
		" id='_fleImageBrowse' size='35' /></form>";
	rowImageBrowse = frm.addRow(eLang.getString("common", "STR_CONF_LMEDIA_IMGFILE"), 
		divFileupload);

	var btnAry = [];
	btnAry.push(createButton("btnAdddImage", eLang.getString("common", 
		"STR_ADD"), validateImageCfg));
	btnAry.push(createButton("btnCancel", eLang.getString("common", 
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_CONF_LMEDIA_ADD_IMAGE"), 
		frm.display(), btnAry);
	//Ignores backspace functionality
	txtImageType.onkeydown = checkBackspace;
	wnd.onclose = getLMediaCfg;
}

/*
 * It will display a form, which contains UI controls to replace a image to local media.
 * @param arg object, contains index of the selected slot in list grid.
 */
function frmReplaceImage(arg)
{
	var frm = new form("replaceImageForm","POST","javascript://","general");

	txtImageType = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LMEDIA_IMGTYPE"), "_txtImageType", 
		LMEDIA_DATA[arg.index-1].IMAGE_TYPE_STR, {"readOnly":true}, "classicTxtBox");
	varImageType = LMEDIA_DATA[arg.index-1].IMAGE_TYPE;

	txtImageInfo = frm.addTextField(eLang.getString("common", 
		"STR_CONF_LMEDIA_IMGINFO"), "_txtImageInfo", 
		LMEDIA_DATA[arg.index-1].IMAGE_INFO, {"readOnly":true}, "bigclassicTxtBox");

	var divFileupload = document.createElement("div");
	divFileupload.innerHTML = "<form name='frmImageUpload' id='_frmImageUpload'" +
		" method='POST' enctype='multipart/form-data' action='file_upload.html'" +
		" target='hiddenFrame' style='margin-bottom:0'><input type='file'" +
		" id='_fleImageBrowse' size='35' /></form>";
	rowImageBrowse = frm.addRow(eLang.getString("common", "STR_CONF_LMEDIA_IMGFILE"),
		divFileupload);

	var btnAry = [];
	btnAry.push(createButton("btnReplaceImage", eLang.getString("common", 
		"STR_REPLACE"), validateImageCfg));
	btnAry.push(createButton("btnCancel", eLang.getString("common", 
		"STR_CANCEL"), closeForm));

	wnd = MessageBox(eLang.getString("common", "STR_CONF_LMEDIA_REPLACE_IMAGE"),
		frm.display(), btnAry);
	//Ignores backspace functionality
	txtImageType.onkeydown = checkBackspace;
	//Ignores backspace functionality
	txtImageInfo.onkeydown = checkBackspace;
	wnd.onclose = getLMediaCfg;
}

/*
 * It will invoke the RPC method to set the local media image configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setlMediaImage()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/setlmediaimage.asp", 
		onrcv:setlMediaImageRes, status:""});
	req.add("IMAGE_OPER", varImageOper);
	req.add("IMAGE_TYPE", varImageType);
	req.add("IMAGE_NAME", varFilePath);
	req.send();
	delete req;
}

/*
 * This is the response function for setlMediaImage RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then set local media configuration is success, intimate proper message to end 
 * user.
 * @param arg object, RPC response data from xmit library
 */
function setlMediaImageRes(arg)
{
	var errstr;		//Error string
	if (varImageOper == 1) {
		$("_btnAdddImage").disabled = false;
	} else if (varImageOper == 2) {
		$("_btnReplaceImage").disabled = false;
	} else if (varImageOper == 3) {
		getLMediaCfg();
	}
	if (varImageOper != 3) {
		$("_btnCancel").disabled = false;
		$("_fleImageBrowse").disabled = false;
	}

	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_LMEDIA_IMG_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		alert (eLang.getString("common", "STR_CONF_LMEDIA_IMG_SUCCESS" + 
			varImageOper));
		closeForm();
	}
}

/*
 * It will invoke the RPC method to delete the local media image configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 * @param index number, selected slot# in list grid.
 */
function deleteImageCfg(index)
{
	varImageType = LMEDIA_DATA[index-1].IMAGE_TYPE;
	varFilePath = "";
	setlMediaImage();
}

/*
 * It will validate the data of all user controls before saving it.
 */
function validateImageCfg()
{
	var filepath;			//File path
	var filename;			//File name
	var fleImageBrowse;		//File Image Browse control

	filepath = "/usr/local/tmplmedia/";
	fleImageBrowse = $("_fleImageBrowse");
	if (eVal.isblank(fleImageBrowse.value)) {
		alert (eLang.getString("common", "STR_CONF_LMEDIA_INVALID_FILE1"));
		return;
	}
	if (varImageType == 2 || varImageType == 4){
		//Image type is Floppy or Harddisk should have extension of .img
		if(!eVal.endsWith(fleImageBrowse.value,".img")) {
			alert (eLang.getString("common", "STR_CONF_LMEDIA_INVALID_FDIMG"));
			return;
		}
	}
	if (varImageType == 1){	//Image type is CD/DVD should have extension of .iso
		if(!eVal.endsWith(fleImageBrowse.value,".iso")) {
			alert (eLang.getString("common", "STR_CONF_LMEDIA_INVALID_CDIMG"));
			return;
		}
	}
	
	filename = fleImageBrowse.value.split("\\");
	if (filename.length) {
		filename = filename[filename.length-1];
	} else {
		filename = filename[0];
	}
	varFilePath = filepath + filename;
	if (compareFilename(filename)){
		alert (eLang.getString("common", "STR_CONF_LMEDIA_INVALID_FILE2"));
		return;
	}
	uploadImage();
}

/*
 * It will upload the file to the web server. Once the upload was complete, it 
 * will call the uploadComplete method automatically. 
 */
function uploadImage()
{
	var filepath_size;	//File path with file size
	var filesize;		//File size
	var fleImageBrowse;	//File Image Browse control

	if (confirm(eLang.getString("common", "STR_CONF_LMEDIA_CNFM_UPLOAD" 
		+ varImageOper))) {
		fleImageBrowse = $("_fleImageBrowse");
		if (fnCookie.read("WebServer").indexOf("lighttpd") != -1) {
			fleImageBrowse.name = varFilePath;
		} else {
			//8GB maximum size (8*1024*1024*1024)   now 16MB
			filesize = 1024 * 1024 * 16 * 1;
			filepath_size = varFilePath + "?" + filesize;
			fleImageBrowse.name = filepath_size;
		}

		showWait(true, "Uploading");
		parent.web_alerts.stop();
		document.forms["frmImageUpload"].submit();

		if (varImageOper == 1) {
			$("_btnAdddImage").disabled = true;
		} else if (varImageOper == 2) {
			$("_btnReplaceImage").disabled = true;
		}

		$("_btnCancel").disabled = true;
		fleImageBrowse.disabled = true;
	}
}

/*
 * Once the upload completed, the control comes to this method.
 * This will invoke the set local media image configuration.
 */
function uploadComplete()
{
	showWait(false);
	parent.web_alerts.monitor();
	setlMediaImage();
}

/*
 * Used to close the form which is used to add or replace the image
 */
function closeForm()
{
	wnd.close();
}

/*
 * It will compare the filename with already existing filenames.
 * @param fnametocmp string, filename to compare.
 * @return boolean, true-filename matches, false-no match.
 */
function compareFilename(fnametocmp)
{
	var fname;	//filename
	var i;		//loop counter
	for (i = 0; i < LMEDIA_DATA.length; i++) {
		fname = LMEDIA_DATA[i].IMAGE_NAME.split("\/");
		if (fname.length) {
			fname = fname[fname.length-1];
		} else {
			fname = fname[0];
		}

		if (fname == fnametocmp) {
			return true;
		}
	}
	return false;
}

/*
 * It will disable the buttons.
 */
function disableButtons()
{
	btnAdd.disabled = true;
	btnReplace.disabled = true;
	btnDelete.disabled = true;
}