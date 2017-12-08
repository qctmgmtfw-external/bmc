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

var MAX_LOG_LEVEL = 8;
var utc_offset = "";
var card_year = 1970;

function doInit()
{
	exposeElms(['_systemLog',
				'_auditLog',
				'_utcOffset',
				'_lblHeader',
				'_filterBy',
				'_logLevel',
				'_logHolder'
				]);

	systemLog.onclick = doSystemLog;
	auditLog.onclick = doAuditLog;
	_begin();
}

function _begin()
{
	optind = 0;
	for (i=1;i<=MAX_LOG_LEVEL;i++) {
		logLevel.add(new Option(eLang.getString("common", "STR_LOG_LEVEL_TYPE" +
			i), i), isIE ? optind++ : null);
	}
	if (top.mainFrame.pageFrame.location.hash) {
		logLevel.value = comboParser(top.mainFrame.pageFrame.location.hash,
			logLevel.id);	//id->id of the selection box
	}

	loadCustomPageElements();
	logLevel.onchange = getSystemLog;

	var tabLastVisit = tabParser(top.mainFrame.pageFrame.location.hash);
	if (tabLastVisit != null) {
		$(tabLastVisit).onclick();
	} else {
		doSystemLog();
	}
}

function doSystemLog()
{
	filterBy.style.display = "";
	logLevel.style.display = "";
	systemLog.style.fontWeight = "bold";
	auditLog.style.fontWeight = "normal";
	getUTCOffset(getUTCOffsetRes);
}

function getUTCOffsetRes(utcOffsetMinutes)
{
	utc_offset = getUTCString(utcOffsetMinutes * 60);		//Global variable used all over the file.
	utcOffset.innerHTML = "<strong class='st'>" + eLang.getString('common',"STR_UTC_OFFSET") + "</strong>";
	utcOffset.innerHTML += eLang.getString('common',"STR_GMT");
	utcOffset.innerHTML += utc_offset;
	utcOffset.innerHTML += ")" + eLang.getString('common','STR_BLANK');

	getDateTime();
}

function getDateTime()
{
	xmit.get({url:"/rpc/getdatetime.asp",onrcv:getDateTimeRes, status:''});
}

function getDateTimeRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr = eLang.getString('common','STR_CONF_DATE_TIME_GETVAL');
		errstr += (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		var tmp_seconds = WEBVAR_JSONVAR_GETDATETIME.WEBVAR_STRUCTNAME_GETDATETIME[0].SECONDS;
		var card_date = new Date(tmp_seconds * 1000);
		card_year = card_date.getFullYear();
	}
	getSystemLog();
}

function getSystemLog()
{
	logTable.clear();
	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common',"STR_SYSTEM_LOG_CNT") +
					"</strong> 0 " + eLang.getString('common',"STR_EVENT_ENTRIES");

	var req = new xmit.getset({url:'/rpc/getsystemlog.asp',onrcv:getSystemLogRes,status:''});
	req.add("LOG_LEVEL",logLevel.value);
	req.send();
	delete req;
}

function getSystemLogRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr =  eLang.getString('common','STR_SYSTEM_EVENT_GETINFO');
		errstr += (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		fillListGrid(WEBVAR_JSONVAR_GETSYSTEMLOG.WEBVAR_STRUCTNAME_GETSYSTEMLOG[0].SYSTEM_LOG, "STR_SYSTEM_LOG_CNT");
	}
}

function doAuditLog()
{
	filterBy.style.display = "none";
	logLevel.style.display = "none";
	systemLog.style.fontWeight = "normal";
	auditLog.style.fontWeight = "bold";
	getAuditLog();
}

function getAuditLog()
{
	logTable.clear();
	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common',"STR_SYSTEM_LOG_CNT") +
					"</strong> 0 " + eLang.getString('common',"STR_EVENT_ENTRIES");

	xmit.get({url:'/rpc/getauditlog.asp',onrcv:getAuditLogRes,status:''});
}

