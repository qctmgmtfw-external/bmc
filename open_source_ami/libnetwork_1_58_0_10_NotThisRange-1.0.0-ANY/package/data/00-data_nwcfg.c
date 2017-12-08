--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Thu Oct  4 14:34:43 2012
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Thu Oct  4 14:35:16 2012
@@ -218,6 +218,10 @@
 	{
 		return -1;
 	}
+	else if( (IPv6Addr->s6_addr[0] & 0x10) == 0x10)
+	{
+		return -1;
+	}
 
 	return 0;
 }
