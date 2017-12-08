--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 20 20:25:11 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/str/EN/common_str.js Tue Sep 20 20:27:06 2011
@@ -72,8 +72,6 @@
 eLang.common_strings["STR_CONF_AD_CONFIRM1"] ="This slot has already been configured with a role group. Would you like to modify that role group instead?";
 eLang.common_strings["STR_CONF_AD_CONFIRM2"] ="This slot is currently empty. Would you like to add a new role group?";
 eLang.common_strings["STR_CONF_AD_RG_DELETE_SUCCESS"] ="The Role Group has been deleted";
-eLang.common_strings["STR_CONF_AD_RG_ADD_SUCCESS"] ="The Role Group was added successfully";
-eLang.common_strings["STR_CONF_AD_RG_MODIFY_SUCCESS"] ="The modified Role Group successfully";
 eLang.common_strings["STR_CONF_AD_RG_ADV_SAVE_SUCCESS"] ="The Active Directory configuration has been successfully set";
 eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINNAME"] ="Invalid User Domain Name!";
 eLang.common_strings["STR_CONF_AD_ADV_INVALID_DOMAINSRVR1"] ="Invalid Domain Controller Server Address1";
@@ -437,7 +435,11 @@
 eLang.common_strings["STR_CONF_SSL_CERT_ERR3"] = "The SSL certificate validation failed. Please try to upload the certificate and key again.";
 eLang.common_strings["STR_CONF_SSL_CERT_ERR4"] = "Uploading SSL certificate failed. Please try uploading the certificate and key again.";
 eLang.common_strings["STR_CONF_SSL_CERT_ERR5"] = "The SSL certificate or key file size exceeds";
-eLang.common_strings["STR_CONF_SSL_CERT_ERR7"] = "The SSL Certificate is untrusted. Please upload trusted certificate.";
+eLang.common_strings["STR_CONF_SSL_CERT_ERR128"] = "The SSL Certificate is " +
+	"expired. Please upload valid certificate.\nNOTE: Please check the BMC " +
+	"current time in NTP under Configuration menu.";
+eLang.common_strings["STR_CONF_SSL_CERT_ERR129"] = "The SSL Certificate is " +
+	"untrusted. Please upload trusted certificate.";
 eLang.common_strings["STR_CONF_SSL_VALIDATE_ERR"] = "There was a problem while validating SSL Certificate and Key";
 eLang.common_strings["STR_CONF_SSLCERTUPLOAD_ABORT"] = "By navigating to other pages the SSL upload process will be aborted.\nDo you want to continue?";
 eLang.common_strings["STR_CONF_SSLCERTUPLOAD_ALERT"] = "Closing the web session causes the SSL upload process to be aborted!"
