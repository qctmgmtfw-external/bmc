--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Thu Feb 23 11:47:34 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Wed Mar  7 16:09:16 2012
@@ -206,7 +206,13 @@
 	" As VLAN is enabled for Slave interfaces. \nVLAN can be disabled, " +
 	"Network under Configuration menu.";
 eLang.common_strings["STR_CONF_NW_BOND_NOT_SUPPORT"] = "Network bonding cannot be supported, due to insufficient number of network interfaces.";
-eLang.common_strings["STR_CONF_NW_BOND_SAVE_CONFIRM"] = "Disabling bond will disable the Bonding VLAN configuration.";
+eLang.common_strings["STR_CONF_NW_BOND_CNFM_BOND"] = "Disabling bond will disable the Bonding VLAN configuration.\n";
+eLang.common_strings["STR_CONF_NW_BOND_CNFM_AUTO1"] = "Auto configuration is" +
+	" enabled, So all the services will be restarted automatically. Click OK" +
+	" to proceed.";
+eLang.common_strings["STR_CONF_NW_BOND_CNFM_AUTO0"] = "Auto configuration is" +
+	"disabled, So the interfaces for services can be configured via IPMI " +
+	"command. Click OK to proceed.";
 eLang.common_strings["STR_CONF_NW_BOND_SAVE_SUCCESS"] = "The Network Bonding configuration are saved successfully.";
 
 /*Configure Network Link*/
