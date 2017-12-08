//;*****************************************************************;
//;*****************************************************************;
//;**                                                             **;
//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2011        **;
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

// File Name  : configure_triggers_imp.js
// Brief      : This implementation is to display and configure the event 
// triggers available for Auto Video Recording feature in BMC.
// Author Name: Arockia Selva Rani. A

var TRIGCFG_DATA;			//It holds the get RPC Event Triggers response data
var SERVICE_DATA;
var TIMESTAMP_EVENT = (9 - 1);	//Index starts with 0, TimeStamp event is 9.
var STARTYEAR = 2005;
var ENDYEAR = 2038;
/*
 * This function will be called when its corresponding page gets loaded.
 * It will expose all the user controls and checks for user privilege.
 * Finally it will invoke the begin method. 
 */
function doInit()
{
	exposeElms([
		"_tblEvents",
		"_btnSave",
		"_btnReset"]);

	if(top.user.isAdmin()) {
		btnSave.onclick = validateTimeStamp;
		btnReset.onclick = reloadTriggerCfg;
	} else if (top.user.isOperator()) {
		disableActions();
	} else {
		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
		location.href = "dashboard.html";
		return;
	}
	_begin();
}

/*
 * It will invoke the RPC method to get the event trigger data for the page.
 */
function _begin()
{
	getTriggerCfg();
	getServiceCfg();
}

/*
 * It will invoke the RPC method to get all event trigger configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function getTriggerCfg()
{
	xmit.get({url:"/rpc/gettriggercfg.asp", onrcv:getTriggerCfgRes, status:""});
}

/*
 * This is the response function for getTriggerCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI. 
 * @param arg object, RPC response data from xmit library
 */
function getTriggerCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_TRIG_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		TRIGCFG_DATA = WEBVAR_JSONVAR_GETTRIGCFG.WEBVAR_STRUCTNAME_GETTRIGCFG;
		designTriggerCfg();
		reloadTriggerCfg();
	}
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadTriggerCfg()
{
	var i;		//loop counter
	for(i = 0; i < TRIGCFG_DATA.length; i++) {
		try {
			$("_chkTrigEnable" + i).checked = (TRIGCFG_DATA[i].ENABLE) ? true : false;
		} catch(e) {
			continue;
		}
	}
	reloadTimestamp();
}

/*
 * It will load response data from global variable to respective controls in UI.
 */
function reloadTimestamp()
{
	var timeStamp;
	var yearCheck;
	var monCheck;
	var dayCheck;
	var hrsCheck;
	var minsCheck;
	var secsCheck;

	//Date object requires milliseconds
	timeStamp = new Date(TRIGCFG_DATA[TIMESTAMP_EVENT].TIMESTAMP * 1000);
	yearCheck = timeStamp.getUTCFullYear();
	if ((yearCheck >= STARTYEAR) && (yearCheck <= ENDYEAR)) {
		lstYear.value = yearCheck;
	} else {
		lstYear.value = STARTYEAR;
	}

	monCheck = timeStamp.getUTCMonth();
	if ((monCheck >= 0) && (monCheck <= 11)) {
		lstMonth.value = monCheck;
	} else {
		lstMonth.value = 0;
	}

	dayCheck = timeStamp.getUTCDate();
	if ((dayCheck >= 1) && (dayCheck <= 31)) {
		lstDate.value = dayCheck;
	} else {
		lstDate.value = 1;
	}
	hrsCheck = timeStamp.getUTCHours();
	minsCheck = timeStamp.getUTCMinutes();
	secsCheck = timeStamp.getUTCSeconds();
	if ((!isNaN(hrsCheck)) && (!isNaN(minsCheck)) && (!isNaN(secsCheck))) {
		txtHour.value = ((hrsCheck < 10) ? "0" : "") + hrsCheck;
		txtMinute.value = ((minsCheck < 10) ? "0" : "") + minsCheck;
		txtSecond.value = ((secsCheck < 10) ? "0" : "") + secsCheck;
	}
	enableTimeStamp();
}

/*
 * It will validate the data of all user controls before saving it.
 */
