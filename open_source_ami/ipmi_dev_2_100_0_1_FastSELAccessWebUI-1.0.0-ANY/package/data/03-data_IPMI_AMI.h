--- .pristine/ipmi_dev-2.100.0-src/data/IPMI_AMI.h Thu Sep 15 22:48:04 2011
+++ source/ipmi_dev-2.100.0-src/data/IPMI_AMI.h Thu Sep 15 22:58:37 2011
@@ -180,5 +180,8 @@
 #define CMD_AMI_SET_PRESERVE_CONF 0x83
 #define CMD_AMI_GET_PRESERVE_CONF 0x84
 
+/* Retrive SEL Entries Command */
+#define CMD_AMI_GET_SEL_ENTIRES 0x85
+
 #endif /* IPMI_AMI_H */
 
