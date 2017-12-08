--- .pristine/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c	Fri May 31 16:02:01 2013
+++ source/libipmimsghndlr-2.159.0-src/data/App/AppDevice/AppDevice.c	Fri May 31 16:16:53 2013
@@ -2262,10 +2262,12 @@
            {
                 /* IP Address */
                   memset(&LANIPv6SesInfo,0,sizeof(LANIPv6SesInfo));
+#if 0                  
                 _fmemcpy (&(LANIPv6SesInfo.IPv6Address),
                     pSessInfo->LANRMCPPkt.IPHdr.SrcAddr,
                     sizeof(struct in6_addr));
-
+#endif
+                
                 EthIndex= GetEthIndex(pSessInfo->Channel & 0x0F, BMCInst);
                 if(0xff == EthIndex)
                 {
