DEBUG = n

LIBRARY_NAME = libopenradius

SRC = pam_radius_auth.c md5.c

CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/user/include
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${TARGETDIR}/usr/include

LIBS += -L$(SPXLIB)/dbgout -Wl,-rpath /usr/local/lib -ldbgout
LIBS+= -L$(IMAGE_TREE)/usr/local/lib -lpdkapp

include ${TOOLDIR}/rules/Rules.make.libs
