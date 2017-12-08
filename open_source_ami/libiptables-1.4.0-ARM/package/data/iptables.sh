#! /bin/sh
#
# We run as soon as networking is live, and before we use NFS!
# chkconfig: S 43 0
#
#Runlevel : S = S43

IPTABLES="/sbin/iptables"
IPTABLES_RESTORE="/sbin/iptables-restore"
IPTABLES_CONF="/conf/iptables.conf"

echo "starting iptables... "
if [ -f $IPTABLES ]; then
    if [ -f $IPTABLES_CONF ]; then     
   	    $IPTABLES_RESTORE -c $IPTABLES_CONF >/dev/null 2>&1
	    if [ $? = 0 ] 
	    then
		    echo "Success"
		    exit 0
	    else
		    echo "Failure" 
	    fi
	fi
else
	echo "failure"
fi

exit 0
