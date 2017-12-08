--- busybox-1.13.2/include/libbb.h	2011-07-25 16:24:53.944941954 -0400
+++ busybox.new/include/libbb.h	2011-07-25 14:33:04.593687796 -0400
@@ -489,7 +489,7 @@
 /* Connect to peer identified by lsa */
 int xconnect_stream(const len_and_sockaddr *lsa) FAST_FUNC;
 /* Get remote address of connected or accepted socket */
-len_and_sockaddr *get_peer_lsa(int fd);
+len_and_sockaddr FAST_FUNC *get_peer_lsa(int fd);
 /* Return malloc'ed len_and_sockaddr with socket address of host:port
  * Currently will return IPv4 or IPv6 sockaddrs only
  * (depending on host), but in theory nothing prevents e.g.
