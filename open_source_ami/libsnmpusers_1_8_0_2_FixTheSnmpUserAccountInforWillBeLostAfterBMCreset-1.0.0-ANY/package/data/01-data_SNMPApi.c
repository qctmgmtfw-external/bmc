--- .pristine/libsnmpusers-1.8.0-src/data/SNMPApi.c Fri May  4 16:14:41 2012
+++ source/libsnmpusers-1.8.0-src/data/SNMPApi.c Fri May  4 18:22:51 2012
@@ -40,7 +40,7 @@
 
 #define SNMPD_CONF      "/conf/snmp_users.conf"
 #ifndef SNMPD_PWD_CONF
-#define SNMPD_PWD_CONF      "/var/lib/snmp/snmpd.conf"
+#define SNMPD_PWD_CONF      "/conf/snmp/snmpd.conf"
 #endif
 #define SNMPD_BK_CONF       "/var/tmp/snmpd.bkup"
 #define SNMPD_PWD_BK_CONF   "/var/tmp/snmpd_pwd.bkup"
