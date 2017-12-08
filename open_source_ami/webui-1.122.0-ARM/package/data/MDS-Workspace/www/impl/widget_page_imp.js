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

var sensorHistory = [];
var ct = null;
var suo = null;
var id;
var isFirstTime = false;

function doInit()
{
	exposeElms(['_wClose',
	 			 '_wRestore',
	 			 '_wSave',
	 			 '_wHelp',
	 			 '_wTitle',
	 			 '_wReading',
	 			 '_graphCanvas',
	 			 '_windowOptions']);

	id = HTTP_GET_VARS['id'];

	wHelp.onclick = function()
	{
		top.openHelp('..'+top.gHelpDir+top.gLangSetting+"/widget_page_hlp.html");
	}

	wClose.onclick = function()
	{
		stopLiveUpdates();
		parent.widgets[id].close();
	}

	wRestore.onclick = function()
	{
		if(this.firstChild.src.indexOf('restore')!=-1)
		{
			this.firstChild.src = "../res/widget_minimize.gif";
			graph.display(''); //make visible
			parent.widgets[id].container.style.height='200px';
			wRestore.title = "Minimize";
		}else
		{
			this.firstChild.src = "../res/widget_restore.gif";
			graph.display('none');
			parent.widgets[id].container.style.height='43px';
			wRestore.title = "Restore";
		}
		parent.widgets[id].autoPositionTop();
	}

	liveUpdates = (parent.liveUpdates!=undefined && parent.liveUpdates!=null)

	if (!liveUpdates)
	{
		isFirstTime = true;

		if (parent.GetAllSensorsResponse == undefined || parent.GetAllSensorsResponse == null)
		{
			parent.GetAllSensorsResponse = function()
			{
				if (isFirstTime)
				{
					doUpdate();
					isFirstTime = false;
				}
			}
		}

		parent.StartPolling();
		parent.liveUpdates();
	}

	_begin();
}

function _begin()
{
	startTime = (new Date()).getUTCTime();

	if (!isFirstTime) doUpdate();
	if (ct == null) ct = setTimeout(UpdateTask, parent.liveUpdateTime);
}

stopLiveUpdates = function()
{
	stopPolling();

	delete sensorHistory;
	clearTimeout(ct);
}

function stopPolling()
{
	parent.StopPolling();
}

function doUpdate()
{
	if (parent.SENSORINFO_DATA == null) return;

	wTitle.innerHTML = parent.SENSORINFO_DATA[id].SensorName;
	suo = parent.getStateAndUnit(id);

	wReading.innerHTML = suo.SensorReading + " " + suo.unitstr;

	if (suo.state.toLowerCase().indexOf("non-critical") != -1) {
		windowOptions.style.background = "url(../res/widget_header_nc2.png) repeat-x";
	} else if (suo.state.toLowerCase().indexOf("critical") != -1) {
		windowOptions.style.background = "url(../res/widget_header_c.png) repeat-x";
	} else if (suo.state.toLowerCase().indexOf("non-recoverable") != -1) {
		windowOptions.style.background = "url(../res/widget_header_nr.png) repeat-x";
	} else {
		windowOptions.style.background = "url(../res/widget_header.png) repeat-x";
	}
	try {
		startTime++; 
		startTime--;
	} catch(e) {
		startTime = (new Date()).getUTCTime();
	}
	for (var s = 0; s < sensorHistory.length; s++) {
		sensorHistory[s][0] += (parent.liveUpdateTime/1000);
	}
	sensorHistory.push([0, suo.SensorReading]);
	//alert(sensorHistory);
	doGraph();
}

function UpdateTask()
{
	doUpdate();
	
	if(ct != null)
		clearTimeout(ct);

	ct = setTimeout(UpdateTask, parent.liveUpdateTime);
}

function roundVal(val){
	var dec = 3;
	var result = Math.round(val*Math.pow(10,dec))/Math.pow(10,dec);
	return result;
}

function doGraph()
{	var i;
	try{if(gphSet);}
	catch(e){
		gphSet = {};
		gphSet.bgColor = "rgba(255,255,255,1)";
		gphSet.width = 160;
		gphSet.height = 110;
		gphSet.borderWidth = 1;
		gphSet._gX = 50;
		gphSet._gY = 8;
		gphSet.graphTitle = '';
		graph._init('_graphCanvas', gphSet);
		var lnr = (suo.SensorReading < 
			(parent.SENSORINFO_DATA[id].LowNRThresh / 1000)) ? suo.SensorReading : 
			(parent.SENSORINFO_DATA[id].LowNRThresh / 1000);
		var unr = (suo.SensorReading > 
			(parent.SENSORINFO_DATA[id].HighNRThresh / 1000)) ? suo.SensorReading : 
			(parent.SENSORINFO_DATA[id].HighNRThresh / 1000);
		var diff = (lnr > unr) ? ((lnr - unr) / 5) : ((unr - lnr) / 5);
		var lc = roundVal(lnr + diff);
		var lnc = roundVal(lc + diff);
		var unc = roundVal(lnc + diff);
		var uc = roundVal(unc + diff);
		if (lnr > unr) {	
			graph.drawYScale([unr,uc,unc,lnc,lc,lnr],
				suo.unitstr.replace('&deg;', 'Degree'));
						
		} else {
			graph.drawYScale([lnr,lc,lnc,unc,uc,unr],
				suo.unitstr.replace('&deg;', 'Degree'));
		}
		//graph.drawXScale([30,25,20,15,10,5,0],"Time Elapsed");
		graph.drawXScale([5,4,3,2,1,0],"Time Elapsed (mins)");
	}
	//alert(sensorHistory);
	graph.drawExtLineGraph(sensorHistory);
}
