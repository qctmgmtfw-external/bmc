--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/sensor_readings_imp.js Wed Aug 29 13:49:44 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/sensor_readings_imp.js Wed Aug 29 13:55:40 2012
@@ -185,7 +185,8 @@
 		wStatus = (parent.widgets[id]==null || parent.widgets[id]=="");
 		if(wStatus)
 		{
-			parent.widgets[id] = new parent.widget(id);
+			//parent.widgets[id] = new parent.widget(id);
+			parent.CreateWidget(id);
 			if(parent.widgets[id].container!=undefined)
 			{
 				widgetStatus.innerHTML = "On";
@@ -193,14 +194,17 @@
 			}
 			else
 			{
-				delete parent.widgets[id]; //clear mem leak
+				//delete parent.widgets[id]; //clear mem leak
+				parent.DeleteWidget(id);
 				widgetStatus.innerHTML = "Off";
 				toggleWidget.innerHTML = "On";
 			}
 		}else
 		{
+			parent.StopPolling();
 			parent.widgets[id].close();
-			delete parent.widgets[id]; //clear mem leak
+			parent.DeleteWidget(id);
+			//delete parent.widgets[id]; //clear mem leak
 			widgetStatus.innerHTML = "Off";
 			toggleWidget.innerHTML = "On";
 		}
