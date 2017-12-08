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

// File Name  : configure_pef_imp.js
// Brief      : This implementation is to configure PEF entries.
// Author Name:

var pefListTable;
var tblJSON;
var PEFCFG_DATA;
var SENSORINFO_DATA;
var g_pefindex;
var gSensorTypeCodes;

function doInit() 
{
	exposeElms(['_eventFilter',
				'_alertPolicy',
				'_lanDests',
				'_lblHeader',
				'_lanChannelLabel',
				'_lanChannel',
				'_listGridHolder',
				'_sendAlert',
				'_addBtn',
				'_modBtn',
				'_delBtn'
				]);

	if (top.user.isOperator())
	{
		disableActions();
	}
	else if (!top.user.isAdmin())
	{
		alert(eLang.getString('common','STR_PERMISSION_DENIED'));
		location.href = 'dashboard.html';
		return;
	}

	eventFilter.onclick = doPEFCfg;
	alertPolicy.onclick = doAlertPolicy;
	lanDests.onclick = doLANDestination;
	gSensorTypeCodes = eLang.getString('common',"STR_SENSOR_TYPES");

	_begin();
}

function _begin()
{
	var tabLastVisit = tabParser(top.mainFrame.pageFrame.location.hash);
	if (tabLastVisit != null) {
		$(tabLastVisit).onclick();
	} else {
		doPEFCfg();
	}
}

function clearPEFUI()
{
	eventFilter.style.fontWeight = 'normal';
	alertPolicy.style.fontWeight = 'normal';
	lanDests.style.fontWeight = 'normal';
	listGridHolder.innerHTML = '';
	lblHeader.innerHTML = '';
	lanChannelLabel.style.display = 'none';
	lanChannel.style.display = 'none';
	sendAlert.style.display = 'none';
	addBtn.onclick = function(){};
	modBtn.onclick = function(){};
	delBtn.onclick = function(){};
	sendAlert.onclick = function(){};
}

function doPEFCfg()
{
	clearPEFUI();
	eventFilter.style.fontWeight = 'bold';
	loadPEFPageElements();
	initPEFAction();
	getAllPEFCfg();
}

function doAlertPolicy()
{
	clearPEFUI();
	alertPolicy.style.fontWeight = 'bold';
	loadAlertPolicyElements();
	getAllPolicyCfg();
	initPolicyAction();
}

function doLANDestination()
{
	clearPEFUI();
	lanDests.style.fontWeight = 'bold';
	sendAlert.style.display = '';
	loadLANDestElements();
	initLANDestAction();
}

function initPEFAction()
{
	addBtn.onclick = doAddPEF;
	modBtn.onclick = doModifyPEF;
	delBtn.onclick = doDeletePEF;
}

