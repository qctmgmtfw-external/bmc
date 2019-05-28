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

. /lib/lsb/init-functions

if [ -e /proc/sys/net/ipv4/conf/all/hidden ]; then
    log_action_begin_msg "Fixing Global ARP Flux"
    echo 1 > /proc/sys/net/ipv4/conf/all/hidden
    log_action_end_msg $?
fi

for idx in 0 1 2 3 4 5 6 7 8
do
	if [ -e /proc/sys/net/ipv4/conf/eth$idx/hidden ]; then
    		log_action_begin_msg "Fixing ARP Flux for eth$idx"
       	 	echo 1 > /proc/sys/net/ipv4/conf/eth$idx/hidden
    		log_action_end_msg $?
    	fi
done
exit 0
