--- .pristine/libldapconf-1.11.0-src/data/ldapconf.h Sat Sep  8 16:16:51 2012
+++ source/libldapconf-1.11.0-src/data/ldapconf.h Sat Sep  8 17:07:53 2012
@@ -42,6 +42,7 @@
 #define	RAC_PRIV_VALID_BIT				(0x80000000)
 
 #define OPENLDAP_OBJECTCLASS_GROUP_CN_QUERY_HEAD	"(&(objectClass=groupOfNames)(cn="
+#define OPENLDAP_OBJECTCLASS_GROUP_QUERY_HEAD	"(&(objectClass=groupOfNames)("
 #define	OPENLDAP_ATTRIBUTE_GROUP_MEMBERS		"member"
 #define GROUP_NOT_CONFIGURED 0
 #define GROUP_SID_FOUND      1
