--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 27 18:53:38 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 27 18:54:13 2011
@@ -171,7 +171,7 @@
 eLang.common_strings["STR_CONF_NW_SETVAL"] = "There was a problem while setting Network configuration.\n";
 eLang.common_strings["STR_CONF_NW_GETERR1"] = "Could not get network settings";
 eLang.common_strings["STR_CONF_NW_SETERR1"] = "Unable to set the supplied network parameters ";
-eLang.common_strings["STR_CONF_NW_SAVE_CONFIRM"] = "This function may change the IP address of the device, and you may lose connectivity in this browser session.\nPlease reconnect using a new browser session after applying the changes.\nDo you want to proceed?";
+eLang.common_strings["STR_CONF_NW_SAVE_CONFIRM"] = "This function may change the IP address of the device, and you may lose connectivity in this browser session.\nPlease reconnect using a new browser session after applying the changes.";
 eLang.common_strings["STR_CONF_NW_RESET_TITLE"] = "Network configuration has been reset";
 eLang.common_strings["STR_CONF_NW_RESET_DESC"] = "Network configuration has been reset successfully. It will take few seconds to bring up the interface again. Please close this browser session and open a new browser session to connect to the device with new IP. ";
 eLang.common_strings["STR_CONF_NW_ERR_222"] = "Either IP Address or Default Gateway IP is out of range.";
@@ -187,6 +187,12 @@
 eLang.common_strings["STR_CONF_NW_INVALID_V6SUBNET"] = "Invalid IPv6 Subnet prefix length";
 eLang.common_strings["STR_CONF_NW_INVALID_VLANID"] = "Invalid VLAN ID";
 eLang.common_strings["STR_CONF_NW_INVALID_VLANPRIORITY"] = "Invalid VLAN Priority";
+eLang.common_strings["STR_CONF_NW_WARNING"] = "\n\nWARNING! Network changes may " +
+	"affect the existing services availability on the network and they may " +
+	"become unavailable on the newly configured network. Please refer to " +
+	"'Services Configuration' page to modify the services availability " +
+	"criteria after the network configuration change. " +
+	"\n\nDo you wish to continue with this change?";
 
 /*Configure Network Bonding*/
 eLang.common_strings["STR_NW_BOND_IFC_1"] = "eth0";
@@ -200,7 +206,7 @@
 	" As VLAN is enabled for Slave interfaces. \nVLAN can be disabled, " +
 	"Network under Configuration menu.";
 eLang.common_strings["STR_CONF_NW_BOND_NOT_SUPPORT"] = "Network bonding cannot be supported, due to insufficient number of network interfaces.";
-eLang.common_strings["STR_CONF_NW_BOND_SAVE_CONFIRM"] = "Disabling bond will disable the Bonding VLAN configuration. Click OK to proceed.";
+eLang.common_strings["STR_CONF_NW_BOND_SAVE_CONFIRM"] = "Disabling bond will disable the Bonding VLAN configuration.";
 eLang.common_strings["STR_CONF_NW_BOND_SAVE_SUCCESS"] = "The Network Bonding configuration are saved successfully.";
 
 /*Configure Network Link*/
