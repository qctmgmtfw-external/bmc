#! /bin/sh
### BEGIN INIT INFO
# Provides:          ncsi-performance
# Required-Start:
# Required-Stop:
# Default-Start:     S
# Default-Stop:
# Short-Description: Fix NCSI performance issue with 1G
# Description:
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin

if [ -e /proc/sys/net/ipv4/tcp_rmem ] && [ -e /proc/sys/net/ipv4/tcp_wmem ] && [ -e /proc/sys/net/ipv4/route/flush ]; then
    echo "Tuning NC-SI performace"
    sysctl -w net.ipv4.tcp_rmem='4096 12288 12288' > /dev/null
    sysctl -w net.ipv4.tcp_wmem='4096 16384 16384' > /dev/null
    sysctl -w net.ipv4.route.flush=1 > /dev/null
fi

exit 0
