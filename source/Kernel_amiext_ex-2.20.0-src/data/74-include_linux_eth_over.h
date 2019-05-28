--- linux_orig/include/linux/eth_over.h	2015-04-09 21:12:11.710834827 +0530
+++ linux/include/linux/eth_over.h	2015-04-09 21:46:42.601103818 +0530
@@ -67,3 +67,24 @@
 }
 
 
+#define ethtool_override_set_pauseparam(index,netdev,cmd)					\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].set_pauseparam != NULL)			\
+		{									\
+			return ethtool_override_ops[index].set_pauseparam(netdev,cmd);	\
+		}									\
+	}										\
+}											
+
+#define ethtool_override_get_pauseparam(index,netdev,cmd)					\
+{											\
+	if (ethtool_override_eth[index]) 						\
+	{										\
+		if (ethtool_override_ops[index].get_pauseparam != NULL)			\
+		{									\
+			return ethtool_override_ops[index].get_pauseparam(netdev,cmd);	\
+		}									\
+	}										\
+}											
