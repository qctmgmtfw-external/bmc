--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_remote_session_imp.js	Tue Jul 10 13:50:20 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_remote_session_imp.js	Wed Jul 11 13:18:26 2012
@@ -31,6 +31,8 @@
 
 var RMTSESS_CFG;		//It holds get RPC Remote session configuration response
 
+var KEYBOARD_LANG = false;              //It holds the feature of physical keyboard language support
+
 /*
  * This function will be called when its corresponding page gets loaded.
  * It will expose all the user controls and checks for user privilege.
@@ -39,6 +41,8 @@
 function doInit()
 {
 	exposeElms(["_chkKVMEncrypt",
+	    "_rowKeyboardLang",
+	    "_lstKeyboardLang",		            
 		"_chkMediaEncrypt",
 		"_lstVMAttach",
 		"_btnSave",
@@ -59,6 +63,11 @@
  */
 function _begin()
 {
+	KEYBOARD_LANG = checkProjectCfg("KB_LANG_SELECT_SUPPORT");
+	if (KEYBOARD_LANG) {
+        rowKeyboardLang.className = "visibleRow";
+        fillKeyboardLanguage();
+	}	
 	fillAttachMode();
 	getRemoteSessionCfg();
 }
@@ -98,6 +107,9 @@
  */
 function reloadRemoteSessionCfg()
 {
+	if (KEYBOARD_LANG) {
+        lstKeyboardLang.value = RMTSESS_CFG.KEYBOARDLANG;
+	}
 	chkKVMEncrypt.checked = RMTSESS_CFG.KVMENCRYPTION?true:false;
 	chkMediaEncrypt.checked = RMTSESS_CFG.MEDIAENCRYPTION?true:false;
 	lstVMAttach.value = RMTSESS_CFG.VMEDIAATTACH;
@@ -109,6 +121,12 @@
  */
 function validateRemoteSessionCfg()
 {
+	if (KEYBOARD_LANG) {
+        if (RMTSESS_CFG.KEYBOARDLANG != lstKeyboardLang.value) {
+                setRemoteSessionCfg();
+                return;
+        }
+	}
 	if ((RMTSESS_CFG.KVMENCRYPTION == chkKVMEncrypt.checked) &&
 		(RMTSESS_CFG.MEDIAENCRYPTION == chkMediaEncrypt.checked) &&
 		(RMTSESS_CFG.VMEDIAATTACH == lstVMAttach.value)) {
@@ -128,6 +146,9 @@
 	if (confirm(eLang.getString("common", "STR_CONF_RMT_SESS_CONFIRM"))) {
 		req = new xmit.getset({url:"/rpc/setremotesession.asp", 
 			onrcv:setRemoteSessionCfgRes, status:""});
+		if (KEYBOARD_LANG) {
+			req.add("KEYBOARDLANG", lstKeyboardLang.value);
+		}		
 		req.add("KVMENCRYPTION", chkKVMEncrypt.checked?1:0);
 		req.add("MEDIAENCRYPTION", chkMediaEncrypt.checked?1:0);
 		req.add("VMEDIAATTACH", lstVMAttach.value);
@@ -158,6 +179,19 @@
 }
 
 /*
+ * It will fill keyboard supported languages in the list box.
+ */
+function fillKeyboardLanguage() {
+        var index                               //loop counter
+        lstKeyboardLang.innerHTML = "";
+        for (index = 0; index < 4; index++) {
+                lstKeyboardLang.add(new Option(eLang.getString("common",
+                        "STR_CONF_RMT_SESS_KEYLANG_" + index), eLang.getString("common",
+                        "STR_CONF_RMT_SESS_KEYLANG_INDEX_" + index)), isIE?index:null);
+        }
+}
+
+/*
  * It will fill attach mode types of Virtual media in the list box.
  */
 function fillAttachMode()
