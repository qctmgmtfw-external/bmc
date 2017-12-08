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

// File Name  : add_mod_pef
// Brief      : 
// Author Name:

var PEFCFG_DATA;
var SENSORINFO_DATA;
var sensorTypeCodes;
var pef_oper;
//var owner_id = 0xFF;
//var owner_lun = 0xFF;

function doInit()
{
	exposeElms(['_pageTitle',
				'_pageDesc',
				'_pefID',
				'_enablePEF',
				'_pefAlert',
				'_powerAction',
				'_policyNo',
				'_eventSeverity',
				'_rawType',
				'_genID1',
				'_genID2',
				'_slaveType',
				'_softwareType',
				'_slave_software',
				'_channelNo',
				'_ipmbDevice',
				'_sensorType',
				'_sensorName',
				'_eventOpt',
				'_sensorEventRow',
				'_sensorEvent',
				'_eventTrigger',
				'_event1ANDMask',
				'_event1Compare1',
				'_event1Compare2',
				'_event2ANDMask',
				'_event2Compare1',
				'_event2Compare2',
				'_event3ANDMask',
				'_event3Compare1',
				'_event3Compare2',
				'_saveBtn',
				'_resetBtn',
				'_cancelBtn']);

	pef_oper = eExt.parseURLvars("pef_oper");
	
	if (pef_oper == 1)			//Add PEF Entry
	{
		pageTitle.innerHTML = eLang.getString('common','STR_ADD_PEF_TITLE');
		pageDesc.innerHTML = eLang.getString('common','STR_ADD_PEF_DESC');
		saveBtn.value = eLang.getString('common','STR_ADD');
	}
	else if(pef_oper == 2)		//Modify PEF Entry
	{
		pageTitle.innerHTML = eLang.getString('common','STR_MODIFY_PEF_TITLE');
		pageDesc.innerHTML = eLang.getString('common','STR_MODIFY_PEF_DESC');
		saveBtn.value = eLang.getString('common','STR_MODIFY');
	}

	if(top.user.isAdmin())
	{
		saveBtn.onclick = validatePEFCfg;
		resetBtn.onclick = reloadPEFCfg;
		cancelBtn.onclick = exitPEFCfg;
		sensorType.onchange = function()
		{
			fillSensorNames();
			fillEventOptions();
			loadSensorEvents();
		};

		sensorName.onchange = function ()
		{
			fillEventOptions();
			loadSensorEvents();
		};
		
		eventOpt.onchange = loadSensorEvents;
		rawType.onchange = toggleRawData;
		slaveType.onclick = toggleSlaveSoftware;
		softwareType.onclick = toggleSlaveSoftware;
		slave_software.onblur = calculateGenID;
		channelNo.onchange = calculateGenID;
		ipmbDevice.onchange = calculateGenID;
	}
	else
	{
		alert (eLang.getString('common','STR_PERMISSION_DENIED'));
		location.href = '/page/configure_pef.html';
	}

	PEFCFG_DATA = top.gPEFInfo;
	sensorEventRow.style.display = "none";
	sensorTypeCodes = eLang.getString('common',"STR_SENSOR_TYPES");
	pefAlert.checked = true;
	pefAlert.disabled = true;
	_begin();
}

function _begin()
{
	getAllSensorsInfo();
	initPowerAction();
	initPolicyNumber();
	initEventSeverity();
	toggleRawData();
	initChannelNo();
	initIPMBDevices();
}

function getAllSensorsInfo()
{
	xmit.get({url:"/rpc/getallsensors.asp",onrcv:getAllSensorsInfoRes, status:''});
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
		fillSensorType();
		reloadPEFCfg();
	}
}

