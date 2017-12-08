--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Tue Sep 20 20:25:10 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Tue Sep 20 20:26:37 2011
@@ -133,11 +133,10 @@
 	},
 	str : function(n)
 	{
-		var filter =  /^([a-zA-Z0-9_\-])+$/;
-		var v = new String(n);
-		if (v.match( (filter)))
+		var filter = /^([a-zA-Z0-9_\-])+$/;
+		if (n.match(filter)) {
 			return true;
-		delete v;
+		}
 		return false;
 	},
 	trim : function(stringToTrim)
