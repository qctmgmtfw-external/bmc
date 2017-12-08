#!/bin/sh
# /etc/init.d/upnp: start UPnP discovery
#
#Starting at rc3.d and stopping at rc6.d
#Runlevel : 3 = S50
#Runlevel : 6 = K50
#Runlevel : 7 = K50
#Runlevel : 8 = K50

PATH=/bin:/usr/bin:/sbin:/usr/sbin
PATHFUL_UPNPDISCOVERY=/usr/local/bin/upnp
test -f /usr/local/bin/upnp || exit 0


# Options for start/restart the daemons

case "$1" in
  start)
    echo -n "Starting upnp discovery"
    start-stop-daemon --start --quiet --exec $PATHFUL_UPNPDISCOVERY 
    echo "."
    ;;
  stop)
    echo -n "Stopping upnp discovery"
    start-stop-daemon --stop --quiet --exec $PATHFUL_UPNPDISCOVERY --signal INT
    echo "."
    ;;
    reload)
	echo -n "Reloading upnp discovery"
	start-stop-daemon --stop --quiet --exec $PATHFUL_UPNPDISCOVERY --signal 1
	echo "."
	;;
    force-reload)
	$0 reload
	;;
    restart)
	echo -n "Restarting upnp discovery"
	start-stop-daemon --stop --quiet --oknodo --exec $PATHFUL_UPNPDISCOVERY
	start-stop-daemon --start --quiet --exec $PATHFUL_UPNPDISCOVERY 
	echo "."
	;;
   *)
    echo "Usage: /etc/init.d/upnp {start|stop|reload|restart|force-reload}"
    exit 1
esac

exit 0
