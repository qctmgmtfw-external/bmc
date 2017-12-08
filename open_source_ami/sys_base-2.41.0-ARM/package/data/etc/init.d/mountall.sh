#! /bin/sh
### BEGIN INIT INFO
# Provides:          mountall
# Required-Start:    checkfs
# Required-Stop: 
# Default-Start:     S
# Default-Stop:
# Short-Description: Mount all filesystems.
# Description:
### END INIT INFO

PATH=/sbin:/bin:/usr/bin:/usr/sbin

. /lib/init/vars.sh

. /lib/lsb/init-functions
. /lib/init/mount-functions.sh

LD_LIB_PATH=$(ls /lib/ld-*.so)
FLASH_ERASE="$LD_LIB_PATH /usr/sbin/flash_erase"
MKFSJFFS2="$LD_LIB_PATH /usr/sbin/mkfs.jffs2"

CONF_DIR="conf"
BKUPCONF_DIR="bkupconf"

# for ntfs-3g to get correct file name encoding
if [ -r /etc/default/locale ]; then
	. /etc/default/locale
	export LANG
fi

do_start() {
	#
	# Mount local file systems in /etc/fstab.
	#
	mount_all_local() {
		if [ -r /proc/cmdline ] && grep -q 'root=\/dev\/nfs' /proc/cmdline 
		then
			# Booting from NFS. mount all except mtd flash filesystems
 	    		mount -a -t sysfs,proc,devpts,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs,gfs2 2>/dev/null || true

		else
			#Unmount the old root automatically created by init
			#Ractrends 4.0 Busybox mount does not support -O option
			#	    mount -a -t nonfs,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs,gfs2 \
			#		-O no_netdev
			#Ractrends 4.0 : Mount return non-zero exit code even on success
 	    		mount -a -t nonfs,nfs4,smbfs,cifs,ncp,ncpfs,coda,ocfs2,gfs,gfs2 2>/dev/null || true
		fi
	}
		
	#
	# function get_mtdinfo, 
	# require MTDNUM as the parameter#1
	# it will get mtd size and mtd erase size from /proc/mtd,
	# and calculate the erase block number  
	#
	get_mtdinfo() {

        MTDNUM=$1
        MTDSIZE=0x$(cat /proc/mtd | grep "mtd$MTDNUM" | awk '{ printf $2 }')
        MTDERASESIZE=0x$(cat /proc/mtd | grep "mtd$MTDNUM" | awk '{ printf $3 }')
        
        #calculate block number, it must convert HEX to DEC before div operation
        BLOCKNUM=`expr $(($MTDSIZE)) / $(($MTDERASESIZE))`
	    
	}
	
	#
	# function mount_conf,
	# mount conf and bkupconf manually.
	# if mount error, it will erase mtd, and make jffs2 file system.
	#
	mount_conf() {
		    
	    mount /dev/mtdblock$NUM1 /$CONF_DIR -t jffs2
        if [ $? == 0 ]; then
        
            echo "JFFS2 Workaround: Mounting default conf partation is successful"
        	mount /dev/mtdblock$NUM2 $BKUPCONF_DIR -t jffs2
        	if [ $? != 0 ]; then
        		echo "JFFS2 Workaround: Backup partition is damaged..."
        		echo "JFFS2 Workaround: Start to recover the backup partition..."

        		get_mtdinfo $NUM2
        		$FLASH_ERASE /dev/mtd$NUM2 0 $BLOCKNUM
        		$MKFSJFFS2 --root=/$CONF_DIR --output=/dev/mtd$NUM2 --eraseblock=$MTDERASESIZE
        		mount /dev/mtdblock$NUM2 /$BKUPCONF_DIR -t jffs2
        		if [ $? == 0 ]; then
        			echo "JFFS2 workaround: Done"
        		else
        			echo "JFFS2 Workaround: The FLASH is damaged->MTD$NUM2!!"
        		fi
        	else
        		#Alway sych with bkupconf
        		cp /$CONF_DIR/* /$BKUPCONF_DIR/ -rf
        	fi
        	
        else
            
            echo "JFFS2 Workaround: Mounting default conf partition is fail..."
        	mount /dev/mtdblock$NUM2 /$CONF_DIR -t jffs2
        	if [ $? == 0 ]; then
        	    echo "JFFS2 Workaround: Start to recover the conf partition..."

        	    get_mtdinfo $NUM1
        	    
        		$FLASH_ERASE /dev/mtd$NUM1 0 $BLOCKNUM
        	    $MKFSJFFS2 --root=/$CONF_DIR --output=/dev/mtd$NUM1 --eraseblock=$MTDERASESIZE
        		
        		mount /dev/mtdblock$NUM1 /$BKUPCONF_DIR -t jffs2
        		if [ $? != 0 ]; then
        			echo "JFFS2 Workaround: The FLASH is damaged->MTD$NUM1!!"
        		fi
        	else
        		echo "JFFS2 Workaround: Both of conf partitions are damaged!!"

        		get_mtdinfo $NUM1
        		
        		$FLASH_ERASE /dev/mtd$NUM1 0 $BLOCKNUM
        		$MKFSJFFS2 --root=/etc/defconfig --output=/dev/mtd$NUM1 --eraseblock=$MTDERASESIZE
        		

        		get_mtdinfo $NUM2
        		
        		$FLASH_ERASE /dev/mtd$NUM2 0 $BLOCKNUM
        		$MKFSJFFS2 --root=/etc/defconfig --output=/dev/mtd$NUM2 --eraseblock=$MTDERASESIZE
        		
        		mount /dev/mtdblock$NUM1 /$CONF_DIR -t jffs2
        		mount /dev/mtdblock$NUM2 /$BKUPCONF_DIR -t jffs2
        		reboot
        	fi
        	echo "JFFS2 Workaround: Done"
        
        fi
     
	}
	
	#
	# function check_mountconf,
	# it will check if conf and bkupconf were mounted successfully.
	# And it will invoke mount_conf if one of them mount failed.
	#
	check_mountconf() {
	    sleep 1
	    # check if conf and bkupconf have been mounted
	    x=`df | grep "/$CONF_DIR" |  awk '{ printf $6 }'`
	    y=`df | grep "/$BKUPCONF_DIR" | awk '{ printf $6 }'`
        
        NUM1=`cat /proc/mtd | grep \"$CONF_DIR\" | awk -F: '{ printf $1 }' | sed 's/mtd//g'`
        NUM2=`cat /proc/mtd | grep \"$BKUPCONF_DIR\" | awk -F: '{ printf $1 }' | sed 's/mtd//g'`
        
        
        if [ "$x" != "" ] && [ "$y" != "" ]; then
            # echo "conf and bkupconf are both mounted normally" 
            echo "."
            
        elif [ "$x" == "" ] && [ "$y" != "" ]; then
            # echo "mount conf error, start fail-safe conf procedure..."
            umount -l /$BKUPCONF_DIR
            mount_conf
        
        elif [ "$x" != "" ] && [ "$y" == "" ]; then
            # echo "mount bkupconf error, start fail-safe conf procedure..."
            umount -l /$CONF_DIR
            mount_conf
        
        else
            # echo "mount conf and bkupconf both failed"
            mount_conf                  
        fi	    
	}
	
	#
	# function check_bkupconf,
	# it will check if there is a bkupconf partition in fstab.
	# And it will return 1 if success.
	#
	check_bkupconf() {
	    # Check if there is bkupconf in fstab
	    VAL=`grep -w "/$BKUPCONF_DIR" /etc/fstab`
	    if [ "$VAL" == "" ]; then
	        return 0
	    else
	        #Found bkupconf
	        return 1
	    fi
	}


	pre_mountall

	if [ "$VERBOSE" = no ]
	then
		log_action_begin_msg "Mounting local filesystems"
		mount_all_local	
		log_action_end_msg $?
	else
#Ractrends 4.0 : Display same messages irespective of the value of VERBOSE.
#		log_daemon_msg "Will now mount local filesystems"
		log_daemon_msg "Mounting local filesystems"	
		mount_all_local
		log_end_msg $?
	fi
	
	check_bkupconf
	if [ $? == 1 ]; then
	    #If there is bkupconf in fstab, do the fail-safeconf checking procedure
	    check_mountconf
	fi
	
	copy_groupfile() {
            if [ -r /etc/default_group ] ; then
		cp /etc/default_group /var/tmp/group
		return 1;
            else
		return 0;
	    fi
        }

        copy_groupfile
	if [ $? == 1 ];
	then
		echo "Group File Copied Successfully...."
	else
		echo "/etc/default_group file not present. /conf is probably corrupted or old."
	fi

	post_mountall

	case "$(uname -s)" in
	  *FreeBSD)
		INITCTL=/etc/.initctl
		;;
	  *)
		INITCTL=/dev/initctl
		;;
	esac

	#
	# We might have mounted something over /dev, see if
	# /dev/initctl is there.
	#
	if [ ! -p $INITCTL ]
	then
		rm -f $INITCTL
		mknod -m 600 $INITCTL p
	fi
	kill -USR1 1

	#
	# Execute swapon command again, in case we want to swap to
	# a file on a now mounted filesystem.
	#
	# Ignore 255 status due to swap already being enabled
	#
	if [ "$NOSWAP" = yes ]
	then
#Ractrends 4.0 does not support swap. Remove this warning message
#		[ "$VERBOSE" = no ] || log_warning_msg "Not activating swap as
#requested via bootoption noswap."
		echo -n 
	else
		if [ "$VERBOSE" = no ]
		then
			log_action_begin_msg "Activating swapfile swap"
			swapon -a -e 2>/dev/null || :  # Stifle "Device or resource busy"
			log_action_end_msg 0
		else
			log_daemon_msg "Will now activate swapfile swap"
			swapon -a -e -v
			log_action_end_msg $?
		fi
	fi
}

case "$1" in
  start|"")
	do_start
	;;
  restart|reload|force-reload)
	echo "Error: argument '$1' not supported" >&2
	exit 3
	;;
  stop)
	# No-op
	;;
  *)
	echo "Usage: mountall.sh [start|stop]" >&2
	exit 3
	;;
esac

:
