--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Tue Sep 20 19:40:36 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Tue Sep 20 18:21:52 2011
@@ -26,12 +26,20 @@
 
 var SELINFO_DATA;
 var SENSORINFO_DATA;
+var FIRSTPAGE = 1;
+
+var bkupPageNumber;
 var eventLogTable;
 var clearlog = false;
 var bmcUTCString;
 var clientUTCSeconds;
-var m_Max_allowed_offset= [];
+var m_Max_allowed_offset = [];
 var m_Max_allowed_SensorSpecific_offset = [];
+
+var MOVEFIRST = 1;
+var MOVEPREVIOUS = 2;
+var MOVENEXT = 3;
+var MOVELAST = 4;
 
 function doInit()
 {
@@ -39,36 +47,46 @@
 		"_eventTypeSel",
 		"_sensorName",
 		"_lblHeader",
+		"_btnFirst",
+		"_btnPrevious",
+		"_btnNext",
+		"_btnLast",
+		"_txtPage",
 		"_rdoBMCTzone",
 		"_rdoClientTzone",
 		"_utcOffset",
 		"_eventLogHolder",
-		"_clearLog"]);
-
-	if(top.user.isAdmin()) {
-		clearLog.onclick = clearSEL;
+		"_btnClearLog"]);
+
+	if (top.user.isAdmin()) {
+		btnClearLog.onclick = clearSEL;
 	} else {
 		disableActions({id: ["_eventTypeSel", "_sensorName"]});
 	}
 
+	btnFirst.onclick = function () {
+		movePage(MOVEFIRST);
+	}
+	btnPrevious.onclick = function () {
+		movePage(MOVEPREVIOUS);
+	}
+	btnNext.onclick = function () {
+		movePage(MOVENEXT);
+	}
+	btnLast.onclick = function () {
+		movePage(MOVELAST);
+	}
+
+	txtPage.onkeydown = movePageNumber;
 	rdoBMCTzone.onclick = refreshEventTzone;
 	rdoClientTzone.onclick = refreshEventTzone;
 
 	_begin();
 }
-/*
-window.onresize= function() 
-{
-	showWait(true, eLang.getString("common","STR_WAIT"));
-	eventLogHolder.innerHTML = "";
-	//eventLogTable.clear();
-	loadCustomPageElements();
-	refreshEvents();
-}
-*/
+
 function _begin()
 {
-	fillSensorType();
+	fillEventType();
 	initializeOffsetValue();
 	loadCustomPageElements();
 	initializeCustomPageElements();
@@ -181,6 +199,7 @@
 	return null;
 }
 
+var sensorNameType = [];
 function getAllSensorsRes(arg)
 {
 	var i = 0;	//loop counter
@@ -191,6 +210,8 @@
 		for(i = 1; i <= SENSORINFO_DATA.length; i++) {
 			sensorName.add(new Option(SENSORINFO_DATA[i-1].SensorName, i),
 				isIE ? i : null);
+			sensorNameType[i-1] = SENSORINFO_DATA[i-1].SensorName;
+			sensorNameCount[i-1] = 0;
 		}
 
 		if(HTTP_GET_VARS["sid"]) {
@@ -219,6 +240,7 @@
 
 function getAllSELEntries()
 {
+	disablePageButtons();
 	xmit.get({url:"/rpc/getallselentries.asp", onrcv:getSELRes, status:""});
 }
 
@@ -233,9 +255,11 @@
 		SELINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSELENTRIES.WEBVAR_STRUCTNAME_HL_GETALLSELENTRIES;
 		if (!SELINFO_DATA.length && !clearlog) {
 			alert(eLang.getString("common", "NO_SEL_STRING"));
+			loadEmptySELEntries();
 		}
 		clearlog = false;
-		refreshEvents();
+		initializeAllEvents();
+		initializePageRange();
 	}
 }
 
@@ -274,9 +298,8 @@
 	utcOffset.innerHTML += ")" + eLang.getString("common","STR_BLANK");
 }
 
