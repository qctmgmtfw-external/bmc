--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Tue Sep 20 19:40:36 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/listgrid.js Tue Sep 20 18:21:52 2011
@@ -588,7 +588,7 @@
 				this.message + "</div>";
 			this.addSingleRow(strMessage);
 			showWait(false);
-		} else if (args.rows.length < 100) {
+		} else if (args.rows.length <= 100) {
 			showWait(true,"Populating");
 			for (r = 0; r < args.rows.length; r++) {
 				lGrid.trace("Row " + r + "disabled::" + args.rows[r].disabled);
