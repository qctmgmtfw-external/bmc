#! /bin/sh
### BEGIN INIT INFO
# Provides:          init-sp
# Required-Start:    checkfs
# Required-Stop: 
# Default-Start:     S
# Default-Stop:
# Short-Description: SP Initialization 
# Description:
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin
. /lib/init/vars.sh
. /lib/lsb/init-functions

mount -t proc /dev/proc /proc

#Check if booting from NFS or Flash
if [ -r /proc/cmdline ] && grep -q 'root=\/dev\/nfs' /proc/cmdline 
then
	# Booting from NFS -> Empty out /var
	rm -r -f /var/*
else
	#Unmount the old root automatically created by init
	umount	/initrd
fi

echo -n "Creating area for var using tmpfs ..."
mount -t tmpfs /dev/shm /var
chmod 777 /var
echo "Done"

CreateDir()
{
     if ! [ -d $1 ]
     then
	mkdir -p $1
     fi 
    
     if ! [ "$2" == "" ]
     then 
	chmod $2 $1
     fi

}

echo -n "Creating directories in /var ..."

# Mandatory as per SP 4.0 File System Hierarchy
CreateDir /var/run 777
CreateDir /var/tmp +t
CreateDir /var/lib
CreateDir /var/lib/dhcpv6
CreateDir /var/cache
CreateDir /var/spool
CreateDir /var/pipe
CreateDir /var/log
CreateDir /var/lock
CreateDir /var/misc
CreateDir /var/empty

#Network needs this directory
CreateDir /var/network/run

#Stunnel needs this directory
CreateDir /var/run/stunnel4

#Stunnel needs this directory
CreateDir /var/run/sshd
chmod 700 /var/run/sshd

# Backward comaptability directories
CreateDir /var/lock/etc     
CreateDir /var/opt
CreateDir /var/state        
CreateDir /var/etc          
CreateDir /var/local
CreateDir /var/lib/logrotate
CreateDir /var/lib/stunnel4
CreateDir /var/lib/urandom	
CreateDir /var/lib/ntp
CreateDir /var/spool/cron/crontabs
rm -f /var/process.conf

echo "done."

exit 0


