#!/bin/sh
# /etc/init.d/pamhelper.sh: start PAM Helper daemon
#
# chkconfig: 2345 10 90
#Runlevel : S = S15
PATH=/bin:/usr/bin:/sbin:/usr/sbin

PATHFUL_PAMHELPERDINIT="/usr/local/bin/pam_helperd"



test -f /usr/local/bin/pam_helperd || exit 0


# Options for start/restart the daemons
#

case "$1" in
  start)
    echo -n "Starting PAM Helper Daemon: PAMHelperD "
    /usr/local/bin/pam_helperd &
#    start-stop-daemon --start --quiet --exec $PATHFUL_PAMHELPERDINIT
    echo "."
    ;;
  stop)
    echo -n "Stopping PAM Helper Daemon: PAMHelperD "
    killall pam_helperd
#    start-stop-daemon --stop --quiet --exec $PATHFUL_PAMHELPERDINIT
    echo "."
    ;;
  restart)
    echo -n "Restarting PAM Helper Daemon: PAMHelperD "
    killall pam_helperd
    /usr/local/bin/pam_helperd &
#	start-stop-daemon --stop --quiet --oknodo --exec $PATHFUL_PAMHELPERDINIT
#	start-stop-daemon --start --quiet --exec $PATHFUL_PAMHELPERDINIT
    echo "."
    ;;
   *)
    echo "Usage: /etc/init.d/pamhelperd {start|stop|restart}"
    exit 1
esac

exit 0
