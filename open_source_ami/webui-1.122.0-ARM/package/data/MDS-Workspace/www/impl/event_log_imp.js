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

var SELINFO_DATA;
var SENSORINFO_DATA;
var FIRSTPAGE = 1;

var bkupPageNumber;
var eventLogTable;
var clearlog = false;
var bmcUTCString;
var clientUTCSeconds;
var m_Max_allowed_offset = [];
var m_Max_allowed_SensorSpecific_offset = [];

var MOVEFIRST = 1;
var MOVEPREVIOUS = 2;
var MOVENEXT = 3;
var MOVELAST = 4;

function doInit()
{
	exposeElms([
		"_eventTypeSel",
		"_sensorName",
		"_lblHeader",
		"_btnFirst",
		"_btnPrevious",
		"_btnNext",
		"_btnLast",
		"_txtPage",
		"_rdoBMCTzone",
		"_rdoClientTzone",
		"_utcOffset",
		"_eventLogHolder",
		"_btnSaveLog",
		"_btnClearLog"]);

	if (top.user.isAdmin()) {
		btnClearLog.onclick = clearSEL;
		btnSaveLog.onclick = saveSEL;
	} else {
		disableActions({id: ["_eventTypeSel", "_sensorName"]});
	}

	btnFirst.onclick = function () {
		movePage(MOVEFIRST);
	}
	btnPrevious.onclick = function () {
		movePage(MOVEPREVIOUS);
	}
	btnNext.onclick = function () {
		movePage(MOVENEXT);
	}
	btnLast.onclick = function () {
		movePage(MOVELAST);
	}

	txtPage.onkeydown = movePageNumber;
	rdoBMCTzone.onclick = refreshEventTzone;
	rdoClientTzone.onclick = refreshEventTzone;

	_begin();
}

function _begin()
{
	fillEventType();
	initializeOffsetValue();
	loadCustomPageElements();
	initializeCustomPageElements();
	getAllSensors();
}

function loadCustomPageElements()
{
	var lgHeight = parent.$("pageFrame").offsetHeight - 250;
	if(lgHeight < 45)
		lgHeight = 45;
	eventLogTable = listgrid({
		w : "100%",
		h : lgHeight + "px",
		msg : eLang.getString("common", "NO_SEL_STRING"),
		doAllowNoSelect : false
	});

	//add the list grid to the body division
	eventLogHolder.appendChild(eventLogTable.table);

	tblJSON = {cols:[
		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD1"), fieldType:2, 
			w:"7%"},
		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD2"), w:"18%"},
		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD3"), w:"23%"},
		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD4"), w:"15%"},
		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD5"), w:"37%"}
		]};

	eventLogTable.loadFromJson(tblJSON);
}

function initializeCustomPageElements()
{
	sensorName.onchange = eventTypeSel.onchange = comboRefresh;

	eventLogTable.ondblclick = function(selectedRow)
	{
		var eventId = getEventId(parseInt(selectedRow.cells[0].innerHTML));
		if(eventId != null) {
			var descFrame = getDescFrame();
			var data = processEventRecordForGraph(SELINFO_DATA[eventId]);

			var eventType = "";
			if (getbits(SELINFO_DATA[eventId].EventDirType,7,7) == 0) {
				eventType = "Asserted";
			} else {
				eventType = "Deasserted";
			}

			var heading = document.createElement("h3");
			heading.innerHTML = SELINFO_DATA[eventId].SensorName + 
				"<font class='grey'> - " + eventType + "</font>";

			var description = document.createElement("p");
			description.innerHTML = "This event was " + eventType+" on " + 
				selectedRow.cells[1].innerHTML;
			description.innerHTML += "<br/><br/>";
			description.innerHTML += "<em>" + data.str + "</em>";

			var eventDeasserted = null;
			var deassertEvent = false;

			for(var i=eventId; i<SELINFO_DATA.length; i++) {
				if(SELINFO_DATA[i].SensorName == SELINFO_DATA[eventId].SensorName
					&& getbits(SELINFO_DATA[i].EventDirType,6,0) == getbits(SELINFO_DATA[eventId].EventDirType,6,0)
					&& getbits(SELINFO_DATA[i].EventData1,3,0) == getbits(SELINFO_DATA[eventId].EventData1,3,0)
					&& SELINFO_DATA[i].GenID1 == SELINFO_DATA[eventId].GenID1) {
					if (getbits(SELINFO_DATA[i].EventDirType,7,7) != 0) {
						eventDeasserted = SELINFO_DATA[i];
						break;
					} else {
						deassertEvent = true;
					}
				}
			}

			if(deassertEvent) {
				var durTxt = "";
				if(eventDeasserted) {
					var duration = SELINFO_DATA[eventId].TimeStamp - eventDeasserted.TimeStamp;
					var fDur = getFormattedDate(duration);
					durTxt = "This event was deasserted after "+fDur;
				} else {
					durTxt = "This event is not yet deasserted";
				}
				description.innerHTML += "<br/><br/>";
				description.innerHTML += durTxt;
			}

			descFrame.appendChild(heading);
			descFrame.appendChild(description);
		}
	}
}

