--- .pristine/libsnmpusers-1.8.0-src/data/SNMPApi.h Fri Sep 16 11:25:18 2011
+++ source/libsnmpusers-1.8.0-src/data/SNMPApi.h Tue Sep 20 11:53:28 2011
@@ -33,6 +33,8 @@
 #ifndef SNMPAPI_H_
 #define SNMPAPI_H_
 
+#include "Types.h"
+
 #define SNMPCFG_CONFIG_FILE "/conf/snmpcfg.conf"
 
 #define SNMP_STATUS "SNMPStatus"
@@ -49,26 +51,20 @@
 
 typedef struct
 {
-	int SNMPStatus;
-	int SNMPAccess;
-	int AUTHProtocol;
-	int PrivProtocol;
+	INT8U SNMPStatus;
+	INT8U SNMPAccess;
+	INT8U AUTHProtocol;
+	INT8U PrivProtocol;
 } SNMPConfigFile_T;
 
-typedef enum {
-        READ_ONLY,
-        READ_WRITE
-         }SNMPAccessType;
-typedef enum {
-        MD5,
-        SHA
-        }SNMPEncType1;
-typedef enum {
-        DES,
-        AES
-        }SNMPEncType2;
+#define        READ_ONLY 0
+#define        READ_WRITE 1
+#define        MD5 0
+#define        SHA 1
+#define        DES 0
+#define        AES 1
 
-int AddSNMPUserPwd(unsigned char *userName,unsigned char *passwd,SNMPEncType1 enctype1, SNMPEncType2 enctype2, SNMPAccessType accessType);
+int AddSNMPUserPwd(unsigned char *userName,unsigned char *passwd, INT8U enctype1, INT8U enctype2, INT8U accessType);
 void DelSNMPUser(unsigned char *userName);
 int GetSNMPUser(char *Username,  SNMPConfigFile_T* retrievedConfigData);
 int SetSNMPConfig(char * Username, SNMPConfigFile_T configData);
@@ -76,7 +72,7 @@
 int InitBackupConf(void);
 int DelUserFromSnmpdConf(char *user);
 int DelUserFromSnmpUserConf(char *user);
-int ModUserSnmpdConf(char *user, SNMPEncType1 enctype1, SNMPEncType2 enctype2);
+int ModUserSnmpdConf(char *user, INT8U enctype1, INT8U enctype2);
 void CloseBackupConf(void);
 
 #endif /* SNMPAPI_H_ */
