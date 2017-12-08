--- .pristine/sys_ipv6-2.3.0-src/data/ipv6autoconf.sh Thu Jan 12 11:49:47 2012
+++ source/sys_ipv6-2.3.0-src/data/ipv6autoconf.sh Fri Jan 13 14:47:43 2012
@@ -3,7 +3,28 @@
 #echo " 02-ipv6autoconf : IFACE - $IFACE"
 
 #    IPV6MODE=`cat /etc/network/interfaces | grep "inet6" | awk '{print $4}'`
-    IPV6MODE=`cat /etc/network/interfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
+VLAN_IFACE=0
+
+# check if iface is vlan interface
+	case "$IFACE" in
+  	*:*)
+    	exit 0
+  	;;
+
+  	eth*.*|bond*.*|wlan*.*)
+    	VLAN_ENABLE=1
+  	;;
+  
+  	*)
+    	VLAN_ENABLE=0
+  	;;
+	esac
+
+    if [ "$VLAN_ENABLE" == "0" ]; then
+        IPV6MODE=`cat /etc/network/interfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
+    else
+        IPV6MODE=`cat /conf/vlaninterfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
+    fi
 
     if [ "$IPV6MODE" == "autoconf" ]; then
         SOURCE=2    
@@ -12,7 +33,11 @@
         SOURCE=1
         
 	# if source is static remove all global IPv6 addresses from device
-        STATIC_IPv6=`cat /etc/network/interfaces | grep address | grep : | awk '{print $2}'`
+    	if [ "$VLAN_ENABLE" == "0" ]; then	
+            STATIC_IPv6=`cat /etc/network/interfaces | grep address | grep : | awk '{print $2}'`
+        else
+            STATIC_IPv6=`cat /conf/vlaninterfaces | grep address | grep : | awk '{print $2}'`
+        fi        
                 
         #Remove IPv6 global addresses
         #IPv6_GLOBAL=`ifconfig eth0 | grep Scope:Global | awk '{print $3}' | grep -v $STATIC_IPv6`
