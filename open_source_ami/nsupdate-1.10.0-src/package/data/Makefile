#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC    = nsupdate.c
TARGET = nsupdate 

CFLAGS += -D_GNU_SOURCE
CFLAGS += -I.
CFLAGS += -I${TARGETDIR}/usr/include
CFLAGS += -I${TARGETDIR}/usr/include/libxml2

CFLAGS += -D_REENTRANT -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 
CFLAGS += -DDIG_SIGCHASE
CFLAGS += -DSESSION_KEYFILE=\"var/run/named/session.key\" 

STATLIBS += $(TARGETDIR)/usr/lib/libisccfg.a
STATLIBS += $(TARGETDIR)/usr/lib/libbind9.a
STATLIBS += $(TARGETDIR)/usr/lib/libdns.a 
STATLIBS += $(TARGETDIR)/usr/lib/libGeoIP.a
STATLIBS += $(TARGETDIR)/usr/lib/libisc.a 
STATLIBS += $(TARGETDIR)/usr/lib/liblwres.a
STATLIBS += $(TARGETDIR)/usr/lib/libxml2.a

LIBS = $(STATLIBS) -Wl,-L$(TARGETDIR)/usr/lib -lcrypto -lpthread
LIBS += -lz -lm -lgssapi_krb5 -lkrb5 -lkrb5support -lk5crypto 
LIBS += $(TARGETDIR)/lib/libcom_err.so.2 $(TARGETDIR)/lib/libkeyutils.so.1


include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------
