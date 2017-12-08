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

/* Module: Live Web Alerts
 * Version  : 1.0
 * Author   : Chandrasekar.R
 * Email    : chandrasekarr@amiindia.co.in
 * Filename : ./lib/web_alerts.js   Comments: Contains logic for live web alerts
 * Brief    : This library is used to monitor events for a specific time 
 * interval. It will show a small pop up for the user if any new events is 
 * logged into BMC. If the user clicks on the pop up, it will display the 
 * event log description for the events logged in the particular time interval.
 */

var web_alerts =
{
	timer : false,
	lastEventId : 0,
	interval : 30,
	viewCount : 0,
	tblJSON : {},

	/*
	 * Constructor, to initialize the web alert arguments
	 */
	construct : function(intervalSeconds, lastEventId)
	{
		web_alerts.interval = intervalSeconds;
		web_alerts.lastEventId = lastEventId;
		web_alerts.monitor(intervalSeconds);
	},

	/*
	 * This will send the RPC request to check whether any new events is
	 * logged. This method will be called for every particular time interval.
	 * Once it get data from RPC, response function will be called 
	 * automatically.
	 */
	monitor : function()
	{
		var req = new xmit.getset({url:"/rpc/getselentries.asp", 
			onrcv:web_alerts.response, status:"", show_progress:false});
		req.add("WEBVAR_LASTEVENTID",web_alerts.lastEventId);
		req.send();
		delete req;

		web_alerts.stop();
		web_alerts.timer = setTimeout(web_alerts.monitor, 
			web_alerts.interval * 1000);
	},

	/*
	 * This is the response method for the monitor RPC request
	 * Need to check status and HAPI_STATUS, intimate end user if it returns 
	 * non-zero value. If success, move the response data to the global 
	 * variable and invoke the method to load the data value in UI.
	 * Status value, 503 is the return code for Firmware flash.
	 * @param arg object, RPC response data from xmit library
	 */
	response : function(arg)
	{
		if (arg.status == 503) {
			web_alerts.stop();
			deviceInFlashMode();
		} else if(arg.HAPI_STATUS) {
			alert(eLang.getString("common", "STR_IPMI_LIBRARY_ERROR"));
		} else {
			web_alerts.viewCount = 0;
			SELINFO_DATA = WEBVAR_JSONVAR_HL_GETSELENTRIES.WEBVAR_STRUCTNAME_HL_GETSELENTRIES;
			if (SELINFO_DATA.length) {
				web_alerts.reload();
			}
		}
	},

	/*
	 * It will load response data from global variable to the list grid.
	 */
	reload : function()
	{
		var count = 0;
		var eventdesc;
		var type;
		var offset;
		var timestamp;
		var event_seconds;
		var JSONRows = [];
		var m_Max_allowed_offset= [];
		var m_Max_allowed_SensorSpecific_offset = [];

		web_alerts.tblJSON = {cols:[
			{text:eLang.getString("common", "STR_EVENT_LOG_HEAD1"), fieldType:2,
				w:"7%"},
			{text:eLang.getString("common", "STR_EVENT_LOG_HEAD2"), w:"18%"},
			{text:eLang.getString("common", "STR_EVENT_LOG_HEAD3"), w:"21%"},
			{text:eLang.getString("common", "STR_EVENT_LOG_HEAD4"), w:"15%"},
			{text:eLang.getString("common", "STR_EVENT_LOG_HEAD5"), w:"35%"}
			]};

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

		for (j = 0; j < SELINFO_DATA.length; j++) {
			//sensor specific events
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
					/* 0x70-0x7F, OEM Discrete */
				}

				if ((SELINFO_DATA[j].GenID1 >= 0x01) && 
					(SELINFO_DATA[j].GenID1 <= 0x1F) && 
					(SELINFO_DATA[j].SensorType == 0xf)) {
					/* 
					 * If it is a BIOS Post event then look up bios_post_String
					 * using offset and event data2. We are clean event data 2 
					 * should be seen only if this bits indicate event data2 
					 * has something.
					 */

					//Only 0 and 1 table are in Bios_post_event_str.js
					if ((getbits(SELINFO_DATA[j].EventData1, 7, 6) == 0xC0) && 
						((offset >= 0) && (offset <= 2))) {
						/* 
						 * Since SensorType 0xf ,Offset 1 and 2 use the same 
						 * table Entry 
						 */
						if(2 == offset) {
							offset =1;
						}
						eventdesc += "-" + eLang.getString("bios_post_event", 
							offset, getbits(SELINFO_DATA[j].EventData2, 3, 0));
					} else {
						eventdesc += "-" + eLang.getString("common", 
							"STR_UNKNOWN");
					}
				}

				if (getbits(SELINFO_DATA[j].EventDirType,7,7) == 0) {
					eventdesc += " - "+eLang.getString("common", 
						"STR_EVENT_LOG_ASSERT");
				} else {
					eventdesc += " - "+eLang.getString("common", 
						"STR_EVENT_LOG_DEASSERT");
				}
				sensortype = eLang.getString("sensortype", 
					SELINFO_DATA[j].SensorType);

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

			event_seconds = SELINFO_DATA[j].TimeStamp;
			timestamp = getTimeStamp(event_seconds);

			if(web_alerts.lastEventId < SELINFO_DATA[j].RecordID) {
				web_alerts.lastEventId = SELINFO_DATA[j].RecordID;
			}

			JSONRows.push({cells:[
				{text:parseInt(SELINFO_DATA[j].RecordID), 
				value:parseInt(SELINFO_DATA[j].RecordID)},
				{text:timestamp, value:timestamp},
				{text:SELINFO_DATA[j].SensorName, 
				value:SELINFO_DATA[j].SensorName},
				{text:sensortype, value:sensortype},
				{text:eventdesc, value:eventdesc}
				]});
			count++;
		}

		web_alerts.viewCount += count;
		web_alerts.tblJSON.rows = JSONRows;
		web_alerts.notify();
	},

	/*
	 * This method will stop the web alerts monitoring.
	 */
	stop : function()
	{
		if(web_alerts.timer) {
			clearTimeout(web_alerts.timer);
		}
		web_alerts.closeNotification();
	},

	/*
	 * This will display a small pop up notification to the user when new events 
	 * logged into the BMC.
	 */
	notify : function()
	{
		var notifyTable = $("notifyTable");

		if(!notifyTable)
		{
			notifyTable = document.createElement("table");
			notifyTable.cellSpacing="0";
			notifyTable.cellPadding="0";
			notifyTable.border = "0";
			notifyTable.align = "center";
			notifyTable.id = "notifyTable";

			var tr = notifyTable.insertRow(0);

			var left_sector = tr.insertCell(0);
			left_sector.innerHTML = "<img src='../res/left_notify_bar.gif' />";
			var data_area	= tr.insertCell(1);
			data_area.style.background = "url(../res/notify_bar_bg.gif) repeat-x";
			var right_sector = tr.insertCell(2);
			right_sector.innerHTML = "<img src='../res/right_notify_bar.gif' />";

			document.body.appendChild(notifyTable);

			notifyTable.onclick = function()
			{
				if($("descFrame")) {
					if($("descFrame").close) {
						$("descFrame").close();
					}
				}

				web_alerts.display();
				web_alerts.closeNotification();
			}
		}

		var notifyBalloon = notifyTable.rows[0].cells[1];
		notifyBalloon.innerHTML = web_alerts.viewCount + " new event" + 
			(web_alerts.viewCount == 1 ? "" : "s") + " occurred";

		with(notifyTable.style) {
			fontWeight = "bold";
			fontSize = "11px";
			position = "absolute";
			cursor = "pointer";
			color = "#746234";
			left = (document.body.clientWidth/2 - notifyTable.offsetWidth/2) + "px";
			top = menuHolder.offsetHeight + "px";
		}
	},

	/*
	 * This will close the displayed small pop up notification.
	 */
	closeNotification : function()
	{
		if($("notifyTable"))
			$("notifyTable").parentNode.removeChild($("notifyTable"));
	},

	/*
	 * This will display the event log description for the events logged in the 
	 * particular time interval, when the user clicks on the pop up notification. 
	 */
	display : function()
	{
		web_alerts.viewCount = 0;
		var para = document.createElement("div");
		para.innerHTML = "<p>New event entries were logged by the system " +
			"recently.</p>" +
			"<strong>Log Report:</strong>" +
			"<div id='logReportHolder' height='230px'></div>";

		var btns = [];
		btns.push(createButton("web_alert_ok", eLang.getString("common",
			"STR_OK"), web_alerts.informed));
		btns.push(createButton("web_alert_viewall", eLang.getString("common",
			"STR_EVENT_LOG_VIEW"), web_alerts.viewAllEvents));
		web_alerts.descframe = MessageBox("Recent Events", para, btns);

		var logReportTable = listgrid({
			h : "230px",
			w : "98%",
			doAllowNoSelect : true
		});

		logReportTable.clear();
		logReportTable.loadFromJson(web_alerts.tblJSON);
		$("logReportHolder").appendChild(logReportTable.table);
		if (isIE) {
			logReportTable.container.header.style.position = "";
		}
	},

	/*
	 * This method will redirect the user to go to Event log page to view
	 * all the events logged into the BMC.
	 */
	viewAllEvents : function()
	{
		web_alerts.informed();
		top.mainFrame.pageFrame.location.href = "event_log.html";
	},

	/*
	 * This method will close the event log description message box.
	 */
	informed : function()
	{
		web_alerts.descframe.close();
	},

	/*
	 * Destructor, It will stop and clear the web alerts process.
	 */
	destruct : function()
	{
		web_alerts.stop();
		delete SELINFO_DATA;
		delete WEBVAR_JSONVAR_HL_GETSELENTRIES;
		//delete web_alerts;
	}
};

web_alerts.loaded = true;
