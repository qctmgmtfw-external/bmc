--- u-boot-2013.07/net/bootp.c	2013-12-13 13:49:25.356958498 -0500
+++ uboot.new/net/bootp.c	2013-12-05 12:17:54.979503948 -0500
@@ -99,10 +99,15 @@
 {
 #if !defined(CONFIG_BOOTP_SERVERIP)
 	IPaddr_t tmp_ip;
-
-	NetCopyIP(&tmp_ip, &bp->bp_siaddr);
-	if (tmp_ip != 0)
-		NetCopyIP(&NetServerIP, &bp->bp_siaddr);
+	
+/* This is the DHCP/BOOTP Server Address. It may not be our boot server */
+	/* So copy only if we have not already set our boot server */
+	if (NetServerIP == 0)
+	{
+		NetCopyIP(&tmp_ip, &bp->bp_siaddr);
+		if (tmp_ip != 0)
+			NetCopyIP(&NetServerIP, &bp->bp_siaddr);
+	}
 	memcpy(NetServerEther, ((struct ethernet_hdr *)NetRxPacket)->et_src, 6);
 #endif
 	NetCopyIP(&NetOurIP, &bp->bp_yiaddr);
