--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/web_alerts.js Tue Sep 20 19:40:35 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/web_alerts.js Tue Sep 20 18:21:52 2011
@@ -382,6 +382,9 @@
 		logReportTable.clear();
 		logReportTable.loadFromJson(web_alerts.tblJSON);
 		$("logReportHolder").appendChild(logReportTable.table);
+		if (isIE) {
+			logReportTable.container.header.style.position = "";
+		}
 	},
 
 	/*