-function refreshEvents()
-{
-	var count = 0;
+function refreshEvents(startPos, endPos, count)
+{
 	var eventtype = 0;
 	var sensortype;
 	var eventdesc;
@@ -288,50 +311,63 @@
 	var bmcTimestamp;		//BMC TimeStamp in string
 	var clientTimestamp;	//Client TimeStamp in string
 
+	showWait(true, "Populating");
 	eventLogTable.clear();
-
-	for (j = 0; j < SELINFO_DATA.length; j++) {
-		if (parseInt(eventTypeSel.value) != 0) {
+	for (j = startPos; j <= endPos; j++) {
+		if (j >= SELINFO_DATA.length) {
+			break;
+		}
+
+		if (parseInt(eventTypeSel.value) != ALL_EVENTS) {
 		//Check for Event Type list box value
-			eventtype = 0;
-			if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 0) {
-				eventdesc = "Slave Address";
-				eventtype = 1;
-			} else if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 1) {
-				if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x0) && 
-					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x0F)) {
-					eventdesc = "System Software ID - BIOS Generated";
-					eventtype = 2;
-				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x10) && 
-					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x1F)) {
-					eventdesc = "System Software ID - SMI Handler";
-					eventtype = 3;
-				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x20) && 
-					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x2F)) {
-					eventdesc = "System Software ID - System Management Software";
-					eventtype = 4;
-				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x30) && 
-					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x3F)) {
-					eventdesc = "System Software ID - OEM";
-					eventtype = 5;
-				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x40) && 
-					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x46)) {
-					eventdesc = "System Software ID - Remote Console software";
-					eventtype = 6;
-				} else if (getbitsval(SELINFO_DATA[j].GenID1, 7, 1) == 0x47) {
-					eventdesc = "System Software ID - Terminal Mode Remote Console software";
-					eventtype = 7;
+			eventtype = ALL_EVENTS;
+			if (SELINFO_DATA[j].RecordType >= 0x0 && 
+				SELINFO_DATA[j].RecordType <= 0xBF) {
+			//System Event Records
+				eventtype = SYSTEM_EVT;
+				if (parseInt(eventTypeSel.value) != SYSTEM_EVT) {
+					if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 1) {
+						if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x0) && 
+							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x0F)) {
+							eventdesc = "System Software ID - BIOS Generated";
+							eventtype = BIOS;
+						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x10) && 
+							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x1F)) {
+							eventdesc = "System Software ID - SMI Handler";
+							eventtype = SMI;
+						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x20) && 
+							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x2F)) {
+							eventdesc = "System Software ID - System Management Software";
+							eventtype = SMS;
+						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x30) && 
+							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x3F)) {
+							eventdesc = "System Software ID - OEM";
+							eventtype = SYSTEM_OEM;
+						} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x40) && 
+							(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x46)) {
+							eventdesc = "System Software ID - Remote Console software";
+							eventtype = RCS;
+						} else if (getbitsval(SELINFO_DATA[j].GenID1, 7, 1) == 0x47) {
+							eventdesc = "System Software ID - Terminal Mode Remote Console software";
+							eventtype = TMRCS;
+						}
+					}
 				}
-			}
-		}
-		if (parseInt(eventTypeSel.value) != eventtype) {
-		//if not matches skips the record
-			continue;
+			} else if ((SELINFO_DATA[j].RecordType >= 0xC0) &&
+				(SELINFO_DATA[j].RecordType <= 0xFF)){
+			//C0h-DFh = OEM timestamped
+			//E0h-FFh = OEM non-timestamped
+				eventtype = OEM_EVT;
+			}
+			if (parseInt(eventTypeSel.value) != eventtype) {
+			//if not matches skips the record
+				continue;
+			}
 		}
 
 		if(parseInt(sensorName.value) != 0) {
 		//Check for Sensor Name list box value
-			if(SENSORINFO_DATA[parseInt(sensorName.value)-1].SensorName != 
+			if(sensorNameType[parseInt(sensorName.value)-1] != 
 				SELINFO_DATA[j].SensorName) {
 			//if not matches skips the record
 				continue;
@@ -368,7 +404,7 @@
 				/* 0x70-0x7F,OEM Discrete */
 			}
 
-			//if it is a BIOS  Post event then
+			//if it is a BIOS Post event then
 			if ((SELINFO_DATA[j].GenID1 >= 0x01) && 
 				(SELINFO_DATA[j].GenID1 <= 0x1F) && 
 				(SELINFO_DATA[j].SensorType == 0xf)) {
@@ -439,7 +475,6 @@
 			{text:sensortype, value:sensortype},
 			{text:eventdesc, value:eventdesc}
 			]});
