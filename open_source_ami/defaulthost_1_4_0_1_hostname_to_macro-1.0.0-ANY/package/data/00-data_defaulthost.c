--- .pristine/defaulthost-1.4.0-src/data/defaulthost.c Tue Sep 20 11:37:43 2011
+++ source/defaulthost-1.4.0-src/data/defaulthost.c Tue Sep 20 12:28:22 2011
@@ -38,7 +38,7 @@
 	}
 
 
-	strcpy(HostNameWithMACZero,"AMI");
+	strcpy(HostNameWithMACZero,CONFIG_SPX_FEATURE_GLOBAL_HOST_PREFIX);
 	strcat(HostNameWithMACZero,"000000000000");
 
 	// Check hostname settings file. whether host name is configured Automatic/Manual. 
