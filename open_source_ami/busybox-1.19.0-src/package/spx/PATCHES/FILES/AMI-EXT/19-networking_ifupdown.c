--- busybox/networking/ifupdown.c	2011-07-20 17:32:11.653796784 +0530
+++ busybox.new/networking/ifupdown.c	2011-07-20 17:31:19.502891081 +0530
@@ -372,6 +372,8 @@
 static int dhcp_up6(struct interface_defn_t *ifd, execfn *exec)
 {
 	int result = 0;
+    if (!execute("ifconfig %iface%[[ hw %hwaddress%]] up", ifd, exec))
+        return 0;
 
     if (!(result = execute("dhcp6.script %iface%", ifd, exec))){
     	return execute("dhcp6c -p /var/run/dhcp6c.%iface%.pid %iface% ", ifd, exec);
