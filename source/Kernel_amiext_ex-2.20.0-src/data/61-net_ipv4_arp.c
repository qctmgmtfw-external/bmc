--- linux-3.14.17/net/ipv4/arp.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/net/ipv4/arp.c	2014-08-21 11:20:28.057539217 -0400
@@ -71,6 +71,8 @@
  *					sending (e.g. insert 8021q tag).
  *		Harald Welte	:	convert to make use of jenkins hash
  *		Jesper D. Brouer:       Proxy ARP PVLAN RFC 3069 support.
+ *		Julian Anastasov:	"hidden" flag: hide the
+ *					interface and don't reply for it
  */
 
 #define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
@@ -327,7 +329,9 @@
 	struct net_device *dev = neigh->dev;
 	__be32 target = *(__be32 *)neigh->primary_key;
 	int probes = atomic_read(&neigh->probes);
-	struct in_device *in_dev;
+	struct in_device *in_dev, *in_dev2;
+	struct net_device *dev2;
+	int mode;
 
 	rcu_read_lock();
 	in_dev = __in_dev_get_rcu(dev);
@@ -335,9 +339,22 @@
 		rcu_read_unlock();
 		return;
 	}
-	switch (IN_DEV_ARP_ANNOUNCE(in_dev)) {
+	mode = IN_DEV_ARP_ANNOUNCE(in_dev);
+	if (mode != 2 && skb &&
+	    (dev2 = __ip_dev_find(dev_net(dev), ip_hdr(skb)->saddr,
+				  false)) != NULL &&
+	    (saddr = ip_hdr(skb)->saddr,
+	     in_dev2 = __in_dev_get_rcu(dev2)) != NULL &&
+	    IN_DEV_HIDDEN(in_dev2)) {
+		saddr = 0;
+		goto get;
+	}
+
+	switch (mode) {
 	default:
 	case 0:		/* By default announce any local IP */
+		if (saddr)
+			break;
 		if (skb && inet_addr_type(dev_net(dev),
 					  ip_hdr(skb)->saddr) == RTN_LOCAL)
 			saddr = ip_hdr(skb)->saddr;
@@ -345,8 +362,9 @@
 	case 1:		/* Restrict announcements of saddr in same subnet */
 		if (!skb)
 			break;
-		saddr = ip_hdr(skb)->saddr;
-		if (inet_addr_type(dev_net(dev), saddr) == RTN_LOCAL) {
+		if (saddr ||
+		    (saddr = ip_hdr(skb)->saddr,
+		     inet_addr_type(dev_net(dev), saddr) == RTN_LOCAL)) {
 			/* saddr should be known to target */
 			if (inet_addr_onlink(in_dev, target, saddr))
 				break;
@@ -356,6 +374,8 @@
 	case 2:		/* Avoid secondary IPs, get a primary/preferred one */
 		break;
 	}
+
+get:
 	rcu_read_unlock();
 
 	if (!saddr)
@@ -433,6 +453,21 @@
 	return flag;
 }
 
+static int arp_hidden(u32 tip, struct net_device *dev)
+{
+	struct net_device *dev2;
+	struct in_device *in_dev2;
+	int ret = 0;
+
+	if (!IPV4_DEVCONF_ALL(dev_net(dev), HIDDEN))
+		return 0;
+
+	if ((dev2 = __ip_dev_find(dev_net(dev), tip, false)) && dev2 != dev &&
+	    (in_dev2 = __in_dev_get_rcu(dev2)) && IN_DEV_HIDDEN(in_dev2))
+		ret = 1;
+	return ret;
+}
+
 /* OBSOLETE FUNCTIONS */
 
 /*
@@ -836,6 +871,7 @@
 	if (sip == 0) {
 		if (arp->ar_op == htons(ARPOP_REQUEST) &&
 		    inet_addr_type(net, tip) == RTN_LOCAL &&
+		    !arp_hidden(tip, dev) &&
 		    !arp_ignore(in_dev, sip, tip))
 			arp_send(ARPOP_REPLY, ETH_P_ARP, sip, dev, tip, sha,
 				 dev->dev_addr, sha);
@@ -854,6 +890,8 @@
 			dont_send = arp_ignore(in_dev, sip, tip);
 			if (!dont_send && IN_DEV_ARPFILTER(in_dev))
 				dont_send = arp_filter(sip, tip, dev);
+			if (!dont_send && skb->pkt_type != PACKET_HOST)
+				dont_send = arp_hidden(tip,dev);
 			if (!dont_send) {
 				n = neigh_event_ns(&arp_tbl, sha, &sip, dev);
 				if (n) {