function validateTimeStamp()
{
	if ($("_chkTrigEnable" + TIMESTAMP_EVENT).checked) {
		if(!eVal.isnumstr(txtHour.value, 0, 23)) {
			alert(eLang.getString("common", "STR_CONF_NTP_INVALID_HOUR"));
			txtHour.focus();
			return;
		}
		if(!eVal.isnumstr(txtMinute.value, 0, 59)) {
			alert(eLang.getString("common", "STR_CONF_NTP_INVALID_MINS"));
			txtMinute.focus();
			return;
		}
		if(!eVal.isnumstr(txtSecond.value, 0, 59)) {
			alert(eLang.getString("common", "STR_CONF_NTP_INVALID_SECS"));
			txtSecond.focus();
			return;
		}
		
		if(lstMonth.value == 1) {//Check for February
			if((lstYear.value % 4) == 0) {
				if(lstDate.value > 29) {
					alert(eLang.getString("common", "STR_CONF_NTP_INVALID_DATE") +
						eLang.getString("common", "STR_CONF_NTP_INVALID_LEAP"));
					return;
				}
			} else {
				if(lstDate.value > 28) {
					alert(eLang.getString("common", "STR_CONF_NTP_INVALID_DATE") +
						eLang.getString("common", "STR_CONF_NTP_INVALID_FEB"));
					return;
				}
			}
		} else if((lstMonth.value == 3)  || (lstMonth.value == 5) || 
			(lstMonth.value == 8) || (lstMonth.value == 10)) {
			if(lstDate.value > 30) {
				alert(eLang.getString("common", "STR_CONF_NTP_INVALID_DATE") +
					eLang.getString("common", "STR_CONF_NTP_INVALID_MONTH"));
				return;
			}
		}
	
		/*
		 * Check for unix timestamp, Check upto 18th January 2038 (Year 2038 Problem)
		 */
		if((lstMonth.value == 0) && (lstDate.value >= 19) &&
			(lstYear.value == ENDYEAR)) {
			alert(eLang.getString("common", "STR_CONF_NTP_INVALID_DATE") +
				eLang.getString("common", "STR_CONF_TRIG_YEAR2038"));
			return;
		}
		var chkSeconds = Date(lstYear.value, lstMonth.value, lstDate.value, 
			parseInt(txtHour.value, 10), parseInt(txtMinute.value, 10),
			parseInt(txtSecond.value, 10));
		chkSeconds /= 1000;		//To milliseconds to seconds
		if (chkSeconds > Math.pow(2,31)){	//Check for Year 2038 Problem exact value
			alert(eLang.getString("common", "STR_CONF_NTP_INVALID_DATE") +
				eLang.getString("common", "STR_CONF_TRIG_YEAR2038"));
			return;
		}
	}
	setTriggerCfg();
}

/*
 * It will invoke the RPC method to set all event trigger configuration.
 * Once it get response from RPC, on receive method will be called automatically.
 */
function setTriggerCfg()
{
	var req;			//xmit object to send RPC request with parameters
	var i;		//loop counter
	var seconds;
	var data;

	req = new xmit.getset({url:"/rpc/settriggercfg.asp", 
		onrcv:setTriggerCfgRes, status:""});
	for(i = 0; i < TRIGCFG_DATA.length; i++) {
		try {
			data = ($("_chkTrigEnable" + i).checked) ? 1 : 0;
			if (i != TIMESTAMP_EVENT) {
				if (data == TRIGCFG_DATA[i].ENABLE) {
					continue;
				}
			}
			req.add("EVTTRIG" + (i + 1), data);
		} catch(e) {
			continue;
		}
	}

	if ($("_chkTrigEnable" + TIMESTAMP_EVENT).checked) {
		seconds = Date.UTC(lstYear.value, lstMonth.value, lstDate.value, 
			parseInt(txtHour.value, 10), parseInt(txtMinute.value, 10),
			parseInt(txtSecond.value, 10), 0);
		seconds /= 1000;	//To milliseconds to seconds
		req.add("TIMESTAMP" + (TIMESTAMP_EVENT + 1), seconds);
	}
	req.send();
	delete req;
}

/*
 * This is the response function for setTriggerCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If zero, then setting network bonding configuration is success, intimate 
 * proper message to end user.
 * @param arg object, RPC response data from xmit library
 */
function setTriggerCfgRes(arg)
{
	var errstr;		//Error string
	var i;		//loop counter

	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_TRIG_SETVAL");
		STATUS_DATA = WEBVAR_JSONVAR_SETTRIGCFG.WEBVAR_STRUCTNAME_SETTRIGCFG;
		for(i = 0; i < STATUS_DATA.length; i++) {
			if (STATUS_DATA[i].STATUS != 0) {
				errstr += eLang.getString("common", "STR_NEWLINE");
				errstr += eLang.getString("common", "STR_CONF_TRIG_EVT" + i);
				if ((i == TIMESTAMP_EVENT) && 
					(GET_ERROR_CODE(STATUS_DATA[i].STATUS) == 0x93)) {
					//Check for TimeStamp invalid data
					errstr += " - " + eLang.getString("common", 
						"STR_CONF_TRIG_EVT_ERR" + TIMESTAMP_EVENT);
				}
			}
		}
		alert(errstr);
	} else {
		errstr = eLang.getString("common", "STR_CONF_TRIG_SUCCESS");
		errstr += (SERVICE_DATA.STATE) ? "" : eLang.getString("common",
			"STR_CONF_TRIG_KVMDISABLE");
		alert(errstr);
	}
	getTriggerCfg();
}

/*
 * It will design the needed check boxes based on RPC response in UI.
 */
