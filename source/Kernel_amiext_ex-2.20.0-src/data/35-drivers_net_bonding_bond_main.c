--- linux-3.14.17/drivers/net/bonding/bond_main.c	2014-08-13 21:38:34.000000000 -0400
+++ linux-3.14.17-ami/drivers/net/bonding/bond_main.c	2014-08-22 12:16:34.655224459 -0400
@@ -349,6 +349,15 @@
 	bond_for_each_slave(bond, slave, iter) {
 		if (slave->link == BOND_LINK_UP) {
 			if (!netif_carrier_ok(bond->dev)) {
+ 				if (rtnl_trylock())
+                {
+                   netdev_link_up(bond->dev);
+                   rtnl_unlock();
+                }
+                else
+                {
+                   netdev_link_up(bond->dev);
+                }
 				netif_carrier_on(bond->dev);
 				return 1;
 			}
@@ -358,6 +367,15 @@
 
 down:
 	if (netif_carrier_ok(bond->dev)) {
+  		if (rtnl_trylock())
+        {
+            netdev_link_down(bond->dev);
+            rtnl_unlock();
+        }
+        else
+        {
+            netdev_link_down(bond->dev);
+        }
 		netif_carrier_off(bond->dev);
 		return 1;
 	}
@@ -843,6 +861,17 @@
 			bond_set_slave_inactive_flags(old_active,
 						      BOND_SLAVE_NOTIFY_NOW);
 
+		if (rtnl_trylock())
+        {
+            netdev_link_down(bond->dev);
+            rtnl_unlock();
+        }
+        else
+        {
+            netdev_link_down(bond->dev);
+        }
+
+
 		if (new_active) {
 			bool should_notify_peers = false;
 
@@ -862,6 +891,17 @@
 
 			write_unlock_bh(&bond->curr_slave_lock);
 
+		    if (rtnl_trylock())
+            {
+                netdev_link_up(bond->dev);
+                rtnl_unlock();
+            }
+            else
+            {
+                netdev_link_up(bond->dev);
+            }
+
+
 			call_netdevice_notifiers(NETDEV_BONDING_FAILOVER, bond->dev);
 			if (should_notify_peers)
 				call_netdevice_notifiers(NETDEV_NOTIFY_PEERS,
@@ -1092,6 +1132,15 @@
 					    struct bonding *bond)
 {
 	if (bond_is_slave_inactive(slave)) {
+
+		/* Added by AMI:
+         * NC-SI packets should not dropped for inactive slave,
+   		 * so that link status will be updated properly for
+         * active back-up mode.
+      	 */
+		if(skb->protocol == __constant_htons(0x88F8))
+        	return false;
+
 		if (bond->params.mode == BOND_MODE_ALB &&
 		    skb->pkt_type != PACKET_BROADCAST &&
 		    skb->pkt_type != PACKET_MULTICAST)
@@ -2001,6 +2050,19 @@
 				slave->speed == SPEED_UNKNOWN ? 0 : slave->speed,
 				slave->duplex ? "full" : "half");
 
+
+			if (rtnl_trylock())
+            {
+                netdev_link_up(bond->dev);
+                rtnl_unlock();
+            }
+            else
+            {
+                netdev_link_up(bond->dev);
+            }
+
+            netif_carrier_on(slave->dev);
+
 			/* notify ad that the link status has changed */
 			if (bond->params.mode == BOND_MODE_8023AD)
 				bond_3ad_handle_link_change(slave, BOND_LINK_UP);