function reloadPEFCfg()
{
	pefID.value = eExt.parseURLvars("pef_id");
	//Ignores backspace functionality
	pefID.onkeydown = checkBackspace;
	enablePEF.checked = (getbitsval(PEFCFG_DATA.FilterConfig,7,7) == 1) ? true : false;
	//pefAlert.checked = (getbitsval(PEFCFG_DATA.EvtFilterAction,0,0) == 1) ? true : false; 
	powerAction.value = PEFCFG_DATA.EvtFilterAction & 14; 		//Binary 1110, Bitwise mask for 0th bit.
	policyNo.value = PEFCFG_DATA.AlertPolicyNum;
	eventSeverity.value = PEFCFG_DATA.EventSeverity;

	genID1.value = "0x" + PEFCFG_DATA.GeneratorByte1.toString(16).toUpperCase();
	genID2.value = "0x" + PEFCFG_DATA.GeneratorByte2.toString(16).toUpperCase();

	sensorType.value = PEFCFG_DATA.SensorType;
	fillSensorNames();
	sensorName.value = (PEFCFG_DATA.SensorName == "Any") ? 0xFF : getSensorNumber(PEFCFG_DATA.SensorName);

	fillEventOptions();
	eventOpt.value = (PEFCFG_DATA.EventData1OffsetMask == 0xFFFF) ? 0 : 1; 
	loadSensorEvents();
	fillSensorEvents(PEFCFG_DATA.EventData1OffsetMask);

	eventTrigger.value = PEFCFG_DATA.EventTrigger;
	event1ANDMask.value = PEFCFG_DATA.EventData1ANDMask;
	event1Compare1.value = PEFCFG_DATA.EventData1Cmp1;
	event1Compare2.value = PEFCFG_DATA.EventData1Cmp2;
	event2ANDMask.value = PEFCFG_DATA.EventData2ANDMask;
	event2Compare1.value = PEFCFG_DATA.EventData2Cmp1;
	event2Compare2.value = PEFCFG_DATA.EventData2Cmp2;
	event3ANDMask.value = PEFCFG_DATA.EventData3ANDMask;
	event3Compare1.value = PEFCFG_DATA.EventData3Cmp1;
	event3Compare2.value = PEFCFG_DATA.EventData3Cmp2;
}

/* This function loads dynamic checkboxes based on sensor type, sensor name and event options choosen */
/*function loadSensorEvents()
{
	if (eventOpt.value == 1) 		//Get the sensor type and load corresponding events as checkbox 
	{
		var tmp_sensortype = 0xFF;
		if (sensorType.value != 0xFF)
			tmp_sensortype = sensorType.value
		else
			if (sensorName.value != 0xFF)
				tmp_sensortype = getSensorType(sensorName.value)

		if (tmp_sensortype != 0xFF)
		{
			sensorEventRow.style.display = "";
			sensorEvent.innerHTML = "";
			if (tmp_sensortype > 0 && tmp_sensortype < 5)		//Threshold sensors range is 0x1 to 0x4
			{
				sensorEvent.innerHTML = fillThresholdEvents();
			}
			else if (tmp_sensortype > 4 && tmp_sensortype < 45)		//Other sensors range is 0x5 to 0x2C
			{
				sensorEvent.innerHTML = fillDiscreteEvents(tmp_sensortype);
			}
			else					//OEM Sensors, get the Sensor event strings using PDK Hooks
			{
				sensorEventRow.style.display = "none";
			}
		}
	}
	else			//Unload Sensor Event checkboxes
	{
		sensorEvent.innerHTML = "";
		sensorEventRow.style.display = "none";
	}
}*/

