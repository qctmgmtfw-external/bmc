#------- Comment/uncomment the following line to enable/disable debugging---------
DEBUG = n

#---------------------- Change according to your files ----------------------------
SRC 	=   websocket.c # Source Files
SRC		+=  websockify.c# Source Files
TARGET  =   websockify 		# Output file

CFLAGS += -I./
CFLAGS += -I${TARGETDIR}/usr/include
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/unix
CFLAGS	+=	-fPIC -DPIC

# Add the Corresponding library
LIBS+= -L${SPXLIB}/unix -lunix
LIBS+= -Wl,-L${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB} -lcrypto
LIBS+= -Wl,-L${IMAGE_TREE}/usr/lib/${MULTI_ARCH_LIB} -lssl
LIBS+= -Wl,-L${IMAGE_TREE}/usr/lib -lresolv

include ${TOOLDIR}/rules/Rules.make.apps

#----------------------------------------------------------------------------------------