function getAuditLogRes(arg)
{
	if (arg.HAPI_STATUS != 0)
	{
		errstr =  eLang.getString('common','STR_SYSTEM_EVENT_GETINFO');
		errstr += (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}
	else
	{
		fillListGrid(WEBVAR_JSONVAR_GETAUDITLOG.WEBVAR_STRUCTNAME_GETAUDITLOG[0].AUDIT_LOG, "STR_SYSTEM_LOG_CNT");
	}
}

function loadCustomPageElements()
{
	var lgHeight = parent.$("pageFrame").offsetHeight-240;
	if(lgHeight < 45)
		lgHeight = 45;
	logTable = listgrid({
		w : "100%",
		h : (lgHeight)+"px",
		msg : eLang.getString("common", "NO_SAL_STRING"),
		doAllowNoSelect : false
	});

	//add the list grid to the body division
	logHolder.appendChild(logTable.table);

	tblJSON = {cols:[
		{text:eLang.getString("common","STR_LOG_EVENT_ID"), fieldType:2, w:"7%",
			textAlign:"center"},
		{text:eLang.getString("common","STR_LOG_TIMESTAMP"), w:"18%", 
			textAlign:"center"},
		{text:eLang.getString("common","STR_LOG_HOSTNAME"), w:"25%",
			textAlign:"center"},
		{text:eLang.getString("common","STR_LOG_DESCRIPTION"), w:"50%"}
		]};

	logTable.loadFromJson(tblJSON);
}

function fillListGrid(data, str)
{
	var record = data.split("\n");
	var JSONRows = new Array();
	var date_col = 0;

	var event_milliseconds;

	logTable.clear();
	var eventid = 0;
	if (record.length > 0)
	{
		for (i=0; i<record.length; i++)
		{
			var rec_data = record[i].split(' ');
			if(rec_data.length > 4)
			{
				date_col = (rec_data[1].length == 0) ? 2 : 1;
				tmp = rec_data[1].length;
				if((rec_data[date_col + 1].split(':').length == 3) && (rec_data[0].length == 3))
				{
					eventid++;
					timestr = rec_data[0] + eLang.getString('common','STR_EMPTY') + 
								rec_data[date_col] + eLang.getString('common','STR_EMPTY') + 
								card_year + eLang.getString('common','STR_EMPTY') + rec_data[date_col + 1];

					event_milliseconds = Date.parse(timestr);							//Date in milliseconds
					event_milliseconds += (getUTCSeconds(utc_offset) * 1000);

					var EvtDate = new Date(event_milliseconds);
					var disp_month=EvtDate.toString().split(' ')[1];
					timestamp = disp_month + '  ' + EvtDate.getDate();

					var disp_hours =EvtDate.getHours();
					disp_hours =((disp_hours < 10)?"0":"") + disp_hours;
					var disp_mins = EvtDate.getMinutes();
					disp_mins =((disp_mins < 10)?"0":"") + disp_mins;
					var disp_secs = EvtDate.getSeconds();
					disp_secs =((disp_secs < 10)?"0":"") + disp_secs;
					timestamp += '  '+disp_hours+':'+disp_mins+':'+disp_secs;

					hostname = rec_data[date_col + 2];
					desc = "";
					for (j = date_col + 3; j<rec_data.length; j++)
						desc += rec_data[j] + eLang.getString('common','STR_EMPTY');

					try
					{
						JSONRows.push({cells:[
						{text:eventid, value:eventid}, {text:timestamp, value:timestamp},
						{text:hostname, value:hostname}, {text:desc, value:desc}
						]});
					}
					catch(e)
					{
						alert(e);
					}
				}
				else
				{
					//alert (rec_data[3] + "::" + rec_data);
				}
			}
		}
		tblJSON.rows = JSONRows;
		logTable.loadFromJson(tblJSON);
	}

	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common',str) +
						"</strong>" + eventid + eLang.getString('common',"STR_EVENT_ENTRIES");
}
