--- linux-3.14.17/include/linux/netdevice.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/linux/netdevice.h	2014-08-21 11:43:16.693564911 -0400
@@ -1762,6 +1762,9 @@
 #define NETDEV_CHANGEUPPER	0x0015
 #define NETDEV_RESEND_IGMP	0x0016
 #define NETDEV_PRECHANGEMTU	0x0017 /* notify before mtu change happened */
+#define NETDEV_LINK_UP     0x0101  /* Link up Notifier   */
+#define NETDEV_LINK_DOWN   0x0102  /* Link down Notifier */
+
 
 int register_netdevice_notifier(struct notifier_block *nb);
 int unregister_netdevice_notifier(struct notifier_block *nb);
@@ -2954,6 +2957,8 @@
 void netdev_state_change(struct net_device *dev);
 void netdev_notify_peers(struct net_device *dev);
 void netdev_features_change(struct net_device *dev);
+void netdev_link_up(struct net_device *dev);
+void netdev_link_down(struct net_device *dev);
 /* Load a device via the kmod */
 void dev_load(struct net *net, const char *name);
 struct rtnl_link_stats64 *dev_get_stats(struct net_device *dev,
