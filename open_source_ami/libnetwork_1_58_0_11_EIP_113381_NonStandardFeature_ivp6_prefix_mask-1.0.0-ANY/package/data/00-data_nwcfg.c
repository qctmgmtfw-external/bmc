--- .pristine/libnetwork-1.58.0-src/data/nwcfg.c Thu Jan 31 15:43:10 2013
+++ source/libnetwork-1.58.0-src/data/nwcfg.c Thu Jan 31 15:52:32 2013
@@ -218,11 +218,86 @@
 	{
 		return -1;
 	}
-	else if( (IPv6Addr->s6_addr[0] & 0x10) == 0x10)
+	else if( (IPv6Addr->s6_addr[0] & 0xF0) == 0x10)  //1000::/4
 	{
 		return -1;
 	}
-
+		else if( (IPv6Addr->s6_addr[0] & 0xFF) == 0x00 ) //0000::/8
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFF) == 0x01 ) //0100::/8
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFE) == 0x02 ) //0200::/7
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFC) == 0x04 ) //     0400::/6
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xF8) == 0x08  )//0800::/5
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xF8) == 0x08  )//0800::/5
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xE0) == 0x40  )//4000::/3
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xE0) == 0x60  )//6000::/3
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xE0) == 0x80  )//8000::/3
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xE0) == 0xA0  )//A000::/3
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xE0) == 0xC0  )//C000::/3
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xF0) == 0xE0  )//E000::/4
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xF8) == 0xF0  )//F000::/5
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFC) == 0xF8  )//F800::/6
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFE) == 0xFC  )//FC00::/7
+	{
+		return -1;
+	}
+	else if( (IPv6Addr->s6_addr[0] & 0xFF) == 0xFF  )//FF00::/8
+	{
+		return -1;
+	}
+	else if( ( (IPv6Addr->s6_addr[0] & 0xFF) == 0xFE  ) && ( (IPv6Addr->s6_addr[1] & 0x80) == 0x00  ))//FE00::/9
+	{
+		return -1;
+	}
+	else if( ( (IPv6Addr->s6_addr[0] & 0xFF) == 0xFE  ) && ( (IPv6Addr->s6_addr[1] & 0xC0) == 0x80  ))//FE80::/10
+	{
+		return -1;
+	}
+	else if( ( (IPv6Addr->s6_addr[0] & 0xFF) == 0xFE  ) && ( (IPv6Addr->s6_addr[1] & 0xC0) == 0xC0  ))//FEC0::/10
+	{
+		return -1;
+	}
 	return 0;
 }
 
