#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC 		= apps.c app_rand.c req.c x509.c openssl.c			# Source File
TARGET 		= openssl 			# Output file

CFLAGS += -I./
CFLAGS += -I${TARGETDIR}/usr/include

# Add the Corresponding library
LIBS+= -L${SPXLIB}/unix -lunix
LIBS+= -L${SPXLIB}/dbgout -ldbgout
LIBS+= -Wl,-L${TARGETDIR}/usr/lib -lcrypto
LIBS+= -Wl,-L${TARGETDIR}/usr/lib -lssl


include ${TOOLDIR}/rules/Rules.make.apps