function getAllSensors()
{
	xmit.get({url:"/rpc/getallsensors.asp",onrcv:getAllSensorsRes,status:""});
}

function getEventId(id)
{
	for(var eid=0; eid < SELINFO_DATA.length; eid++) {
		if(SELINFO_DATA[eid].RecordID == id) {
			return eid;
		}
	}
	return null;
}

var sensorNameType = [];
function getAllSensorsRes(arg)
{
	var i = 0;	//loop counter
	var index;
	if(arg.HAPI_STATUS == 0) {
		SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
		sensorName.add(new Option("All Sensors", i), isIE ? i : null);
		for(i = 1; i <= SENSORINFO_DATA.length; i++) {
			sensorName.add(new Option(SENSORINFO_DATA[i-1].SensorName, i),
				isIE ? i : null);
			sensorNameType[i-1] = SENSORINFO_DATA[i-1].SensorName;
			sensorNameCount[i-1] = 0;
		}

		if(HTTP_GET_VARS["sid"]) {
			index = HTTP_GET_VARS["sid"];
			index++;
			sensorName.value = index;
		}
		if (top.mainFrame.pageFrame.location.hash) {
			eventTypeSel.value = comboParser(top.mainFrame.pageFrame.location.hash,
				eventTypeSel.id);	//id->id of the selection box
			sensorName.value = comboParser(top.mainFrame.pageFrame.location.hash,
				sensorName.id);
		}
	}
	getUTCOffset(getUTCOffsetRes);
}

function getUTCOffsetRes(utcOffsetMinutes)
{
	var clientDate = new Date();
	bmcUTCString = getUTCString(utcOffsetMinutes * 60);
	clientUTCSeconds = -(clientDate.getTimezoneOffset() * 60);	//JS Date object method
	refreshEventTzone();
	getAllSELEntries();
}

function getAllSELEntries()
{
	disablePageButtons();
	xmit.get({url:"/rpc/getallselentries.asp", onrcv:getSELRes, status:""});
}

