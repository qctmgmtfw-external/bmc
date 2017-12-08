--- .pristine/libldapconf-1.11.0-src/data/ldapconf.h Tue Sep  4 18:16:29 2012
+++ source/libldapconf-1.11.0-src/data/ldapconf.h Fri Sep  7 15:22:27 2012
@@ -60,6 +60,7 @@
 
 
 #define		DEFAULT_LDAP_PORT			389
+#define		DEFAULT_LDAP_ATTRIBUTE_OF_USERLOGIN			"cn"
 /* LDAP configuration file */
 #define		LDAP_CONFIG_FILE			"/conf/ldap.conf"
 #define		LDAP_CONFIG_FILE_DISABLED		"/conf/ldap.conf.disabled"
@@ -70,6 +71,7 @@
 #define	LDAP_CFG_STR_BINDDN		"binddn"
 #define	LDAP_CFG_STR_BINDPWD		"bindpw"
 #define	LDAP_CFG_STR_BINDPWD		"bindpw"
+#define	LDAP_CFG_STR_ATTRIBUTE_OF_USERLOGIN	"attribute_userlogin"
 #define LDAP_CFG_PORT_NUM		"port"
 #define LDAP_CFG_PAM_PWD		"pam_password"
 #define LDAP_CFG_NSS_RECONNECT_TRIES	"nss_reconnect_tries"
@@ -84,6 +86,7 @@
 	unsigned char Password[48];
 	unsigned char BindDN[64];
 	unsigned char SearchBase[128];
+	unsigned char AttributeOfUserLogin[8];
 	unsigned char DefaultRole;
 } LDAPCONFIG;
 
