#!/bin/sh
# /etc/init.d/drvload.sh: automatically mount load ncsi and sdhc_hw.
#
#Runlevel : S = S38

case "$1" in
  start|"")
        modprobe sdhc_hw > /dev/null 2>&1
        ;;
  restart|reload|force-reload)
        echo "Error: argument '$1' not supported" >&2
        exit 3
        ;;
  stop)
        # No-op
        ;;
  *)
        echo "Usage: drvload.sh [start|stop]" >&2
        exit 3
        ;;
esac

