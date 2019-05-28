--- linux-3.14.17/include/linux/inetdevice.h	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/include/linux/inetdevice.h	2014-08-21 11:33:07.477553474 -0400
@@ -107,6 +107,7 @@
 						      SECURE_REDIRECTS)
 #define IN_DEV_IDTAG(in_dev)		IN_DEV_CONF_GET(in_dev, TAG)
 #define IN_DEV_MEDIUM_ID(in_dev)	IN_DEV_CONF_GET(in_dev, MEDIUM_ID)
+#define IN_DEV_HIDDEN(in_dev)		IN_DEV_ANDCONF((in_dev), HIDDEN)
 #define IN_DEV_PROMOTE_SECONDARIES(in_dev) \
 					IN_DEV_ORCONF((in_dev), \
 						      PROMOTE_SECONDARIES)
