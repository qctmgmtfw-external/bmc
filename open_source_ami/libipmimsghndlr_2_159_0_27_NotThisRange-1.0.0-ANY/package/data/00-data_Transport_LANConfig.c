--- .pristine/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Thu Sep 27 14:36:04 2012
+++ source/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Thu Sep 27 15:35:06 2012
@@ -1220,6 +1220,14 @@
                 return sizeof (INT8U);
             }
             
+            /*Validate the IPv6 address*/
+			if(0 != IsValidGlobalIPv6Addr((struct in6_addr*)&pSetLanReq->ConfigData.IPv6_GatewayIPAddr))
+			{
+				TCRIT("Invalid Global IPv6 Address\n");
+				*pRes = CC_INV_DATA_FIELD;
+				return sizeof(INT8U);
+			}
+            
             _fmemcpy( pBMCInfo->LANCfs[EthIndex].IPv6_GatewayIPAddr, pSetLanReq->ConfigData.IPv6_GatewayIPAddr, IP6_ADDR_LEN );
             
             _fmemcpy( NWConfig6.Gateway, pSetLanReq->ConfigData.IPv6_GatewayIPAddr, IP6_ADDR_LEN );
