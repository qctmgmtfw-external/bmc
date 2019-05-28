--- linux.pristine/net/ipv4/devinet.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/net/ipv4/devinet.c	2014-02-03 15:11:17.159028528 -0500
@@ -1190,7 +1190,8 @@
 			continue;
 
 		for_primary_ifa(in_dev) {
-			if (ifa->ifa_scope != RT_SCOPE_LINK &&
+			if (!IN_DEV_HIDDEN(in_dev) &&
+			    ifa->ifa_scope != RT_SCOPE_LINK &&
 			    ifa->ifa_scope <= scope) {
 				addr = ifa->ifa_local;
 				goto out_unlock;
@@ -2119,6 +2120,7 @@
 		DEVINET_SYSCTL_RW_ENTRY(BOOTP_RELAY, "bootp_relay"),
 		DEVINET_SYSCTL_RW_ENTRY(LOG_MARTIANS, "log_martians"),
 		DEVINET_SYSCTL_RW_ENTRY(TAG, "tag"),
+		DEVINET_SYSCTL_RW_ENTRY(HIDDEN, "hidden"),
 		DEVINET_SYSCTL_RW_ENTRY(ARPFILTER, "arp_filter"),
 		DEVINET_SYSCTL_RW_ENTRY(ARP_ANNOUNCE, "arp_announce"),
 		DEVINET_SYSCTL_RW_ENTRY(ARP_IGNORE, "arp_ignore"),
