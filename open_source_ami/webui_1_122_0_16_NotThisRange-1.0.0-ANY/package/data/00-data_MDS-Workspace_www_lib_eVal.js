--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Thu Sep 27 14:36:57 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Thu Sep 27 16:19:27 2012
@@ -317,6 +317,10 @@
 					//alert ("FF00->Broadcast IP Not allowed");
 					return true;
 				}
+				if (i==0 && decip >= 0x1000 && decip <= 0x2000) {
+					//alert ("Quanta Not allowed");
+					return true;
+				}
 				if(!allowlocal) {
 					if (i==0 && decip == 0xFE80) {
 						//alert ("FE80->Link local IP Not allowed");
