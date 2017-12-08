--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Thu Sep 15 12:23:59 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Thu Sep 15 14:21:33 2011
@@ -128,6 +128,8 @@
     /*------------------------------------------------- AMI SEL Commands -------------------------------------------------*/
     { CMD_AMI_GET_SEL_POLICY,   PRIV_USER,      AMI_GET_SEL_POLICY,     0x00,                           0xAAAA,     0xFFFF },
     { CMD_AMI_SET_SEL_POLICY,   PRIV_ADMIN,     AMI_SET_SEL_POLICY,     sizeof(AMISetSELPolicyReq_T),   0xAAAA,     0xFFFF },
+    { CMD_AMI_GET_SEL_ENTIRES,   PRIV_USER,     AMI_GET_SEL_ENTRIES, sizeof(AMIGetSELEntriesReq_T) , 0xAAAA, 0xFFFF},
+
 
 	/*-------------------- AMI Preserve Conf Commands -----------------------------------------*/
   { CMD_AMI_SET_PRESERVE_CONF,	   PRIV_ADMIN,	AMI_SET_PRESERVE_CONF,	sizeof(SetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
