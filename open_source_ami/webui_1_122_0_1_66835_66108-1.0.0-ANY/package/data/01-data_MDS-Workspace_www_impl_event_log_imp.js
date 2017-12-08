--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Thu Sep  8 15:23:14 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Fri Sep  9 17:50:20 2011
@@ -77,12 +77,13 @@
 
 function loadCustomPageElements()
 {
-	var lgHeight = parent.$("pageFrame").offsetHeight-235;
+	var lgHeight = parent.$("pageFrame").offsetHeight - 250;
 	if(lgHeight < 45)
 		lgHeight = 45;
 	eventLogTable = listgrid({
 		w : "100%",
-		h : (lgHeight) + "px",
+		h : lgHeight + "px",
+		msg : eLang.getString("common", "NO_SEL_STRING"),
 		doAllowNoSelect : false
 	});
 
@@ -90,12 +91,12 @@
 	eventLogHolder.appendChild(eventLogTable.table);
 
 	tblJSON = {cols:[
-		{text:eLang.getString("common","STR_EVENT_LOG_HEAD1"), fieldType:2, 
+		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD1"), fieldType:2, 
 			w:"7%"},
-		{text:eLang.getString("common","STR_EVENT_LOG_HEAD2"), w:"18%"},
-		{text:eLang.getString("common","STR_EVENT_LOG_HEAD3"), w:"23%"},
-		{text:eLang.getString("common","STR_EVENT_LOG_HEAD4"), w:"15%"},
-		{text:eLang.getString("common","STR_EVENT_LOG_HEAD5"), w:"37%"}
+		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD2"), w:"18%"},
+		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD3"), w:"23%"},
+		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD4"), w:"15%"},
+		{text:eLang.getString("common", "STR_EVENT_LOG_HEAD5"), w:"37%"}
 		]};
 
 	eventLogTable.loadFromJson(tblJSON);
