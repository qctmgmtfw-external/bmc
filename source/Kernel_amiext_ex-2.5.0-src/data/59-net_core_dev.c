--- linux-3.14.17/net/core/dev.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/net/core/dev.c	2014-08-21 11:23:00.841542085 -0400
@@ -1154,6 +1154,18 @@
 	return err;
 }
 
+void netdev_link_up(struct net_device *dev)
+{
+	call_netdevice_notifiers(NETDEV_LINK_UP, dev);
+}
+EXPORT_SYMBOL(netdev_link_up);
+
+void netdev_link_down(struct net_device *dev)
+{
+	call_netdevice_notifiers(NETDEV_LINK_DOWN, dev);
+}
+EXPORT_SYMBOL(netdev_link_down);
+
 /**
  *	dev_set_alias - change ifalias of a device
  *	@dev: device
