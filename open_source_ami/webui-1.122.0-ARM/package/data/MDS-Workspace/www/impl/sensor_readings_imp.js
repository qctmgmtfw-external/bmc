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

var sensorResultsTable;
var showThresholds;
var SENSORINFO_DATA;
var SELINFO_DATA;
var tblJSON;
var gCurSensor = 0x00;
var firstTime = true;

function doInit()
{
	 // TODO: add page initialization code
	 exposeElms(['_infoLeft',
	 			'_infoRight',
	 			'_sensorType',
	 			'_sensorState',
	 			'_sensorName',
	 			'_lowerNR',
	 			'_lowerC',
	 			'_lowerNC',
	 			'_lowerNR',
	 			'_upperC',
	 			'_upperNC',
	 			'_upperNR',
	 			'_lblHeader',
	 			'_widgetStatus',
	 			'_toggleWidget',
	 			'_showEventLog']);

	 gSensorTypeCodes = eLang.getString('common',"STR_SENSOR_TYPES");

	 _begin();
}

function _begin()
{
	sensorResultsTable = listgrid({
		w				: '100%',

		doAllowNoSelect : false
	});

	infoLeft.appendChild(sensorResultsTable.table);
	loadCustomPageElements();
	getSELInfo();
}

function getSELInfo()
{
	xmit.get({url:'/rpc/getallselentries.asp', onrcv:function(arg)
	{
		if(arg.HAPI_STATUS)
		{
			errstr = eLang.getString('common',"STR_EVENT_LOG_GETVAL");
			errstr +=  (eLang.getString('common','STR_IPMI_ERROR') + GET_ERROR_CODE(arg.HAPI_STATUS));
			alert(errstr);
		}else
		{
			SELINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSELENTRIES.WEBVAR_STRUCTNAME_HL_GETALLSELENTRIES;
		}
		getSensorsInfo();
	}});
}

function loadCustomPageElements()
{
	sensorResultsTable.clear();

	tblJSON = {
				cols:[
					{text:"Sensor Name", fieldName:'name', w:'35%'},
					{text:"Status", fieldName:'status', w:'35%'},
					{text:"Current Reading", fieldName:'reading', w:'30%'}
					]
					};

	sensorResultsTable.loadFromJson(tblJSON);
	sensorResultsTable.table.style.width = "100%";
	sensorResultsTable.onrowselect = showSensorDetails;
	sensorResultsTable.ondblclick = doToggleWidget;

	sensorType.onchange = function()
	{
		gCurSensor = sensorType.options.selectedIndex;
		updateSensors();
		graph.reposition();
	}
}

function updateSensors()
{
	var count = 0;
	var j = 0;
	var unitstr;
	var state;
	var SensorReading;

	var JSONRows = new Array();
	tblJSON.rows = new Array();

	sensorResultsTable.clear();
	for(j=0;j<SENSORINFO_DATA.length;j++)
	{
		if((SENSORINFO_DATA[j].SensorType == sensorType.value) || (sensorType.value == 0))
		{
			var suo = getStateAndUnit(j);

			state = suo.state;
			unitstr = suo.unitstr;
			SensorReading = suo.SensorReading;
			JSONRows.push({cells:[
						{text:SENSORINFO_DATA[j].SensorName, value:SENSORINFO_DATA[j].SensorName},
						{text:state, value:state},
						{text:(SensorReading)+" "+unitstr, value:(SensorReading)+" "+unitstr}
						]});
			count++;
		}
	}

	tblJSON.rows = JSONRows;

	sensorResultsTable.loadFromJson(tblJSON);

	lblHeader.innerHTML = "<strong class='st'>"+eLang.getString('common',"STR_SENSOR_CNT")+"</strong>"+
							count+eLang.getString('common',"STR_SENSOR_SENSORS");

	var r=1;
	if(firstTime && HTTP_GET_VARS['id'])
	{
		gCurId = HTTP_GET_VARS['id'];

		while(sensorResultsTable.container.rows[r])
		{
			if(sensorResultsTable.container.rows[r].cells[0].innerHTML.replace('&nbsp;','')==SENSORINFO_DATA[gCurId].SensorName)
			{
				break;
			}
			r++;
		}
	}
	sensorResultsTable.container.rows[r].onclick();
}

