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

# With default (01-01-1970)/proc/stat/btime is set to 0
# Many programs does not like it. So setting to some non default date
date "1970-01-01 00:00:00"

mount -t proc /dev/proc /proc
mount -t sysfs /sys

#Check if booting from NFS or Flash
if [ -r /proc/cmdline ] && grep -q 'root=\/dev\/nfs' /proc/cmdline 
then
	# Booting from NFS -> Empty out /var
	rm -r -f /var/*
else
	#Unmount the old root automatically created by init
	mount | awk '{ print $3 }' | grep /initrd >/dev/null
	if [ $? == 0 ]
	then
		umount	/initrd
	fi
fi

#Get Target version
if [ -r /etc/target.ver ]
then
	VER=`cat /etc/target.ver`
fi


log_action_begin_msg  "Creating area for var using tmpfs"
mount -t tmpfs /dev/shm /var
chmod 777 /var
log_action_end_msg  0

if [ "$VER" == "Debian-7.0.0" ]
then
	log_action_begin_msg  "Creating area for run using tmpfs"
	mount -t tmpfs /dev/shm /run
	chmod 777 /run
	log_action_end_msg  0
fi




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

log_action_begin_msg "Creating directories in /var"

# Mandatory as per SP 4.0 File System Hierarchy
if [ "$VER" == "Debian-7.0.0" ]
then
	CreateDir /run/udev
	CreateDir /run/lock
	ln -s /run  /var/run
	ln -s /run/lock /var/lock
else
	CreateDir /var/run 777
	CreateDir /var/lock
fi
CreateDir /var/tmp +t
CreateDir /var/lib
CreateDir /var/lib/dhcpv6
CreateDir /var/lib/initscripts
CreateDir /var/cache
CreateDir /var/spool
CreateDir /var/pipe
CreateDir /var/log
CreateDir /var/misc
CreateDir /var/empty

#Network needs this directory
CreateDir /var/network/run

#Stunnel needs this directory
CreateDir /var/run/stunnel4

#Stunnel needs this directory
CreateDir /var/run/sshd
chmod 700 /var/run/sshd

#Depmod needed files
touch /var/run/modules.dep
touch /var/run/modules.alias
touch /var/run/modules.symbols

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

if [ -d /etc/defvar/var ];
then
	cp -R /etc/defvar/var/* /var 
fi


#Xend related - Link non-volatile configuration to actual location
if [ -d /etc/xend.domains ];
then
	ln -s /etc/xend.domains /var/lib/xend
fi

log_action_end_msg 0

copy_groupfile() {
    if [ -r /etc/default_group ] ; then
        cp /etc/default_group /var/tmp/group
        return 1;
    else
        return 0;
    fi
}

log_action_begin_msg "Copying Group File"
copy_groupfile
if [ $? != 1 ]; then
	log_action_end_msg 0 "Failed"
        log_action_msg "/etc/default_group file not present. /conf is probably corrupted or old."
else
	log_action_end_msg 0
fi

# We are moving mounting www to mountall script. Based on online flashing support www will be mounted from RAM/SPI.
# we should not mount the www mtdblock directly
# we dd the www mtdblock partition into /dev/ram3 Ramdisk
# Doing that will create cramfs filesystem in /dev/ram3
# Then we mount the /dev/ram3 as /usr/local/www mountpoint
#checkfstab=`cat /etc/dupfstab | grep -w "\/usr\/local\/www"`
#if [ "$checkfstab" == "" ]; then
#        wwwmtd=`cat /proc/mtd | grep -w "www" | awk -F : '{ print $1}' | sed s/mtd/mtdblock/`
#        if [ "$wwwmtd" != "" ]; then
#				log_action_begin_msg "Mounting the www from Ramdisk"
#                dd if=/dev/$wwwmtd of=/dev/ram3
#                mount -t cramfs /dev/ram3 /usr/local/www
#                log_action_end_msg 0
#        fi
#else
#        echo "WWW already mounted from Flash."
#fi

exit 0


