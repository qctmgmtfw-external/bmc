#Runlevel : 3 = S90
#Runlevel : 6 = K10
# Don't add for run level 8, because it is for web based firmware upgrade

PATH=/bin:/usr/bin:/sbin:/usr/sbin

WEBGO=/usr/local/bin/webgo
WWWPATH=/usr/local/www 

test -f $WEBGO || exit 0

export LD_LIBRARY_PATH=/lib:/usr/lib:/usr/local/lib:/usr/local/cim/usr/local/lib

case "$1" in
  start)
#    echo -n "Starting WebServer ..."
    start-stop-daemon --start --quiet --exec $WEBGO $WWWPATH
    echo "."
    ;;
  stop)
    echo -n "Stopping Webserver ..."
    start-stop-daemon --stop --quiet --exec $WEBGO $WWWPATH >/dev/null
    echo "."
	;;
   restart)
#	echo -n "Restarting WebServer"
    start-stop-daemon --stop --quiet --exec $WEBGO $WWWPATH >/dev/null
# Sometime webgo does not start immediatly after getting killed. So tried two times.
# if already started in first attempt, the second will not do anything.
    start-stop-daemon --start --quiet --exec $WEBGO $WWWPATH 
    start-stop-daemon --start --quiet --exec $WEBGO $WWWPATH 
	;;
   *)
    echo "Usage: /etc/init.d/webgo {start|stop|restart}"
    exit 1
esac

exit 0
