--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Wed Apr 18 13:50:25 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/event_log_imp.js Fri Apr 20 15:57:45 2012
@@ -56,10 +56,12 @@
 		"_rdoClientTzone",
 		"_utcOffset",
 		"_eventLogHolder",
+		"_btnSaveLog",
 		"_btnClearLog"]);
 
 	if (top.user.isAdmin()) {
 		btnClearLog.onclick = clearSEL;
+		btnSaveLog.onclick = saveSEL;
 	} else {
 		disableActions({id: ["_eventTypeSel", "_sensorName"]});
 	}
@@ -590,6 +592,35 @@
 	}
 }
 
+function saveSEL()
+{
+	if (!SELINFO_DATA.length ) {
+		alert(eLang.getString("common", "NO_SEL_STRING"));
+	} else {
+		xmit.get({url:"/rpc/WEBSES/validate.asp", onrcv:function(arg) {
+			if(arg.HAPI_STATUS == 0) {
+				if (top.user.isAdmin()) {
+					xmit.get({url:"/rpc/saveAllSELEntries.asp", onrcv:saveAllSELEntriesRes, status:""});
+				} else {
+					alert(eLang.getString("common","STR_CONF_ADMIN_PRIV"));
+				}
+			} else {
+				parent.gLogout = 1;
+				top.document.cookie = "SessionExpired=true;path=/";
+				parent.location.href = "login.html";
+			}
+		},evalit:false});
+	}
+}
+
+function saveAllSELEntriesRes(arg)
+{
+	if(arg.HAPI_STATUS == 0) {
+		window.location.href='/SELLog';
+	} else {
+		alert(eLang.getString("event_log", "EVENT_LOG_SAVEEVENTS_ERROR"));
+	}
+}
 function movePage(navCtrl)
 {
 	var pgcnt;
@@ -938,4 +969,4 @@
 	m_Max_allowed_SensorSpecific_offset[42] = 1;
 	m_Max_allowed_SensorSpecific_offset[43] = 7;
 	m_Max_allowed_SensorSpecific_offset[44] = 7;
-}
+}

