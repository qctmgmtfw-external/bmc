--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_nw_bond_imp.js Tue Sep 27 18:53:37 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_nw_bond_imp.js Tue Sep 27 18:53:59 2011
@@ -158,11 +158,14 @@
 		(NWBONDCFG_DATA.BOND_IFC != lstBondIfc.value)) {
 		if (NWBONDCFG_DATA.VLAN_ENABLE && !bondEnable) {
 			if (confirm(eLang.getString("common", 
-				"STR_CONF_NW_BOND_SAVE_CONFIRM"))) {
+				"STR_CONF_NW_BOND_SAVE_CONFIRM") + 
+				eLang.getString("common", "STR_CONF_NW_WARNING"))) {
 				setNWBondCfg();
 			}
 		} else {
-			setNWBondCfg();
+			if (confirm(eLang.getString("common", "STR_CONF_NW_WARNING"))) {
+				setNWBondCfg();
+			}
 		}
 	}
 }
