--- .pristine/sys_ipv6-2.3.0-src/data/ipv6autoconf.sh Fri Feb  3 16:17:32 2012
+++ source/sys_ipv6-2.3.0-src/data/ipv6autoconf.sh Tue Jan 31 12:38:02 2012
@@ -41,13 +41,13 @@
                 
         #Remove IPv6 global addresses
         #IPv6_GLOBAL=`ifconfig eth0 | grep Scope:Global | awk '{print $3}' | grep -v $STATIC_IPv6`
-        IPv6_GLOBAL=`ifconfig $IFACE | grep Scope:Global | awk '{print $3}' | grep -v $STATIC_IPv6`
-        for j in $IPv6_GLOBAL; do
-            if [ $j ]; then
-                #echo "Removing Global address $j from $IFACE"
-                ifconfig $IFACE del $j
-            fi
-        done
+#        IPv6_GLOBAL=`ifconfig $IFACE | grep Scope:Global | awk '{print $3}' | grep -v $STATIC_IPv6`
+#        for j in $IPv6_GLOBAL; do
+#            if [ $j ]; then
+#                #echo "Removing Global address $j from $IFACE"
+#                ifconfig $IFACE del $j
+#            fi
+#        done
         
     else
     	echo "IPv6 entry is not present in interfaces file"