function loadSensorEvents()
{
	if (eventOpt.value != 1)
	{
		sensorEvent.innerHTML = "";
		sensorEventRow.style.display = "none";
		return;
	}

	var tmp_sensortype = 0xFF;
	var tmpSensorEventType = 0x00;

	if (sensorType.value != 0xFF)
	{
		tmp_sensortype = sensorType.value;

		if (sensorName.value != 0xFF)
			tmpSensorEventType = GetSensorEventReadingType(sensorName.value);
	}
	else
	{
		if (sensorName.value != 0xFF)
		{
			tmp_sensortype = getSensorType(sensorName.value);
			tmpSensorEventType = GetSensorEventReadingType(sensorName.value);
		}
	}

	if (tmp_sensortype == 0xFF || 
		(tmp_sensortype != 0xFF && tmpSensorEventType == 0x00)) return;

	sensorEventRow.style.display = "";
	sensorEvent.innerHTML = "";

	if (tmp_sensortype > 0 && tmp_sensortype < 5)		//Threshold sensors range is 0x1 to 0x4
		sensorEvent.innerHTML = fillThresholdEvents(tmpSensorEventType);
	else if (tmp_sensortype > 4 && tmp_sensortype < 45)		//Other sensors range is 0x5 to 0x2C
	{
		if (tmpSensorEventType == 0x6F)
			sensorEvent.innerHTML = fillDiscreteEvents(tmp_sensortype);
		else if (tmpSensorEventType >= 0x01 && tmpSensorEventType <= 0x0C)
			sensorEvent.innerHTML = fillThresholdEvents(tmpSensorEventType);
	}
	else				//OEM Sensors, get the Sensor event strings using PDK Hooks
		sensorEventRow.style.display = "none";
}

function initPowerAction()
{
	var index = 0;
	powerAction.add(new Option(eLang.getString('common','STR_NONE'),0),isIE?index++:null);
	
	for (var i=1; i<4; i++)
		powerAction.add(new Option(eLang.getString('common','STR_PEF_POWER_' + i),Math.pow(2,i)), isIE?index++:null);
}

function initPolicyNumber()
{
	var index = 0;
	for (var i=1; i<16; i++)
		policyNo.add(new Option(i,i), isIE?index++:null);
}

function initEventSeverity()
{
	var index = 0;
	eventSeverity.add(new Option(eLang.getString('common','STR_PEF_SEVERITY'),0),isIE?index++:null);
	
	for (var i=0; i<6; i++)
		eventSeverity.add(new Option(eLang.getString('common','STR_PEF_SEVERITY_' + i),Math.pow(2,i)), isIE?index++:null);
}

/* This function returns Sensor number for the given Sensor Name */
function getSensorNumber(sensorname)
{
	var sensornum = 0;
	for(var j=0; j<SENSORINFO_DATA.length; j++)
	{
		if(SENSORINFO_DATA[j].SensorName == sensorname)
		{ 
			sensornum = SENSORINFO_DATA[j].SensorNumber;
			break;
		}
	}
	return sensornum;
}

/* This function returns SensorType, for the given Sensor Number */
function getSensorType(sensornum)
{
	var sensortype = 0;
	for(var j=0; j<SENSORINFO_DATA.length; j++)
	{
		if(SENSORINFO_DATA[j].SensorNumber == sensornum)
		{
			sensortype = SENSORINFO_DATA[j].SensorType;
			break;
		}
	}
	return sensortype;
}

function GetSensorEventReadingType(sensorNum)
{
	var eventReadingType = 0;

	for (var i = 0; i < SENSORINFO_DATA.length; i++)
	{
		if (SENSORINFO_DATA[i].SensorNumber == sensorNum)
		{
			eventReadingType = SENSORINFO_DATA[i].SensorEventType;
			break;
		}
	}
	
	return eventReadingType;
}

/* This function fills the available Sensor types in SensorType selectbox */
function fillSensorType()
{
	var index = 0;
	if (!SENSORINFO_DATA.length)
	{
		alert (eLang.getString('common',"NO_SENSOR_STRING"));
		return;
	}

	sensorType.add(new Option(sensorTypeCodes[0x0], 0xFF), isIE?index++:null);
	for(var i=1; i<sensorTypeCodes.length; i++)
	{
		for(var j=0; j<SENSORINFO_DATA.length; j++)
		{
			if(SENSORINFO_DATA[j].SensorType == i)
			{ 
				sensorType.add(new Option(sensorTypeCodes[i],i),isIE?index++:null);
				break;
			}
		}
	}
	//fillSensorNames();
}

