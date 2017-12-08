--- .pristine/ipmi_dev-2.100.0-src/data/Ethaddr.h Fri Feb  3 16:17:29 2012
+++ source/ipmi_dev-2.100.0-src/data/Ethaddr.h Thu Jan 19 10:57:04 2012
@@ -21,5 +21,6 @@
 extern INT8U GetChannelByAddr(char * IPAddr,int BMCInst);
 extern INT8U GetChannelByIPv6Addr(char * IPAddr,int BMCInst);
 extern INT8U IsLANChannel(INT8U Channel, int BMCInst)  ;
+extern int InitDNSConfiguration(int BMCInst);
 extern int GetLinkStatus(INT8U Channel, int BMCInst);
 
