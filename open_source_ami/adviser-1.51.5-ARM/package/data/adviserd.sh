#!/bin/sh
# /etc/init.d/adviserd: start video Server
#
# chkconfig: 2345 10 90
#
#Runlevel : 3 = S45
#Runlevel : 6 = K45
#Runlevel : 7 = K45
#Runlevel : 8 = K45
#

PATH=/bin:/usr/bin:/sbin:/usr/sbin
VIDEOSERVER_PATH="/usr/local/bin/"
VIDEOSERVER_STOP_CMD="killall"
VIDEOSERVER_STOP_CMDOPTS="INT"
VIDEOSERVER_STOP_CMDOPTS_CONF="USR1"
VIDEOSERVER_NAME="adviserd"

VIDEO_DRIVERNAME=videocap

#check service state with ncml conf
SERVICE_NAME="[kvm]"
SERVICE_CONF="/conf/ncml.conf"
#check service state if enabled
check_service_enabled()
{
	if [ -f $SERVICE_CONF ];
	then
	
		SERVICE_STATE=`cat $SERVICE_CONF | awk -v val=$SERVICE_NAME '$1 == val, $1 == ""' | 
								   awk 'BEGIN {FS="="} /current_state/ {print $2 }'       | 
								   awk 'NR==1 {print $0}'`

		if [ $SERVICE_STATE == 0 ];
		then
			echo $SERVICE_NAME is disabled
			exit 1
#		else
#			echo $SERVICE_NAME is enabled   
		fi
		
	else
		echo "$SERVICE_CONF does not exist"
    fi			
}

#check if video driver is loaded
check_driver_loaded()
{
	lsmod | grep $VIDEO_DRIVERNAME >/dev/null
	if [ $? == 1 ]
        then
  		echo -n "Loading Video capture driver... "
	        insmod $VIDEO_DRIVERNAME
	       	echo "Done"
	fi
}

# Options for start/stop the daemons

case "$1" in
  start)
    check_driver_loaded	   
    check_service_enabled
    echo -n "Starting Video Redirection Server :adviserd"
    start-stop-daemon --start --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME
    echo "."
    echo -n
    ;;
  stop)
    echo -n "Stopping Video Redirection Server: adviserd"
    start-stop-daemon --stop --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME --signal $VIDEOSERVER_STOP_CMDOPTS
    echo "."
    echo -n
    ;;
  restart)
    echo -n "Restarting Video Redirection Server: adviserd"
    start-stop-daemon --stop --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME --signal $VIDEOSERVER_STOP_CMDOPTS
    sleep 2
    check_driver_loaded	   
    check_service_enabled
    start-stop-daemon --start --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME
    echo "."
    echo -n
    ;;
  restart_conf)
    echo -n "Restarting Video Redirection Server: adviserd"
    start-stop-daemon --stop --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME --signal $VIDEOSERVER_STOP_CMDOPTS_CONF
    sleep 2
    check_driver_loaded	   
    check_service_enabled
    start-stop-daemon --start --quiet --exec $VIDEOSERVER_PATH$VIDEOSERVER_NAME -n $VIDEOSERVER_NAME
    echo "."
    echo -n
    ;;
  *)
    echo "Usage: /etc/init.d/adviserd {start|stop}"
    echo "."
    echo -n
    exit 1
esac
  
exit 0
