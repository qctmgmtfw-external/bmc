--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Fri Sep 16 11:25:22 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMIConf.c Mon Sep 19 12:14:16 2011
@@ -1601,7 +1601,7 @@
     _NEAR_ AMIGetSNMPConfRes_T *pAMIGetSNMPConfRes = (_NEAR_ AMIGetSNMPConfRes_T *)pRes;
     _NEAR_ AMIGetSNMPConfReq_T * snmpcfg = (_NEAR_ AMIGetSNMPConfReq_T*)pReq;
     
-    SNMPGetUser((char *)snmpcfg->Username, (int*)&snmpcfg->snmp_enable, (int*)&snmpcfg->snmp_access_type, (int*)&snmpcfg->snmp_enc_type_1, (int*)&snmpcfg->snmp_enc_type_2);
+    SNMPGetUser((char *)snmpcfg->Username, (unsigned char *)&snmpcfg->snmp_enable, (unsigned char*)&snmpcfg->snmp_access_type, (unsigned char*)&snmpcfg->snmp_enc_type_1, (unsigned char*)&snmpcfg->snmp_enc_type_2);
     pAMIGetSNMPConfRes->CompletionCode = CC_SUCCESS;
 
     strcpy((char*)pAMIGetSNMPConfRes->Username, (char*)snmpcfg->Username);
