--- linux/drivers/net/bonding/bond_main.c	2011-08-29 14:52:49.091878282 +0530
+++ linux.new/drivers/net/bonding/bond_main.c	2011-08-29 14:50:00.653756179 +0530
@@ -2374,6 +2374,15 @@
 			       ": %s: link status definitely "
 			       "up for interface %s.\n",
 			       bond->dev->name, slave->dev->name);
+			if (rtnl_trylock())
+			{
+				netdev_link_up(bond->dev);
+				rtnl_unlock();
+			}
+			else
+			{
+				netdev_link_up(bond->dev);
+			}
 
 			/* notify ad that the link status has changed */
 			if (bond->params.mode == BOND_MODE_8023AD)
@@ -2404,6 +2413,15 @@
 			       ": %s: link status definitely down for "
 			       "interface %s, disabling it\n",
 			       bond->dev->name, slave->dev->name);
+			if (rtnl_trylock())
+			{
+				netdev_link_down(bond->dev);
+				rtnl_unlock();
+			}
+			else
+			{
+				netdev_link_down(bond->dev);
+			}
 
 			if (bond->params.mode == BOND_MODE_8023AD)
 				bond_3ad_handle_link_change(slave,
