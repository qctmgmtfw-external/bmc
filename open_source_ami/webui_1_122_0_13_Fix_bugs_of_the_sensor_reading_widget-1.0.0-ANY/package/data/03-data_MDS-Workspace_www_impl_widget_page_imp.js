--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/widget_page_imp.js Wed Aug 29 13:49:44 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/widget_page_imp.js Wed Aug 29 13:56:18 2012
@@ -26,6 +26,9 @@
 
 var sensorHistory = [];
 var ct = null;
+var suo = null;
+var id;
+var isFirstTime = false;
 
 function doInit()
 {
@@ -71,128 +74,90 @@
 
 	liveUpdates = (parent.liveUpdates!=undefined && parent.liveUpdates!=null)
 
-	if(!liveUpdates)
-	{
-		parent.liveUpdates = function()
+	if (!liveUpdates)
+	{
+		isFirstTime = true;
+
+		if (parent.GetAllSensorsResponse == undefined || parent.GetAllSensorsResponse == null)
 		{
-			try
+			parent.GetAllSensorsResponse = function()
 			{
-				xmit.get({url:'/rpc/getallsensors.asp',
-					onrcv:function(arg)
-					{
-						if (arg.status == 503)
-						{
-							stopPolling();
-							clearTimeout(ct);
-						}
-						else if(arg.HAPI_STATUS == 0) 
-						{
-							parent.SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
-							ct = null;
-							_begin();
-						} else if (arg.HAPI_STATUS == null) {
-						} else {
-							alert("Error while updating live sensors");
-							stopLiveUpdates();
-						}
-					},
-					show_progress:false,onerror:errorHandler
-				});
-			}
-			catch(e)
-			{
-				alert(e);
+				if (isFirstTime)
+				{
+					doUpdate();
+					isFirstTime = false;
+				}
 			}
 		}
+
+		parent.StartPolling();
 		parent.liveUpdates();
-	}else
-	{
-		_begin();
-	}
+	}
+
+	_begin();
 }
 
 function _begin()
 {
-	if(parent.liveUpdateTimer==null)
-	{
-		startTime = (new Date()).getUTCTime();
-		parent.liveUpdateTimer = setInterval(parent.liveUpdates,parent.liveUpdateTime);
-	}
-
-	if (ct == null) {
-		doUpdate();
-		events.register('beforeunload',stopLiveUpdates,window);
-		events.register('unload',stopLiveUpdates,window);
-	}
+	startTime = (new Date()).getUTCTime();
+
+	if (!isFirstTime) doUpdate();
+	if (ct == null) ct = setTimeout(UpdateTask, parent.liveUpdateTime);
 }
 
 stopLiveUpdates = function()
 {
-	try{
-		if(parent.widgets.nullLessLength()<=1)
-		{
-			stopPolling();
-		}
-	}catch(e)
-	{
-		stopPolling();
-	}
+	stopPolling();
+
 	delete sensorHistory;
 	clearTimeout(ct);
 }
 
 function stopPolling()
 {
-	clearInterval(parent.liveUpdateTimer);
-	parent.liveUpdateTimer = null;
-	parent.liveUpdates = null;
-}
-
-/*
- * This function will handle any error for getallsensors RPC response. 
- */
-function errorHandler() 
-{
-	parent.SENSORINFO_DATA = null;
-	alert(eLang.getString("common", "STR_NETWORK_ERROR"));
-	stopLiveUpdates();
-}
-
-var suo=null;
+	parent.StopPolling();
+}
 
 function doUpdate()
 {
-	if (parent.SENSORINFO_DATA != null) {
-		wTitle.innerHTML = parent.SENSORINFO_DATA[id].SensorName;
-		suo = parent.getStateAndUnit(id);
-		wReading.innerHTML = suo.SensorReading + " " + suo.unitstr;
+	if (parent.SENSORINFO_DATA == null) return;
+
+	wTitle.innerHTML = parent.SENSORINFO_DATA[id].SensorName;
+	suo = parent.getStateAndUnit(id);
+
+	wReading.innerHTML = suo.SensorReading + " " + suo.unitstr;
+
+	if (suo.state.toLowerCase().indexOf("non-critical") != -1) {
+		windowOptions.style.background = "url(../res/widget_header_nc2.png) repeat-x";
+	} else if (suo.state.toLowerCase().indexOf("critical") != -1) {
+		windowOptions.style.background = "url(../res/widget_header_c.png) repeat-x";
+	} else if (suo.state.toLowerCase().indexOf("non-recoverable") != -1) {
+		windowOptions.style.background = "url(../res/widget_header_nr.png) repeat-x";
+	} else {
+		windowOptions.style.background = "url(../res/widget_header.png) repeat-x";
+	}
+	try {
+		startTime++; 
+		startTime--;
+	} catch(e) {
+		startTime = (new Date()).getUTCTime();
+	}
+	for (var s = 0; s < sensorHistory.length; s++) {
+		sensorHistory[s][0] += (parent.liveUpdateTime/1000);
+	}
+	sensorHistory.push([0, suo.SensorReading]);
+	//alert(sensorHistory);
+	doGraph();
+}
+
+function UpdateTask()
+{
+	doUpdate();
 	
-		if (suo.state.toLowerCase().indexOf("non-critical") != -1) {
-			windowOptions.style.background = "url(../res/widget_header_nc2.png) repeat-x";
-		} else if (suo.state.toLowerCase().indexOf("critical") != -1) {
-			windowOptions.style.background = "url(../res/widget_header_c.png) repeat-x";
-		} else if (suo.state.toLowerCase().indexOf("non-recoverable") != -1) {
-			windowOptions.style.background = "url(../res/widget_header_nr.png) repeat-x";
-		} else {
-			windowOptions.style.background = "url(../res/widget_header.png) repeat-x";
-		}
-		try {
-			startTime++; 
-			startTime--;
-		} catch(e) {
-			startTime = (new Date()).getUTCTime();
-		}
-		for (var s = 0; s < sensorHistory.length; s++) {
-			sensorHistory[s][0] += (parent.liveUpdateTime/1000);
-		}
-		sensorHistory.push([0, suo.SensorReading]);
-		//alert(sensorHistory);
-		doGraph();
-		if(ct != null) {
-			clearTimeout(ct);
-		}
-	}
-	ct = setTimeout(doUpdate, parent.liveUpdateTime);
+	if(ct != null)
+		clearTimeout(ct);
+
+	ct = setTimeout(UpdateTask, parent.liveUpdateTime);
 }
 
 function roundVal(val){
