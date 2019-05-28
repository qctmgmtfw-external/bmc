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
LIBS+= -Wl,-L${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB} -lcrypto
LIBS+= -Wl,-L${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB} -lssl


include ${TOOLDIR}/rules/Rules.make.apps
