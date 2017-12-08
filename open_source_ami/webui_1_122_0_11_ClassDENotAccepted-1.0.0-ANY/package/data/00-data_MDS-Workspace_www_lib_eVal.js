--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Fri Jun 29 11:36:57 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Fri Jun 29 11:47:15 2012
@@ -195,6 +195,14 @@
 		} else {
 			i = 0;
 		}
+		
+		if (ipv[0]==224  || ipv[0]==240){
+			if (this.verbose) {
+				eLang.getString('err',EXTENDED_ERROR+30);
+			}
+			this.lastErrorNum = EXTENDED_ERROR+30;
+			return false;
+		}
 
 		for (;i < arLen; i++) {
 			if ((ipv[0]==invalidIPs[i][0])&&
