--- linux.pristine/drivers/net/eth_over.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-amiext/drivers/net/eth_over.c	2014-02-03 15:10:59.999029095 -0500
@@ -0,0 +1,44 @@
+#include <linux/init.h>
+#include <linux/etherdevice.h>
+#include <linux/delay.h>
+#include <linux/ethtool.h>
+#include <linux/mii.h>
+
+
+struct ethtool_ops ethtool_override_ops[CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT];
+#if (CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT == 2)
+int ethtool_override_eth[CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT] = { 0, 0};
+#else
+int ethtool_override_eth[CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT] = { 0 };
+#endif
+
+int
+register_ethtool_override(struct ethtool_ops *ops , int ethindex)
+{
+	if (ethindex >= CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT)
+		return -1;
+	ethtool_override_ops[ethindex].get_drvinfo 	= ops->get_drvinfo,
+	ethtool_override_ops[ethindex].get_link  	= ops->get_link;
+	ethtool_override_ops[ethindex].nway_reset 	= ops->nway_reset;
+	ethtool_override_ops[ethindex].get_settings 	= ops->get_settings;
+	ethtool_override_ops[ethindex].set_settings 	= ops->set_settings;
+
+	ethtool_override_eth[ethindex] = 1;
+	return 0;
+}
+
+int
+unregister_ethtool_override(int ethindex)
+{
+	if (ethindex >= CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT)
+		return -1;
+
+	ethtool_override_eth[ethindex] = 0;
+	return 0;
+}
+
+EXPORT_SYMBOL(register_ethtool_override);
+EXPORT_SYMBOL(unregister_ethtool_override);
+EXPORT_SYMBOL(ethtool_override_eth);
+EXPORT_SYMBOL(ethtool_override_ops);
+
