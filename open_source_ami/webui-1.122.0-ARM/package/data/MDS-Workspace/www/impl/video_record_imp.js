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

// File Name  : video_record_imp.js
// Brief      : This implementation is to display the video files in list grid. 
// It contains implementation to play, download and delete the video files.
// Author Name: Arockia Selva Rani. A

var tblJSON;		//object to hold video information in JSON structure
var tblVideo		//List grid object to hold video information
var VIDEO_INFO;		//It holds the get RPC response data
var varVideoOper = 0;	//Specifies the Video Operation

/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_lblHeader",
		"_lgdVideo",
		"_btnPlay",
		"_btnDownload",
		"_btnDelete"]);

	btnPlay.onclick = playVideo;
	btnDownload.onclick = downloadVideo;
	btnDelete.onclick = deleteVideo;
	_begin();
}

/*
 * It will invoke loadCustomPageElements method to load list grid.
 * Also it will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	loadCustomPageElements();
	disableButtons(true);
	getVideoInfo();
}

/*
 * This function is used to load the list grid and its header information.
 * Also initializes the list grid select and double click event handler.  
 */
function loadCustomPageElements()
{
	lgdVideo.innerHTML = "";
	tblVideo = listgrid({
		w : "100%",
		doAllowNoSelect : false
	});

	lgdVideo.appendChild(tblVideo.table);

	tblJSON = {cols:[
		{text : eLang.getString("common", "STR_HASH"),
			fieldName:"file_no", fieldType:2, w:"20%", textAlign:"center"},
		{text:eLang.getString("common", "STR_VIDEO_RCRD_FILENAME"),
			fieldName:"img_name", w:"40%", textAlign:"center"},
		{text:eLang.getString("common", "STR_VIDEO_RCRD_FILEINFO"),
			fieldName:"img_info", w:"40%", textAlign:"center"}
		]};
	tblVideo.loadFromJson(tblJSON);

	/*
	 * This event handler will be invoked when the list grid row is selected.
	 */
	tblVideo.ontableselect = function ()
	{
		var filename;		//Selected file name
		if (this.selected.length) {
			filename = tblVideo.getRow(tblVideo.selected[0]).cells[1].innerHTML;
			filename.replace("&nbsp;", "").replace(" ", "");
			if (filename == "~") {
				disableButtons(true);
			} else {
				disableButtons(false);
			}
		}
	}

	/*
	 * This event handler will be invoked when double click in list grid row.
	 */
	tblVideo.ondblclick = function ()
	{
		var filename;		//Selected file name
		filename = tblVideo.getRow(tblVideo.selected[0]).cells[1].innerHTML;
		filename.replace("&nbsp;", "").replace(" ", "");
		if (filename != "~") {
			playVideo();
		}
	}
}

/*
 * It will invoke the RPC method to get the video information.
 * Once it get data from RPC, response function will be called automatically. 
 */
function getVideoInfo()
{
	xmit.get({url:"/rpc/getvideoinfo.asp", onrcv:getVideoInfoRes, status:""});
}

/*
 * This is the response function for getVideo RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getVideoInfoRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_VIDEO_RCRD_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		VIDEO_INFO = WEBVAR_JSONVAR_GETVIDEOFILE.WEBVAR_STRUCTNAME_GETVIDEOFILE;
		loadVideoInfo();
		if (varVideoOper == top.CONSTANTS.DELETE) {
			alert (eLang.getString("common", "STR_VIDEO_RCRD_DEL_SUCCESS"));
			varVideoOper = 0;
		}
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function loadVideoInfo()
{
	var filename_todisplay;		//File name to display in List grid
	var fileinfo_todisplay;		//File name to display in List grid
	var index;					//loop counter
	var rowJSON = [];			//Object of array of rows to load list grid
	var video_count = 0;		//Number of available video files

	tblVideo.clear();
	for (index = 0; index < VIDEO_INFO.length; index++) {
		// Use ~ char to indicate free slot so it will sort alphabetically
		filename_todisplay = "~";
		fileinfo_todisplay = "~";

		if ((VIDEO_INFO[index].FILE_NAME != "") && 
			(VIDEO_INFO[index].FILE_INFO != "")) {
			filename_todisplay = VIDEO_INFO[index].FILE_NAME;
			fileinfo_todisplay = VIDEO_INFO[index].FILE_INFO;
			video_count++;
		}

		try {
			rowJSON.push({cells:[
				{text:(index + 1), value:(index + 1)},
				{text:filename_todisplay, value:filename_todisplay},
				{text:fileinfo_todisplay, value:fileinfo_todisplay}
			]});
		} catch (e) {
			alert(e);
		}
	}
	tblJSON.rows = rowJSON;
	tblVideo.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + 
		eLang.getString("common", "STR_VIDEO_RCRD_FILE_CNT") + "</strong>" + 
		video_count + eLang.getString("common", "STR_BLANK");
}

/*
 * This will open a new window for launch that will download and open the passing 
 * JNLP file.
 */
