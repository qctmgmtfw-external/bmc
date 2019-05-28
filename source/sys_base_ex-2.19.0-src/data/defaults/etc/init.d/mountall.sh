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

#LD_LIB_PATH=$(ls /lib/ld-*.so)
FLASH_ERASE="/usr/sbin/flash_erase"
MKFSJFFS2="/usr/sbin/mkfs.jffs2"

CORE_FEATURES="/etc/core_features"

CONF_DIR="conf"
BKUPCONF_DIR="bkupconf"
EXTLOG_DIR="extlog"
DEFCONF_DIR="/etc/defconfig"
WWW="www"
PRIMARY=0
SECONDARY=0

DUPFSTAB=/etc/dupfstab
PROCMTD=/proc/mtd
CMDLINE=/proc/cmdline 

# for ntfs-3g to get correct file name encoding
if [ -r /etc/default/locale ]; then
	. /etc/default/locale
	export LANG
fi

do_start() {

	ShouldBeSkipped()
    	{
        	echo $1 | grep "^#" > /dev/null
        	if [ $? == 0 ]
        	then
            		return 1
        	fi
        	if [ "`echo $1`" == "" ]
        	then
            		return 1
        	fi

        	return 0
    	}

     
	isfeatureenabled()
    	{
        	feature="$1"

        	if [ -f $CORE_FEATURES ]
        	then
            		line=`cat $CORE_FEATURES | grep -w $feature`
            		if [ "$line" == "" ]; then
                    		return 0
            		fi

            		ShouldBeSkipped $line
            		if [ $? == 1 ]; then
                    		return 0
            		fi

            		return 1
        	fi

        	return 0
    	}

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
        # function mount_section,
        # mount a section and its backup manually.
        # if mount error, it will erase mtd, and make jffs2 file system.
        #
        mount_section()
        {
            mount /dev/mtdblock$NUM1 /$CONF_DIR -t jffs2
            if [ $? == 0 ]; then

                log_action_msg "JFFS2 Workaround: Mounting default $CONF_DIR partition is successful"
                mount /dev/mtdblock$NUM2 $BKUPCONF_DIR -t jffs2
                if [ $? != 0 ]; then
                        log_action_msg "JFFS2 Workaround: Backup partition is damaged..."
                        log_action_msg "JFFS2 Workaround: Start to recover the backup partition..."

                        get_mtdinfo $NUM2
                        $FLASH_ERASE /dev/mtd$NUM2 0 $BLOCKNUM
                        $MKFSJFFS2 --root=/$CONF_DIR --output=/dev/mtd$NUM2 --eraseblock=$MTDERASESIZE
                        mount /dev/mtdblock$NUM2 /$BKUPCONF_DIR -t jffs2
                        if [ $? == 0 ]; then
                                log_action_msg "JFFS2 workaround: Done"
                        else
                                log_failure_msg "JFFS2 Workaround: The FLASH is damaged->MTD$NUM2!!"
                        fi
                else
                        #Alway sych with bkupconf
                        cp /$CONF_DIR/* /$BKUPCONF_DIR/ -rf
                fi
	   else

                log_action_msg "JFFS2 Workaround: Mounting default $CONF_DIR partition has failed ..."
                mount /dev/mtdblock$NUM2 /$CONF_DIR -t jffs2
                if [ $? == 0 ]; then
                    log_action_msg "JFFS2 Workaround: Start to recover the $CONF_DIR partition..."

                    get_mtdinfo $NUM1

                        $FLASH_ERASE /dev/mtd$NUM1 0 $BLOCKNUM
                        $MKFSJFFS2 --root=/$CONF_DIR --output=/dev/mtd$NUM1 --eraseblock=$MTDERASESIZE

                        mount /dev/mtdblock$NUM1 /$BKUPCONF_DIR -t jffs2
                        if [ $? != 0 ]; then
                                log_failure_msg "JFFS2 Workaround: The FLASH is damaged->MTD$NUM1!!"
                        fi
                else
                        log_warning_msg "JFFS2 Workaround: Both of $CONF_DIR partitions are damaged!!"

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
                log_action_msg "JFFS2 Workaround: Done"

        fi

        }


	#
	# function do_recovery
	# if any section is not mounted then it will do recovery process
	#
	do_recovery() 
	{
		if [ "$bootimg" -eq 1 ]; then
			NUM1=$firstentry
			NUM2=$secentry
                else
			NUM1=$secentry
			NUM2=$firstentry
                fi

        	if [ $PRIMARY == 0 ]; then
        
            		log_action_msg "JFFS2 Workaround: Mounting default $name partition is successful"
        		if [ $SECONDARY != 0 ]; then
        			log_action_msg "JFFS2 Workaround: Backup partition is damaged..."
	        		log_action_msg "JFFS2 Workaround: Start to recover the backup partition..."
	
        			get_mtdinfo $NUM2
        			$FLASH_ERASE /dev/mtd$NUM2 0 $BLOCKNUM
        			$MKFSJFFS2 --root=$fullname --output=/dev/mtd$NUM2 --eraseblock=$MTDERASESIZE
	        		mount /dev/mtdblock$NUM2 /bkup$name -t jffs2
        			if [ $? == 0 ]; then
        				log_action_msg "JFFS2 workaround: Done"
					cp $fullname/* /bkup$name/ -rf
        			else
        				log_failure_msg "JFFS2 Workaround: The FLASH is damaged->MTD$NUM2!!"
        			fi
        		else
        		#Alway sych with bkupconf
        		cp $fullname/* /bkup$name/ -rf
        	fi
        	
        	else
            
            		log_action_msg "JFFS2 Workaround: Mounting default $name partition has failed ..."
        		if [ $SECONDARY == 0 ]; then
        			log_action_msg "JFFS2 Workaround: Start to recover the $name partition..."

				get_mtdinfo $NUM1
        	    
        			$FLASH_ERASE /dev/mtd$NUM1 0 $BLOCKNUM
	        	        $MKFSJFFS2 --root=$fullname --output=/dev/mtd$NUM1 --eraseblock=$MTDERASESIZE
        		
        			mount /dev/mtdblock$NUM1 $fullname -t jffs2
        			if [ $? != 0 ]; then
        				log_failure_msg "JFFS2 Workaround: The FLASH is damaged->MTD$NUM1!!"
				else
					cp /bkup$name/* $fullname/ -rf
        			fi
	        	else
        			log_warning_msg "JFFS2 Workaround: Both of $name partitions are damaged!!"

        			get_mtdinfo $NUM1
        		
        			$FLASH_ERASE /dev/mtd$NUM1 0 $BLOCKNUM
	        		$MKFSJFFS2 --root=/etc/defconfig --output=/dev/mtd$NUM1 --eraseblock=$MTDERASESIZE

        			get_mtdinfo $NUM2
        		
        			$FLASH_ERASE /dev/mtd$NUM2 0 $BLOCKNUM
        			$MKFSJFFS2 --root=/etc/defconfig --output=/dev/mtd$NUM2 --eraseblock=$MTDERASESIZE
        		
	        		mount /dev/mtdblock$NUM1 $fullname -t jffs2
        			mount /dev/mtdblock$NUM2 /bkup$name -t jffs2
        			reboot
        		fi
        	log_action_msg "JFFS2 Workaround: Done"
        fi
     
	}
	
	#
	# function check_mountconf,
	# it will check if conf and bkupconf were mounted successfully.
	# And it will invoke mount_section if one of them mount failed.
	#
	check_mountconf()
	{
		sleep 1
	    	# check if conf and bkupconf have been mounted
	    	x=`df | grep "/$CONF_DIR" |  awk '{ printf $6 }'`
	    	y=`df | grep "/$BKUPCONF_DIR" | awk '{ printf $6 }'`
        
        	NUM1=`cat /proc/mtd | grep \"$CONF_DIR\" | awk -F: '{ printf $1 }' | sed 's/mtd//g'`
        	NUM2=`cat /proc/mtd | grep \"$BKUPCONF_DIR\" | awk -F: '{ printf $1 }' | sed 's/mtd//g'`
       
        	if [ "$x" != "" ] && [ "$y" != "" ]; then
             		#echo "conf and bkupconf are both mounted normally" 
            		log_action_msg "."
            
        	elif [ "$x" == "" ] && [ "$y" != "" ]; then
             		#echo "mount conf error, start fail-safe conf procedure..."
            		umount -l /$BKUPCONF_DIR
            		mount_section
        
        	elif [ "$x" != "" ] && [ "$y" == "" ]; then
             		#echo "mount bkupconf error, start fail-safe conf procedure..."
            		umount -l /$CONF_DIR
            		mount_section
        
        	else
             		#echo "mount conf and bkupconf both failed"
           		mount_section                  
        	fi	    
	}
	
	check_conf()
        {
                DIR=$1
                if [ -f /$DIR/AMI ];then
                        echo "file exists and conf mounted correctly"
                else
                        #copy all files
                        echo "Copying all files from /etc/defconfig to /conf..."
                        cp /$DEFCONF_DIR/* /$DIR/ -rf
                        touch /$DIR/AMI
                fi
        }

	 #
        # function mountconf,
        # it will check if section and backup sections were mounted successfully.
        #
        mountconf() {
	SECTION=$1
        MTD_NUM=$2
            sleep 1
        NUM1=`cat /proc/mtd | grep \"$CONF_DIR\" | awk -F: '{ printf $1 }' | sed 's/mtd//g'`

        if [ $NUM1 == 0 ]; then
            echo "Only one SPI chip available, not mounting conf"
            return
        fi

                mount /dev/mtdblock$MTD_NUM /$SECTION -t jffs2
            if [ $? == 0 ]; then
                        echo "checking the mounted $SECTION.."
                        check_conf $SECTION
                else
                        echo "mounting $SECTION failed...recovering the $SECTION partition"
                        get_mtdinfo $MTD_NUM
                        $FLASH_ERASE /dev/mtd$MTD_NUM 0 $BLOCKNUM
                        echo "mounting /$SECTION after erasing..."
                        mount /dev/mtdblock$MTD_NUM /$SECTION -t jffs2
                        if [ $? != 0 ]; then
                                echo "Failed"
                                return
                        fi
                        echo "Done."
                        echo "Copying default config files ..."
                cp /$DEFCONF_DIR/* /$SECTION/ -rf
                        touch /$SECTION/AMI
                        echo "Done."
                fi
        }

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
	#log_daemon_msg "Will now mount local filesystems"
		log_daemon_msg "Mounting local filesystems"	
		mount_all_local
		log_end_msg $?
	fi


	mount_dupsections()
	{
		cmd=$1
		# Fetch the name and filesystem type from the Duplicate Fstab file
       		name=`echo $cmd | awk -F" " '{ print $2 }' | awk -F"/" '{ print $NF }'`
		if [ "$ismedium_sd" -eq "1" ] && [ "$name" == "$EXTLOG_DIR" ]; then
			return
		fi
       		fullname=`echo $cmd | awk -F" " '{ print $2 }'`
       		fstype=`echo $cmd | awk -F" " '{ print $3 }'`
		bootimg=`cat $CMDLINE | awk -F "imagebooted=" '{print$2}' | awk -F " " '{print$1}'` 
		
		# Search the /proc/mtd to find if duplicate entries exists for these sections
      	 	dupentries=`cat $PROCMTD | grep -w -c $name`

        	# Check for the given section name. Process only the first entry
	        # Parse the mtd number from the /proc/mtd and mount that partition
       		firstentry=`cat $PROCMTD | grep -w $name -m 1 | awk -F":" '{ print $1 }' | awk -F"mtd" '{ print $2 }'`
                www_count=`cat $PROCMTD | grep -c $name`
                if [ "$www_count" -eq "1" ]; then
                secentry=$firstentry
                else
                secentry=`cat $PROCMTD | grep -v "mtd$firstentry" | grep -w $name -m 1 | awk -F":" '{ print $1 }' | awk -F"mtd" '{ print $2 }'`
                fi
		dupmnt=`df -h | grep -w -c "$fullname"`

		isfeatureenabled CONFIG_SPX_FEATURE_GLOBAL_ON_LINE_FLASHING_SUPPORT
   		isonline=$?

		# If online flashing support is enabled then mounting www from RAM.
		if [ "$isonline" -eq "1" ] && [ "$name" == "$WWW" ]; then
			if [ "$dupmnt" -ne "0" ]; then
				umount -l $fullname
			fi

			if [ "$bootimg" -eq 1 ]; then
				dd if=/dev/mtdblock$firstentry of=/dev/ram3
               			mount -t cramfs /dev/ram3 $fullname
			else
				dd if=/dev/mtdblock$secentry of=/dev/ram3
                                mount -t cramfs /dev/ram3 $fullname
			fi

			return
		fi

		if [ "$dupmnt" -eq "0" ]; then
			if [ "$bootimg" -eq 1 ]; then 
        			mount /dev/mtdblock$firstentry $fullname -t $fstype
			else
				mount /dev/mtdblock$secentry $fullname -t $fstype
			fi
			PRIMARY=$?
		fi 	
		firstresult=$?
	
        		# If duplicate sections exists and its filesystem type is JFFS2,
	        	# then mount the first entry as such, mount the second entry as bkup section
			if [ "$dupentries" -gt "1" ] && [ "$fstype" == "jffs2" ]; then
                            isfeatureenabled CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
                            isconf=$?
                            isfeatureenabled CONFIG_SPX_FEATURE_GLOBAL_FAILSAFEEXTLOG
                            isextlog=$?
                            if [[ "$isconf" -eq "1" && "$fullname" == "/$CONF_DIR" ]] || [[ "$isextlog" -eq "1" && "$fullname" == "/$EXTLOG_DIR" ]]; then
        			# Check for the given section name. Skip the first entry, thereby processing only the duplicate entry
	        		# Parse the mtd number from the /proc/mtd and mount that partition with a 'bkup' tag as prefix.
				dupmnt=`df -h | grep -c "/bkup$name"`
				if [ "$dupmnt" -eq "0" ]; then
					if [ "$bootimg" -eq 1 ]; then
        					mount /dev/mtdblock$secentry /bkup$name -t $fstype
					else
						mount /dev/mtdblock$firstentry /bkup$name -t $fstype
					fi
					SECONDARY=$?
					do_recovery
				fi
			    fi
			fi
	}

	isfeatureenabled CONFIG_SPX_FEATURE_EXTENDEDLOG_MEDIUM_TYPE_SD
	ismedium_sd=$?

	# Check if the /etc/dupfstab file exists. If not, then the tree is possibly not proper
	if [ ! -f $DUPFSTAB ]; then
       		echo " \"$DUPFSTAB\" does not exist in the tree."
	
		# check if bkupconf is enabled and is available in fstab
		check_bkupconf
		if [ $? == 1 ]; then
	 		#If there is bkupconf in fstab, do the fail-safeconf checking procedure
		    	check_mountconf
		fi
	else
		# Loop through the dupfstab file and process it line by line
		while read line
		do
			mount_dupsections "$line"
		done < $DUPFSTAB
	fi

   isfeatureenabled CONFIG_SPX_FEATURE_GLOBAL_DUAL_IMAGE_SUPPORT
   isdualimg=$?
   isfeatureenabled CONFIG_SPX_FEATURE_COMMON_CONF_SECTION
   iscmnconf=$?
   
   if [ "$isdualimg" -eq "1" ] && [ "$iscmnconf" -eq "1" ]; then
       firstentry=`cat $PROCMTD | grep -w $CONF_DIR -m 1 | awk -F":" '{ print $1 }' | awk -F"mtd" '{ print $2 }'`
       dupmnt=`df -h | grep -w -c "/$CONF_DIR"`
       if [ "$dupmnt" -eq "0" ]; then
           mountconf $CONF_DIR $firstentry
       fi
       isfeatureenabled CONFIG_SPX_FEATURE_GLOBAL_FAILSAFECONF
       isconf=$?
       if [ "$isconf" -eq "1" ]; then
           dupmnt=`df -h | grep -w -c "/$BKUPCONF_DIR"`
           if [ "$dupmnt" -eq "0" ]; then
               secentry=`cat $PROCMTD | grep -w $CONF_DIR | grep -v "mtd$firstentry" | awk -F":" '{ print $1 }' | awk -F"mtd" '{ print $2 }'`
               mountconf $BKUPCONF_DIR $secentry
           fi
       fi
   fi
 
    if [ -f /conf/restorefactory ];then
         echo "Restored Factory Defaults..."
         rm -rf /conf/restorefactory
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



