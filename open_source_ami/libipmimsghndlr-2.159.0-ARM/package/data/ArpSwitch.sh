#!/bin/sh

if [ ! "$#" = 2 ]; then
    echo "Usage:"
    echo "    ArpSwitch eth0 0, or"
    echo "    ArpSwitch eth0 8"
    exit 1
fi
[ a"$1" = a ] && exit 1
[ a"$2" = a ] && exit 1

ARP_CONTROL="/proc/sys/net/ipv4/conf/$1/arp_ignore"
[ -f "${ARP_CONTROL}" ] && echo $2 > ${ARP_CONTROL}

