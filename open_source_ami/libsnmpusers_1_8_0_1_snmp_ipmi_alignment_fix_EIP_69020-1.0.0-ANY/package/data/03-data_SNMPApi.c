--- .pristine/libsnmpusers-1.8.0-src/data/SNMPApi.c Fri Sep 16 11:25:18 2011
+++ source/libsnmpusers-1.8.0-src/data/SNMPApi.c Tue Sep 20 11:54:28 2011
@@ -80,7 +80,7 @@
 		return -1;
 }
 
-int AddSNMPUserPwd(unsigned char *userName,unsigned char *passwd,SNMPEncType1 enctype1, SNMPEncType2 enctype2, SNMPAccessType accessType)
+int AddSNMPUserPwd(unsigned char *userName,unsigned char *passwd,INT8U enctype1, INT8U enctype2, INT8U accessType)
 {
     char str[1024];
     char AccessGroups[2][32] = {"rouser",
@@ -269,7 +269,7 @@
     fclose(snmpdpwdbkfile);
 }
 
-int ModUserSnmpdConf(char *user, SNMPEncType1 enctype1, SNMPEncType2 enctype2)
+int ModUserSnmpdConf(char *user, INT8U enctype1, INT8U enctype2)
 {
     char str[1024];
     char hexUserName[512];