function widgetListener(rid)
{
	if(rid==gCurId)
	{
		widgetStatus.innerHTML = "Off";
		toggleWidget.innerHTML = "On";
	}
}

function doToggleWidget()
{
	var sname = sensorResultsTable.getRow(sensorResultsTable.selected[0]).cells[0].innerHTML.replace('&nbsp;','');
	var id = getSensorID(sname);

	if(SENSORINFO_DATA[id].SensorState)		//doToggleWidget for Threshold sensors.
	{
		wStatus = (parent.widgets[id]==null || parent.widgets[id]=="");
		if(wStatus)
		{
			//parent.widgets[id] = new parent.widget(id);
			parent.CreateWidget(id);
			if(parent.widgets[id].container!=undefined)
			{
				widgetStatus.innerHTML = "On";
				toggleWidget.innerHTML = "Off";
			}
			else
			{
				//delete parent.widgets[id]; //clear mem leak
				parent.DeleteWidget(id);
				widgetStatus.innerHTML = "Off";
				toggleWidget.innerHTML = "On";
			}
		}else
		{
			parent.StopPolling();
			parent.widgets[id].close();
			parent.DeleteWidget(id);
			//delete parent.widgets[id]; //clear mem leak
			widgetStatus.innerHTML = "Off";
			toggleWidget.innerHTML = "On";
		}
	}
}


function showSensorDetails(selectedRow)
{
	var id = getSensorID(selectedRow.cells[0].innerHTML.replace('&nbsp;',''));
	gCurId = id;

	var suo = getStateAndUnit(id);
	stateClass  = 'normal';

	sensorName.innerHTML = SENSORINFO_DATA[id].SensorName+": <font class='"+stateClass+"'>"+suo.SensorReading+" "+suo.unitstr+"</font>";
	sensorState.innerHTML =  suo.state;

	if(SENSORINFO_DATA[id].SensorState)
	{
		lowerNR.innerHTML = SENSORINFO_DATA[id].LowNRThresh/1000 +" "+ suo.unitstr;
		lowerC.innerHTML = SENSORINFO_DATA[id].LowCTThresh/1000 +" "+ suo.unitstr;
		lowerNC.innerHTML = SENSORINFO_DATA[id].LowNCThresh/1000 +" "+ suo.unitstr;
		upperNR.innerHTML = SENSORINFO_DATA[id].HighNRThresh/1000 +" "+ suo.unitstr;
		upperC.innerHTML = SENSORINFO_DATA[id].HighCTThresh/1000 +" "+ suo.unitstr;
		upperNC.innerHTML = SENSORINFO_DATA[id].HighNCThresh/1000 +" "+ suo.unitstr;

		//Widget Listener for threshold sensors.
		wStatus = (parent.widgets[id]==null || parent.widgets[id]=="");
		widgetStatus.innerHTML = !wStatus?"On":"Off";

		toggleWidget.innerHTML = !wStatus?"Off":"On";
		toggleWidget.onclick = doToggleWidget;
	}
	else
	{
		lowerNR.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		lowerC.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		lowerNC.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		upperNR.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		upperC.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		upperNC.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");

		widgetStatus.innerHTML = eLang.getString('common',"STR_NOT_APPLICABLE");
		toggleWidget.innerHTML = '';
	}
	showEventLog.onclick = function()
	{
		location.href = "event_log.html?sid="+id;
	}

	try
	{
		graph.clearDataArea();
	}catch(e){
		gphSet = {};
		gphSet.bgColor = "rgba(255,255,255,1)";
		gphSet.width = 500;
		gphSet.height = 250;
		gphSet.borderWidth = 1;
		gphSet._gX = 50;
		gphSet._gY = 10;
		gphSet.canvasName = 'graphCanvas';
		gphSet.barHeight = 20;
		gphSet.barOffset = 10;
		gphSet.graphTitle = '';
		graph._init(gphSet.canvasName, gphSet);
		var diff=SELINFO_DATA.length/4;
		var scale=new Array();
		scale[0]=0;
		for(var i=0;i<4;i++)
			scale[i+1]=Math.round(diff+scale[i]);
		
		graph.drawXScale([scale[0],scale[1],scale[2],scale[3],SELINFO_DATA.length],'Number of Entries');
	
	}

	drawEventsGraph(SENSORINFO_DATA[id].SensorName);
}


