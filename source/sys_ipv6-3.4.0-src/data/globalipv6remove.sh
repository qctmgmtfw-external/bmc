#!/bin/sh

# echo " globalipv6remove : IFACE - $IFACE"

# check if iface is vlan interface
. /etc/nwhelperapi.sh
. /conf/dns.conf
case "$IFACE" in
  *:*)
    exit 0
  ;;

  eth*.*|bond*.*|wlan*.*)
    VLAN_ENABLE=1
  ;;
  eth*|bond*|wlan*)
    VLAN_ENABLE=0
  ;;
  *)
   exit 0
  ;;
esac

if [ "$SET_DNS" == 'yes' -o "$SET_DNS" == 'y' ]; then
Clear_IPV6
fi

    if [ "$VLAN_ENABLE" == "0" ]; then
        IPV6MODE=`cat /etc/network/interfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
    else
        IPV6MODE=`cat /conf/vlaninterfaces | grep $IFACE | grep "inet6" | awk '{print $4}'`
    fi
    
    if [ "$IPV6MODE" == "autoconf" ]; then  
        # Remove IPv6 global addresses
        IPv6_GLOBAL=`ifconfig $IFACE | grep Scope:Global | awk '{print $3}'`
        for j in $IPv6_GLOBAL; do
            if [ $j ]; then
               # echo "Removing Global address $j from $IFACE"
               ifconfig $IFACE del $j
            fi
        done
    fi	

