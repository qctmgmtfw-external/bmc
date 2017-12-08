--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/dashboard_imp.js Fri Mar 23 19:22:04 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/dashboard_imp.js Fri Mar 23 19:28:21 2012
@@ -218,10 +218,18 @@
 		tblPermissionDeny.style.display = "none";
 		tblNetworkInfo.style.display = "";
 
+		IPAddr = getIPAddress();
 		for (i=0; i<LANCFG_DATA.length; i++) {
-			if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr 
-				!= eLang.getString("common", "STR_IPV4_ADDR0"))) {
+			if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr == IPAddr)) {
 				break;
+			}
+		}
+		if (i >= LANCFG_DATA.length) {
+			for (i=0; i<LANCFG_DATA.length; i++) {
+				if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr 
+					!= eLang.getString("common", "STR_IPV4_ADDR0"))) {
+					break;
+				}
 			}
 		}
 
@@ -259,6 +267,26 @@
 	return;
 	}
 }
+
+function getIPAddress()
+{
+	var externalIP = "";	//External Client IP
+	var windowURL;			//Window URL with full path
+	
+	windowURL = window.location.href.split("/");	
+	windowURL = windowURL[2];
+
+	if (windowURL.indexOf("]") != -1) {
+		externalIP = windowURL.split("]");
+		externalIP = externalIP[0] + "]";
+	} else {
+		externalIP = windowURL.split(":");
+		externalIP = externalIP[0];
+	}
+	return externalIP;
+}
+
+
 
 IMG_WARNING = "<img src='../res/status_warning.png' title='Warning' width='14' height='14' border='0'/>";
 IMG_OK = "<img src='../res/status_ok.png' width='14' title='Normal' height='14' border='0'/>";
