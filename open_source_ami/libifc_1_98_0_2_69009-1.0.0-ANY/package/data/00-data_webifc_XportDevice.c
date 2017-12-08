--- .pristine/libifc-1.98.0-src/data/webifc_XportDevice.c Wed Sep 14 15:41:10 2011
+++ source/libifc-1.98.0-src/data/webifc_XportDevice.c Wed Sep 14 15:46:40 2011
@@ -1143,17 +1143,22 @@
 			goto error_out;
 		}
 		TDBG ("Success converting v4 DNS Server1::%d.%d.%d.%d\n",
-				v4dnscfg.DNSServer1[0],v4dnscfg.DNSServer1[1],v4dnscfg.DNSServer1[2],v4dnscfg.DNSServer1[3]);
-
-		retval = inet_pton(AF_INET, (char *)v4DNS2, v4dnscfg.DNSServer2);
-		if (retval <= 0)
-		{
-			TCRIT("Error while converting V4 DNS Server2::%d\n",retval);
-			retval = INVALID_V4IP;
-			goto error_out;
-		}
-		TDBG ("Success converting v4 DNS Server2::%d.%d.%d.%d\n",
-				v4dnscfg.DNSServer2[0],v4dnscfg.DNSServer2[1],v4dnscfg.DNSServer2[2],v4dnscfg.DNSServer2[3]);
+			v4dnscfg.DNSServer1[0], v4dnscfg.DNSServer1[1],
+			v4dnscfg.DNSServer1[2], v4dnscfg.DNSServer1[3]);
+
+		if (strlen(v4DNS2) != 0)
+		{
+			retval = inet_pton(AF_INET, (char *)v4DNS2, v4dnscfg.DNSServer2);
+			if (retval <= 0)
+			{
+				TCRIT("Error while converting V4 DNS Server2::%d\n",retval);
+				retval = INVALID_V4IP;
+				goto error_out;
+			}
+			TDBG ("Success converting v4 DNS Server2::%d.%d.%d.%d\n",
+				v4dnscfg.DNSServer2[0], v4dnscfg.DNSServer2[1],
+				v4dnscfg.DNSServer2[2], v4dnscfg.DNSServer2[3]);
+		}
 	}
 
 	/* This block will validate and fill the v6 DNS server configuration arguments */
