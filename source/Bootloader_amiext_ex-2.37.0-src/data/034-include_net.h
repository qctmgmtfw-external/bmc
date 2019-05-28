--- uboot_old/include/net.h	2013-07-23 07:58:13.000000000 -0400
+++ uboot/include/net.h	2014-12-19 04:22:33.112798448 -0500
@@ -242,6 +242,7 @@
 #define PROT_ARP	0x0806		/* IP ARP protocol		*/
 #define PROT_RARP	0x8035		/* IP ARP protocol		*/
 #define PROT_VLAN	0x8100		/* IEEE 802.1q protocol		*/
+#define PROT_NCSI	0x88F8      /* DMTP NCSI 1.0 protocol       */
 
 #define IPPROTO_ICMP	 1	/* Internet Control Message Protocol	*/
 #define IPPROTO_UDP	17	/* User Datagram Protocol		*/
@@ -437,7 +438,9 @@
 
 enum proto_t {
 	BOOTP, RARP, ARP, TFTPGET, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP,
-	TFTPSRV, TFTPPUT, LINKLOCAL
+	TFTPSRV, TFTPPUT, LINKLOCAL,
+	R2C,NCSI,
+    RECOVERY
 };
 
 /* from net/net.c */
