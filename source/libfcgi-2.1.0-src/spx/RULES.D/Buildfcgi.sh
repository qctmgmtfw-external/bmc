#!/bin/sh

ARCH_LC=`echo ${ARCH} | tr '[:upper:]' '[:lower:]'`

# Workaround for first time build error
touch ./aclocal.m4
touch ./Makefile.am
touch ./configure
touch ./Makefile.in

./configure --disable-dependency-tracking  --target=$ARCH_LC-linux --build=i686-linux --host=$ARCH_LC-linux CC=$CROSS_COMPILE"gcc" AR=$CROSS_COMPILE"ar" AS=$CROSS_COMPILE"as" LD=$CROSS_COMPILE"ld" NM=$CROSS_COMPILE"nm" RANLIB=$CROSS_COMPILE"ranlib" SIZE=$CROSS_COMPILE"size" STRIP=$CROSS_COMPILE"strip" OBJCOPY=$CROSS_COMPILE"objcopy" CXX=$CROSS_COMPILE"c++" CPP=$CROSS_COMPILE"cpp"

make CC=$CROSS_COMPILE"gcc" AR=$CROSS_COMPILE"ar" AS=$CROSS_COMPILE"as" LD=$CROSS_COMPILE"ld" NM=$CROSS_COMPILE"nm" RANLIB=$CROSS_COMPILE"ranlib" SIZE=$CROSS_COMPILE"size" STRIP=$CROSS_COMPILE"strip" OBJCOPY=$CROSS_COMPILE"objcopy" CXX=$CROSS_COMPILE"c++" CPP=$CROSS_COMPILE"cpp"

make install DESTDIR=${BUILDAREA}/target 

#Copy all the output files to 'OUTPUT' folder
mkdir ${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/OUTPUT
make install DESTDIR=${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/OUTPUT

#Rename all the platform specific output files
#mv ${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/output/usr/local/sbin/$ARCH_LC-linux-libfcgi ${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/output/usr/local/sbin/libfcgi
#mv ${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/output/usr/local/sbin/$ARCH_LC-linux-libfcgi-angel ${BUILD}/${PACKAGE_NAME}/data/.workspace/libfcgi/output/usr/local/sbin/libfcgi-angel
