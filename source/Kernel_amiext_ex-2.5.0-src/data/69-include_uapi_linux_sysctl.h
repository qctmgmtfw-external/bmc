--- linux-3.14.17/include/uapi/linux/sysctl.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/uapi/linux/sysctl.h	2014-08-21 11:38:30.141559531 -0400
@@ -481,6 +481,7 @@
 	NET_IPV4_CONF_PROMOTE_SECONDARIES=20,
 	NET_IPV4_CONF_ARP_ACCEPT=21,
 	NET_IPV4_CONF_ARP_NOTIFY=22,
+	NET_IPV6_CONF_HIDDEN=23,
 };
 
 /* /proc/sys/net/ipv4/netfilter */
