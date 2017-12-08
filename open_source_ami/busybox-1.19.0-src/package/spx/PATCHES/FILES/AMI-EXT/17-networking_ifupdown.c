--- busybox/networking/ifupdown.c	2011-06-22 12:27:27.055779154 +0530
+++ busybox.new/networking/ifupdown.c	2011-06-22 12:26:36.202756982 +0530
@@ -355,7 +355,7 @@
 #else
 	result = execute("ifconfig %iface%[[ media %media%]][[ hw %hwaddress%]][[ mtu %mtu%]] up", ifd, exec);
 	result += execute("ifconfig %iface% add %address%/%netmask%", ifd, exec);
-	result += execute("[[route -A inet6 add ::/0 gw %gateway%]]", ifd, exec);
+	result += execute("[[route -A inet6 add ::/0 gw %gateway% %iface%]]", ifd, exec);
 #endif
 	return ((result == 3) ? 3 : 0);
 }
