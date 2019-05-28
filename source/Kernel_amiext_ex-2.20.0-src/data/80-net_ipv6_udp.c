--- linux_org/net/ipv6/udp.c	2015-11-25 08:03:05.238806987 -0500
+++ linux/net/ipv6/udp.c	2015-11-25 08:06:18.771887915 -0500
@@ -515,8 +515,7 @@
 	}
 	unlock_sock_fast(sk, slow);
 
-	if (noblock)
-		return -EAGAIN;
+	cond_resched();
 
 	/* starting over for a new packet */
 	msg->msg_flags &= ~MSG_TRUNC;
