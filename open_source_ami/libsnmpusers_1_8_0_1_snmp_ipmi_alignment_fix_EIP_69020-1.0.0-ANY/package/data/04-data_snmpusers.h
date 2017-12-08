--- .pristine/libsnmpusers-1.8.0-src/data/snmpusers.h Fri Sep 16 11:25:18 2011
+++ source/libsnmpusers-1.8.0-src/data/snmpusers.h Mon Sep 19 10:14:22 2011
@@ -32,8 +32,8 @@
 
 #ifndef SNMPUSERS_H_
 #define SNMPUSERS_H_
-int SNMPAddUser(char* UserName,char* ClearPassword, unsigned int access, unsigned int enc_type_1, unsigned int enc_type_2);
+int SNMPAddUser(char* UserName,char* ClearPassword, INT8U access, INT8U enc_type_1, INT8U enc_type_2);
 int SNMPDelUser(char* UserName);
-int SNMPGetUser(char* UserName, int *SNMPStatus, int *SNMPAccess, int *AUTHProtocol, int *PrivProtocol);
+int SNMPGetUser(char* UserName, INT8U *SNMPStatus, INT8U *SNMPAccess, INT8U *AUTHProtocol, INT8U *PrivProtocol);
 
 #endif /* SNMPUSERS_H_ */
