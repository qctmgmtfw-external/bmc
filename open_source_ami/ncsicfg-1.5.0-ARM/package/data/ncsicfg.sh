#!/bin/sh
# /etc/init.d/ncsicfg.sh: Enable user specified ncsi port
#
#Running at RCS.d
#Runlevel : S = S38

if [ -x /usr/local/bin/ncsicfg ]
then
	echo "Enabling User Specified NCSI Port ..."
	/usr/local/bin/ncsicfg
fi

