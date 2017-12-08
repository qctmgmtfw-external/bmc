--- .pristine/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Fri Feb  3 16:18:15 2012
+++ source/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Tue Jan 31 10:38:26 2012
@@ -226,6 +226,7 @@
 
     static INT8U macEthIndex = 0xFF;
     INT8U TOS = 0, SkbPriority = 0; /* For VLAN priority parameter */
+    int pendStatus = PEND_STATUS_COMPLETED;
 
 #ifdef CONFIG_SPX_FEATURE_PHY_SUPPORT
     char InterfaceName[8] = {0}; /* Eth interface name */
@@ -386,6 +387,12 @@
             break;
 
         case LAN_PARAM_IP_ADDRESS:
+            pendStatus = GetPendStatus(PEND_OP_SET_IP);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
 
             //we need to do a read in hte pend task and not here
             // because if pend task is still working on setting the source for example t- by then we would have got the
@@ -401,6 +408,13 @@
             if(IPAddrCheck(pSetLanReq->ConfigData.IPAddr,LAN_PARAM_IP_ADDRESS))
             {
                 *pRes = CC_INV_DATA_FIELD;
+                return sizeof (INT8U);
+            }
+
+            pendStatus = GetPendStatus(PEND_OP_SET_IP);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
                 return sizeof (INT8U);
             }
 
@@ -427,6 +441,12 @@
             pBMCInfo->LANCfs[EthIndex].IPAddrSrc = pSetLanReq->ConfigData.IPAddrSrc ;
             if ( (pSetLanReq->ConfigData.IPAddrSrc == STATIC_IP_SOURCE ) || (pSetLanReq->ConfigData.IPAddrSrc == DHCP_IP_SOURCE ) )
             {
+                pendStatus = GetPendStatus(PEND_OP_SET_SOURCE);
+                if(pendStatus == PEND_STATUS_PENDING)
+                {
+                    *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                    return sizeof (INT8U);
+                }
                 NWConfig.CfgMethod = pSetLanReq->ConfigData.IPAddrSrc;
                 SetPendStatus(PEND_OP_SET_SOURCE,PEND_STATUS_PENDING);
                 PostPendTask(PEND_OP_SET_SOURCE,(INT8U*) &NWConfig,sizeof(NWConfig),(pSetLanReq->ChannelNum & 0x0F), BMCInst );
@@ -491,6 +511,13 @@
 				memset(&macAddrEnabled, 0, sizeof(EnableSetMACAddress_T));
 				macAddrEnabled.EthIndex = macEthIndex;
 				memcpy(&macAddrEnabled.MACAddress, &pSetLanReq->ConfigData.MACAddr, MAC_ADDR_LEN);
+			
+                pendStatus = GetPendStatus(PEND_OP_SET_MAC_ADDRESS);
+                if(pendStatus == PEND_STATUS_PENDING)
+                {
+                    *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                    return sizeof (INT8U);
+                }			
 			
 				SetPendStatus(PEND_OP_SET_MAC_ADDRESS, PEND_STATUS_PENDING);
 				PostPendTask(PEND_OP_SET_MAC_ADDRESS, (INT8U*)&macAddrEnabled, sizeof(EnableSetMACAddress_T), (pSetLanReq->ChannelNum & 0x0F), BMCInst);
@@ -506,6 +533,12 @@
 #endif
 
         case LAN_PARAM_SUBNET_MASK:
+            pendStatus = GetPendStatus(PEND_OP_SET_SUBNET);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
 
             /*Returning valid completion code in case of attempt to set netmask in DHCP mode */
             if(pBMCInfo->LANCfs[EthIndex].IPAddrSrc == DHCP_IP_SOURCE)
@@ -528,6 +561,12 @@
             break;
 
         case LAN_PARAM_IPv4_HEADER:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV4_HEADERS);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }        
             if(!pSetLanReq->ConfigData.Ipv4HdrParam.TimeToLive > 0)
             {
                 IPMI_DBG_PRINT("The requested IPv4 header(TTL) to set is invalid.\n");
@@ -550,6 +589,12 @@
             break;
 
         case LAN_PARAM_PRI_RMCP_PORT:
+            pendStatus = GetPendStatus(PEND_RMCP_PORT_CHANGE);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }        
             LOCK_BMC_SHARED_MEM(BMCInst);
             pBMCInfo->LANCfs[EthIndex].PrimaryRMCPPort = ipmitoh_u16 (pSetLanReq->ConfigData.PrimaryRMCPPort);
             UNLOCK_BMC_SHARED_MEM(BMCInst);
@@ -600,6 +645,12 @@
             break;
 
         case LAN_PARAM_DEFAULT_GATEWAY_IP:
+            pendStatus = GetPendStatus(PEND_OP_SET_GATEWAY);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
 
             nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, netindex);
             if(IPAddrCheck(pSetLanReq->ConfigData.DefaultGatewayIPAddr,LAN_PARAM_DEFAULT_GATEWAY_IP))
