--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Wed May  9 16:57:40 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Thu May 10 14:56:24 2012
@@ -191,6 +191,39 @@
 }
 
 /*
+ * @fn IsValidGlobalIPv6Addr
+ * @brief This function will check the given IPv6 address as global IP address or not
+ * @params IPv6Addr [in] IPv6 address
+ * @return Return 0 on success and -1 on failure
+ */
+int IsValidGlobalIPv6Addr(struct in6_addr * IPv6Addr)
+{
+	if(IN6_IS_ADDR_LOOPBACK(IPv6Addr))
+	{
+		return -1;
+	}
+	else if(IN6_IS_ADDR_MULTICAST(IPv6Addr))
+	{
+		return -1;
+	}
+	else if(IN6_IS_ADDR_LINKLOCAL(IPv6Addr))
+	{
+		return -1;
+	}
+	else if (IN6_IS_ADDR_SITELOCAL(IPv6Addr))
+	{
+		return -1;
+	}
+	else if(IN6_IS_ADDR_V4MAPPED(IPv6Addr))
+	{
+		return -1;
+	}
+
+	return 0;
+}
+
+
+/*
  * @fn ConvertNETMaskArrayTonum
  * @brief This function is used to calculate the IPV6 prefix length 
  * @params var[in] Netmask IPv6 address
