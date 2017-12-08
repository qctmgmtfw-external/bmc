--- .pristine/libsnmpusers-1.8.0-src/data/snmpusers.c Fri Sep 16 11:25:18 2011
+++ source/libsnmpusers-1.8.0-src/data/snmpusers.c Tue Sep 20 11:56:01 2011
@@ -35,10 +35,10 @@
 #include <string.h>
 #include "SNMPApi.h"
 
-int SNMPAddUser(char* UserName,char* ClearPassword, unsigned int access, unsigned int enc_type_1, unsigned int enc_type_2){
+int SNMPAddUser(char* UserName,char* ClearPassword, INT8U access, INT8U enc_type_1, INT8U enc_type_2){
 
-	SNMPEncType1 enctype1 = SHA;	
-	SNMPEncType2 enctype2 = AES;
+	INT8U enctype1 = SHA;	
+	INT8U enctype2 = AES;
 
 	if (enc_type_1 == SHA_CONF){
 		enctype1 = SHA;
@@ -68,7 +68,7 @@
 	return 0;
 }
 
-int SNMPGetUser(char* UserName, int *SNMPStatus, int *SNMPAccess, int *AUTHProtocol, int *PrivProtocol){
+int SNMPGetUser(char* UserName, INT8U *SNMPStatus, INT8U *SNMPAccess, INT8U *AUTHProtocol, INT8U *PrivProtocol){
 	SNMPConfigFile_T retrievedConfigData = {0};
 	
 	//printf("%s, %d, %d, %d, %d\n\n", (char*)UserName, *SNMPStatus, *SNMPAccess, *AUTHProtocol, *PrivProtocol);