-		count++;
 	}
 
 	tblJSON.rows = JSONRows;
@@ -447,18 +482,79 @@
 
 	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString("common", 
 		"STR_EVENT_LOG_CNT") + "</strong>" + count + eLang.getString("common", 
-		"STR_EVENT_ENTRIES") + eLang.getString("common", "STR_BLANK");
+		"STR_EVENT_ENTRIES") + ", " + Math.ceil(count / 100) + 
+		eLang.getString("common", "STR_EVENT_LOG_PAGES") + 
+		eLang.getString("common", "STR_BLANK");
 
 	if(top.user.isAdmin()) {
-		clearLog.disabled = (SELINFO_DATA.length) ? false : true;
-	}
+		btnClearLog.disabled = (SELINFO_DATA.length) ? false : true;
+	}
+}
+
+function loadEmptySELEntries()
+{
+	disablePageButtons();
+	txtPage.value = "";
+	tblJSON.rows = [];
+	eventLogTable.loadFromJson(tblJSON);
+	lblHeader.innerHTML = "<strong class='st'>" + 
+		eLang.getString("common", "STR_EVENT_LOG_CNT") + "</strong>0" + 
+		eLang.getString("common", "STR_EVENT_ENTRIES") + 
+		eLang.getString("common", "STR_BLANK");
 }
 
 function comboRefresh()
 {
-	showWait(true, eLang.getString("common","STR_WAIT"));
-	//setTimeout("refreshEvents()",1000);
-	refreshEvents();
+	var sensorIndex;
+	showWait(true, eLang.getString("common", "STR_WAIT"));
+	switch (parseInt(eventTypeSel.value)) {
+	case ALL_EVENTS:
+	case SYSTEM_EVT:
+		sensorName.disabled = false;
+		if (parseInt(sensorName.value) == ALL_SENSORS) {
+			if (eventTypeCount[eventTypeSel.value] != 0) {
+				disablePageButtons();
+				refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
+					eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
+					eventTypeCount[eventTypeSel.value]);
+				bkupPageNumber = FIRSTPAGE
+				txtPage.value = FIRSTPAGE;
+				enablePageButtons(eventTypeCount[eventTypeSel.value]);
+			} else {
+				loadEmptySELEntries();
+			}
+		} else {
+			//sensorNameType array value starts with 0.
+			sensorIndex = parseInt(sensorName.value) - 1;
+			if (sensorNameCount[sensorIndex] != 0) {
+				disablePageButtons();
+				refreshEvents(sensorNamePage[sensorIndex][FIRSTPAGE].startIndex, 
+					sensorNamePage[sensorIndex][FIRSTPAGE].endIndex,
+					sensorNameCount[sensorIndex]);
+				bkupPageNumber = FIRSTPAGE
+				txtPage.value = FIRSTPAGE;
+				enablePageButtons(sensorNameCount[sensorIndex]);
+			} else {
+				loadEmptySELEntries();
+			}
+		}
+		break;
+	default:
+		sensorName.disabled = true;
+		sensorName.value = 0;
+		if (eventTypeCount[eventTypeSel.value] != 0) {
+			disablePageButtons();
+			refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
+				eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
+				eventTypeCount[eventTypeSel.value]);
+			bkupPageNumber = FIRSTPAGE
+			txtPage.value = FIRSTPAGE;
+			enablePageButtons(eventTypeCount[eventTypeSel.value]);
+		} else {
+			loadEmptySELEntries();
+		}
+		break;
+	}
 }
 
 function clearSELRes(arg)
@@ -481,7 +577,7 @@
 	if (top.user.isAdmin()) {
 		if (!SELINFO_DATA.length) {
 			alert(eLang.getString("common", "NO_SEL_STRING"));
-			clearLog.disabled = true;
+			btnClearLog.disabled = true;
 			return;
 		}
 
@@ -494,14 +590,300 @@
 	}
 }
 
