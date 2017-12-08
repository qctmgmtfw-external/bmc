--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Thu Apr  5 12:37:55 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Thu Apr  5 17:46:28 2012
@@ -85,6 +85,47 @@
 }
 
 /*
+ * It will invoke the RPC method to get the network bonding configuration.
+ * Once it get response from RPC, on receive method will be called automatically.
+ */
+function getNWBondCfg()
+{
+	xmit.get({url:"/rpc/getnwbondcfg.asp", onrcv:getNWBondCfgRes, status:""});
+}
+
+/*
+ * This is the response function for getNWBondCfg RPC. 
+ * Need to check HAPI_STATUS, intimate end user if it returns non-zero value.
+ * If success, move the response data to the global variable and invoke the 
+ * method to load the data value in UI. 
+ * @param arg object, RPC response data from xmit library
+ */
+function getNWBondCfgRes(arg)
+{
+	var errstr;		//Error string
+	if(arg.HAPI_STATUS != 0) {
+		errstr = eLang.getString("common", "STR_CONF_NW_BOND_GETVAL");
+		errstr += (eLang.getString("common", "STR_IPMI_ERROR") + 
+			GET_ERROR_CODE(arg.HAPI_STATUS));
+		alert(errstr);
+		
+	} else {
+		NWBONDCFG_DATA = WEBVAR_JSONVAR_GETNWBONDCFG.WEBVAR_STRUCTNAME_GETNWBONDCFG[0];
+		if(NWBONDCFG_DATA.BOND_ENABLE) 
+			disableV6();
+	}
+}
+
+/*
+ * It will disable the v6 user controls.
+ */
+function disableV6()
+{
+	chkV6Enable.disabled = true;
+	chkV6IPSource.disabled = true;
+}
+
+/*
  * It will fill LAN Interface list box with eth Interface string and LAN
  * Channel number.
  */
@@ -442,6 +483,8 @@
 	chkV6IPSource.disabled = (chkV6Enable.disabled || (!chkV6Enable.checked));
 	DHCPEnable((chkV6Enable.disabled || chkV6IPSource.disabled || 
 		chkV6IPSource.checked), "V6");
+	if(checkProjectCfg("NWBONDING"))
+		getNWBondCfg();
 }
 
 /*
@@ -475,4 +518,4 @@
 	bopt = !chkVLANEnable.checked;
 	txtVLANId.disabled = bopt;
 	txtVLANPriority.disabled = bopt;
-}
+}
