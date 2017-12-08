--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Tue Sep 27 18:53:37 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_network_imp.js Tue Sep 27 18:53:52 2011
@@ -318,7 +318,8 @@
  */
 function setNetworkInfo()
 {
-	if(confirm(eLang.getString("common", "STR_CONF_NW_SAVE_CONFIRM"))) {
+	if(confirm(eLang.getString("common", "STR_CONF_NW_SAVE_CONFIRM") + 
+			eLang.getString("common", "STR_CONF_NW_WARNING"))) {
 		/* Disable the SAVE button here till the request is served */
 		btnSave.disabled = true;
 		var req = new xmit.getset({url:"/rpc/setnwconfig.asp", 
