#!/bin/sh

#------------------------------------------------------------
# Create all the symlinks to /bin/busybox
#------------------------------------------------------------
/bin/busybox --install -s

echo "Unpacking initramfs ..."
#------------------------------------------------------------
# Mount things needed by this script
#------------------------------------------------------------
mkdir /proc
mount -t proc proc /proc
mkdir /sys
mount -t sysfs sysfs /sys

#------------------------------------------------------------
# Disable kernel messages from popping onto the screen
#------------------------------------------------------------
#echo 0 > /proc/sys/kernel/printk

#------------------------------------------------------------
# Create device nodes - Needed to access root device
#------------------------------------------------------------
echo /sbin/mdev > /proc/sys/kernel/hotplug
mdev -s

#------------------------------------------------------------
# Function for parsing command line options with "=" in them
#------------------------------------------------------------
get_opt() 
{
	echo "$@" | cut -d "=" -f 2-
}

#------------------------------------------------------------
# Rescue shell - Has some basic tools to rescue devices
#------------------------------------------------------------
shell() 
{
	echo "Entering rescue shell"
	exec sh
}

#------------------------------------------------------------
# Defaults values to be used for switching root
#------------------------------------------------------------
init="/sbin/init"
root=""
initargs=""
nfspath=""
permissions="rw"
rootdelay=""

#------------------------------------------------------------
# Process command line options
#------------------------------------------------------------
for i in $(cat /proc/cmdline); do
	case $i in
		root\=*)
			root=$(get_opt $i)
			;;
		rootdelay\=*)
			rootdelay=$(get_opt $i)
			;;
		init\=*)
			init=$(get_opt $i)
			;;
		nfsroot\=*)
			nfspath=$(get_opt $i)
			;;
		rescue)
			echo "Rescue shell requested"
			shell
			;;
		silent)
			echo 0 > /proc/sys/kernel/printk
			;;
		ro)
			permissions="ro"
			;;
		rw)
			permissions="rw"
			;;
			
		*\=*)	
			#Ignore kernel paramters 
			;;
		*)
		# Treat all other arguments for init ??
			initargs=$i" "
			;;	
	esac
done


#------------------------------------------------------------
# No root specified. Enter into Rescue shell
#------------------------------------------------------------
if [ "${root}" == "" ]
then
	echo "Root device not specified"
	shell
fi

#-----------------------------------------------------------
# if rootdelay is specified wait before mounting root
#-----------------------------------------------------------
if [ "${rootdelay}" != "" ]
then
	echo "Waiting for root devices to be available ..."
	sleep "${rootdelay}"
fi

#------------------------------------------------------------
# Mount the boot root device
#------------------------------------------------------------
mkdir /mnt
if [ "${root}" == "/dev/nfs" ]
then
	mount -t nfs -o nolock "${nfspath}" /mnt	# Without nolock nfs mount does not work
else
	echo "Mounting ${root} with option ${permissions}"
	mount "${root}" /mnt -o "${permissions}"
fi

#------------------------------------------------------------
# For filesystem which uses udev and empty /dev :
# But /sbin/init need a console and null devices. So
# Create a tmpfs with the nodes and mount to newroot /dev
#------------------------------------------------------------
mount -t tmpfs dev  /mnt/dev
mknod /mnt/dev/console c 5 1
mknod /mnt/dev/null c 1 3


#------------------------------------------------------------
# Check if $init exists and is executable
#------------------------------------------------------------
if [[ -x "/mnt/${init}" ]] ; then
	#Unmount all other mounts except /dev
	umount /sys /proc

	#Switch to the new root and execute init
	exec switch_root /mnt "${init}" "${initargs}"
fi

#------------------------------------------------------------
# if failed, enter rescue mode
#------------------------------------------------------------
shell