function playVideo()
{
	var filename;		//Selected file name
	filename = tblVideo.getRow(tblVideo.selected[0]).cells[1].innerHTML;
	filename.replace("&nbsp;", "").replace(" ", "");
	if (filename != "~") {
		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
			if(arg.HAPI_STATUS == 0) {
				if (top.user.isAdmin()) {
					getConsoles();
					window.open(top.gPageDir + "jviewer_launch.html?" + 
						"JNLPSTR=PlayVideo&JNLPNAME=" + top.gVideoJnlp + 
						"&FILENAME=" + filename, "PlayVideo", "toolbar=0," +
						" resizable=yes, width=400, height=110, left=350, top=300");
				} else {
					alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
				}
			} else {
				parent.gLogout = 1;
				top.document.cookie = "SessionExpired=true;path=/";
				parent.location.href = "login.html";
			}
		},evalit:false});
	} else {
		alert(eLang.getString("common", "STR_VIDEO_RCRD_FILESEL_ERR"));
	}
}

/*
 * This will open a new window for launch that will download and open the passing 
 * JNLP file.
 */
function downloadVideo()
{
	var filename;		//Selected file name
	filename = tblVideo.getRow(tblVideo.selected[0]).cells[1].innerHTML;
	filename.replace("&nbsp;", "").replace(" ", "");
	if (filename != "~") {
		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
			if(arg.HAPI_STATUS == 0) {
				if (top.user.isAdmin()) {
					getConsoles();
					window.open(top.gPageDir + "jviewer_launch.html?" + 
						"JNLPSTR=SaveVideo&JNLPNAME=" + top.gVideoJnlp +
						"&FILENAME=" + filename, "DownloadVideo", "toolbar=0," +
						" resizable=yes, width=400, height=110, left=350, top=300");
				} else {
					alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
				}
			} else {
				parent.gLogout = 1;
				top.document.cookie = "SessionExpired=true;path=/";
				parent.location.href = "login.html";
			}
		},evalit:false});
	} else {
		alert(eLang.getString("common", "STR_VIDEO_RCRD_FILESEL_ERR"));
	}
}

/*
 * It will invoke the RPC method to delete the video file. Once we get response
 * from RPC request, response function will be called automatically.
 * Filename is passed as argument for RPC request.
 */
function deleteVideo()
{
	var filename;		//Selected file name
	var req;			//xmit object to send RPC request with parameters

	filename = tblVideo.getRow(tblVideo.selected[0]).cells[1].innerHTML;
	filename.replace("&nbsp;", "").replace(" ", "");
	if (filename != "~") {
		if (confirm(eLang.getString("common", "STR_CONFIRM_DELETE"))) {
			varVideoOper = top.CONSTANTS.DELETE;
			req = new xmit.getset({url:"/rpc/deletevideo.asp", 
				onrcv:deleteVideoRes, status:""});
			req.add("FILE_NAME", filename);
			req.send();
			delete req;
		}
	} else {
		alert(eLang.getString("common", "STR_VIDEO_RCRD_FILESEL_ERR"));
	}
}

/*
 * This is the response function for deleteVideo RPC.
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then delete video is success, intimate proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function deleteVideoRes(arg)
{
	var errstr;		//Error string
	if (arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_VIDEO_RCRD_DELVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		getVideoInfo();
	}
}

/*
 * This function will disable the buttons.
 * @param opt boolean, true-disable, false-enable.
 */
function disableButtons(opt)
{
	btnPlay.disabled = opt;
	btnDownload.disabled = opt;
	btnDelete.disabled = opt;
}
