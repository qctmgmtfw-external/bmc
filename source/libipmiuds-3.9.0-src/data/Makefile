#--------------------------------------------------------------------------------------------------------
#                     Makefile to build libipmiuds
#                 Copyright (c) 2008 American Megatrends Inc 
#---------------------------------------------------------------------------------------------------------

#------- Comment/uncomment the following line to enable/disable debugging-------
DEBUG     = n
#---------------------------------------------------------------------------------------------------------

#---------------------- Change according to your files ------------------------------------------
LIBRARY_NAME  = libipmilocal
SRC = UDSIfc.c UDSMetaInfo.c

CFLAGS += -I${SPXINC}/ipmi
CFLAGS += -I${SPXINC}/global
CFLAGS += -I${SPXINC}/dbgout
CFLAGS += -I${SPXINC}/unix
CFLAGS += -I${SPXINC}/network
CFLAGS += -I${SPXINC}/libipmi
CFLAGS += -I${SPXINC}/pdk
CFLAGS += -I${SPXINC}/ipmihalapi
CFLAGS += -I${SPXINC}/hostname
CFLAGS += -I${SPXINC}/featuredefine
CFLAGS += -I${SPXINC}/blowfish
CFLAGS += -I${SPXINC}/encoding
CFLAGS += -I${TARGETDIR}/usr/include

CFLAGS += -lpthread -D_GNU_SOURCE

include ${TOOLDIR}/rules/Rules.make.libs
#------------------------------------------------------------------------------

