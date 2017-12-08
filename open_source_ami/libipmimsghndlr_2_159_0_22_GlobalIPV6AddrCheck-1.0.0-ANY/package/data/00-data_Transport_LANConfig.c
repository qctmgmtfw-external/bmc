--- .pristine/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Wed May  9 16:58:27 2012
+++ source/libipmimsghndlr-2.159.0-src/data/Transport/LANConfig.c Thu May 10 14:21:43 2012
@@ -1146,6 +1146,14 @@
                 return sizeof (INT8U);
             }
             
+			/*Validate the IPv6 address*/
+			if(0 != IsValidGlobalIPv6Addr((struct in6_addr*)&pSetLanReq->ConfigData.IPv6_IPAddr))
+			{
+				TCRIT("Invalid Global IPv6 Address\n");
+				*pRes = CC_INV_DATA_FIELD;
+				return sizeof(INT8U);
+			}
+            
             _fmemcpy(pBMCInfo->LANCfs[EthIndex].IPv6_IPAddr, pSetLanReq->ConfigData.IPv6_IPAddr, IP6_ADDR_LEN );
             
             _fmemcpy( NWConfig6.GlobalIPAddr, pSetLanReq->ConfigData.IPv6_IPAddr, IP6_ADDR_LEN );
