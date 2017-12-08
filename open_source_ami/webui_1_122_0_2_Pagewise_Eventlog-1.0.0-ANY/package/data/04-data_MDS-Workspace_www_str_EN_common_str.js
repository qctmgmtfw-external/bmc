--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 20 19:40:36 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 20 18:21:52 2011
@@ -525,19 +525,21 @@
 /*Event Log*/
 eLang.common_strings["STR_EVENT_LOG_CNT"] ="Event Log: ";
 eLang.common_strings["STR_EVENT_LOG_ENTRIES"] =" event entries";
+eLang.common_strings["STR_EVENT_LOG_PAGES"] ="page(s)";
 eLang.common_strings["STR_EVENT_LOG_HEAD1"] ="Event ID";
 eLang.common_strings["STR_EVENT_LOG_HEAD2"] ="Time Stamp";
 eLang.common_strings["STR_EVENT_LOG_HEAD3"] ="Sensor Name";
 eLang.common_strings["STR_EVENT_LOG_HEAD4"] ="Sensor Type";
 eLang.common_strings["STR_EVENT_LOG_HEAD5"] ="Description";
 eLang.common_strings["STR_EVENT_LOG_TYPE0"] ="All Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE1"] ="Sensor-Specific Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE2"] ="BIOS Generated Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE3"] ="SMI Handler Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE4"] ="System Management Software Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE5"] ="OEM Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE6"] ="Remote Console software Events";
-eLang.common_strings["STR_EVENT_LOG_TYPE7"] ="Terminal Mode Remote Console software Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE1"] ="System Event Records";
+eLang.common_strings["STR_EVENT_LOG_TYPE2"] ="OEM Event Records";
+eLang.common_strings["STR_EVENT_LOG_TYPE3"] ="BIOS Generated Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE4"] ="SMI Handler Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE5"] ="System Management Software Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE6"] ="System Software - OEM Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE7"] ="Remote Console software Events";
+eLang.common_strings["STR_EVENT_LOG_TYPE8"] ="Terminal Mode Remote Console software Events";
 eLang.common_strings["STR_EVENT_LOG_CLEAR_SUCCESS"] ="Sensor Event Log has been cleared";
 eLang.common_strings["STR_EVENT_LOG_CLEAR_CONFIRM"] ="This will clear all the events in the log. Click Ok if you want to proceed?";
 eLang.common_strings["STR_EVENT_LOG_GETVAL"] = "There was a problem while getting event logs";
@@ -726,6 +728,7 @@
 eLang.common_strings["STR_INVALID_DOMAIN"] ="Invalid Domain Name.";
 eLang.common_strings["STR_INVALID_SEARCHBASE"] = "Invalid Search base.";
 eLang.common_strings["STR_INVALID_RGNAME"] ="Invalid Role Group Name.";
+eLang.common_strings["STR_INVALID_PAGENO"] ="Invalid Page Number.";
 eLang.common_strings["STR_NETWORK_ERROR"] ="There is some problem in network connection.";
 
 eLang.common_strings["STR_APP_STR_ALL_DEASSERTED"] = "All deasserted";
