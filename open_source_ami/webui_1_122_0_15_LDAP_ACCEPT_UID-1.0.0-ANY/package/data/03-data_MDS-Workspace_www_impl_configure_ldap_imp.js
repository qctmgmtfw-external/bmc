--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_ldap_imp.js Tue Sep  4 18:16:38 2012
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_ldap_imp.js Fri Sep  7 15:07:04 2012
@@ -287,6 +287,10 @@
 		"STR_CONF_LDAP_SEARCHBASE"), "_txtSearchBase", "", {"maxLength":64},
 		"bigclassicTxtBox");
 
+	txtAttributeOfUserLogin = frm.addTextField(eLang.getString("common", 
+		"STR_CONF_LDAP_ATTRIBUTE_OF_USER_LOGIN"), "_txtAttributeOfUserLogin", "", {"maxLength":5},
+		"smallclassicTxtBox");
+
 	var btnAry = [];
 	btnAry.push(createButton("btnSave", eLang.getString("common",
 		"STR_SAVE"), isLDAPCfgChange));
@@ -316,6 +320,7 @@
 	txtBindDN.value = LDAPCFG_DATA.BINDDN;
 	txtBindPword.value = "";
 	txtSearchBase.value = LDAPCFG_DATA.SEARCHBASE;
+	txtAttributeOfUserLogin.value = LDAPCFG_DATA.ATTRIBUTEOFUSERLOGIN;
 }
 
 /*
@@ -332,6 +337,7 @@
 		txtBindDN.disabled = bopt;
 		txtBindPword.disabled = bopt;
 		txtSearchBase.disabled = bopt;
+		txtAttributeOfUserLogin.disabled = bopt;
 		if(!bopt) {
 			txtIP.focus();
 		} else {
@@ -351,7 +357,7 @@
 		(LDAPCFG_DATA.PORTNUM == txtPort.value) && 
 		(LDAPCFG_DATA.BINDDN == txtBindDN.value) && 
 		("" == txtBindPword.value) && 
-		(LDAPCFG_DATA.SEARCHBASE == txtSearchBase.value)) {
+		(LDAPCFG_DATA.SEARCHBASE == txtSearchBase.value) && (LDAPCFG_DATA.ATTRIBUTEOFUSERLOGIN == txtAttributeOfUserLogin.value)) {
 		return;
 	}
 	validateLDAPCfg();
@@ -396,6 +402,13 @@
 			alert(eLang.getString("common", "STR_INVALID_SEARCHBASE") +
 				eLang.getString("common", "STR_HELP_INFO"));
 			txtSearchBase.focus();
+			return false;
+		}
+
+		if (eVal.isblank(txtAttributeOfUserLogin.value)) {
+			alert(eLang.getString("common", "STR_CONF_LDAP_INVALID_ATTRIBUTE_OF_USERLOGIN") +
+				eLang.getString("common", "STR_HELP_INFO"));
+			txtuserlogin.focus();
 			return false;
 		}
 	}
@@ -418,6 +431,7 @@
 		req.add("BINDDN", txtBindDN.value);
 		req.add("PASSWORD", txtBindPword.value);
 		req.add("SEARCHBASE", txtSearchBase.value);
+		req.add("ATTRIBUTEOFUSERLOGIN", txtAttributeOfUserLogin.value);
 		req.send();
 		delete req;
 	} else {