/* This function fills available sensors (based on the choice choosen in SensorType) in SensorName selectbox */
function fillSensorNames()
{
	var index = 0;
	sensorName.innerHTML = '';
	sensorName.add(new Option(sensorTypeCodes[0x0], 0xFF), isIE?index++:null);
	for(var j=0; j<SENSORINFO_DATA.length; j++)
	{
		if((SENSORINFO_DATA[j].SensorType == sensorType.value) || (sensorType.value == 0xFF))
		{
			sensorName.add(new Option(SENSORINFO_DATA[j].SensorName, SENSORINFO_DATA[j].SensorNumber),isIE?index++:null);
		}
	}
	//fillEventOptions();
}

/* This function fills All Events, Specific Sensor Events in Event Option selectbox(based on SensorType and SensorName) */
function fillEventOptions()
{
	var index = 0;
	eventOpt.innerHTML = '';
	eventOpt.add(new Option(eLang.getString('common','STR_PEF_ALL_EVENTS'), 0),isIE?index++:null);
	if (sensorType.value != 0xFF && sensorName.value != 0xFF)
		eventOpt.add(new Option(eLang.getString('common','STR_PEF_SENSOR_EVENTS'), 1),isIE?index++:null);
	//loadSensorEvents();
}

/* This function loads 12checkboxes for threshold sensors */
/*function fillThresholdEvents()
{
	var sensor_spec = eLang.getString('common','STR_SENSOR_THRESHOLD');
	var sensorThreshold = "<table>"; 
	for (var i=0; i<12; i+=2)
	{
		sensorThreshold += "<tr><td>" + sensor_spec[(i/2)] + "</td><td>:</td>" +
			"<td><input type='checkbox' id='_thres_bit" + i + "'/>" + 
			"<label for='_thres_bit" + i + "'>" + eLang.getString('common','STR_PEF_EVENT_LOW') + "</label></td>" +
			"<td><input type='checkbox' id='_thres_bit" + (i+1) + "'/>" +
			"<label for='_thres_bit" + (i+1) + "'>" + eLang.getString('common','STR_PEF_EVENT_HIGH') + "</label></td></tr>";
	}
	sensorThreshold += "</table>";
	return (sensorThreshold);
}*/

function fillThresholdEvents(sensorEventType)
{
	var sensor_spec = eLang.getString('event', sensorEventType);
	var sensorThreshold = "<table>"; 

	for (var i = 0; i < sensor_spec.length; i+=2)
	{
		sensorThreshold += "<tr><td><input type='checkbox' id='_thres_bit" + i + "'/>" + 
						   "<label for='_thres_bit" + i + "'>" + sensor_spec[i] + "</label></td>";

		j = i + 1;

		if (j != sensor_spec.length)
		{
			sensorThreshold += "<td><input type='checkbox' id='_thres_bit" + j + "'/>" +
							   "<label for='_thres_bit" + j + "'>" + sensor_spec[j] + "</label></td>";
		}

		sensorThreshold += "</tr>";
	}

	sensorThreshold += "</table>";
	return (sensorThreshold);
}

/* This function gets the sensor specific event count and loads checkboxes */  
function fillDiscreteEvents(sensortype)
{
	var sensor_spec = eLang.getString('sensor_specific_event',sensortype);
	var sensorThreshold = "<table>"; 
	for (var i=0; i<sensor_spec.length; i+=2)
	{
		sensorThreshold += "<tr><td><input type='checkbox' id='_thres_bit" + i + "'/>" + 
			"<label for='_thres_bit" + i + "'>" + sensor_spec[i] + "</label></td>";
		j = i+1;
		if (j != sensor_spec.length)
		{
			sensorThreshold += "<td><input type='checkbox' id='_thres_bit" + j + "'/>" +
				"<label for='_thres_bit" + j + "'>" + sensor_spec[j] + "</label></td>";
		}
		sensorThreshold += "</tr>";
	}
	sensorThreshold += "</table>";

	return (sensorThreshold);
}