function getSELRes(arg)
{
	if (arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_EVENT_LOG_GETVAL");
		errstr +=  (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		SELINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSELENTRIES.WEBVAR_STRUCTNAME_HL_GETALLSELENTRIES;
		if (!SELINFO_DATA.length && !clearlog) {
			alert(eLang.getString("common", "NO_SEL_STRING"));
			loadEmptySELEntries();
		}
		clearlog = false;
		initializeAllEvents();
		initializePageRange();
	}
}

function toggleBMCClient(visibleStyle, hiddenStyle)
{
	var theRules = new Array();
	var searchStyle = "";
	for (i=0;i<document.styleSheets.length;i++) {
		theRules = document.styleSheets[i].cssRules ? 
			document.styleSheets[i].cssRules : document.styleSheets[i].rules;
		for (j=0;j<theRules.length;j++) {
			searchStyle = theRules[j].cssText ? theRules[j].cssText : 
				theRules[j].selectorText;
			if (searchStyle.indexOf(visibleStyle) != -1) {
				theRules[j].style.display = "";
			}
			if (searchStyle.indexOf(hiddenStyle) != -1) {
				theRules[j].style.display = "none";
			}
		}
	}
}

function refreshEventTzone()
{
	utcOffset.innerHTML = "<strong class='st'>" + eLang.getString("common", 
		"STR_UTC_OFFSET") + "</strong>";
	utcOffset.innerHTML += eLang.getString("common","STR_GMT");
	if (rdoBMCTzone.checked) {
		toggleBMCClient("toggleBMC", "toggleClient");
		utcOffset.innerHTML += bmcUTCString;
	} else if (rdoClientTzone.checked) {
		toggleBMCClient("toggleClient", "toggleBMC");
		utcOffset.innerHTML += getUTCString(clientUTCSeconds);
	}
	utcOffset.innerHTML += ")" + eLang.getString("common","STR_BLANK");
}

function refreshEvents(startPos, endPos, count)
{
	var eventtype = 0;
	var sensortype;
	var eventdesc;
	var type;
	var offset;
	var JSONRows = [];
	var eventTimeStamp;		//Event TimeStamp in seconds
	var bmcUTCSeconds;		//BMC UTC Offset value in seconds
	var bmcTimestamp;		//BMC TimeStamp in string
	var clientTimestamp;	//Client TimeStamp in string

	showWait(true, "Populating");
	eventLogTable.clear();
	for (j = startPos; j <= endPos; j++) {
		if (j >= SELINFO_DATA.length) {
			break;
		}

		if (parseInt(eventTypeSel.value) != ALL_EVENTS) {
		//Check for Event Type list box value
			eventtype = ALL_EVENTS;
			if (SELINFO_DATA[j].RecordType >= 0x0 && 
				SELINFO_DATA[j].RecordType <= 0xBF) {
			//System Event Records
				eventtype = SYSTEM_EVT;
				if (parseInt(eventTypeSel.value) != SYSTEM_EVT) {
					if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 1) {
						if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x0) && 
							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x0F)) {
							eventdesc = "System Software ID - BIOS Generated";
							eventtype = BIOS;
						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x10) && 
							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x1F)) {
							eventdesc = "System Software ID - SMI Handler";
							eventtype = SMI;
						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x20) && 
							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x2F)) {
							eventdesc = "System Software ID - System Management Software";
							eventtype = SMS;
						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x30) && 
							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x3F)) {
							eventdesc = "System Software ID - OEM";
							eventtype = SYSTEM_OEM;
						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x40) && 
							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x46)) {
							eventdesc = "System Software ID - Remote Console software";
							eventtype = RCS;
						} else if (getbitsval(SELINFO_DATA[j].GenID1, 7, 1) == 0x47) {
							eventdesc = "System Software ID - Terminal Mode Remote Console software";
							eventtype = TMRCS;
						}
					}
				}
			} else if ((SELINFO_DATA[j].RecordType >= 0xC0) &&
				(SELINFO_DATA[j].RecordType <= 0xFF)){
			//C0h-DFh = OEM timestamped
			//E0h-FFh = OEM non-timestamped
				eventtype = OEM_EVT;
			}
			if (parseInt(eventTypeSel.value) != eventtype) {
			//if not matches skips the record
				continue;
			}
		}

		if(parseInt(sensorName.value) != 0) {
		//Check for Sensor Name list box value
			if(sensorNameType[parseInt(sensorName.value)-1] != 
				SELINFO_DATA[j].SensorName) {
			//if not matches skips the record
				continue;
			}
		}

		if (SELINFO_DATA[j].RecordType >= 0x0 && 
			SELINFO_DATA[j].RecordType <= 0xBF) {
			//Range reserved for standard SEL Record Types

			//7-bit Event/Reading Type Code Range
			type = getbits(SELINFO_DATA[j].EventDirType, 6, 0);
			if (type == 0x0) {
				//Event/Reading Type unspecified
			} else if (type == 0x6F) {
				//Sensor-specific Discrete
				//If event type is 0x6F then read from sensor-specific table
				offset = getbits(SELINFO_DATA[j].EventData1, 3, 0);
				if(m_Max_allowed_SensorSpecific_offset[SELINFO_DATA[j].SensorType] >= offset) {
					eventdesc = eLang.getString("sensor_specific_event", 
						SELINFO_DATA[j].SensorType, offset);
				} else {
					eventdesc = eLang.getString("common", "INVALID_OFFSET");
				}
			} else if ((type >= 0x01) && (type <= 0x0C)) {
				//0x01-Threshold based, 0x02-0x0C Generic Discrete
				offset = getbits(SELINFO_DATA[j].EventData1, 3, 0);
				if(m_Max_allowed_offset[type] >= offset) {
					eventdesc = eLang.getString("event", type, offset);
				} else {
					eventdesc = eLang.getString("common", "INVALID_OFFSET");
				}
			} else {
				/* 0x70-0x7F,OEM Discrete */
			}

			//if it is a BIOS Post event then
			if ((SELINFO_DATA[j].GenID1 >= 0x01) && 
				(SELINFO_DATA[j].GenID1 <= 0x1F) && 
				(SELINFO_DATA[j].SensorType == 0xf)) {
				/* look up bios_post_String using offset and evtdata2
				we are clean event data 2 should be seen only if this bits 
				indicate evtdata2 has something */

				//Only 0 and 1 table are in Bios_post_event_str.js
				if ((getbits(SELINFO_DATA[j].EventData1,7,6) == 0xC0) && 
					((offset >= 0) && (offset <= 2))) {
					/* Since SensorType 0xf ,Offset 1 and 2 use the same table Entry */
					if(2 == offset) {
						offset =1;
					}
					eventdesc += "-" + eLang.getString("bios_post_event", 
						offset, getbits(SELINFO_DATA[j].EventData2, 3, 0));
				} else {
					eventdesc += "-" + eLang.getString("common", "STR_UNKNOWN");
				}
			}

			if (getbits(SELINFO_DATA[j].EventDirType,7,7) == 0) {
				eventdesc += " - "+eLang.getString("common", "STR_EVENT_LOG_ASSERT");
			} else {
				eventdesc += " - "+eLang.getString("common", "STR_EVENT_LOG_DEASSERT");
			}
			sensortype = eLang.getString("sensortype", SELINFO_DATA[j].SensorType);

		} else if (SELINFO_DATA[j].RecordType >= 0xC0 && 
			SELINFO_DATA[j].RecordType <= 0xDF) {
			//Range reserved for timestamped OEM SEL records.
			eventdesc = "OEM timestamped";
			sensortype = "OEM Record";
		} else if (SELINFO_DATA[j].RecordType >= 0xE0 && 
			SELINFO_DATA[j].RecordType <= 0xFF) {
			//Range reserved for non-timestamped OEM SEL records.
			eventdesc = "OEM non-timestamped";
			sensortype = "OEM Record";
		}

		var eventTimeStamp = SELINFO_DATA[j].TimeStamp;		//Event TimeStamp in Seconds
		var bmcUTCSeconds = getUTCSeconds(bmcUTCString);	//BMC UTC Offset value in seconds
		var strBmcTimeStamp=getTimeStamp(eventTimeStamp);
		if (strBmcTimeStamp.substring(6,10) == "1970") {
			var bmcTimestamp = "<span class='toggleBMC'>" + 
				eLang.getString("common", "STR_PRE_INIT_TIMESTAMP") + 
				"</span>";
		} else {
			var bmcTimestamp = "<span class='toggleBMC'>" + 
				strBmcTimeStamp + "</span>";
		}
		var strClientTimeStamp = getTimeStamp(eventTimeStamp - 
			bmcUTCSeconds + clientUTCSeconds);
		if (strClientTimeStamp.substring(6,10) == "1970") {
			var clientTimestamp = "<span class='toggleClient'>" + 
				eLang.getString("common", "STR_PRE_INIT_TIMESTAMP") + 
				"</span>";
		} else {
			var clientTimestamp = "<span class='toggleClient'>" + 
				getTimeStamp(eventTimeStamp - bmcUTCSeconds + 
				clientUTCSeconds) + "</span>";
		}

		JSONRows.push({cells:[
			{text:parseInt(SELINFO_DATA[j].RecordID), value:parseInt(SELINFO_DATA[j].RecordID)},
			{text:bmcTimestamp + clientTimestamp, value:bmcTimestamp + clientTimestamp},
			{text:SELINFO_DATA[j].SensorName, value:SELINFO_DATA[j].SensorName},
			{text:sensortype, value:sensortype},
			{text:eventdesc, value:eventdesc}
			]});
	}

	tblJSON.rows = JSONRows;
	eventLogTable.loadFromJson(tblJSON);

	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString("common", 
		"STR_EVENT_LOG_CNT") + "</strong>" + count + eLang.getString("common", 
		"STR_EVENT_ENTRIES") + ", " + Math.ceil(count / 100) + 
		eLang.getString("common", "STR_EVENT_LOG_PAGES") + 
		eLang.getString("common", "STR_BLANK");

	if(top.user.isAdmin()) {
		btnClearLog.disabled = (SELINFO_DATA.length) ? false : true;
	}
}

