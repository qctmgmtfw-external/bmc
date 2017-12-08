--- .pristine/sys_base-2.41.0-src/data/defaults/etc/nwhelperapi.sh Fri Feb  3 16:12:20 2012
+++ source/sys_base-2.41.0-src/data/defaults/etc/nwhelperapi.sh Mon Feb 13 19:21:23 2012
@@ -88,7 +88,7 @@
 {
     API_RESOLV_CONF_FILE=/conf/resolv.conf
     API_DNSIP_ALREADY_EXISTS=0
-
+    . /conf/dns.conf
     if [ -f $API_RESOLV_CONF_FILE ]; then
 
 	#find out whether the given IP address is IPv4 or IPv6 address
@@ -152,7 +152,17 @@
     
 	    if [ $2 -gt $COUNT ]; then
                 #echo "AddDNSserverIP_New - There are less DNS entries already present. so just insert your new entry here."
-                echo "nameserver $1" >> $API_RESOLV_CONF_FILE
+                if [ "$SET_IPV6_PRIORITY" == 'yes' -o "$SET_IPV6_PRIORITY" == 'y' ]; then
+					TMP=`grep nameserver $API_RESOLV_CONF_FILE | grep "\."`
+					if ! [ -z "$TMP" ]; then
+						sed '/'"$TMP"'/ i\nameserver '$1 $API_RESOLV_CONF_FILE >> /var/tmp/resolv.conf
+						mv /var/tmp/resolv.conf $API_RESOLV_CONF_FILE
+					else
+						echo "nameserver $1" >> $API_RESOLV_CONF_FILE
+					fi
+				else
+	                echo "nameserver $1" >> $API_RESOLV_CONF_FILE
+				fi
             fi
     fi
 }