/* This function check/uncheck the corresponding sensor event checkbox based on Event data offset mask value */
function fillSensorEvents(offsetmask)
{
	for (var i=0; i<16; i++)
	{
		if (getbitsval(offsetmask,i,i) == 1)
		{
			try
			{
				chkboxname = "_thres_bit" + i;
				chkbox = document.getElementById(chkboxname);
				chkbox.checked = true
			}
			catch(e)
			{
				break;
			}
		} 
	}
}

function validatePEFCfg()
{
	if(!pefAlert.checked)
	{
		alert (eLang.getString('common','STR_PEF_ALERT_ERR'));
		pefAlert.focus();
		return;
	}
	if(!eVal.isnumstr(eventTrigger.value, 1, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT_TRIG_ERR') + eLang.getString('common','STR_HELP_INFO'));
		eventTrigger.focus();
		return;
	}
	if(!eVal.isnumstr(event1ANDMask.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT1_MASK_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event1ANDMask.focus();
		return;
	}
	if(!eVal.isnumstr(event1Compare1.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT1_CMP1_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event1Compare1.focus();
		return;
	}
	if(!eVal.isnumstr(event1Compare2.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT1_CMP2_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event1Compare2.focus();
		return;
	}
	if(!eVal.isnumstr(event2ANDMask.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT2_MASK_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event2ANDMask.focus();
		return;
	}
	if(!eVal.isnumstr(event2Compare1.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT2_CMP1_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event2Compare1.focus();
		return;
	}
	if(!eVal.isnumstr(event2Compare2.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT2_CMP2_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event2Compare2.focus();
		return;
	}
	if(!eVal.isnumstr(event3ANDMask.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT3_MASK_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event3ANDMask.focus();
		return;
	}
	if(!eVal.isnumstr(event3Compare1.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT3_CMP1_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event3Compare1.focus();
		return;
	}
	if(!eVal.isnumstr(event3Compare2.value, 0, 255))
	{
		alert (eLang.getString('common','STR_PEF_EVT3_CMP2_ERR') + eLang.getString('common','STR_HELP_INFO'));
		event3Compare2.focus();
		return;
	}
	setPEFCfg();
}

/* This function returns Sensor OwnerID and Owner LUN for the given Sensor Number */
/*function getSensorDetails(sensornum)
{
	for(var j=0; j<SENSORINFO_DATA.length; j++)
	{
		if(SENSORINFO_DATA[j].SensorNumber == sensornum)
		{
			owner_id = SENSORINFO_DATA[j].OwnerID;
			owner_lun = SENSORINFO_DATA[j].OwnerLUN;
			break;
		}
	}
}*/

function setPEFCfg()
{
	var pefactionval = 0;
	var req = new xmit.getset({url:"/rpc/configurepef.asp", onrcv:setPEFRes, status:''});
	req.add ("WEBVAR_ENTRYINDEX",pefID.value);
	req.add ("WEBVAR_FILTERCFG",enablePEF.checked ? 0x80 : 0);

	pefactionval += pefAlert.checked ? 1 : 0;
	pefactionval += parseInt(powerAction.value);
	req.add ("WEBVAR_EVTFILTERACTION",pefactionval); 

	req.add ("WEBVAR_POLICYNUM",policyNo.value);
	req.add ("WEBVAR_SEVERITY",eventSeverity.value);

	var sensortypeval = 0xFF;
	if (sensorType.value != 0xFF)
	{
		sensortypeval = sensorType.value
//		if (sensorName.value != 0xFF)
//			getSensorDetails(sensorName.value);
	}
	else
	{
		if (sensorName.value != 0xFF)
		{
			sensortypeval = getSensorType(sensorName.value);
	//		getSensorDetails(sensorName.value);
		}
	}
	req.add ("WEBVAR_GENID1",genID1.value);
	req.add ("WEBVAR_GENID2",genID2.value);
	req.add ("WEBVAR_SENSORTYPE",sensortypeval);
	req.add ("WEBVAR_SENSORNUM",sensorName.value);

	var eventdataval = 0;
	if (eventOpt.value == 0)
		eventdataval = 0xFFFF;
	else
	{
		for (i=0; i<16; i++)
		{
			try
			{
				chkboxname = "_thres_bit" + i;
				chkbox = document.getElementById(chkboxname);
				if (chkbox.checked)
					eventdataval += Math.pow(2,i);
			}
			catch(e)
			{
				break;
			} 
		}
	}
	req.add ("WEBVAR_EVTDATA1OFFSETMASK", eventdataval);

	req.add ("WEBVAR_EVTTRIGGER",parseInt(eventTrigger.value,10));
	req.add ("WEBVAR_EVTDATA1AND",parseInt(event1ANDMask.value,10));
	req.add ("WEBVAR_EVTDATA1COMP1",parseInt(event1Compare1.value,10));
	req.add ("WEBVAR_EVTDATA1COMP2",parseInt(event1Compare2.value,10));
	req.add ("WEBVAR_EVTDATA2AND",parseInt(event2ANDMask.value,10));
	req.add ("WEBVAR_EVTDATA2COMP1",parseInt(event2Compare1.value,10));
	req.add ("WEBVAR_EVTDATA2COMP2",parseInt(event2Compare2.value,10));
	req.add ("WEBVAR_EVTDATA3AND",parseInt(event3ANDMask.value,10));
	req.add ("WEBVAR_EVTDATA3COMP1",parseInt(event3Compare1.value,10));
	req.add ("WEBVAR_EVTDATA3COMP2",parseInt(event3Compare2.value,10));

	req.send();
	delete req; 
}

function setPEFRes(arg)
{
	if(arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_CONF_PEF_CFGINFO");
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
		alert (errstr);
	}
	else
	{
		if (pef_oper == 1)			//Add PEF Entry
			alert (eLang.getString('common','STR_CONF_PEF_ADD_SUCCESS'));
		else if(pef_oper == 2)		//Modify PEF Entry
			alert (eLang.getString('common','STR_CONF_PEF_MOD_SUCCESS'));

		exitPEFCfg();
	}
}

function exitPEFCfg()
{
	location.href='configure_pef.html';
}

function toggleRawData()
{
	var opt = rawType.checked;
	genID1.disabled = !opt;
	genID2.disabled = !opt;
	slaveType.disabled = opt;
	softwareType.disabled = opt;
	slave_software.disabled = opt;
	channelNo.disabled = opt;
	ipmbDevice.disabled = opt;
	if (!opt)
		slaveType.checked = true;
}

function toggleSlaveSoftware()
{
	ipmbDevice.disabled = !slaveType.checked;
	calculateGenID();
	slave_software.focus();
}

function initChannelNo()
{
	var index = 0;
	for (var i=0; i<12; i++)
		channelNo.add(new Option(i,i), isIE?index++:null);
	channelNo.add(new Option(15,15), isIE?index++:null);
}

function initIPMBDevices()
{
	var index = 0;
	for (var i=1; i<4; i++)
		ipmbDevice.add(new Option(i,i), isIE?index++:null);
}

function calculateGenID()
{
	var genvalue1,genvalue2;
	if (slaveType.checked)
		genvalue1 = 0x0;
	else if (softwareType.checked)
		genvalue1 = 0x1;

	if (eVal.isnumstr(slave_software.value,0,127))
		genvalue1 = ((slave_software.value << 1) & 0xFE) | genvalue1;

	genID1.value = "0x" + genvalue1.toString(16).toUpperCase();

	genvalue2 = (channelNo.value << 4) & 0xF0;
	if (slaveType.checked)
		genvalue2 = (ipmbDevice.value | genvalue2);
	genID2.value = "0x" + genvalue2.toString(16).toUpperCase();
}
