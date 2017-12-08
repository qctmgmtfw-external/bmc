--- .pristine/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Fri Feb  3 16:18:15 2012
+++ source/libipmimsghndlr-2.159.0-src/data/AMI/AMI.c Wed Jan 18 23:00:28 2012
@@ -95,11 +95,8 @@
    { CMD_AMI_LINK_DOWN_RESILENT, PRIV_ADMIN,        AMI_LINK_DOWN_RESILENT , sizeof(AMILinkDownResilentReq_T), 0xAAAA},
    
     /*-------------------- AMI DNS Commands -----------------------------------------*/
-   { CMD_AMI_GET_DNS_CONF,     	PRIV_USER,     AMI_GET_DNS_CONF,      	0x00,                           0xAAAA  },
+   { CMD_AMI_GET_DNS_CONF,     	PRIV_USER,     AMI_GET_DNS_CONF,      	sizeof(AMIGetDNSConfReq_T),                           0xAAAA  },
    { CMD_AMI_SET_DNS_CONF,     	PRIV_ADMIN,     AMI_SET_DNS_CONF,      	0xff,                           0xAAAA  },
-
-   { CMD_AMI_GET_V6DNS_CONF,     	PRIV_USER,     AMI_GET_V6DNS_CONF,      	0x00,                           0xAAAA  },
-   { CMD_AMI_SET_V6DNS_CONF,     	PRIV_ADMIN,     AMI_SET_V6DNS_CONF,      	0xff,                           0xAAAA  },
    
    /*-------------------- AMI Iface State Commands -----------------------------------------*/
    { CMD_AMI_GET_IFACE_STATE,       PRIV_OPERATOR,    AMI_GET_IFACE_STATE,     0xff,     0xAAAA },
