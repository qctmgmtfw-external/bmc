--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/system_audit_log_imp.js Thu Sep  8 15:23:14 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/system_audit_log_imp.js Fri Sep  9 17:50:02 2011
@@ -169,33 +169,35 @@
 
 function loadCustomPageElements()
 {
-	var lgHeight = parent.$('pageFrame').offsetHeight-240;
+	var lgHeight = parent.$("pageFrame").offsetHeight-240;
 	if(lgHeight < 45)
 		lgHeight = 45;
 	logTable = listgrid({
-		w				: '100%',
-		h				: (lgHeight)+'px',
+		w : "100%",
+		h : (lgHeight)+"px",
+		msg : eLang.getString("common", "NO_SAL_STRING"),
 		doAllowNoSelect : false
 	});
 
 	//add the list grid to the body division
 	logHolder.appendChild(logTable.table);
 
-	tblJSON = {
-		cols:[
-		{text:eLang.getString('common',"STR_LOG_EVENT_ID"), fieldName:'event_id', fieldType:2, w:"7%", textAlign:'center'},
-		{text:eLang.getString('common',"STR_LOG_TIMESTAMP"), fieldName:'timestamp', w:"18%", textAlign:'center'},
-		{text:eLang.getString('common',"STR_LOG_HOSTNAME"), fieldName:'hostname', w:"25%", textAlign:'center'},
-		{text:eLang.getString('common',"STR_LOG_DESCRIPTION"), fieldName:'description', w:"50%"}
-		]
-	};
+	tblJSON = {cols:[
+		{text:eLang.getString("common","STR_LOG_EVENT_ID"), fieldType:2, w:"7%",
+			textAlign:"center"},
+		{text:eLang.getString("common","STR_LOG_TIMESTAMP"), w:"18%", 
+			textAlign:"center"},
+		{text:eLang.getString("common","STR_LOG_HOSTNAME"), w:"25%",
+			textAlign:"center"},
+		{text:eLang.getString("common","STR_LOG_DESCRIPTION"), w:"50%"}
+		]};
 
 	logTable.loadFromJson(tblJSON);
 }
 
 function fillListGrid(data, str)
 {
-	var record = data.split('\n');
+	var record = data.split("\n");
 	var JSONRows = new Array();
 	var date_col = 0;
 
