#!/bin/sh
# /etc/init.d/phycfg.sh: Set user specified network link mode
#
#Running at RCS.d
#Runlevel : S = S38

if [ -x /usr/local/bin/phycfg ]
then
	echo "Setting the network link mode as specified in the configuration file - /conf/phycfg.conf ..."
	/usr/local/bin/phycfg
else
	echo "Application not found -/usr/local/bin/phycfg ####"
	echo -n 
fi