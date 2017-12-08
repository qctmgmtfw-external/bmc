--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_active_directory_imp.js Tue Sep 20 20:25:10 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_active_directory_imp.js Tue Sep 20 20:26:15 2011
@@ -417,7 +417,7 @@
 function frmAddRoleGroup(arg)
 {
 	var frm = new form("addRoleGroup","POST","javascript://","general");
-	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName","",{"maxLength":255},"classicTxtBox");
+	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName","",{"maxLength":64},"classicTxtBox");
 	rgDomain = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_DOMAIN"),"_rgDomain","",{"maxLength":255},"bigclassicTxtBox");
 
 	var privVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
@@ -466,14 +466,14 @@
 	}
 	else
 	{
-		alert(eLang.getString("common","STR_CONF_AD_RG_ADD_SUCCESS"));
+		alert(eLang.getString("common","STR_CONF_RG_SAVE_SUCCESS_1"));
 	}
 	closeForm();
 }
 
 function validateRoleGroup()
 {
-	if (!eVal.domainname(rgName.value,0))
+	if (!eVal.str(rgName.value))
 	{
 		alert(eLang.getString("common","STR_CONF_AD_RG_INVALID_RGNAME")+eLang.getString("common","STR_HELP_INFO"));
 		return false;
@@ -521,7 +521,7 @@
 function frmModifyRoleGroup(arg)
 {
 	var frm = new form("modRoleGroup","POST","javascript://","general");
-	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName",arg.rgname,{"maxLength":255},"classicTxtBox");
+	rgName = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_NAME"),"_rgName",arg.rgname,{"maxLength":64},"classicTxtBox");
 	rgDomain = frm.addTextField(eLang.getString("common","STR_CONF_AD_RG_DOMAIN"),"_rgDomain",arg.rgdomain,{"maxLength":255},"bigclassicTxtBox");
 
 	var privVals = {4:"Administrator", 3:"Operator", 2:"User", 5:"OEM Proprietary", 0xf:"No Access"};
@@ -569,7 +569,7 @@
 	}
 	else
 	{
-		alert(eLang.getString("common","STR_CONF_AD_RG_MODIFY_SUCCESS"));
+		alert(eLang.getString("common","STR_CONF_RG_SAVE_SUCCESS_2"));
 	}
 	closeForm();
 }