function loadEmptySELEntries()
{
	disablePageButtons();
	txtPage.value = "";
	tblJSON.rows = [];
	eventLogTable.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + 
		eLang.getString("common", "STR_EVENT_LOG_CNT") + "</strong>0" + 
		eLang.getString("common", "STR_EVENT_ENTRIES") + 
		eLang.getString("common", "STR_BLANK");
}

function comboRefresh()
{
	var sensorIndex;
	showWait(true, eLang.getString("common", "STR_WAIT"));
	switch (parseInt(eventTypeSel.value)) {
	case ALL_EVENTS:
	case SYSTEM_EVT:
		sensorName.disabled = false;
		if (parseInt(sensorName.value) == ALL_SENSORS) {
			if (eventTypeCount[eventTypeSel.value] != 0) {
				disablePageButtons();
				refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
					eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
					eventTypeCount[eventTypeSel.value]);
				bkupPageNumber = FIRSTPAGE
				txtPage.value = FIRSTPAGE;
				enablePageButtons(eventTypeCount[eventTypeSel.value]);
			} else {
				loadEmptySELEntries();
			}
		} else {
			//sensorNameType array value starts with 0.
			sensorIndex = parseInt(sensorName.value) - 1;
			if (sensorNameCount[sensorIndex] != 0) {
				disablePageButtons();
				refreshEvents(sensorNamePage[sensorIndex][FIRSTPAGE].startIndex, 
					sensorNamePage[sensorIndex][FIRSTPAGE].endIndex,
					sensorNameCount[sensorIndex]);
				bkupPageNumber = FIRSTPAGE
				txtPage.value = FIRSTPAGE;
				enablePageButtons(sensorNameCount[sensorIndex]);
			} else {
				loadEmptySELEntries();
			}
		}
		break;
	default:
		sensorName.disabled = true;
		sensorName.value = 0;
		if (eventTypeCount[eventTypeSel.value] != 0) {
			disablePageButtons();
			refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
				eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
				eventTypeCount[eventTypeSel.value]);
			bkupPageNumber = FIRSTPAGE
			txtPage.value = FIRSTPAGE;
			enablePageButtons(eventTypeCount[eventTypeSel.value]);
		} else {
			loadEmptySELEntries();
		}
		break;
	}
}

