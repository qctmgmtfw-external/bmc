--- linux_org/net/ipv4/udp.c	2015-11-25 08:03:00.786873838 -0500
+++ linux/net/ipv4/udp.c	2015-11-25 08:04:29.073545427 -0500
@@ -1317,8 +1317,7 @@
 	}
 	unlock_sock_fast(sk, slow);
 
-	if (noblock)
-		return -EAGAIN;
+	cond_resched();
 
 	/* starting over for a new packet */
 	msg->msg_flags &= ~MSG_TRUNC;
