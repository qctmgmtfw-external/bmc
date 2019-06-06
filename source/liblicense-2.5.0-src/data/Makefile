DEBUG = n
LIBRARY_NAME = liblicense

SRC	= license.c

CFLAGS 		+= -I${SPXINC}/global
CFLAGS		+= -I${SPXINC}/dbgout
CFLAGS		+= -I${SPXINC}/unix
CFLAGS		+= -I${SPXINC}/ipmi
CFLAGS		+= -I${SPXINC}/featuredefine
CFLAGS 		+= -I${SPXINC}/network
CFLAGS 		+= -I${SPXINC}/hostname


# Add the corresponding library

LIBS		+= -L${SPXLIB}/dbgout -ldbgout
LIBS		+= -L${SPXLIB}/unix   -lunix
LIBS		+= -L${SPXLIB}/featuredefine -lfeaturedefine
LIBS    	+= -L${SPXLIB}/network -lnetwork

include ${TOOLDIR}/rules/Rules.make.libs