function clearSELRes(arg)
{
	if (arg.HAPI_STATUS) {
		errstr = eLang.getString("common", "STR_EVENT_LOG_CLEARLOG");
		errstr +=  (eLang.getString("common", "STR_IPMI_ERROR") + 
			GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	} else {
		clearlog = true;
		parent.web_alerts.lastEventId = 0;
		alert(eLang.getString("common", "STR_EVENT_LOG_CLEAR_SUCCESS"));
		getAllSELEntries();
	}
}

function clearSEL()
{
	if (top.user.isAdmin()) {
		if (!SELINFO_DATA.length) {
			alert(eLang.getString("common", "NO_SEL_STRING"));
			btnClearLog.disabled = true;
			return;
		}

		if (confirm(eLang.getString("common", "STR_EVENT_LOG_CLEAR_CONFIRM"))) {
			xmit.get({url:"/rpc/clearsel.asp", onrcv:clearSELRes, status:"", 
				timeout:60});
		}
	} else {
		alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
	}
}

function saveSEL()
{
	if (!SELINFO_DATA.length ) {
		alert(eLang.getString("common", "NO_SEL_STRING"));
	} else {
		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
			if(arg.HAPI_STATUS == 0) {
				if (top.user.isAdmin()) {
					xmit.get({url:"/rpc/saveAllSELEntries.asp", onrcv:saveAllSELEntriesRes, status:""});
				} else {
					alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
				}
			} else {
				parent.gLogout = 1;
				top.document.cookie = "SessionExpired=true;path=/";
				parent.location.href = "login.html";
			}
		},evalit:false});
	}
}

