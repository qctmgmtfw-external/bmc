--- .pristine/libnetwork-1.58.0-src/data/nwcfg.h Wed May  9 16:57:40 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.h Thu May 10 11:17:55 2012
@@ -554,6 +554,13 @@
 int nwGetResolvConf_v4_v6(char* DNS1,char*DNS2,char *DNS3, INT8U DNSIPPriority,char* domain,unsigned char* domainnamelen);
 
 /**
+*@fn IsValidGlobalIPv6Addr
+*@brief This function will check the given IPv6 address as global IP address or not
+*@return Return 0 on success and -1 on failure
+*/
+int IsValidGlobalIPv6Addr(struct in6_addr * IPv6Addr);
+
+/**
 *@fn nwReadNWCfg_v4_v6
 *@brief This function is invoked to Get the current network status of both IPv4 and IPv6 networks.
 *@		If there is no need of IPv6 data means, then just pass NULL to IPv6 pointer.
