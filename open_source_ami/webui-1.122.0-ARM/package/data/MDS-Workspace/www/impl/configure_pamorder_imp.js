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

// File Name  : configure_pamorder_imp.js
// Brief      : This implementation is to display and configure the PAM Order 
// for User authentication into the BMC.
// Author Name: Arockia Selva Rani. A

var PAMORDER_DATA;	//It holds RPC response data of services configuration.
var MININDEX = 0;
var MAXINDEX = 2;
var pamHighlightIndex;
/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms(["_tblPAM",
		"_divPAM1",
		"_divPAM2",
		"_divPAM3",
		"_btnUp",
		"_btnDown",
		"_btnSave",
		"_btnReset"]);
	
	if(top.user.isAdmin()) {
		divPAM1.onclick = highlightPAM;
		divPAM2.onclick = highlightPAM;
		divPAM3.onclick = highlightPAM;
		btnUp.onclick = movePAMUp;
		btnDown.onclick = movePAMDown;
		btnSave.onclick = setPAMOrder;
		btnReset.onclick = reloadPAMOrder;
	} else {
		disableActions();
	}
	_begin();
}

/*
 * It will invoke the RPC method to get the data for the page.
 */
function _begin()
{
	getPAMOrder();
}

/*
 * It will invoke the RPC method to get the PAM Order configuration. Once it
 * get response from RPC, on receive method will be called automatically.
 */
function getPAMOrder()
{
	xmit.get({url:"/rpc/getpamorder.asp", onrcv:getPAMOrderRes, status:""});
}

/*
 * This is the response function for getPAMOrder RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getPAMOrderRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_PAM_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		PAMORDER_DATA = WEBVAR_JSONVAR_GETPAMORDER.WEBVAR_STRUCTNAME_GETPAMORDER;
		reloadPAMOrder();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadPAMOrder()
{
	var i;		//loop counter
	var index;	//index of the PAM table
	clearPAM();
	enableNavButtons(true);
	for (i = 0; i < PAMORDER_DATA.length; i++) {
		index = getHighlightPAMIndex("_divPAM" + PAMORDER_DATA[i].POSITION);
		if (i != index) {
			swapElement(i, index);
			initializePAM();
		}
	}
}

/*
 * It will initialize the function handler for all the PAM div elements.
 */
function initializePAM()
{
	$("_divPAM1").onclick = highlightPAM;
	$("_divPAM2").onclick = highlightPAM;
	$("_divPAM3").onclick = highlightPAM;
}

/*
 * It will clear all the PAM div elements to default background.
 */
function clearPAM()
{
	$("_divPAM1").style.backgroundColor = "#ffffff";
	$("_divPAM2").style.backgroundColor = "#ffffff";
	$("_divPAM3").style.backgroundColor = "#ffffff";
}

/*
 * Whenever user clicks a PAM div element, it gets highlighted.
 * It will invoke the disable navigation buttons based on its index.
 */
function highlightPAM()
{
	clearPAM();
	this.style.backgroundColor = "#cccccc";
	pamHighlightIndex = getHighlightPAMIndex(this.id);
	disableNavButtons(pamHighlightIndex);
}
/*
 * It will move up the selected PAM and initialize the function handler.
 */
function movePAMUp()
{
	if (pamHighlightIndex != MININDEX) {
		swapElement (pamHighlightIndex, pamHighlightIndex - 1);
		pamHighlightIndex--;
		disableNavButtons(pamHighlightIndex);
		initializePAM();
	}
}

/*
 * It will move down the selected PAM and initialize the function handler.
 */
function movePAMDown()
{
	if (pamHighlightIndex != MAXINDEX) {
		swapElement (pamHighlightIndex, pamHighlightIndex + 1);
		pamHighlightIndex++;
		disableNavButtons(pamHighlightIndex);
		initializePAM();
	}
}

/*
 * It will swap the row content of the PAM table.
 */
function swapElement(elmtIndex1, elmtIndex2)
{
	var temp;
	temp = tblPAM.rows[elmtIndex1].cells[0].innerHTML;
	tblPAM.rows[elmtIndex1].cells[0].innerHTML = tblPAM.rows[elmtIndex2].cells[0].innerHTML;
	tblPAM.rows[elmtIndex2].cells[0].innerHTML = temp;
}

/*
 * It will invoke the RPC method to set the PAM order configuration. Once it
 * get response from RPC, on receive method will be called automatically.
 */
function setPAMOrder()
{
	var req;			//xmit object to send RPC request with parameters
	if (confirm(eLang.getString("common", "STR_CONF_PAM_CONFIRM") + 
			eLang.getString("common", "STR_CONF_SERVICES_CNFMWEB"))) {
		req = new xmit.getset({url:"/rpc/setpamorder.asp", onrcv:setPAMOrderRes, 
			status:""});
		for (i = 1; i <= PAMORDER_DATA.length; i++) {
			index = getHighlightPAMIndex("_divPAM" + i);
			req.add("POSITION" + index, i);
		}
		req.send();
		delete req;
	}
}

/*
 * This is the response function for setPAMOrder RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting virtual media configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setPAMOrderRes(arg)
{
	var errstr;		//Error string
	switch(GET_ERROR_CODE(arg.HAPI_STATUS)) {
	case 0x0:
		prepareDeviceShutdown();
		var mb = MessageBox(eLang.getString("common", "STR_WEB_RESET_TITLE"), 
			p(eLang.getString("common", "STR_WEB_RESET_DESC")), [], true);
		clearCookies();
		break;
	case 0x90:
		alert (eLang.getString("common", "STR_CONF_PAM_ERR"));
		break;
	default:
		errstr = eLang.getString("common", "STR_CONF_PAM_SETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
}

function getHighlightPAMIndex(highlightID)
{
	var i;
	var pamIndex = -1;
	for (i = 0; i < tblPAM.rows.length; i++) {
		if (tblPAM.rows[i].cells[0].innerHTML.indexOf(highlightID) != -1) {
			pamIndex = i;
			break;
		}
	}
	if (pamIndex == -1) {
		alert ("Error in getting PAM Index");
	}
	return (pamIndex);
}

function disableNavButtons(index)
{
	enableNavButtons(false);
	if (index == MININDEX) {
		btnUp.disabled = true;
	} else if (index == MAXINDEX) {
		btnDown.disabled = true;
	}
}

function enableNavButtons(option)
{
	btnUp.disabled = option;
	btnDown.disabled = option;
}