function saveAllSELEntriesRes(arg)
{
	if(arg.HAPI_STATUS == 0) {
		window.location.href='/SELLog';
	} else {
		alert(eLang.getString("event_log", "EVENT_LOG_SAVEEVENTS_ERROR"));
	}
}
function movePage(navCtrl)
{
	var pgcnt;
	disablePageButtons();
	showWait(true, eLang.getString("common", "STR_WAIT"));
	if (parseInt(sensorName.value) != ALL_SENSORS) {
		//sensorNameType array value starts with 0.
		sensorIndex = parseInt(sensorName.value) - 1;
		pgcnt = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
		switch (navCtrl) {
		case MOVEFIRST:
			refreshEvents(sensorNamePage[sensorIndex][FIRSTPAGE].startIndex, 
				sensorNamePage[sensorIndex][FIRSTPAGE].endIndex,
				sensorNameCount[sensorIndex]);
			bkupPageNumber = FIRSTPAGE;
			txtPage.value = FIRSTPAGE;
			break;
		case MOVEPREVIOUS:
			bkupPageNumber = parseInt(bkupPageNumber) - 1;
			refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
				sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
				sensorNameCount[sensorIndex]);
			txtPage.value = bkupPageNumber;
			break;
		case MOVENEXT:
			bkupPageNumber = parseInt(bkupPageNumber) + 1;
			refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
				sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
				sensorNameCount[sensorIndex]);
			txtPage.value = bkupPageNumber;
			break;
		case MOVELAST:
			refreshEvents(sensorNamePage[sensorIndex][pgcnt].startIndex, 
				sensorNamePage[sensorIndex][pgcnt].endIndex,
				sensorNameCount[sensorIndex]);
			bkupPageNumber = pgcnt;
			txtPage.value = pgcnt;
			break;
		}
		enablePageButtons(sensorNameCount[sensorIndex]);
	} else {
		pgcnt = Math.ceil(eventTypeCount[eventTypeSel.value] / EVENT_PAGE_SIZE);
		switch (navCtrl) {
		case MOVEFIRST:
			refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
				eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
				eventTypeCount[eventTypeSel.value]);
			bkupPageNumber = FIRSTPAGE;
			txtPage.value = FIRSTPAGE;
			break;
		case MOVEPREVIOUS:
			bkupPageNumber = parseInt(bkupPageNumber) - 1;
			refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
				eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
				eventTypeCount[eventTypeSel.value]);
			txtPage.value = bkupPageNumber;
			break;
		case MOVENEXT:
			bkupPageNumber = parseInt(bkupPageNumber) + 1;
			refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
				eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
				eventTypeCount[eventTypeSel.value]);
			txtPage.value = bkupPageNumber;
			break;
		case MOVELAST:
			refreshEvents(eventTypePage[eventTypeSel.value][pgcnt].startIndex, 
				eventTypePage[eventTypeSel.value][pgcnt].endIndex,
				eventTypeCount[eventTypeSel.value]);
			bkupPageNumber = pgcnt;
			txtPage.value = pgcnt;
			break;
		}
		enablePageButtons(eventTypeCount[eventTypeSel.value]);
	}
}

function movePageNumber(e)
{
	var keycode;
	var pgcnt;
	if (window.event) { //IE
		keycode = window.event.keyCode;
	} else if (e.which) { //Netscape/Firefox/Opera
		keycode = e.which
	}
	if(keycode == 13) { //13-Enter keycode
		if (parseInt(sensorName.value) != ALL_SENSORS) {
			pgcnt = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
		} else {
			pgcnt = Math.ceil(eventTypeCount[eventTypeSel.value] / EVENT_PAGE_SIZE);
		}
		if (eVal.isnumstr(txtPage.value, FIRSTPAGE, pgcnt)) {
			disablePageButtons();
			bkupPageNumber = parseInt(txtPage.value);
			if (parseInt(sensorName.value) != ALL_SENSORS) {
				//sensorNameType array value starts with 0.
				sensorIndex = parseInt(sensorName.value) - 1;
				refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
					sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
					sensorNameCount[sensorIndex]);
				enablePageButtons(sensorNameCount[sensorIndex]);
			} else {
				refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
					eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
					eventTypeCount[eventTypeSel.value]);
				enablePageButtons(eventTypeCount[eventTypeSel.value]);
			}
		} else {
			alert (eLang.getString("common", "STR_INVALID_PAGENO") + 
				eLang.getString("common", "STR_HELP_INFO"));
			txtPage.value = bkupPageNumber;
		}
	}
}

function enablePageButtons(count)
{
	var pgcnt = Math.ceil(count / EVENT_PAGE_SIZE);
	if (SELINFO_DATA.length != 0) {
		if (parseInt(txtPage.value) != FIRSTPAGE) {
			btnFirst.disabled = false;
			btnPrevious.disabled = false;
		}
		if (parseInt(txtPage.value) != pgcnt) {
			btnNext.disabled = false;
			btnLast.disabled = false;
		}
		if (FIRSTPAGE != pgcnt) {
			txtPage.disabled = false;
		}
	}
}

function disablePageButtons()
{
	btnFirst.disabled = true;
	btnPrevious.disabled = true;
	btnNext.disabled = true;
	btnLast.disabled = true;
	txtPage.disabled = true;
}

