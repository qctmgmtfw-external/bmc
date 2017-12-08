--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Wed Sep  7 19:52:24 2011
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Wed Sep  7 19:59:12 2011
@@ -108,17 +108,17 @@
    { CMD_AMI_SET_FIREWALL, 	   PRIV_ADMIN,	    AMI_SET_FIREWALL, 		0xff, 	 0xAAAA },
    { CMD_AMI_GET_FIREWALL,	   PRIV_USER,		AMI_GET_FIREWALL,		0xff, 	 0xAAAA },
    { CMD_AMI_GET_FRU_DETAILS,	   PRIV_USER,		AMI_GET_FRU_DETAILS,	  sizeof(AMIGetFruDetailReq_T), 	 0xAAAA },
-   { CMD_AMI_GET_ROOT_USER_ACCESS, PRIV_ADMIN,      AMI_GET_ROOT_USER_ACCESS  ,   0x00                     , 0xAAAA, 0xFFFF},
+   { CMD_AMI_GET_ROOT_USER_ACCESS, PRIV_OPERATOR,      AMI_GET_ROOT_USER_ACCESS  ,   0x00                     , 0xAAAA, 0xFFFF},
    { CMD_AMI_SET_ROOT_PASSWORD,	   PRIV_ADMIN,		AMI_SET_ROOT_PASSWORD     ,	  0xff                     , 0xAAAA, 0xFFFF},
-   { CMD_AMI_GET_USER_SHELLTYPE,   PRIV_ADMIN,		AMI_GET_USER_SHELLTYPE  ,sizeof(AMIGetUserShelltypeReq_T)  , 0xAAAA, 0xFFFF},
+   { CMD_AMI_GET_USER_SHELLTYPE,   PRIV_OPERATOR,		AMI_GET_USER_SHELLTYPE  ,sizeof(AMIGetUserShelltypeReq_T)  , 0xAAAA, 0xFFFF},
    { CMD_AMI_SET_USER_SHELLTYPE,   PRIV_ADMIN,		AMI_SET_USER_SHELLTYPE  ,sizeof(AMISetUserShelltypeReq_T)  , 0xAAAA, 0xFFFF},
    { CMD_AMI_SET_TRIGGER_EVT,PRIV_ADMIN,      AMI_SET_TRIGGER_EVT,0xFF,0xAAAA,0xFFFF},
-   { CMD_AMI_GET_TRIGGER_EVT,PRIV_ADMIN,      AMI_GET_TRIGGER_EVT,sizeof(AMIGetTriggerEventReq_T),0xAAAA,0xFFFF},
+   { CMD_AMI_GET_TRIGGER_EVT,PRIV_OPERATOR,      AMI_GET_TRIGGER_EVT,sizeof(AMIGetTriggerEventReq_T),0xAAAA,0xFFFF},
    /*-------------------- AMI SOL Commands -----------------------------------------*/
    { CMD_AMI_GET_SOL_CONFIG_PARAMS, PRIV_USER, AMI_GET_SOL_CONF, 0x00, 0xAAAA, 0xFFFF  },
    
    { CMD_AMI_SET_LOGIN_AUDIT_CFG,		PRIV_ADMIN,      AMI_SET_LOGIN_AUDIT_CFG,		sizeof(AMISetLoginAuditCfgReq_T),	0xAAAA,	0xFFFF	},
-   { CMD_AMI_GET_LOGIN_AUDIT_CFG,		PRIV_ADMIN,      AMI_GET_LOGIN_AUDIT_CFG,		0x00,	0xAAAA,	0xFFFF	},
+   { CMD_AMI_GET_LOGIN_AUDIT_CFG,		PRIV_USER,      AMI_GET_LOGIN_AUDIT_CFG,		0x00,	0xAAAA,	0xFFFF	},
    {CMD_AMI_SET_PAM_ORDER, PRIV_ADMIN,    AMI_SET_PAM_ORDER, 0xff, 0xAAAA, 0xFFFF},
    {CMD_AMI_GET_PAM_ORDER, PRIV_USER,    AMI_GET_PAM_ORDER, 0xff, 0xAAAA, 0xFFFF},
    /*                                 AMI SNMP Commands                                                                                 */
@@ -131,7 +131,7 @@
 
 	/*-------------------- AMI Preserve Conf Commands -----------------------------------------*/
   { CMD_AMI_SET_PRESERVE_CONF,	   PRIV_ADMIN,	AMI_SET_PRESERVE_CONF,	sizeof(SetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
-  { CMD_AMI_GET_PRESERVE_CONF,		PRIV_ADMIN,  AMI_GET_PRESERVE_CONF,	 sizeof(GetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
+  { CMD_AMI_GET_PRESERVE_CONF,		PRIV_USER,  AMI_GET_PRESERVE_CONF,	 sizeof(GetPreserveConfigReq_T),	0xAAAA, 0xFFFF },
 
    { 0 , 0 , 0 , 0, 0 ,0}, // End of Table
 
