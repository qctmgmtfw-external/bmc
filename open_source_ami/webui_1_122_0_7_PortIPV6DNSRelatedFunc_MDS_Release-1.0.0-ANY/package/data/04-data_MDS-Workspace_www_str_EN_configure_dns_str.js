--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/configure_dns_str.js Fri Feb  3 16:19:05 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/configure_dns_str.js Fri Feb  3 19:23:25 2012
@@ -1,7 +1,7 @@
 //;*****************************************************************;
 //;*****************************************************************;
 //;**                                                             **;
-//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2010        **;
+//;**     (C) COPYRIGHT American Megatrends Inc. 2008-2011        **;
 //;**                     ALL RIGHTS RESERVED                     **;
 //;**                                                             **;
 //;**  This computer software, including display screens and      **;
@@ -33,19 +33,22 @@
 
 eLang.configure_dns_strings = {};
 
-eLang.configure_dns_strings["CFG_DNS_DNS2"] = "Alternate DNS Server";
+eLang.configure_dns_strings["CFG_DNS_SERVER3"] = "DNS Server3";
+eLang.configure_dns_strings["CFG_DNS_SERVER2"] = "DNS Server2";
+eLang.configure_dns_strings["CFG_DNS_SERVER1"] = "DNS Server1";
+eLang.configure_dns_strings["CFG_DNS_DNS_TITLE"] = "Domain Name Server Configuration";
+eLang.configure_dns_strings["CFG_DNS_PRIORITY"] = "IP Priority";
 eLang.configure_dns_strings["CFG_DNS_DOMAIN_NAME"] = "Domain Name";
-eLang.configure_dns_strings["CFG_DNS_DNS1"] = "Preferred DNS Server";
 eLang.configure_dns_strings["CFG_DNS_DDNS"] = "Direct Dynamic DNS";
 eLang.configure_dns_strings["CFG_DNS_DHCP_FQDN"] = "DHCP Client FQDN";
-eLang.configure_dns_strings["CFG_DNS_IPV4_TITLE"] = "IPv4 Domain Name Server Configuration";
 eLang.configure_dns_strings["CFG_DNS_HOST_TITLE"] = "Host Configuration";
 eLang.configure_dns_strings["CFG_DNS_BMC_REGISTER"] = "Register BMC";
+eLang.configure_dns_strings["CFG_DNS_V6"] = "IPv6";
 eLang.configure_dns_strings["CFG_DNS_HOST_SETTING"] = "Host Settings";
 eLang.configure_dns_strings["CFG_DNS_DOMAIN_SETTING"] = "Domain Settings";
+eLang.configure_dns_strings["CFG_DNS_V4"] = "IPv4";
 eLang.configure_dns_strings["CFG_DNS_RESET"] = "Reset";
 eLang.configure_dns_strings["CFG_DNS_LAN_INTERFACE"] = "LAN Interface";
-eLang.configure_dns_strings["CFG_DNS_IPV6_TITLE"] = "IPv6 Domain Name Server Configuration";
 eLang.configure_dns_strings["CFG_DNS_TITLE"] = "DNS Server Settings";
 eLang.configure_dns_strings["CFG_DNS_DOMAIN_TITLE"] = "Domain Name Configuration";
 eLang.configure_dns_strings["CFG_DNS_DESC"] = "Manage DNS settings of the device.";