var EVENT_PAGE_SIZE = 100;
var MAX_EVENT_TYPE = 9;	//0 to 8
var ALL_SENSORS = 0;
var ALL_EVENTS = 0;
var SYSTEM_EVT = 1;
var OEM_EVT = 2;
var BIOS = 3;
var SMI = 4;
var SMS = 5;
var SYSTEM_OEM = 6;
var RCS = 7;
var TMRCS = 8;

var eventTypeCount = [];
var eventTypePage = [];

var sensorNameCount = [];
var sensorNamePage = [];

function fillEventType()
{
	var i;	//loop counter
	eventTypeSel.innerHTML = "";
	for (i = 0; i < MAX_EVENT_TYPE; i++) {
		eventTypeSel.add(new Option(eLang.getString("common", 
			"STR_EVENT_LOG_TYPE" + i), i), isIE ? i : null);
		eventTypeCount[i] = 0;
	}
}

function initializePageRange()
{
	var sensorIndex;
	try {
	for (j = 0; j < SELINFO_DATA.length; j++) {
		if (SELINFO_DATA[j].RecordType >= 0x0 && 
			SELINFO_DATA[j].RecordType <= 0xBF) {
			//System Event Records
			initializeEventRecords(SYSTEM_EVT, j);
			/*
			 * The following events are filtered under System Event Records
			 * And it is sorted based on its Sensor's Name.
			 */
			if (SELINFO_DATA[j].SensorName != "Unknown") {
				sensorIndex = sensorNameType.indexOf(SELINFO_DATA[j].SensorName);
				if (sensorIndex != -1) {
					initializeSensorEvents(sensorIndex, j);
				}
			}

			/*
			 * The following events are filtered undered System Software ID.
			 * And it is sorted based on its various types. Refer System 
			 * Software IDs table in IPMI specification
			 */
			if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 1) {
				if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x0) && 
					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x0F)) {
					//System Software ID - BIOS Generated
					initializeEventRecords(BIOS, j);
				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x10) && 
					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x1F)) {
					//System Software ID - SMI Handler
					initializeEventRecords(SMI, j);
				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x20) && 
					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x2F)) {
					//System Software ID - System Management Software
					initializeEventRecords(SMS, j);
				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x30) && 
					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x3F)) {
					//System Software ID - OEM
					initializeEventRecords(SYSTEM_OEM, j);
				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x40) && 
					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x46)) {
					//System Software ID - Remote Console software
					initializeEventRecords(RCS, j);
				} else if (getbitsval(SELINFO_DATA[j].GenID1, 7, 1) == 0x47) {
					//System Software ID - Terminal Mode Remote Console software
					initializeEventRecords(TMRCS, j);
				}
			}
		} else if ((SELINFO_DATA[j].RecordType >= 0xC0) &&
			(SELINFO_DATA[j].RecordType <= 0xFF)){
			//C0h-DFh = OEM timestamped
			//E0h-FFh = OEM non-timestamped
			initializeEventRecords(OEM_EVT, j);
		}
	}
	} catch(e){alert(e);}
}

function initializeAllEvents()
{
	try{
	if (SELINFO_DATA.length != 0) {
		var pgcnt = Math.ceil(SELINFO_DATA.length / EVENT_PAGE_SIZE);
		eventTypeCount[ALL_EVENTS] = SELINFO_DATA.length;
		eventTypePage[ALL_EVENTS] = [];
		for (i = 0; i < pgcnt; i++) {
			eventTypePage[ALL_EVENTS][i+1] = {};
			eventTypePage[ALL_EVENTS][i+1].startIndex = (i * EVENT_PAGE_SIZE);
			eventTypePage[ALL_EVENTS][i+1].endIndex = (i * EVENT_PAGE_SIZE) + 99;
		}
	
		refreshEvents(eventTypePage[ALL_EVENTS][FIRSTPAGE].startIndex, 
			eventTypePage[ALL_EVENTS][FIRSTPAGE].endIndex,
			eventTypeCount[ALL_EVENTS]);
		bkupPageNumber = FIRSTPAGE;
		txtPage.value = FIRSTPAGE;
		enablePageButtons(eventTypeCount[ALL_EVENTS]);
	} else {
		loadEmptySELEntries();
	}
	} catch(e){alert(e);}
}

