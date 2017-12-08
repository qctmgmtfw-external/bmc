--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_nw_bond_imp.js Thu Feb 23 11:47:34 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_nw_bond_imp.js Mon Mar  5 19:08:57 2012
@@ -43,6 +43,7 @@
 	exposeElms([
 		"_chkEnableBond",
 		"_lstBondIfc",
+		"_chkAutoConf",
 		"_btnSave",
 		"_btnReset"
 		]);
@@ -64,6 +65,7 @@
  */
 function _begin()
 {
+	chkAutoConf.checked = true;
 	checkNWBondSupport();
 }
 
@@ -150,22 +152,26 @@
 	chkEnableBond.checked = (NWBONDCFG_DATA.BOND_ENABLE == 1) ? true : false;
 	lstBondIfc.value = NWBONDCFG_DATA.BOND_IFC;
 	enableNWBond();
+	chkAutoConf.checked = true;
 }
 
 function validateNWBondCfg()
 {
+	var autoconf = (chkAutoConf.checked) ? 1 : 0;
 	if ((NWBONDCFG_DATA.BOND_ENABLE != bondEnable) || 
 		(NWBONDCFG_DATA.BOND_IFC != lstBondIfc.value)) {
 		if (NWBONDCFG_DATA.VLAN_ENABLE && !bondEnable) {
-			if (confirm(eLang.getString("common", 
-				"STR_CONF_NW_BOND_SAVE_CONFIRM") + 
-				eLang.getString("common", "STR_CONF_NW_WARNING"))) {
+			if (confirm(eLang.getString("common",
+				"STR_CONF_NW_BOND_CNFM_BOND") + eLang.getString("common",
+				"STR_CONF_NW_BOND_CNFM_AUTO" + autoconf))) {
 				setNWBondCfg();
 			}
 		} else {
-			if (confirm(eLang.getString("common", "STR_CONF_NW_WARNING"))) {
+			if (confirm(eLang.getString("common", "STR_CONF_NW_BOND_CNFM_AUTO" +
+				autoconf))) {
 				setNWBondCfg();
 			}
+			
 		}
 	}
 }
@@ -181,6 +187,7 @@
 		status:""});
 	req.add("BOND_ENABLE", bondEnable);
 	req.add("BOND_IFC", lstBondIfc.value);
+	req.add("AUTO_CONF", (chkAutoConf.checked) ? 1 : 0);
 	req.send();
 	delete req;
 }
