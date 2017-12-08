--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Fri Feb  3 16:19:05 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Mon Feb  6 15:46:16 2012
@@ -221,13 +221,12 @@
 eLang.common_strings["STR_CONF_PHY_SAVE_SUCCESS"] = "The Network Link configuration has been saved successfully.";
 
 /*Configure DNS*/
+eLang.common_strings["STR_CONF_DNS_INVALID_HOST"] = "Invalid Host Name";
 eLang.common_strings["STR_CONF_DNS_INVALID_DOMAIN"] = "Invalid Domain Name";
-eLang.common_strings["STR_CONF_DNS_INVALID_V4DNS1"] = "Invalid IPv4 Preferred DNS Server";
-eLang.common_strings["STR_CONF_DNS_INVALID_V4DNS2"] = "Invalid IPv4 Alternate DNS Server";
-eLang.common_strings["STR_CONF_DNS_INVALID_V6DNS1"] = "Invalid IPv6 Preferred DNS Server";
-eLang.common_strings["STR_CONF_DNS_INVALID_V6DNS2"] = "Invalid IPv6 Alternate DNS Server";
-eLang.common_strings["STR_CONF_DNS_DIFF_DNS"] = "The preferred DNS Server should be different from Alternate DNS Server";
-eLang.common_strings["STR_CONF_DNS_INVALID_HOST"] = "Invalid Host Name";
+eLang.common_strings["STR_CONF_DNS_INVALID_DNS"] = "Invalid DNS Server Address";
+eLang.common_strings["STR_CONF_DNS_V6DISABLE"] = "\nNOTE: IPv6 is disabled in network configuration.";
+eLang.common_strings["STR_CONF_DNS_BLANK"] = "DNS Server addresses cannot be blank.";
+eLang.common_strings["STR_CONF_DNS_DIFF"] = "DNS Server addresses should be different.";
 
 /*Configure NTP*/
 eLang.common_strings["STR_CONF_NTP_GETVAL"] = "There was a problem while getting NTP configuration";