function initializeSensorEvents(sensorIndex, eventIndex)
{
	try {
	var pageNo = 0;
	sensorNameCount[sensorIndex] += 1;
	if (sensorNameCount[sensorIndex] == 1) {
		sensorNamePage[sensorIndex] = [];
	}
	pageNo = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
	if ((sensorNameCount[sensorIndex] % EVENT_PAGE_SIZE) == 1) {
		sensorNamePage[sensorIndex][pageNo] = {};
		sensorNamePage[sensorIndex][pageNo].startIndex = eventIndex;
		sensorNamePage[sensorIndex][pageNo].endIndex = eventIndex;
	} else {
		sensorNamePage[sensorIndex][pageNo].endIndex = eventIndex;
	}
	} catch(e){alert(e);}
}

function initializeEventRecords(eventType, eventIndex)
{
	try {
	var pageNo = 0;
	eventTypeCount[eventType] += 1;
	if (eventTypeCount[eventType] == 1) {
		eventTypePage[eventType] = [];
	}
	pageNo = Math.ceil(eventTypeCount[eventType] / EVENT_PAGE_SIZE);
	if ((eventTypeCount[eventType] % EVENT_PAGE_SIZE) == 1) {
		eventTypePage[eventType][pageNo] = {};
		eventTypePage[eventType][pageNo].startIndex = eventIndex;
		eventTypePage[eventType][pageNo].endIndex = eventIndex;
	} else {
		eventTypePage[eventType][pageNo].endIndex = eventIndex;
	}
	} catch(e){alert(e);}
}

function initializeOffsetValue()
{
	/* 
	 * Please refer the IPMI specification. This array contain max allowed 
	 * offset for particular generic Event/Reading Type for 0x1 to 0x0c.
	 */
	m_Max_allowed_offset[0] = 0x0;
	m_Max_allowed_offset[1] = 0x0b;
	m_Max_allowed_offset[2] = 0x3;
	m_Max_allowed_offset[3] = 0x2;
	m_Max_allowed_offset[4] = 0x2;
	m_Max_allowed_offset[5] = 0x2;
	m_Max_allowed_offset[6] = 0x2;
	m_Max_allowed_offset[7] = 0x8;
	m_Max_allowed_offset[8] = 0x2;
	m_Max_allowed_offset[9] = 0x2;
	m_Max_allowed_offset[10] = 0x8;
	m_Max_allowed_offset[11] = 0x7;
	m_Max_allowed_offset[12] = 0x3;

	/* 
	 * Please refer the IPMI specification. This array contain max allowed 
	 * offset for particular generic Event/Reading Type for 0x6f
	 */
	m_Max_allowed_SensorSpecific_offset[5] = 6;
	m_Max_allowed_SensorSpecific_offset[6] = 5;
	m_Max_allowed_SensorSpecific_offset[7] = 10;
	m_Max_allowed_SensorSpecific_offset[8] = 6;
	m_Max_allowed_SensorSpecific_offset[9] = 7;
	m_Max_allowed_SensorSpecific_offset[12] = 8;
	m_Max_allowed_SensorSpecific_offset[15] = 2;
	m_Max_allowed_SensorSpecific_offset[16] = 5;
	m_Max_allowed_SensorSpecific_offset[17] = 7;
	m_Max_allowed_SensorSpecific_offset[18] = 5;
	m_Max_allowed_SensorSpecific_offset[19] = 10;
	m_Max_allowed_SensorSpecific_offset[20] = 4;
	m_Max_allowed_SensorSpecific_offset[25] = 0;
	m_Max_allowed_SensorSpecific_offset[29] = 4;
	m_Max_allowed_SensorSpecific_offset[30] = 4;
	m_Max_allowed_SensorSpecific_offset[31] = 6;
	m_Max_allowed_SensorSpecific_offset[32] = 1;
	m_Max_allowed_SensorSpecific_offset[33] = 9;
	m_Max_allowed_SensorSpecific_offset[34] = 13;
	m_Max_allowed_SensorSpecific_offset[35] = 8;
	m_Max_allowed_SensorSpecific_offset[36] = 3;
	m_Max_allowed_SensorSpecific_offset[37] = 2;
	m_Max_allowed_SensorSpecific_offset[39] = 1;
	m_Max_allowed_SensorSpecific_offset[40] = 3;
	m_Max_allowed_SensorSpecific_offset[41] = 2;
	m_Max_allowed_SensorSpecific_offset[42] = 1;
	m_Max_allowed_SensorSpecific_offset[43] = 7;
	m_Max_allowed_SensorSpecific_offset[44] = 7;
}
