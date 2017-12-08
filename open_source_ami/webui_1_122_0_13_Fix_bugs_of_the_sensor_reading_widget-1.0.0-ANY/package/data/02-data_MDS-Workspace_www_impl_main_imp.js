--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/main_imp.js Wed Aug 29 13:49:44 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/main_imp.js Wed Aug 29 13:55:20 2012
@@ -26,6 +26,8 @@
 
 modulesLoaded = false;
 pageLoaded = false;
+isUpdating = false;
+
 
 function doInit()
 {
@@ -38,6 +40,7 @@
 	liveUpdateTimer = null;
 	liveUpdates = null;
 	SENSORINFO_DATA = null;
+	GetAllSensorsResponse = null;
 
 	if(top.user.pno>=2)
 		_begin();
@@ -94,4 +97,70 @@
 	$('pageFrame').style.height = document.body.clientHeight - (menuHolder.offsetHeight);
 }
 
+function StartPolling()
+{
+	if (liveUpdates != undefined && liveUpdates != null) return;
 
+	liveUpdates = function()
+	{
+		if (isUpdating) return;
+
+		isUpdating = true;
+		try
+		{
+			xmit.get({url:'/rpc/getallsensors.asp', onrcv:OnGetAllSensorsRes, show_progress:false, onerror:ErrorHandler});
+		}
+		catch(e)
+		{
+			alert(e);
+		}
+	}
+
+	if (liveUpdateTimer == null)
+		liveUpdateTimer = setInterval(liveUpdates, liveUpdateTime);
+}
+
+function StopPolling()
+{
+	if (widgets.nullLessLength() <= 1)
+	{
+		clearInterval(liveUpdateTimer);
+		liveUpdateTimer = null;
+		liveUpdates = null;
+		GetAllSensorsResponse = null;
+		isUpdating = false;
+	}
+}
+
+function OnGetAllSensorsRes(arg)
+{
+	if (arg.HAPI_STATUS == 0)
+	{
+		SENSORINFO_DATA = WEBVAR_JSONVAR_HL_GETALLSENSORS.WEBVAR_STRUCTNAME_HL_GETALLSENSORS;
+
+		if (GetAllSensorsResponse != undefined && 
+			GetAllSensorsResponse != null) GetAllSensorsResponse();
+	}
+
+	isUpdating = false;
+}
+
+function ErrorHandler() 
+{
+	isUpdating = false;
+	alert(eLang.getString("common", "STR_NETWORK_ERROR"));
+	/*
+	parent.SENSORINFO_DATA = null;
+	stopLiveUpdates();
+	*/
+}
+
+function CreateWidget(id)
+{
+	widgets[id] = new widget(id);
+}
+
+function DeleteWidget(id)
+{
+	delete widgets[id];
+}
