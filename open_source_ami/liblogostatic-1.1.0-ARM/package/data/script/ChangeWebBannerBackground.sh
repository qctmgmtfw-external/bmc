#!/bin/sh

echo "Change banner background of web page......"
if [ $2 == 0 ] && [ $1 == 0 ] 
then
	echo "Error read file.."
	echo "Usage : $0 <Input File Name> <Replace File Name>"
	exit 1
fi

if [ -s $2 ] && [ -s $1 ]
then
	echo "Read replace file OK."
	./fwm -i $1 -s
	if [ $? != 0 ]
	then
		echo "Execute fwm error."
		exit 1
	fi
#	mkdir ./replace/src/www
	mkdir -p ./tmp/www
	mount -t cramfs ./output/www.bin ./tmp/www -o loop
	if [ $? != 0 ]
	then
		echo "Mount www dir error."
		exit 1
	fi
	cp ./tmp/* ./replace/src/ -fr
	if [ $? != 0 ]
	then
		echo "Copy www dir error."
		exit 1
	fi
	cp $2 ./replace/src/www/res/oem/banner_bg.jpg -f
	if [ $? != 0 ]
	then
		echo "Replace file error."
		exit 1
	fi
	
	echo "Clean replus files......."
	umount ./tmp/www
	rm ./tmp -fr
	
	./fwm -i $1 -r -o new.ima
	if [ $? != 0 ]
	then
		echo "Generate rom file error."
		exit 1
	fi
#Replace file over. Clean replus files
	echo "Clean replace files......."
	rm ./replace/src/* -fr
	echo "Done."
else
	echo "Error read file.."
	echo "Usage : $0 <Input File Name> <Replace File Name>"
	exit 1
fi
echo "Change banner background of web page finished."