@@ -709,6 +760,12 @@
             break;
 
         case LAN_PARAM_SELECT_DEST_ADDR:
+            pendStatus = GetPendStatus(PEND_OP_SET_GATEWAY);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
 
             // if (pSetLanReq->ConfigData.DestAddr.SetSelect > NUM_LAN_DESTINATION)
             if (pSetLanReq->ConfigData.DestType.SetSelect > pBMCInfo->LANCfs[EthIndex].NumDest )
@@ -996,6 +1053,13 @@
 
 
         case LAN_PARAMS_AMI_OEM_IPV6_ENABLE:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_ENABLE);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+            
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_ENABLE \n");         
             /*if ( pPMConfig->LANConfig[EthIndex].IPv6_Enable == pSetLanReq->ConfigData.IPv6_Enable)
             {
@@ -1011,6 +1075,13 @@
 
 
         case LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR_SOURCE:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_IP_ADDR_SOURCE);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+        
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR_SOURCE \n");
              /* check for IPv6 enable state */
             if ( pBMCInfo->LANCfs[EthIndex].IPv6_Enable != 1)
@@ -1051,6 +1122,13 @@
 
 
         case LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_IP_ADDR);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+        
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_IP_ADDR \n");
             /* check for IPv6 enable state */
             if ( pBMCInfo->LANCfs[EthIndex].IPv6_Enable != 1)
@@ -1077,6 +1155,13 @@
 
 
         case LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_PREFIX_LENGTH);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+        
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_PREFIX_LENGTH \n");
             /* check for IPv6 enable state */
             if ( pBMCInfo->LANCfs[EthIndex].IPv6_Enable != 1)
@@ -1103,6 +1188,13 @@
 
 
         case LAN_PARAMS_AMI_OEM_IPV6_GATEWAY_IP:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_GATEWAY);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+        
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_GATEWAY_IP \n");
             /* check for IPv6 enable state */
             if ( pBMCInfo->LANCfs[EthIndex].IPv6_Enable != 1)
@@ -1128,6 +1220,13 @@
             break;
 
         case LAN_PARAMS_AMI_OEM_IPV6_DNS_SETTINGS:
+            pendStatus = GetPendStatus(PEND_OP_SET_IPV6_DNS_CFG);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+        
             TDBG("\n Entered in LAN_PARAMS_AMI_OEM_IPV6_DNS_SETTINGS \n");
             /* check for IPv6 enable state */
             if ( pBMCInfo->LANCfs[EthIndex].IPv6_Enable != 1)
@@ -1259,6 +1358,13 @@
 
     case LAN_PARAMS_SSI_OEM_2ND_PRI_ETH_MAC_ADDR:
         #ifdef CONFIG_SPX_FEATURE_SSICOMPUTEBLADE_SUPPORT
+            pendStatus = GetPendStatus(PEND_OP_SET_MAC_ADDRESS);
+            if(pendStatus == PEND_STATUS_PENDING)
+            {
+                *pRes = CC_PARAM_NOT_SUP_IN_CUR_STATE;
+                return sizeof (INT8U);
+            }
+            
             if (!enableSetMACAddr)
             {
                 pSetLanRes->CompletionCode = CC_ATTEMPT_TO_SET_RO_PARAM;
@@ -1501,7 +1607,7 @@
     INT8U IsOemDefined = FALSE;
     NWCFG_STRUCT        NWConfig;
     NWCFG6_STRUCT        NWConfig6;
-    V6DNS_CONFIG v6dnsconfig;
+    //V6DNS_CONFIG v6dnsconfig;
     INT8U EthIndex,netindex= 0xFF,i;
     _FAR_ BMCInfo_t* pBMCInfo = &g_BMCInfo[BMCInst];
 #ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
@@ -1937,14 +2043,8 @@
                 break;
 
             case LAN_PARAMS_AMI_OEM_IPV6_DNS_SETTINGS:
-                if(GetIPv6DNSConf( &v6dnsconfig ) < 0 )
-                {
-                    *pRes = CC_UNSPECIFIED_ERR;
-                    return sizeof (INT8U);
-                }
-                
-                _fmemcpy ( &pGetLanRes->ConfigData.V6DNSConfig, &v6dnsconfig, sizeof(V6DNS_CONFIG) );
-                return sizeof(GetLanCCRev_T) + sizeof(V6DNS_CONFIG);
+                pGetLanRes->CCParamRev.CompletionCode = CC_PARAM_NOT_SUPPORTED;
+                return sizeof(INT8U);
 
                 break;
                 
@@ -2084,6 +2184,7 @@
 int
 GetIPv6DNSConf( V6DNS_CONFIG *pV6dnscfg )
 {  
+#if 0
     HOSTNAMECONF HostnameConfig;
     DOMAINCONF DomainConfig;
     DNSIPV4CONF Dnsv4IPConfig;
@@ -2124,7 +2225,7 @@
     {
         return -1;
     }
-    
+#endif    
 return 0;
 }
 
