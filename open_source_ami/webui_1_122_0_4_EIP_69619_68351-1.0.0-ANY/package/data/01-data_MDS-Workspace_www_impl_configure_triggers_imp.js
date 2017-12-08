--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_triggers_imp.js Tue Sep 20 22:27:27 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_triggers_imp.js Tue Sep 20 22:27:58 2011
@@ -49,8 +49,12 @@
 	if(top.user.isAdmin()) {
 		btnSave.onclick = validateTimeStamp;
 		btnReset.onclick = reloadTriggerCfg;
-	} else {
+	} else if (top.user.isOperator()) {
 		disableActions();
+	} else {
+		alert(eLang.getString("common","STR_PERMISSION_DENIED"));
+		location.href = "dashboard.html";
+		return;
 	}
 	_begin();
 }
