--- uboot_old/net/net.c	2013-07-23 07:58:13.000000000 -0400
+++ uboot/net/net.c	2014-12-19 04:21:09.612117669 -0500
@@ -187,7 +187,7 @@
 uchar *NetRxPackets[PKTBUFSRX];
 
 /* Current UDP RX packet handler */
-static rxhand_f *udp_packet_handler;
+static rxhand_f *udp_packet_handler = NULL;
 /* Current ARP RX packet handler */
 static rxhand_f *arp_packet_handler;
 #ifdef CONFIG_CMD_TFTPPUT
@@ -207,6 +207,16 @@
 
 static int NetTryCount;
 
+#ifdef CONFIG_NCSI_SUPPORT
+extern void NCSI_Start(void);
+#endif
+
+#ifdef CONFIG_R2C_SUPPORT
+extern void R2C_Initiate(char * Ether);
+#endif
+
+
+
 /**********************************************************************/
 
 static int on_bootfile(const char *name, const char *value, enum env_op op,
@@ -323,6 +333,9 @@
 {
 	bd_t *bd = gd->bd;
 	int ret = -1;
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+    char *value;
+#endif
 
 	NetRestarted = 0;
 	NetDevExists = 0;
@@ -351,7 +364,24 @@
 	 */
 	debug_cond(DEBUG_INT_STATE, "--- NetLoop Init\n");
 	NetInitLoop();
-
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+	if(protocol == RECOVERY)
+	{
+		NetServerIP = getenv_IPaddr ("recoveryserverip");
+		load_addr = YAFU_IMAGE_UPLOAD_LOCATION;
+		value = getenv("recoverybootfile");
+		if(value == NULL)
+       	{
+        	/*default boot file is rom.ima*/
+            copy_filename(BootFile,"rom.ima",sizeof(BootFile));
+        }
+        else
+    	{
+        	copy_filename(BootFile,value,sizeof(BootFile));
+        }
+	}
+#endif
+	
 	switch (net_check_prereq(protocol)) {
 	case 1:
 		/* network not configured */
@@ -434,6 +464,22 @@
 			link_local_start();
 			break;
 #endif
+#ifdef CONFIG_R2C_SUPPORT	
+		case R2C:
+			R2C_Initiate((char *)NetOurEther);
+			break;
+#endif 
+#ifdef CONFIG_NCSI_SUPPORT
+		case NCSI:
+			NCSI_Start();
+			net_state = NETLOOP_SUCCESS;
+			break;
+#endif
+#ifdef CONFIG_SPX_FEATURE_BMC_FIRMWARE_AUTO_RECOVERY
+		 case RECOVERY:
+         	TftpStart(TFTPGET);
+			break;
+#endif
 		default:
 			break;
 		}
@@ -474,6 +520,13 @@
 		 *	Abort if ctrl-c was pressed.
 		 */
 		if (ctrlc()) {
+#ifdef CONFIG_R2C_SUPPORT		
+			if (protocol == R2C)
+			{		
+				printf("\nR2C should not be aborted. Resetting ...\n");
+				do_reset(NULL,0,0,NULL);
+			}
+#endif
 			/* cancel any ARP that may not have completed */
 			NetArpWaitPacketIP = 0;
 
@@ -547,6 +600,7 @@
 			goto done;
 
 		case NETLOOP_FAIL:
+			eth_halt();		/* AMI : Bug Fix */
 			net_cleanup_loop();
 			/* Invalidate the last protocol */
 			eth_set_last_protocol(BOOTP);
@@ -1187,6 +1241,9 @@
 				ntohs(ip->udp_src),
 				ntohs(ip->udp_len) - UDP_HDR_SIZE);
 		break;
+	default:
+		if(udp_packet_handler)
+			(*udp_packet_handler)((uchar *)NetRxPacket,0,0,0,NetRxPacketLen);
 	}
 }
 
@@ -1235,7 +1292,7 @@
 common:
 #endif
 		/* Fall through */
-
+    case RECOVERY:
 	case NETCONS:
 	case TFTPSRV:
 		if (NetOurIP == 0) {