-function fillSensorType()
+function movePage(navCtrl)
+{
+	var pgcnt;
+	disablePageButtons();
+	showWait(true, eLang.getString("common", "STR_WAIT"));
+	if (parseInt(sensorName.value) != ALL_SENSORS) {
+		//sensorNameType array value starts with 0.
+		sensorIndex = parseInt(sensorName.value) - 1;
+		pgcnt = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
+		switch (navCtrl) {
+		case MOVEFIRST:
+			refreshEvents(sensorNamePage[sensorIndex][FIRSTPAGE].startIndex, 
+				sensorNamePage[sensorIndex][FIRSTPAGE].endIndex,
+				sensorNameCount[sensorIndex]);
+			bkupPageNumber = FIRSTPAGE;
+			txtPage.value = FIRSTPAGE;
+			break;
+		case MOVEPREVIOUS:
+			bkupPageNumber = parseInt(bkupPageNumber) - 1;
+			refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
+				sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
+				sensorNameCount[sensorIndex]);
+			txtPage.value = bkupPageNumber;
+			break;
+		case MOVENEXT:
+			bkupPageNumber = parseInt(bkupPageNumber) + 1;
+			refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
+				sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
+				sensorNameCount[sensorIndex]);
+			txtPage.value = bkupPageNumber;
+			break;
+		case MOVELAST:
+			refreshEvents(sensorNamePage[sensorIndex][pgcnt].startIndex, 
+				sensorNamePage[sensorIndex][pgcnt].endIndex,
+				sensorNameCount[sensorIndex]);
+			bkupPageNumber = pgcnt;
+			txtPage.value = pgcnt;
+			break;
+		}
+		enablePageButtons(sensorNameCount[sensorIndex]);
+	} else {
+		pgcnt = Math.ceil(eventTypeCount[eventTypeSel.value] / EVENT_PAGE_SIZE);
+		switch (navCtrl) {
+		case MOVEFIRST:
+			refreshEvents(eventTypePage[eventTypeSel.value][FIRSTPAGE].startIndex, 
+				eventTypePage[eventTypeSel.value][FIRSTPAGE].endIndex,
+				eventTypeCount[eventTypeSel.value]);
+			bkupPageNumber = FIRSTPAGE;
+			txtPage.value = FIRSTPAGE;
+			break;
+		case MOVEPREVIOUS:
+			bkupPageNumber = parseInt(bkupPageNumber) - 1;
+			refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
+				eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
+				eventTypeCount[eventTypeSel.value]);
+			txtPage.value = bkupPageNumber;
+			break;
+		case MOVENEXT:
+			bkupPageNumber = parseInt(bkupPageNumber) + 1;
+			refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
+				eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
+				eventTypeCount[eventTypeSel.value]);
+			txtPage.value = bkupPageNumber;
+			break;
+		case MOVELAST:
+			refreshEvents(eventTypePage[eventTypeSel.value][pgcnt].startIndex, 
+				eventTypePage[eventTypeSel.value][pgcnt].endIndex,
+				eventTypeCount[eventTypeSel.value]);
+			bkupPageNumber = pgcnt;
+			txtPage.value = pgcnt;
+			break;
+		}
+		enablePageButtons(eventTypeCount[eventTypeSel.value]);
+	}
+}
+
+function movePageNumber(e)
+{
+	var keycode;
+	var pgcnt;
+	if (window.event) { //IE
+		keycode = window.event.keyCode;
+	} else if (e.which) { //Netscape/Firefox/Opera
+		keycode = e.which
+	}
+	if(keycode == 13) { //13-Enter keycode
+		if (parseInt(sensorName.value) != ALL_SENSORS) {
+			pgcnt = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
+		} else {
+			pgcnt = Math.ceil(eventTypeCount[eventTypeSel.value] / EVENT_PAGE_SIZE);
+		}
+		if (eVal.isnumstr(txtPage.value, FIRSTPAGE, pgcnt)) {
+			disablePageButtons();
+			bkupPageNumber = parseInt(txtPage.value);
+			if (parseInt(sensorName.value) != ALL_SENSORS) {
+				//sensorNameType array value starts with 0.
+				sensorIndex = parseInt(sensorName.value) - 1;
+				refreshEvents(sensorNamePage[sensorIndex][bkupPageNumber].startIndex, 
+					sensorNamePage[sensorIndex][bkupPageNumber].endIndex,
+					sensorNameCount[sensorIndex]);
+				enablePageButtons(sensorNameCount[sensorIndex]);
+			} else {
+				refreshEvents(eventTypePage[eventTypeSel.value][bkupPageNumber].startIndex, 
+					eventTypePage[eventTypeSel.value][bkupPageNumber].endIndex,
+					eventTypeCount[eventTypeSel.value]);
+				enablePageButtons(eventTypeCount[eventTypeSel.value]);
+			}
+		} else {
+			alert (eLang.getString("common", "STR_INVALID_PAGENO") + 
+				eLang.getString("common", "STR_HELP_INFO"));
+			txtPage.value = bkupPageNumber;
+		}
+	}
+}
+
+function enablePageButtons(count)
+{
+	var pgcnt = Math.ceil(count / EVENT_PAGE_SIZE);
+	if (SELINFO_DATA.length != 0) {
+		if (parseInt(txtPage.value) != FIRSTPAGE) {
+			btnFirst.disabled = false;
+			btnPrevious.disabled = false;
+		}
+		if (parseInt(txtPage.value) != pgcnt) {
+			btnNext.disabled = false;
+			btnLast.disabled = false;
+		}
+		if (FIRSTPAGE != pgcnt) {
+			txtPage.disabled = false;
+		}
+	}
+}
+
+function disablePageButtons()
+{
+	btnFirst.disabled = true;
+	btnPrevious.disabled = true;
+	btnNext.disabled = true;
+	btnLast.disabled = true;
+	txtPage.disabled = true;
+}
+
+var EVENT_PAGE_SIZE = 100;
+var MAX_EVENT_TYPE = 9;	//0 to 8
+var ALL_SENSORS = 0;
+var ALL_EVENTS = 0;
+var SYSTEM_EVT = 1;
+var OEM_EVT = 2;
+var BIOS = 3;
+var SMI = 4;
+var SMS = 5;
+var SYSTEM_OEM = 6;
+var RCS = 7;
+var TMRCS = 8;
+
+var eventTypeCount = [];
+var eventTypePage = [];
+
+var sensorNameCount = [];
+var sensorNamePage = [];
+
+function fillEventType()
 {
 	var i;	//loop counter
 	eventTypeSel.innerHTML = "";
-	for (i = 0; i < 8; i++) {
+	for (i = 0; i < MAX_EVENT_TYPE; i++) {
 		eventTypeSel.add(new Option(eLang.getString("common", 
 			"STR_EVENT_LOG_TYPE" + i), i), isIE ? i : null);
-	}
+		eventTypeCount[i] = 0;
+	}
+}
+
+function initializePageRange()
+{
+	var sensorIndex;
+	try {
+	for (j = 0; j < SELINFO_DATA.length; j++) {
+		if (SELINFO_DATA[j].RecordType >= 0x0 && 
+			SELINFO_DATA[j].RecordType <= 0xBF) {
+			//System Event Records
+			initializeEventRecords(SYSTEM_EVT, j);
+			/*
+			 * The following events are filtered under System Event Records
+			 * And it is sorted based on its Sensor's Name.
+			 */
+			if (SELINFO_DATA[j].SensorName != "Unknown") {
+				sensorIndex = sensorNameType.indexOf(SELINFO_DATA[j].SensorName);
+				if (sensorIndex != -1) {
+					initializeSensorEvents(sensorIndex, j);
+				}
+			}
+
+			/*
+			 * The following events are filtered undered System Software ID.
+			 * And it is sorted based on its various types. Refer System 
+			 * Software IDs table in IPMI specification
+			 */
+			if (getbits(SELINFO_DATA[j].GenID1, 0, 0) == 1) {
+				if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x0) && 
+					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x0F)) {
+					//System Software ID - BIOS Generated
+					initializeEventRecords(BIOS, j);
+				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x10) && 
+					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x1F)) {
+					//System Software ID - SMI Handler
+					initializeEventRecords(SMI, j);
+				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x20) && 
+					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x2F)) {
+					//System Software ID - System Management Software
+					initializeEventRecords(SMS, j);
+				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x30) && 
+					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x3F)) {
+					//System Software ID - OEM
+					initializeEventRecords(SYSTEM_OEM, j);
+				} else if ((getbitsval(SELINFO_DATA[j].GenID1, 7, 1) >= 0x40) && 
+					(getbitsval(SELINFO_DATA[j].GenID1, 7, 1) <= 0x46)) {
+					//System Software ID - Remote Console software
+					initializeEventRecords(RCS, j);
+				} else if (getbitsval(SELINFO_DATA[j].GenID1, 7, 1) == 0x47) {
+					//System Software ID - Terminal Mode Remote Console software
+					initializeEventRecords(TMRCS, j);
+				}
+			}
+		} else if ((SELINFO_DATA[j].RecordType >= 0xC0) &&
+			(SELINFO_DATA[j].RecordType <= 0xFF)){
+			//C0h-DFh = OEM timestamped
+			//E0h-FFh = OEM non-timestamped
+			initializeEventRecords(OEM_EVT, j);
+		}
+	}
+	} catch(e){alert(e);}
+}
+
+function initializeAllEvents()
+{
+	try{
+	if (SELINFO_DATA.length != 0) {
+		var pgcnt = Math.ceil(SELINFO_DATA.length / EVENT_PAGE_SIZE);
+		eventTypeCount[ALL_EVENTS] = SELINFO_DATA.length;
+		eventTypePage[ALL_EVENTS] = [];
+		for (i = 0; i < pgcnt; i++) {
+			eventTypePage[ALL_EVENTS][i+1] = {};
+			eventTypePage[ALL_EVENTS][i+1].startIndex = (i * EVENT_PAGE_SIZE);
+			eventTypePage[ALL_EVENTS][i+1].endIndex = (i * EVENT_PAGE_SIZE) + 99;
+		}
+	
+		refreshEvents(eventTypePage[ALL_EVENTS][FIRSTPAGE].startIndex, 
+			eventTypePage[ALL_EVENTS][FIRSTPAGE].endIndex,
+			eventTypeCount[ALL_EVENTS]);
+		bkupPageNumber = FIRSTPAGE;
+		txtPage.value = FIRSTPAGE;
+		enablePageButtons(eventTypeCount[ALL_EVENTS]);
+	} else {
+		loadEmptySELEntries();
+	}
+	} catch(e){alert(e);}
+}
+
+function initializeSensorEvents(sensorIndex, eventIndex)
+{
+	try {
+	var pageNo = 0;
+	sensorNameCount[sensorIndex] += 1;
+	if (sensorNameCount[sensorIndex] == 1) {
+		sensorNamePage[sensorIndex] = [];
+	}
+	pageNo = Math.ceil(sensorNameCount[sensorIndex] / EVENT_PAGE_SIZE);
+	if ((sensorNameCount[sensorIndex] % EVENT_PAGE_SIZE) == 1) {
+		sensorNamePage[sensorIndex][pageNo] = {};
+		sensorNamePage[sensorIndex][pageNo].startIndex = eventIndex;
+		sensorNamePage[sensorIndex][pageNo].endIndex = eventIndex;
+	} else {
+		sensorNamePage[sensorIndex][pageNo].endIndex = eventIndex;
+	}
+	} catch(e){alert(e);}
+}
+
+function initializeEventRecords(eventType, eventIndex)
+{
+	try {
+	var pageNo = 0;
+	eventTypeCount[eventType] += 1;
+	if (eventTypeCount[eventType] == 1) {
+		eventTypePage[eventType] = [];
+	}
+	pageNo = Math.ceil(eventTypeCount[eventType] / EVENT_PAGE_SIZE);
+	if ((eventTypeCount[eventType] % EVENT_PAGE_SIZE) == 1) {
+		eventTypePage[eventType][pageNo] = {};
+		eventTypePage[eventType][pageNo].startIndex = eventIndex;
+		eventTypePage[eventType][pageNo].endIndex = eventIndex;
+	} else {
+		eventTypePage[eventType][pageNo].endIndex = eventIndex;
+	}
+	} catch(e){alert(e);}
 }
 
 function initializeOffsetValue()
