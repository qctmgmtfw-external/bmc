--- .pristine/libldapconf-1.11.0-src/data/ldapconf.c Tue Sep  4 18:16:29 2012
+++ source/libldapconf-1.11.0-src/data/ldapconf.c Fri Sep  7 15:23:48 2012
@@ -105,6 +105,14 @@
 	else
 		ldapconfig->PortNum = (unsigned short)atol(value);
 
+	/* Read Attribute of user login */
+	ret = get_keyvaluepair(ldap_config_data, entries_count, LDAP_CFG_STR_ATTRIBUTE_OF_USERLOGIN, value, STR_CASE_INSENSITIVE);
+
+	if( ret!=0)
+		strcpy((char *)ldapconfig->AttributeOfUserLogin, DEFAULT_LDAP_ATTRIBUTE_OF_USERLOGIN);/*Default value: cn*/
+	else
+		strcpy((char *)ldapconfig->AttributeOfUserLogin, value);
+
 	free(ldap_config_data);
 	return 0;	
 }
@@ -304,6 +312,10 @@
 	ret = set_keyvaluepair(&ldap_config_data, &entries_count, LDAP_CFG_PORT_NUM,							port_num, STR_CASE_INSENSITIVE );
 	if ( ret != 0 ){ free(ldap_config_data);  return 1; }
 
+	/* Attribute of user login */
+	ret = set_keyvaluepair(&ldap_config_data, &entries_count, LDAP_CFG_STR_ATTRIBUTE_OF_USERLOGIN, (char *)ldapconfig->AttributeOfUserLogin, STR_CASE_INSENSITIVE );
+	if ( ret != 0 ){ free(ldap_config_data);  return 1; }
+
 	/* Default key values */
 	ret = add_default_keyvalues(&ldap_config_data, &entries_count);
 	if ( ret != 0 ){ free(ldap_config_data);  return 1; }
