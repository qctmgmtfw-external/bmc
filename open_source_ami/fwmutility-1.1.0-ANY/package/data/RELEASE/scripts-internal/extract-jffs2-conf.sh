#!/bin/sh

modprobe mtd
modprobe jffs2
modprobe mtdram
modprobe mtdchar
modprobe mtdblock

if [ -f $1 ];then
	echo "Please spcify the correct filename"
fi

if [ -f /dev/mtd0 ]; then
	echo "/dev/mtd0 is missing"
	echo "Perhaps your need to install mtd-tools packages"
	exit -1
fi

dd if=$1 of=/dev/mtdr0 > /dev/null

if [ $? != 0 ]; then
	echo "The script for extracting configuration partition is failed"
	echo "Please check your Linux environment"
	echo "Perhaps your need to install mtd-tools packages"
	exit -1
fi

mkdir tmp-config
mount /dev/mtdblock0 tmp-config -t jffs2
tar cf BMC-CONF.tar.dat tmp-config
umount tmp-config
rm tmp-config -rf