function designTriggerCfg()
{
	var rowCount = 0;	//Row count
	var celIndex = 0;	//Cell index
	var i;		//loop counter

	clearTable(tblEvents);
	rowCount = Math.floor(TRIGCFG_DATA.length / 2) + (TRIGCFG_DATA.length % 2);
	for(i = 0; i < rowCount; i++) {
		rowEvent = tblEvents.insertRow(i);
		//Check boxed to be drawn in left side
		celIndex = i + i;
		celLeft = rowEvent.insertCell(0);
		tagCheckbox = "<input type='checkbox' id='_chkTrigEnable" + celIndex + 
			"'>&nbsp;<strong><label for='_chkTrigEnable" + celIndex + "'>" + 
			eLang.getString("common", "STR_CONF_TRIG_EVT" + celIndex) + 
			"</label></strong>";
		celLeft.innerHTML = tagCheckbox;
		if (celIndex == TIMESTAMP_EVENT) {
			celLeft.innerHTML += designTimestamp();
			lstDate = $("_lstDate");
			lstMonth = $("_lstMonth");
			lstYear = $("_lstYear");
			txtHour = $("_txtHour");
			txtMinute = $("_txtMinute");
			txtSecond = $("_txtSecond");
			initTimestamp();
			$("_chkTrigEnable" + TIMESTAMP_EVENT).onclick = enableTimeStamp;
		}

		//Check boxed to be drawn in right side
		celIndex++;
		if (celIndex < TRIGCFG_DATA.length) {
			celRight = rowEvent.insertCell(1);
			tagCheckbox = "<input type='checkbox' id='_chkTrigEnable" + celIndex + 
				"'>&nbsp;<strong><label for='_chkTrigEnable" + celIndex + "'>" + 
				eLang.getString("common", "STR_CONF_TRIG_EVT" + celIndex) + 
				"</label></strong>";
			celRight.innerHTML = tagCheckbox;
		}
	}
}

/*
 * It will design the Time stamp control includes Date, Month, Year, Hours, 
 * Minutes and Seconds for event which needs time stamp control, based on RPC 
 * response in UI.
 */
function designTimestamp()
{
	var tagTimestampTbl;
	tagTimestampTbl = "<br><table cellspacing='2' cellpadding='5' border='0' width='500'>" +
		"<tr><td><strong>Date:</strong></td><td>" +
		"<select id='_lstMonth' class='classicTxtBox'></select> &nbsp;" +
		"<select id='_lstDate' class='smallclassicTxtBox'></select> &nbsp;" +
		"<select id='_lstYear' class='smallclassicTxtBox'></select>" +
		"</td></tr><tr><td><strong>Time:</strong><br><em>(hh:mm:ss)</em></td><td>" +
		"<input type='text' maxlength='2' style='text-align:center;'" +
		" id='_txtHour' class='smallclassicTxtBox'/> &nbsp;" +
		"<input type='text' maxlength='2' style='text-align:center;'" +
		" id='_txtMinute' class='smallclassicTxtBox'/> &nbsp;" +
		"<input type='text' maxlength='2' style='text-align:center;'" +
		" id='_txtSecond' class='smallclassicTxtBox'/>" +
		"</td></tr></table>";
	return tagTimestampTbl;
}

/*
 * It will initialize the Time stamp control for Year, Month and Year.
 */
function initTimestamp()
{
	var month = ["January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"];

	optind = 0;
	for (i = 1; i <= 31; i++) {
		lstDate.add(new Option(i, i), isIE?optind++:null);
	}

	for (i = 0; i < 12; i++) {
		lstMonth.add(new Option(month[i], i), isIE?i:null);
	}

	optind = 0;
	for (i = STARTYEAR; i <= ENDYEAR; i++) {
		lstYear.add(new Option(i, i), isIE?optind++:null);
	}
}

/*
 * This will enable or disable the Timestamp UI controls based on the Timestamp 
 * event check box value.
 */
function enableTimeStamp()
{
	var bopt;
	if(top.user.isAdmin()) {
		bopt = !$("_chkTrigEnable" + TIMESTAMP_EVENT).checked;
		lstMonth.disabled = bopt;
		lstDate.disabled = bopt;
		lstYear.disabled = bopt;
		txtHour.disabled = bopt;
		txtMinute.disabled = bopt;
		txtSecond.disabled = bopt;
	}
}

/*
 * It will invoke the RPC method to get the service configuration.
 * Once it get data from RPC, response function will be called automatically. 
 */
function getServiceCfg()
{
	var req;			//xmit object to send RPC request with parameters
	req = new xmit.getset({url:"/rpc/getservicecfg.asp", onrcv:getServiceCfgRes,
		status:""});
	req.add("SERVICEBIT", top.CONSTANTS.KVM_SERVICE_ID_BIT);
	req.send();
	delete req;
}

/*
 * This is the response function for getServiceCfg RPC. 
 * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
 * If success, move the response data to the global variable and invoke the 
 * method to load the data value in UI.
 * @param arg object, RPC response data from xmit library
 */
function getServiceCfgRes(arg)
{
	var errstr;		//Error string
	if(arg.HAPI_STATUS != 0) {
		errstr = eLang.getString("common", "STR_CONF_SERVICES_GETVAL");
		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		SERVICE_DATA = WEBVAR_JSONVAR_GETSERVICECFG.WEBVAR_STRUCTNAME_GETSERVICECFG[0];
	}
}