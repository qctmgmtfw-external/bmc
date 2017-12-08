--- .pristine/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c	Mon Jun 30 10:29:17 2014
+++ source/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c	Mon Jun 30 11:17:10 2014
@@ -405,6 +405,14 @@
                 return sizeof (INT8U);
             }
 
+			nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, netindex);
+		    if(memcmp ( NWConfig.IPAddr, pSetLanReq->ConfigData.IPAddr, IP_ADDR_LEN ) == 0)
+		    {
+				TCRIT("Same IP address, do nothing\n");
+				_fmemcpy (pBMCInfo->LANCfs[EthIndex].IPAddr, pSetLanReq->ConfigData.IPAddr, IP_ADDR_LEN);
+				break;
+		    }
+
             if(IPAddrCheck(pSetLanReq->ConfigData.IPAddr,LAN_PARAM_IP_ADDRESS))
             {
                 *pRes = CC_INV_DATA_FIELD;
@@ -547,6 +555,14 @@
                 return sizeof (INT8U);
             }
 
+		    nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, netindex);
+		    if(memcmp ( NWConfig.Mask, pSetLanReq->ConfigData.SubNetMask, IP_ADDR_LEN ) == 0)
+		    {
+				TCRIT("Same Mask, do nothing\n");
+				_fmemcpy (pBMCInfo->LANCfs[EthIndex].SubNetMask, pSetLanReq->ConfigData.SubNetMask, IP_ADDR_LEN);
+				break;
+		    }
+
             if(IPAddrCheck(pSetLanReq->ConfigData.SubNetMask,LAN_PARAM_SUBNET_MASK))
             {
                 *pRes = CC_INV_DATA_FIELD;
@@ -653,6 +669,14 @@
             }
 
             nwReadNWCfg_v4_v6( &NWConfig, &NWConfig6, netindex);
+
+		    if(memcmp ( NWConfig.Gateway, pSetLanReq->ConfigData.DefaultGatewayIPAddr, IP_ADDR_LEN ) == 0)
+		    {
+				TCRIT("Same Gateway, do nothing\n");
+				_fmemcpy(pBMCInfo->LANCfs[EthIndex].DefaultGatewayIPAddr, pSetLanReq->ConfigData.DefaultGatewayIPAddr,IP_ADDR_LEN);
+				break;
+            }
+					
             if(IPAddrCheck(pSetLanReq->ConfigData.DefaultGatewayIPAddr,LAN_PARAM_DEFAULT_GATEWAY_IP))
             {
                 *pRes = CC_INV_DATA_FIELD;
