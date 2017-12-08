#! /bin/sh
### BEGIN INIT INFO
# Provides:          arpflux
# Required-Start:    
# Required-Stop: 
# Default-Start:     S
# Default-Stop:
# Short-Description: Fix ARP FLUX
# Description:
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin

if [ -e /proc/sys/net/ipv4/conf/all/hidden ]; then
    echo -n "Fixing Global ARP Flux ..."
    echo 1 > /proc/sys/net/ipv4/conf/all/hidden
    echo "done."
fi

for idx in 0 1 2 3 4 5 6 7 8
do
	if [ -e /proc/sys/net/ipv4/conf/eth$idx/hidden ]; then
    		echo -n "Fixing ARP Flux for eth$idx ..."
       	 	echo 1 > /proc/sys/net/ipv4/conf/eth$idx/hidden
        	echo "done."
    	fi
done
exit 0
