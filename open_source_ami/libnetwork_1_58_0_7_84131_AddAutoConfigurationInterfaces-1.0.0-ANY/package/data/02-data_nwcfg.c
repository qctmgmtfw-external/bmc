--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Wed Mar  7 21:55:03 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Fri Mar  9 11:48:15 2012
@@ -80,6 +80,7 @@
 #include <netdb.h>
 #include <netinet/ether.h>
 #include <netpacket/packet.h>
+#include "ncml.h"
 //#include "lanchcfg.h" //moved to nwcfg.h
 /*ipv6*/
 #include <arpa/inet.h>
@@ -114,6 +115,23 @@
 INT8S   IfEnslave[MAX_STR_LENGTH]="/sbin/ifenslave";
 INT8S PID[MAX_STR_LENGTH]=".pid";
 
+SERVICE_CONF_STRUCT g_serviceconf;
+
+char *ModifyServiceNameList[MAX_SERVICE]={ 
+                                                   WEB_SERVICE_NAME,    
+                                                   KVM_SERVICE_NAME,    
+                                                   CDMEDIA_SERVICE_NAME,
+                                                   FDMEDIA_SERVICE_NAME,
+                                                   HDMEDIA_SERVICE_NAME,
+                                };
+
+char *RestartServices[MAX_RESTART_SERVICE] = {
+        "/etc/init.d/webgo.sh restart &",
+        "/etc/init.d/adviserd.sh restart &",
+        "/etc/init.d/cdserver restart &",
+        "/etc/init.d/fdserver restart &",
+        "/etc/init.d/hdserver restart &",
+};
 
  IfcName_T Ifcnametable [MAX_CHANNEL] = 
 {
@@ -1761,12 +1779,13 @@
 int nwConfigureBonding(BondIface *BondCfg,INT8U EthIndex)
 {
     char IfupdownStr[64],EnslaveIface[64],IName[16],str[64],BondSlaves[64],ifcname[16] = {0};
-    int i,Ethindex;
+    int i,j,Ethindex;
     NWCFG6_STRUCT cfg6;
     NWCFG_STRUCT cfg;
     DOMAINCONF      DomainCfg;
     DNSCONF     DNS;
     INT8U               regBMC_FQDN[MAX_CHANNEL];
+    INT8U Index=0;
     struct stat       Buf;
 
     memset(IfupdownStr,'\0',64);
@@ -1885,9 +1904,60 @@
         DomainCfg.v4v6=1;
         DNS.DNSIndex=Ethindex;
         regBMC_FQDN[Ethindex] = regBMC_FQDN[EthIndex];
+
+        if(BondCfg->AutoConf == 1)
+        {
+            for(Index=0;Index < MAX_SERVICE;Index++)
+            {
+                get_service_configurations(ModifyServiceNameList[Index],&g_serviceconf);
+                if(strcmp(g_serviceconf.InterfaceName,"both") == 0)
+                {
+                    sprintf(g_serviceconf.InterfaceName,"bond%d",BondCfg->BondIndex);
+                    if(set_service_configurations(ModifyServiceNameList[Index],&g_serviceconf)!=0)
+                    {
+                        TCRIT("Error in Setting the Configuration for the Requested Service\n");
+                    }
+                }
+                else
+                {
+                    for(j=0;j < MAX_ETH;j++)
+                    {
+                        if((BondCfg->Slaves >> j) & IFACE_ENABLED)
+                        {
+                            sprintf(str,"eth%d",j);
+                            if(strcmp((g_serviceconf.InterfaceName),str)==0)
+                            {
+                                sprintf(g_serviceconf.InterfaceName,"bond%d",BondCfg->BondIndex);
+                                if(set_service_configurations(ModifyServiceNameList[Index],&g_serviceconf)!=0)
+                                {
+                                TCRIT("Error in Setting the Configuration for the Requested Service\n");
+                                }
+                                break;
+                            }
+                        }
+                    }
+                }
+            }
+        }
+
     }
     else
     {
+        if(BondCfg->AutoConf == 1)
+        {
+            for(Index=0;Index < MAX_SERVICE;Index++)
+            {
+                get_service_configurations(ModifyServiceNameList[Index],&g_serviceconf);
+                if(strcmp(g_serviceconf.InterfaceName,ifcname)==0)
+                {
+                    sprintf(g_serviceconf.InterfaceName,"both");
+                    if(set_service_configurations(ModifyServiceNameList[Index],&g_serviceconf) != 0 )
+                    {
+                        TCRIT("Error in Setting the service configuration\n");
+                    }
+                }
+            }
+        }
         Ifcnametable[Ethindex].Enabled = 0;
     }
 
@@ -1993,6 +2063,12 @@
                 return -1;
             }
         }
+    }
+
+    /*Restart the service to effect the changes*/
+    for(i=0;i<MAX_RESTART_SERVICE;i++)
+    {
+        safe_system(RestartServices[i]);
     }
 
     return 0;
