--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Tue Sep  4 18:16:42 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Wed Sep  5 11:31:13 2012
@@ -570,6 +570,7 @@
 	{
 		var v = "";
 		var cn = /^(cn=)[a-z]{1}[\w\-\.]{2,63}$/i;
+		var uid = /^(uid=)[a-z]{1}[\w\-\.]{2,63}$/i;
 		var ou = /^(ou=)[a-z]{1}[\w\-\.]{1,63}$/i;
 		var dc = /^(dc=)[a-z]{1}[\w\-\.]{1,63}$/i;
 		var o = /^(o=)[a-z]{1}[\w\-\.]{1,63}$/i;
@@ -585,13 +586,13 @@
 		}
 
 		if (property == "binddn") {
-			if (!cn.test(v[0])) {
+			if (!(cn.test(v[0]) || uid.test(v[0]))) {
 				return false;
 			}
 		}
 
 		for (i = min-1; i < v.length; i++) {
-			if ((!dc.test(v[i])) && (!ou.test(v[i])) && (!cn.test(v[i]))) {
+			if ((!dc.test(v[i])) && (!ou.test(v[i])) && (!(cn.test(v[i]) || uid.test(v[i])))) {
 				if (!((property == "searchbase" ) && (o.test(v[i])))) {
 					return false;
 				}