function drawEventsGraph(name)
{
	//var sensor_specific=0;
	var discrete = 0;
	var other_events=0;
	var lnr=0;
	var lc=0;
	var lnc=0;
	var hnr=0;
	var hc=0;
	var hnc=0;

	var uniqueSensors = [];
	var uniqueEvents = [];

	for(i=0; i<SELINFO_DATA.length;i++)
	{
		if((id=uniqueSensors.indexOf(SELINFO_DATA[i].SensorName))==-1)
		{
			uniqueSensors.push(SELINFO_DATA[i].SensorName);
			uniqueEvents.push(1);
		}else
		{
			uniqueEvents[id]++;
		}

		if(SELINFO_DATA[i].SensorName==name)
		{
			var res = processEventRecordForGraph(SELINFO_DATA[i]);
			//sensor_specific += res.sensor_specific;
			discrete += res.discrete;
			other_events += res.other_events;
			lnr += res.lnr;
			lc += res.lc;
			lnc += res.lnc;
			hnr += res.hnr;
			hc += res.hc;
			hnc += res.hnc;
		}
	}

	delete uniqueSensors;

	gData = [];
	//gData.push({type:'SS',count:sensor_specific,color:'rgba(0,180,0,.7)'});
	gData.push({type:'Discrete',count:discrete,color:'rgba(0,180,0,.7)'});
	gData.push({type:'Other',count:other_events,color:'rgba(0,140,200,.7)'});
	gData.push({type:'UNC',count:hnc,color:'rgba(255,255,0,.7)'});
	gData.push({type:'UC',count:hc,color:'rgba(255,150,0,.7)'});
	gData.push({type:'UNR',count:hnr,color:'rgba(255,0,0,.7)'});
	gData.push({type:'LNC',count:lnc,color:'rgba(255,255,0,.7)'});
	gData.push({type:'LC',count:lc,color:'rgba(255,150,0,.7)'});
	gData.push({type:'LNR',count:lnr,color:'rgba(255,0,0,.7)'});

	graph.drawBarChart(gData,SELINFO_DATA.length);
}


function getSensorsInfoRes(arg)
{
	if (arg.HAPI_STATUS)
	{
		errstr = eLang.getString('common',"STR_SENSOR_GETVAL")
		errstr +=  (eLang.getString('common','STR_IPMI_ERROR')+GET_ERROR_CODE(arg.HAPI_STATUS));
		alert(errstr);
	}else
	{
		SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
		if (!SENSORINFO_DATA.length)
		{
			alert(eLang.getString('common',"NO_SENSOR_STRING"));
			lblHeader.innerHTML = eLang.getString('common',"STR_SENSOR_CNT") + 
				(SENSORINFO_DATA.length) + eLang.getString('common',"STR_SENSOR_SENSORS");
			return;
		}

		optind = 0;
		sensorType.innerHTML = '';
		for(var i=0; i<gSensorTypeCodes.length; i++)
			if(sensorExists(i) || i==0x00)
				sensorType.add(new Option(gSensorTypeCodes[i],i),isIE?optind++:null);

		sensorType.options[gCurSensor].selected = true;
		if (top.mainFrame.pageFrame.location.hash)
			sensorType.value = comboParser(top.mainFrame.pageFrame.location.hash,sensorType.id);	//id->id of the selection box,0->2nd selectionbox id
		updateSensors();
	}
}

function getSensorsInfo()
{
	xmit.get({url:"/rpc/getallsensors.asp",onrcv:getSensorsInfoRes, status:'',timeout:120,ontimeout:timedOut});
}

function sensorExists(i)
{
	for(_ex=0; _ex<SENSORINFO_DATA.length; _ex++)
		if(SENSORINFO_DATA[_ex].SensorType == i)
			return true;

	return false;
}

function timedOut()
{
	alert(eLang.getString('common', "STR_TIME_OUT"));
}

function doSort(i,dir)
{
	sensorResultsTable.sortCol(i,dir);
}
