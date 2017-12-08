--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/main_imp.js Mon Nov  5 16:04:02 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/main_imp.js Mon Dec  3 11:44:44 2012
@@ -109,6 +109,7 @@
 		try
 		{
 			xmit.get({url:'/rpc/getallsensors.asp', onrcv:OnGetAllSensorsRes, show_progress:false, onerror:ErrorHandler});
+			isUpdating = false;
 		}
 		catch(e)
 		{
@@ -141,8 +142,6 @@
 		if (GetAllSensorsResponse != undefined && 
 			GetAllSensorsResponse != null) GetAllSensorsResponse();
 	}
-
-	isUpdating = false;
 }
 
 function ErrorHandler() 
