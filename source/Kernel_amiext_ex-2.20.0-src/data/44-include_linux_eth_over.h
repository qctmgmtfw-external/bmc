--- linux.pristine/include/linux/eth_over.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-amiext/include/linux/eth_over.h	2014-02-03 15:11:00.003029096 -0500
@@ -0,0 +1,69 @@
+#include <linux/init.h>
+#include <linux/etherdevice.h>
+#include <linux/delay.h>
+#include <linux/ethtool.h>
+#include <linux/mii.h>
+
+extern  struct ethtool_ops ethtool_override_ops[CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT];
+extern  int ethtool_override_eth[CONFIG_SPX_FEATURE_GLOBAL_NIC_COUNT];
+
+
+# define  ethtool_override_get_drvinfo(index,netdev,info)				\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].get_drvinfo != NULL)			\
+		{									\
+			ethtool_override_ops[index].get_drvinfo(netdev,info);		\
+			return;								\
+		}									\
+	}										\
+}											
+
+
+#define ethtool_override_get_settings(index,netdev,cmd)					\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].get_settings != NULL)			\
+		{									\
+			return ethtool_override_ops[index].get_settings(netdev,cmd);	\
+		}									\
+	}										\
+}											
+
+
+#define ethtool_override_set_settings(index,netdev,cmd)					\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].set_settings != NULL)			\
+		{									\
+			return ethtool_override_ops[index].set_settings(netdev,cmd);	\
+		}									\
+	}										\
+}											
+
+#define ethtool_override_nway_reset(index,netdev)					\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].nway_reset != NULL)			\
+		{									\
+			return ethtool_override_ops[index].nway_reset(netdev);		\
+		}									\
+	}										\
+}											
+
+#define ethtool_override_get_link(index,netdev)						\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].get_link != NULL)			\
+		{									\
+			return ethtool_override_ops[index].get_link(netdev);		\
+		}									\
+	}										\
+}
+
+
