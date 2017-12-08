--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/dashboard_imp.js Thu Aug 30 13:29:34 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/dashboard_imp.js Thu Aug 30 18:19:53 2012
@@ -217,21 +217,35 @@
 		LANCFG_DATA = WEBVAR_JSONVAR_GETALLNETWORKCFG.WEBVAR_STRUCTNAME_GETALLNETWORKCFG;
 		tblPermissionDeny.style.display = "none";
 		tblNetworkInfo.style.display = "";
-
-		IPAddr = getIPAddress();
+		IPAddr = top.fnCookie.read('BMC_IP_ADDR');
+
+	//	IPAddr = getIPAddress();
 		for (i=0; i<LANCFG_DATA.length; i++) {
-			if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr == IPAddr)) {
-				break;
-			}
-		}
-		if (i >= LANCFG_DATA.length) {
-			for (i=0; i<LANCFG_DATA.length; i++) {
-				if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr 
-					!= eLang.getString("common", "STR_IPV4_ADDR0"))) {
+		//	if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr == IPAddr)) {
+		//		break;
+		//	}
+			if(eVal.ip(IPAddr)) {
+				if (((LANCFG_DATA[i].lanEnable == 1) || (LANCFG_DATA[i].v6Enable == 1)) && (LANCFG_DATA[i].v4IPAddr == IPAddr))
 					break;
-				}
-			}
-		}
+			} else {
+				if (((LANCFG_DATA[i].lanEnable == 1) || (LANCFG_DATA[i].v6Enable == 1)) && (LANCFG_DATA[i].v6IPAddr == IPAddr))
+					break;
+			}
+		}
+
+	//	if (i >= LANCFG_DATA.length) {
+	//		for (i=0; i<LANCFG_DATA.length; i++) {
+	//			if ((LANCFG_DATA[i].lanEnable == 1) && (LANCFG_DATA[i].v4IPAddr 
+	//				!= eLang.getString("common", "STR_IPV4_ADDR0"))) {
+	//				break;
+	//			}
+	//		}
+	//	}
+
+
+
+
+
 
 		macAddress.innerHTML = LANCFG_DATA[i].macAddress;
 		ipv4Address.innerHTML = LANCFG_DATA[i].v4IPAddr;