function loadPEFPageElements()
{
	var height = parent.$('pageFrame').offsetHeight - 270;
	height = (height > 45) ? height : 45;
	pefListTable = listgrid
	({
		w				: '100%',
		h				: height + 'px',
		doAllowNoSelect : false
	});
	listGridHolder.appendChild(pefListTable.table);

	try
	{
		tblJSON =
		{
			cols:[
				{text:eLang.getString('common',"STR_CONF_PEF_ID"), fieldName:'pef_id', fieldType:2, w:'10%', textAlign:'center'},
				{text:eLang.getString('common',"STR_CONF_PEF_CONFIGURATION"), fieldName:'sensor_type', w:'20%', textAlign:'center'},
				{text:eLang.getString('common',"STR_CONF_PEF_ACTION"), fieldName:'pef_action', w:'25%', textAlign:'center'},
				{text:eLang.getString('common',"STR_CONF_PEF_EVENT_SEVERITY"), fieldName:'event_trigger', w:'20%', textAlign:'center'},
				{text:eLang.getString('common',"STR_CONF_PEF_SENSOR_NAME"), fieldName:'sensor_name', w:'25%', textAlign:'center'}
			]};

		pefListTable.loadFromJson(tblJSON);
	}
	catch(e)
	{
		alert(e);
	}

	if(top.user.isAdmin())
	{
		pefListTable.ontableselect = function ()
		{
			disableButtons();
			if(this.selected.length)
			{
				var pefevent = pefListTable.getRow(pefListTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
				if (pefevent == "~")
					addBtn.disabled = false;
				else
				{
					modBtn.disabled = false;
					delBtn.disabled = false;
				}
			}
		}

		pefListTable.ondblclick = function()
		{
			var pefevent = pefListTable.getRow(pefListTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','');
			if (pefevent == "~")
				doAddPEF();
			else
				doModifyPEF();
		}
	}
}

function disableButtons()
{
	addBtn.disabled = true;
	modBtn.disabled = true;
	delBtn.disabled = true;
	sendAlert.disabled = true;
}

function getAllPEFCfg()
{
	disableButtons();
	xmit.get({url:"/rpc/getallpefcfg.asp",onrcv:getAllPEFCfgRes, status:''});
}

function getAllSensorsInfo()
{
	xmit.get({url:"/rpc/getallsensors.asp",onrcv:getAllSensorsInfoRes, status:''});
}

function getAllPEFCfgRes (arg)
{
	if (GET_ERROR_CODE(arg.HAPI_STATUS) == 0xD4)	//Insufficient privilege level
	{
		alert (eLang.getString('common',"STR_PERMISSION_DENIED"));
		location.href = 'dashboard.html';
	}
	else if (arg.HAPI_STATUS)
	{
		errstr =  eLang.getString('common','STR_CONF_PEF_GETINFO');
		errstr += (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		PEFCFG_DATA = WEBVAR_JSONVAR_HL_GETPEFTABLE.WEBVAR_STRUCTNAME_HL_GETPEFTABLE;
		loadPEFTable();
	}
	getAllSensorsInfo();
}

function loadPEFTable()
{
	var filtertodisplay;
	var actiontodisplay;
	var severitytodisplay;
	var sensornametodisplay;

	var JSONRows = new Array();
	pefListTable.clear();

	var tPEFCnt = 0;
	for (i=0;i<PEFCFG_DATA.length;i++)
//	for (i=0;i<20;i++)
	{
		// Use ~ char to indicate free slot so it will sort alphabetically
		filtertodisplay = '~';
		sensornametodisplay = "~";
		eventtodisplay = "~";
		severitytodisplay = '~';
		actiontodisplay = getPEFActionString(PEFCFG_DATA[i].EvtFilterAction);
		if(actiontodisplay != '~')
		{
			tPEFCnt++;
			filtertodisplay = (getbitsval(PEFCFG_DATA[i].FilterConfig,7,7) == 1) ? eLang.getString('common','STR_ENABLED') : eLang.getString('common','STR_DISABLED');
			severitytodisplay = getEventSeverity(PEFCFG_DATA[i].EventSeverity);
			sensornametodisplay = PEFCFG_DATA[i].SensorName;
		}

		try
		{
			JSONRows.push({cells:[
				{text:(i+1), value:(i+1)},
				{text:filtertodisplay, value:filtertodisplay},
				{text:actiontodisplay, value:actiontodisplay},
				{text:severitytodisplay, value:severitytodisplay},
				{text:sensornametodisplay, value:sensornametodisplay}
			]});
		}
		catch(e)
		{
			alert(e);
		}
	}

	tblJSON.rows = JSONRows;
	pefListTable.loadFromJson(tblJSON);
	lblHeader.innerHTML = "<strong class='st'>" + eLang.getString('common','STR_CONF_PEF_CNT') + 
						"</strong>" + tPEFCnt + eLang.getString('common','STR_BLANK');
}

function getAllSensorsInfoRes(arg)
{
	if (arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_SENSOR_GETVAL")
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
	}
}

/* This function returns corresponding PEF Action string, using pefaction code as input 
and performing bitwise operations */
function getPEFActionString(pefaction)
{
	var actionstr = "~";
	if (getbitsval(pefaction,0,0))
		actionstr = " [Alert] ";
	if (getbitsval(pefaction,1,1))
		actionstr += " [Power Down] ";
	else if (getbitsval(pefaction,2,2))
		actionstr += " [Reset] ";
	else if (getbitsval(pefaction,3,3))
		actionstr += " [Power Cycle] ";

	return actionstr;
}

/* This function returns corresponding SensorType string using sensortype code as input based on IPMI Spec */
/*function getSensorTypeString(sensortype)
{
	var sensortypestr = "";
	switch(sensortype)
	{
		case 0x0:
			sensortypestr = eLang.getString('common','STR_NONE');
			break;
		case 0xFF:
			sensortypestr = eLang.getString('common','STR_SENSOR_TYPES', 0);
			break;
		default:
			sensortypestr = eLang.getString('common', 'STR_SENSOR_TYPES', sensortype);
			//sensortypestr = gSensorTypeCodes[sensortype];
			break;
	}
	return sensortypestr;
}

function getEventString(sensortype, evtoffsetmask)
{
	var eventstr = "";
	if (evtoffsetmask == 0xFFFF)
		eventstr = eLang.getString('common',"STR_ANY");
	else
	{
		for (var i=0; i<16; i++)
		{
			if (getbitsval(evtoffsetmask,i,i))
			{
				try 
				{
					if (sensortype > 0 && sensortype < 5)				//Threshold Sensors (range is 0x1 to 0x4)
						eventstr += '[' + eLang.getString('event',1,i) + ']';
					else if (sensortype > 4 && sensortype < 45)			//Discrete Sensors (range is 0x5 to 0x2C)
						eventstr += '[' + eLang.getString('sensor_specific_event',sensortype,i) + ']';
					else 												//OEM Sensors
						eventstr += '[OEM Sensor-Event]'; 
				}
				catch (e)
				{
					break;
				}
			}
		}
	}
	if (eventstr == "")
		eventstr = eLang.getString('common',"STR_NONE");
	return eventstr;
}*/

function getEventSeverity(eventseverity)
{
	for (var i=0; i<6; i++)
		if (getbitsval(eventseverity,i,i) == 1)
			return (eLang.getString('common','STR_PEF_SEVERITY_' + i));
	return (eLang.getString('common','STR_PEF_SEVERITY'));
}

function doAddPEF()
{
	var index;
	index = parseInt(pefListTable.getRow(pefListTable.selected[0]).cells[0].innerHTML.replace('&nbsp;','').replace(' ',''));
	top.gPEFInfo = PEFCFG_DATA[index-1];
	location.href='add_mod_pef.html?pef_oper=1&pef_id=' + index;			//pef_oper=1 for ADD operation
}

function doModifyPEF()
{
	var index;
	index = parseInt(pefListTable.getRow(pefListTable.selected[0]).cells[0].innerHTML.replace('&nbsp;','').replace(' ',''));
	top.gPEFInfo = PEFCFG_DATA[index-1];
	location.href='add_mod_pef.html?pef_oper=2&pef_id=' + index;			//pef_oper=2 for MODIFY operation
}

function doDeletePEF()
{
 	if ((pefListTable.selected.length != 1) || 
		(pefListTable.selected[0].cells[0] == undefined) || 
		(pefListTable.selected[0].cells[0] == null))
	{
		alert (eLang.getString('common','STR_CONF_PEF_ERR1'));
		disableButtons();
	}
	else if (pefListTable.getRow(pefListTable.selected[0]).cells[1].innerHTML.replace('&nbsp;','').replace(' ','') == "~")
	{
		alert (eLang.getString('common','STR_CONF_PEF_ERR2'));
	}
	else
		if (confirm(eLang.getString('common','STR_CONFIRM_DELETE')))
			deletePEF(parseInt(pefListTable.getRow(pefListTable.selected[0]).cells[0].innerHTML));
}

function deletePEF(pefid)
{
	var req = new xmit.getset({url:"/rpc/configurepef.asp", onrcv:deletePEFRes, status:''});
	req.add ("WEBVAR_ENTRYINDEX", pefid);
	req.add ("WEBVAR_FILTERCFG", 0);
	req.add ("WEBVAR_EVTFILTERACTION", 0); 
	req.add ("WEBVAR_POLICYNUM", 0); 
	req.add ("WEBVAR_SEVERITY", 0);
	req.add ("WEBVAR_GENID1", 0xFF);
	req.add ("WEBVAR_GENID2", 0xFF); 
	req.add ("WEBVAR_SENSORTYPE", 0); 
	req.add ("WEBVAR_SENSORNUM", 0); 
	req.add ("WEBVAR_EVTDATA1OFFSETMASK", 0);
	req.add ("WEBVAR_EVTTRIGGER", 0);
	req.add ("WEBVAR_EVTDATA1AND", 0);
	req.add ("WEBVAR_EVTDATA1COMP1", 0);
	req.add ("WEBVAR_EVTDATA1COMP2", 0);
	req.add ("WEBVAR_EVTDATA2AND", 0);
	req.add ("WEBVAR_EVTDATA2COMP1", 0);
	req.add ("WEBVAR_EVTDATA2COMP2", 0);
	req.add ("WEBVAR_EVTDATA3AND", 0);
	req.add ("WEBVAR_EVTDATA3COMP1", 0);
	req.add ("WEBVAR_EVTDATA3COMP2", 0);
	req.send();
	delete req; 
}

function deletePEFRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_CONF_PEF_DELINFO");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		alert (eLang.getString('common','STR_CONF_PEF_DELETE_SUCCESS'));
		getAllPEFCfg();
	}
}
