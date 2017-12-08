--- .pristine/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Fri Sep 16 11:25:19 2011
+++ source/libipmi-2.38.0-src/data/libipmi_AMIOEM.c Fri Sep 16 14:22:59 2011
@@ -2952,15 +2952,15 @@
      {
         TCRIT("Error While getting SNMP configuration %d\n",wRet);
      }
-     snmp_enable = (INT8U)Res.snmp_enable;
-     snmp_access_type = (INT8U)Res.snmp_access_type;
-     snmp_enc_type_1 = (INT8U)Res.snmp_enc_type_1;
-     snmp_enc_type_2 = (INT8U)Res.snmp_enc_type_2;
+     snmp_enable = Res.snmp_enable;
+     snmp_access_type = Res.snmp_access_type;
+     snmp_enc_type_1 = Res.snmp_enc_type_1;
+     snmp_enc_type_2 = Res.snmp_enc_type_2;
      
-     memcpy(SNMPStatus, &snmp_enable, sizeof(SNMPStatus));
-     memcpy(SNMPAccess, &snmp_access_type, sizeof(SNMPAccess));
-     memcpy(AUTHProtocol, &snmp_enc_type_1, sizeof(AUTHProtocol));
-     memcpy(PrivProtocol, &snmp_enc_type_2, sizeof(PrivProtocol));
+     memcpy(SNMPStatus, &snmp_enable, sizeof(INT8U));
+     memcpy(SNMPAccess, &snmp_access_type, sizeof(INT8U));
+     memcpy(AUTHProtocol, &snmp_enc_type_1, sizeof(INT8U));
+     memcpy(PrivProtocol, &snmp_enc_type_2, sizeof(INT8U));
      
      return wRet;
  }
