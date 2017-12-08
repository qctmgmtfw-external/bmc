--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_users_imp.js Thu Sep  8 15:23:14 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_users_imp.js Fri Sep  9 17:51:56 2011
@@ -177,6 +177,7 @@
 				tUsrCnt++;
 			} else {
 				userstatustodisplay = "~";
+				userprivtodisplay = "~";
 				if (SNMP_SUPPORT) {
 					snmpstatustodisplay = "~";
 				}
@@ -234,10 +235,7 @@
 function interpretPrivileges(privbyte)
 {
 	var privlevel = getbits(privbyte,3,0);
-	if (privbyte==0xf)		//Use ~ char to indicate free slot so it will sort alphabetically
-		return "~";
-	else
-		return IPMIPrivileges[privlevel];
+	return IPMIPrivileges[privlevel];
 }
 
 function doAddUser()
