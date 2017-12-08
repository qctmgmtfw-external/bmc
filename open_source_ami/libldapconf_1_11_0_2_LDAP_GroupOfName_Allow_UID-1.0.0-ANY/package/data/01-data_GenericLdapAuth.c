--- .pristine/libldapconf-1.11.0-src/data/GenericLdapAuth.c Sat Sep  8 16:16:51 2012
+++ source/libldapconf-1.11.0-src/data/GenericLdapAuth.c Sat Sep  8 17:08:27 2012
@@ -484,7 +484,7 @@
 		{
 			TINFO("gettting members for group: %s\n",LDAPcfg[i].LDAPRoleGroupNameStr);
 			
-			tmpret = snprintf( tmpStr, LDAP_MAX_QUERY_LENGTH, "%s%s%s", OPENLDAP_OBJECTCLASS_GROUP_CN_QUERY_HEAD, LDAPcfg[i].LDAPRoleGroupNameStr, "))" );
+			tmpret = snprintf( tmpStr, LDAP_MAX_QUERY_LENGTH, "%s%s%s%s%s", OPENLDAP_OBJECTCLASS_GROUP_QUERY_HEAD,genericldap_cfg->AttributeOfUserLogin,"=" ,LDAPcfg[i].LDAPRoleGroupNameStr, "))" );
 
 			if(tmpret >= LDAP_MAX_QUERY_LENGTH)
 			{
