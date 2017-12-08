--- .pristine/libldapconf-1.11.0-src/data/GenericLdapAuth.c Tue Sep  4 18:16:29 2012
+++ source/libldapconf-1.11.0-src/data/GenericLdapAuth.c Fri Sep  7 14:23:58 2012
@@ -373,8 +373,8 @@
 		TDBG("LDAP secret user bind fails\n");
 		return( -1 );
 	}
-	/* defining the filter which will narrow the user search like cn=%uname */
-	snprintf(Filter, ARRAY_SIZE, "cn=%s", userName);
+	/* defining the filter which will narrow the user search like cn=%uname or uid=%uname */
+	snprintf(Filter, ARRAY_SIZE, "%s=%s",genericldap_cfg->AttributeOfUserLogin, userName);
 
 	/* search for all entries with uname */
 	if ( ldap_search_s( Templd, (char *)genericldap_cfg->SearchBase , LDAP_SCOPE_SUBTREE, Filter, NULL, 0, &Result ) != LDAP_SUCCESS )
