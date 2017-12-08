--- .pristine/libifc-1.98.0-src/data/webifc_XportDevice.c Wed Sep 14 16:37:04 2011
+++ source/libifc-1.98.0-src/data/webifc_XportDevice.c Thu Sep 15 14:15:04 2011
@@ -1495,6 +1495,7 @@
 	return(0);
 }
 
+#ifdef CONFIG_SPX_FEATURE_NCSI_IPMI_COMMAND_SUPPORT
 WEBIFC_HAPIFN_DEFINE(WebGetNCSICfg)
 {
 	int retval = 0;
@@ -1579,6 +1580,7 @@
 
 	return 0;
 }
+#endif
 
 #ifdef CONFIG_SPX_FEATURE_PHY_SUPPORT
 WEBIFC_HAPIFN_DEFINE(WebGetPHYSupportCfg)
