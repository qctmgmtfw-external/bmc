#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC 		= timeoutd.c			# Source File
TARGET = timeoutd 			# Output file

CFLAGS += -fomit-frame-pointer -O2 -s

# Add the Corresponding library
# Below libraries will be useful if getpwnam enabled 
#LIBS += -L${SPXLIB}/ipmi -lipmi
#LIBS += -L${SPXLIB}/user_auth -luserauth
#LIBS += ${TARGETDIR}/usr/lib/libssl.so
#LIBS += ${TARGETDIR}/usr/lib/libcrypto.so
#LIBS += ${TARGETDIR}/lib/libpam.so.0

include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------
