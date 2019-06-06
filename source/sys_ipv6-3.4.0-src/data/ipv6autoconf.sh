#!/bin/sh

#echo " 02-ipv6autoconf : IFACE - $IFACE"

VLAN_IFACE=0

# check if iface is vlan interface
case "$IFACE" in
  *:*)
    exit 0
  ;;

  eth*.*|bond*.*|wlan*.*)
    VLAN_ENABLE=1
  ;;
  
  *)
    VLAN_ENABLE=0
  ;;
esac

    if [ "$VLAN_ENABLE" == "0" ]; then
        IPV6MODE=`cat /etc/network/interfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
    else
        IPV6MODE=`cat /conf/vlaninterfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
    fi
    
    if [ "$IPV6MODE" == "autoconf" ]; then
        SOURCE=2    
    
    elif [ "$IPV6MODE" == "static" ]; then
        SOURCE=1
        
	    # if source is static remove all global IPv6 addresses from device
    	if [ "$VLAN_ENABLE" == "0" ]; then	
            STATIC_IPv6=`cat /etc/network/interfaces | grep address | grep : | awk '{print $2}'`
            # Get other IPV6 address configured for interface as we are able to configure 16 IPV6 address statically.
            OTHER_IPV6=`cat /etc/network/interfaces | grep addr | grep -v address | grep : | awk '{print $5}'`
        else
            STATIC_IPv6=`cat /conf/vlaninterfaces | grep address | grep : | awk '{print $2}'`
            # Get other IPV6 address configured for interface as we are able to configure 16 IPV6 address statically.
            OTHER_IPV6=`cat /conf/vlaninterfaces | grep addr | grep -v address | grep : | awk '{print $5}'`
        fi
                
        #Remove IPv6 global addresses
        IPv6_GLOBAL=`ifconfig $IFACE | grep Scope:Global | awk '{print $3}' | grep -v $STATIC_IPv6`
        for j in $IPv6_GLOBAL; do
            if [ $j ]; then
                flag=0
                for ip in $OTHER_IPV6; do
                    if [ $ip == $j ]; then
                        flag=1
                        break
                    fi
                done
                if [ $flag == 0 ]; then
                    #echo "Removing Global address $j from $IFACE"
                    ifconfig $IFACE del $j
                fi
            fi
        done
        
    else
    	echo "IPv6 entry is not present in interfaces file"
    	SOURCE=0
    	
    	#Remove IPv6 global addresses
		#IPv6_GLOBAL=`ifconfig eth0 | grep Scope:Global | awk '{print $3}'`
		IPv6_GLOBAL=`ifconfig $IFACE | grep Scope:Global | awk '{print $3}'`
	
		for j in $IPv6_GLOBAL; do
	    	if [ $j ]; then
	        	#echo "Removing Global address $j from $IFACE"
	        	ifconfig $IFACE del $j
	    	fi
		done
	
		#Remove IPv6 Link-local Address
		#LINKADDR=`ifconfig eth0 | grep Scope:Link | awk '{print $3}'`
		LINKADDR=`ifconfig $IFACE | grep Scope:Link | awk '{print $3}'`
		
#		if [ -z "$LINKADDR" ]; then
#	   	 	sleep 1
#	   		LINKADDR=`ifconfig eth0 | grep Scope:Link | awk '{print $3}'`
#	    	echo "LINKADDR : $LINKADDR"
#        fi
        
		#echo "Removing Link-local address $LINKADDR from eth0"
#    	ifconfig eth0 del $LINKADDR
    	ifconfig $IFACE del $LINKADDR
      	
      	#exit 0
    fi
		
    #echo "SOURCE : $SOURCE"
		
    if ! [ -e /proc/sys/net/ipv6/conf/$IFACE/accept_ra ]; then
        echo "ipv6autoconf.sh : Nodes not present for IPv6"
    	exit 0
    fi

    if [ $SOURCE == 1 ]; then
		ACCEPT_RA_EXP_VAL=0
		AUTOCONF_EXP_VAL=0
    elif [ $SOURCE == 2 ]; then
		ACCEPT_RA_EXP_VAL=1
		AUTOCONF_EXP_VAL=1
    else
    	ACCEPT_RA_EXP_VAL=0
		AUTOCONF_EXP_VAL=0
    fi
    
    echo $ACCEPT_RA_EXP_VAL > /proc/sys/net/ipv6/conf/$IFACE/accept_ra
    echo $AUTOCONF_EXP_VAL > /proc/sys/net/ipv6/conf/$IFACE/autoconf
    
    
    ACCEPT_RA=`cat /proc/sys/net/ipv6/conf/$IFACE/accept_ra`
    AUTOCONF=`cat /proc/sys/net/ipv6/conf/$IFACE/autoconf`
    while [ $ACCEPT_RA != $ACCEPT_RA_EXP_VAL -o $AUTOCONF != $AUTOCONF_EXP_VAL ]
    do
	    echo $ACCEPT_RA_EXP_VAL > /proc/sys/net/ipv6/conf/$IFACE/accept_ra
	    echo $AUTOCONF_EXP_VAL > /proc/sys/net/ipv6/conf/$IFACE/autoconf
    
	    sleep 1
    
	    ACCEPT_RA=`cat /proc/sys/net/ipv6/conf/$IFACE/accept_ra`
	    AUTOCONF=`cat /proc/sys/net/ipv6/conf/$IFACE/autoconf`
    done

