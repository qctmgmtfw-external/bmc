#!/bin/sh
# /etc/init.d/websockify: start websockets
#
# chkconfig: 2345 10 90
#
#Runlevel : 3 = S45
#Runlevel : 6 = K45
#Runlevel : 7 = K45
#Runlevel : 8 = K45
#

PATH=/bin:/usr/bin:/sbin:/usr/sbin
WSCK_SERVER_PATH="/usr/local/bin/"
WSCK_SERVER_STOP_CMD="killall"
WSCK_SERVER_STOP_CMDOPTS="HUP"
WSCK_SERVER_STOP_CMDOPTS_CONF="USR1"
WSCK_SERVER_NAME="websockify"

# Options for start/stop the daemons
case "$1" in
  start)
    echo -n "Starting Web Sockets :websockify"
    #start-stop-daemon --start --quiet --exec $WSCK_SERVER_PATH$WSCK_SERVER_NAME -n $WSCK_SERVER_NAME
    $WSCK_SERVER_PATH$WSCK_SERVER_NAME -D 9666 127.0.0.1:7578
    $WSCK_SERVER_PATH$WSCK_SERVER_NAME -D 9999 127.0.0.1:5120
    echo "."
    echo -n
    ;;
  stop)
    echo -n "Stopping Web Sockets :websockify"
    #start-stop-daemon --stop --quiet --exec $WSCK_SERVER_PATH$WSCK_SERVER_NAME -n $WSCK_SERVER_NAME --signal $WSCK_SERVER_STOP_CMDOPTS
    $WSCK_SERVER_STOP_CMD -9 $WSCK_SERVER_NAME
    echo "."
    echo -n
    ;;
  restart)
    echo -n "Restarting Web Sockets :websockify"
    #start-stop-daemon --stop --quiet --exec $WSCK_SERVER_PATH$WSCK_SERVER_NAME -n $WSCK_SERVER_NAME --signal $WSCK_SERVER_STOP_CMDOPTS
    $WSCK_SERVER_STOP_CMD -9 $WSCK_SERVER_NAME
    sleep 3
    #start-stop-daemon --start --quiet --exec $WSCK_SERVER_PATH$WSCK_SERVER_NAME -n $WSCK_SERVER_NAME
    $WSCK_SERVER_PATH$WSCK_SERVER_NAME -D 9666 127.0.0.1:7578
    $WSCK_SERVER_PATH$WSCK_SERVER_NAME -D 9999 127.0.0.1:5120
    echo "."
    echo -n
    ;;
  *)
    echo "Usage: /etc/init.d/websockify {start|stop|restart}"
    echo "."
    echo -n
    exit 1
esac
  
exit 0
