--- ../../../linux/net/ipv6/ndisc.c	2015-05-04 12:40:50.485631844 +0530
+++ linux/net/ipv6/ndisc.c	2015-05-04 12:59:35.491919365 +0530
@@ -1193,7 +1193,14 @@
 	if (rt)
 		rt6_set_expires(rt, jiffies + (HZ * lifetime));
 	if (ra_msg->icmph.icmp6_hop_limit) {
-		in6_dev->cnf.hop_limit = ra_msg->icmph.icmp6_hop_limit;
+    	/* Only set hop_limit on the interface if it is higher than
+		 * the current hop_limit.
+		 */
+		 if (in6_dev->cnf.hop_limit < ra_msg->icmph.icmp6_hop_limit) {
+		 in6_dev->cnf.hop_limit = ra_msg->icmph.icmp6_hop_limit;
+		 } else {
+				 ND_PRINTK(2, warn, "RA: Got route advertisement with lower hop_limit than current\n");
+		 }
 		if (rt)
 			dst_metric_set(&rt->dst, RTAX_HOPLIMIT,
 				       ra_msg->icmph.icmp6_hop_limit);
