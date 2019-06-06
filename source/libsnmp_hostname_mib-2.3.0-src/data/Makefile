#
# Warning: you may need more libraries than are included here on the
# build line.  The agent frequently needs various libraries in order
# to compile pieces of it, but is OS dependent and we can't list all
# the combinations here.  Instead, look at the libraries that were
# used when linking the snmpd master agent and copy those to this
# file.
#

DEBUG = n
LIBRARY_NAME = libsnmp_hostname_mib
SRC = amiMibModule.c

CFLAGS=-I.
CFLAGS += -I${TARGETDIR}/usr/include
CFLAGS += -I${SPXINC}/hostname
CFLAGS += -L${SPXLIB}/hostname -lhostname

# shared library flags (assumes gcc)
DLFLAGS=-fPIC -shared

libsnmp_hostname_mib.so: amiMibModule.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o amiMibModule.o amiMibModule.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o libsnmp_hostname_mib.so.${PKG_MAJOR}.${PKG_MINOR}.${PKG_AUX} amiMibModule.o

include ${TOOLDIR}/rules/Rules.make.libs