--- .pristine/libipmidcmi-2.8.0-src/data/DCMDevice.c Tue Nov  5 19:58:50 2013
+++ source/libipmidcmi-2.8.0-src/data/DCMDevice.c Wed Nov  6 19:33:28 2013
@@ -34,6 +34,7 @@
 #include "PDKAccess.h"
 #include "AMIConf.h"
 #include "IPMI_AMIConf.h"
+#include "PendTask.h"
 
 /**************************************/
 /* INTERNAL MACRO DEFINITIONS         */
@@ -828,7 +829,15 @@
     }    
 
     /* Store new hostname to DNS HostName */
-    nwSetAllDNSConf( &HostConfig, &DomainConfig,&DnsIPConfig,regBMC_FQDN);
+    //nwSetAllDNSConf( &HostConfig, &DomainConfig,&DnsIPConfig,regBMC_FQDN);
+    LOCK_BMC_SHARED_MEM(BMCInst);
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostSetting = HostConfig.HostSetting;
+    BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostNameLen = HostConfig.HostNameLen;
+    memcpy(BMC_GET_SHARED_MEM (BMCInst)->DNSconf.HostName,HostConfig.HostName,HostConfig.HostNameLen);
+    UNLOCK_BMC_SHARED_MEM(BMCInst);
+    //Pend to set all DNS config, or the command will timeout to respond.
+    SetPendStatus(PEND_OP_SET_ALL_DNS_CFG,PEND_STATUS_PENDING);
+    PostPendTask(PEND_OP_SET_ALL_DNS_CFG, 0, 0,g_BMCInfo[BMCInst].Msghndlr.CurChannel  & 0xF,BMCInst);
 
     pSetManagementControllerIdStringRes->CompletionCode = CC_NORMAL;
     pSetManagementControllerIdStringRes->GroupExtnID    = IPMI_GROUP_EXTN_CODE_DCMI;
