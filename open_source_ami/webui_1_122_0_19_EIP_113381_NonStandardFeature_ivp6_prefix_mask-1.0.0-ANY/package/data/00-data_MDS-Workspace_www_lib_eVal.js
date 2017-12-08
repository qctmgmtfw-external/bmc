--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Wed Jan 30 11:31:18 2013
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/lib/eVal.js Thu Jan 31 14:51:33 2013
@@ -321,12 +321,105 @@
 					//alert ("Quanta Not allowed");
 					return true;
 				}
+				if (i==0 && decip >= 0xFF00) {
+					//alert ("FF00->Broadcast IP Not allowed");
+					return true;
+				}
+				
 				if(!allowlocal) {
 					if (i==0 && decip == 0xFE80) {
 						//alert ("FE80->Link local IP Not allowed");
 						return true;
 					}
-				}				
+				}
+				if(!allowlocal) {
+					if (i==0 && decip == 0xFE80) {
+						//alert ("FE80->Link local IP Not allowed");
+						return true;
+					}
+				}
+				
+				if(!allowlocal){
+					if( (decip & 0xF000) == 0x1000)  //1000::/4
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFF00) == 0x0000)  //0000::/8
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFF00) == 0x0100)  //0100::/8
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFE00) == 0x0100)  //0200::/7
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFC00) == 0x0100)  //0400::/6
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xF800) == 0x0800)  //0200::/5
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFE00) == 0x0100)  //0200::/7
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xE000) == 0x4000)  //4000::/3
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xE000) == 0x6000)  //6000::/3
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xE000) == 0x8000)  //8000::/3
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xE000) == 0xA000)  //A000::/3
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xE000) == 0xC000)  //C000::/3
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xF000) == 0xE000)  //E000::/4
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xF800) == 0xF000)  //F000::/5
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFC00) == 0xF800)  //F800::/6
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFE00) == 0xFC00)  //FC00::/7
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFF00) == 0xFF00)  //FF00::/8
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFF80) == 0xFE00)  //FE00::/9
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFFC0) == 0xFE80)  //FE80::/10
+					return true;
+				}
+				if(!allowlocal){
+					if( (decip & 0xFFC0) == 0xFEC0)  //FEC0::/10
+					return true;
+				}
+				
 				if(!allow00) {
 					if (i == 0 && decip == 0) {
 						//alert ("First octet should not be zero");
